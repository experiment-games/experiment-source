#include "cbase.h"
#include "luamanager.h"
#include "lwebconnections.h"
#include "LKeyValues.h"

#include "tier0/memdbgon.h"

static CLuaWebConnectionManager s_LuaWebConnectionManager;
extern CLuaWebConnectionManager *g_pLuaWebConnectionManager = &s_LuaWebConnectionManager;

#define WEBCONNECTIONSHEADERSMETANAME "WebConnectionHeadersHandle"

LUA_REGISTRATION_INIT( WebConnectionHeadersHandle )

LUA_BINDING_BEGIN( WebConnectionHeadersHandle, __index, "class", "Get the headers of the HTTP request", "client" )
{
    // Check if the first argument is a WebConnectionHeadersHandle
    luaL_getmetatable( L, WEBCONNECTIONSHEADERSMETANAME );
    lua_getmetatable( L, 1 );

    if (!lua_istable( L, 1 ) || !lua_rawequal( L, -1, -2 ))
    {
        luaL_typeerror( L, 1, WEBCONNECTIONSHEADERSMETANAME );
        return 0;
    }

    lua_getfield( L, 1, "__handle" );
    HTTPRequestHandle handle = ( uint32 )luaL_checknumber( L, -1 );

    const char *headerName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "headerName" );

    uint32 size;
    if ( !SteamHTTP()->GetHTTPResponseHeaderSize( handle, headerName, &size ) )
    {
        lua_pushnil( L );
        return 1;
    }

    uint8 *pData = new uint8[size + 1];

    if ( !SteamHTTP()->GetHTTPResponseHeaderValue( handle, headerName, pData, size ) )
    {
        delete[] pData;
        lua_pushnil( L );
        return 1;
    }

    // Null-terminate
    pData[size] = 0;

    lua_pushstring( L, reinterpret_cast< const char * >( pData ) );

    delete[] pData;
    return 1;
}
LUA_BINDING_END()

LuaWebConnectionHandle::~LuaWebConnectionHandle()
{
    if ( m_CompleteResult )
    {
        delete m_CompleteResult;
    }

    if ( lua_isrefvalid( m_L, m_SuccessCallback ) )
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_SuccessCallback );
    }

    if ( lua_isrefvalid( m_L, m_FailureCallback ) )
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_FailureCallback );
    }
}

void LuaWebConnectionHandle::CallSuccessCallback( EHTTPStatusCode statusCode, const char *body, int bodySize )
{
    if ( !lua_isrefvalid( m_L, m_SuccessCallback ) )
    {
        return;
    }

    lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_SuccessCallback );
    lua_pushinteger( m_L, statusCode );
    lua_pushlstring( m_L, body, bodySize );
    // lua_newtable( m_L );
    //  The Steamworks API doesn't provide a way to get all the headers, so we push a table with a __index metamethod that will forward to GetHTTPResponseHeaderValue
    lua_newtable( m_L );
    lua_pushnumber( m_L, ( uint32 )m_Handle );
    lua_setfield( m_L, -2, "__handle" );

    luaL_getmetatable( m_L, WEBCONNECTIONSHEADERSMETANAME );
    lua_setmetatable( m_L, -2 );

    luasrc_pcall( m_L, 3, 0 );
}

void LuaWebConnectionHandle::CallFailureCallback( const char *message )
{
    if ( !lua_isrefvalid( m_L, m_SuccessCallback ) )
    {
        DevWarning( "Lua WebConnection Request failed with error: %s", message );
        return;
    }

    lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_FailureCallback );
    lua_pushstring( m_L, message );
    luasrc_pcall( m_L, 1, 0 );
}

// Source for these methods: https://github.com/momentum-mod/game/blob/2e490c7e722788ade7221ba7ba5d4d503d60a115/mp/src/game/client/momentum/mom_api_requests.cpp

CLuaWebConnectionManager::CLuaWebConnectionManager()
{
    SetDefLessFunc( m_WebConnectionsMap );
}

/// <summary>
/// Upon receiving the headers, use Content-Length to allocate the buffer for the response.
/// </summary>
/// <param name="pCallback"></param>
void CLuaWebConnectionManager::OnHTTPHeader( HTTPRequestHeadersReceived_t *pCallback )
{
    const uint16 connectionHandleIndex = m_WebConnectionsMap.Find( pCallback->m_hRequest );

    if ( connectionHandleIndex == m_WebConnectionsMap.InvalidIndex() )
    {
        // Happens when the server gets a client request
        // TODO: Won't this cause problems if a handle matches both the client and server?
        return;
    }

    uint32 size;
    if ( !SteamHTTP()->GetHTTPResponseHeaderSize( pCallback->m_hRequest, "Content-Length", &size ) )
    {
        size = 0;
    }

    LuaWebConnectionHandle *call = m_WebConnectionsMap[connectionHandleIndex];

    if ( size == 0 )
    {
        DevWarning( "Content-Length header was not found in WebConnections request!\n" );
        delete call;
        return;
    }

    uint8 *pData = new uint8[size + 1];

    if ( !SteamHTTP()->GetHTTPResponseHeaderValue( pCallback->m_hRequest, "Content-Length", pData, size ) )
    {
        Assert( 0 );  // TODO: Should this be allowed to happen?
        delete[] pData;
        return;
    }

    // Null-terminate
    pData[size] = 0;
    const uint64 responseSize = Q_atoui64( reinterpret_cast< const char * >( pData ) );

    if ( !responseSize )
    {
        Assert( 0 );  // TODO: Should this be allowed to happen?
        delete[] pData;
        return;
    }

    call->SetResponseBufferSize( responseSize );

    delete[] pData;
    pData = nullptr;
}

void CLuaWebConnectionManager::OnHTTPData( HTTPRequestDataReceived_t *pCallback )
{
    const uint16 connectionHandleIndex = m_WebConnectionsMap.Find( pCallback->m_hRequest );

    if ( connectionHandleIndex == m_WebConnectionsMap.InvalidIndex() )
    {
        // Happens when the server gets a client request
        // TODO: Won't this cause problems if a handle matches both the client and server?
        return;
    }

    uint8 *pDataTemp = new uint8[pCallback->m_cBytesReceived];

    if ( !SteamHTTP()->GetHTTPStreamingResponseBodyData( pCallback->m_hRequest, pCallback->m_cOffset, pDataTemp, pCallback->m_cBytesReceived ) )
    {
        Assert( 0 );  // TODO: Should this be allowed to happen?
        delete[] pDataTemp;
        return;
    }

    LuaWebConnectionHandle *call = m_WebConnectionsMap[connectionHandleIndex];
    call->PutInResponseBuffer( pDataTemp, pCallback->m_cBytesReceived );

    delete[] pDataTemp;
    pDataTemp = nullptr;
}

void CLuaWebConnectionManager::OnHTTPComplete( HTTPRequestCompleted_t *pCallback, bool bIO )
{
    const uint16 connectionHandleIndex = m_WebConnectionsMap.Find( pCallback->m_hRequest );

    if ( connectionHandleIndex == m_WebConnectionsMap.InvalidIndex() )
    {
        // Happens when the server gets a client request
        // TODO: Won't this cause problems if a handle matches both the client and server?
        return;
    }

    LuaWebConnectionHandle *call = m_WebConnectionsMap[connectionHandleIndex];

    if ( bIO || !pCallback->m_bRequestSuccessful )
    {
        char errorMessage[256];
        Q_snprintf( errorMessage, sizeof( errorMessage ), "HTTP request failed with status code %d", pCallback->m_eStatusCode );
        call->CallFailureCallback( errorMessage );
    }
    else
    {
        call->CallSuccessCallback(
            pCallback->m_eStatusCode,
            call->GetResponseBuffer(),
            call->GetResponseBufferSize() );
    }

    RemoveRequest( pCallback->m_hRequest );
    delete call;

    SteamHTTP()->ReleaseHTTPRequest( pCallback->m_hRequest );
}

LUA_REGISTRATION_INIT( WebConnections )

// POST:
// lua_run_menu WebConnections.RequestHttpMethod( "https://www.postb.in/1727536026647-4169531788211", "POST", "Hello, World!", nil, nil, "text/plain; charset=utf-8", 60, function( code, body, headers ) print( "body", code, body, headers ) end, function( error ) print( "err", error ) end )
// GET:
// lua_run_menu WebConnections.RequestHttpMethod( "https://www.postb.in/1727536026647-4169531788211", "GET", nil, nil, nil, nil, nil, function( code, body, headers ) print( "body", code, body, headers ) end, function( error ) print( "err", error ) end )
// GET with parameters:
// lua_run_menu PARAMETERS = KeyValues.Create("parameters") PARAMETERS:SetString("name", "value")
// lua_run_menu WebConnections.RequestHttpMethod( "https://www.postb.in/1727536026647-4169531788211", "GET", nil, PARAMETERS, nil, nil, nil, function( code, body, headers ) print( "body", code, body, headers ) end, function( error ) print( "err", error ) end )
// lua_run_menu WebConnections.RequestHttpMethod( "https://www.postb.in/1727537932498-3552409373223", "GET", nil, nil, nil, nil, nil, function( code, body, headers ) print( "body", code, body, headers, type(headers), headers["Content-Length"] ) end )
LUA_BINDING_BEGIN( WebConnections, RequestHttpMethod, "library", "Performs an HTTP request of the specified method", "client" )
{
    const char *url = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "url" );
    const char *methodAsString = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "method" );
    const char *requestBody = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, nullptr, "body" );

    KeyValues *parameters = !lua_isnoneornil( L, 4 ) && luaL_checkkeyvalues( L, 4 )
                                ? LUA_BINDING_ARGUMENT_NILLABLE( lua_tokeyvalues, 4, "parameters" )
                                : nullptr;

    KeyValues *headers = !lua_isnoneornil( L, 5 ) && luaL_checkkeyvalues( L, 5 )
                             ? LUA_BINDING_ARGUMENT_NILLABLE( lua_tokeyvalues, 5, "headers" )
                             : nullptr;

    const char *contentType = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 6, "text/plain; charset=utf-8", "contentType" );
    int timeout = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 7, 60, "timeout" );

    const int successCallbackIndex = 8;
    if ( !lua_isnoneornil( L, successCallbackIndex ) && !lua_isfunction( L, successCallbackIndex ) )
        luaL_typeerror( L, successCallbackIndex, "function" );

    const int failureCallbackIndex = 9;
    if ( !lua_isnoneornil( L, failureCallbackIndex ) && !lua_isfunction( L, failureCallbackIndex ) )
        luaL_typeerror( L, 9, "function" );

    EHTTPMethod method = k_EHTTPMethodInvalid;

    if ( Q_stricmp( methodAsString, "GET" ) == 0 )
    {
        method = k_EHTTPMethodGET;
    }
    else if ( Q_stricmp( methodAsString, "POST" ) == 0 )
    {
        method = k_EHTTPMethodPOST;
    }
    else if ( Q_stricmp( methodAsString, "HEAD" ) == 0 )
    {
        method = k_EHTTPMethodHEAD;
    }
    else if ( Q_stricmp( methodAsString, "PUT" ) == 0 )
    {
        method = k_EHTTPMethodPUT;
    }
    else if ( Q_stricmp( methodAsString, "DELETE" ) == 0 )
    {
        method = k_EHTTPMethodDELETE;
    }
    else if ( Q_stricmp( methodAsString, "OPTIONS" ) == 0 )
    {
        method = k_EHTTPMethodOPTIONS;
    }
    else if ( Q_stricmp( methodAsString, "PATCH" ) == 0 )
    {
        method = k_EHTTPMethodPATCH;
    }

    if ( method == k_EHTTPMethodInvalid )
    {
        luaL_error( L, "Invalid HTTP method: %s", methodAsString );

        lua_pushboolean( L, false );
        return 1;
    }

    ISteamHTTP *pHTTP = steamapicontext->SteamHTTP();
    HTTPRequestHandle hRequest = pHTTP->CreateHTTPRequest( method, url );

    pHTTP->SetHTTPRequestNetworkActivityTimeout( hRequest, timeout );

    // Set headers
    if ( headers )
    {
        for ( KeyValues *key = headers->GetFirstSubKey(); key; key = key->GetNextKey() )
        {
            const char *headerName = key->GetName();
            const char *headerValue = key->GetString();
            if ( !pHTTP->SetHTTPRequestHeaderValue( hRequest, headerName, headerValue ) )
            {
                luaL_error( L, "Failed to set header %s: %s (not allowed)", headerName, headerValue );
            }
        }
    }

    // Set parameters for GET, POST, HEAD
    if ( parameters )
    {
        for ( KeyValues *key = parameters->GetFirstSubKey(); key; key = key->GetNextKey() )
        {
            pHTTP->SetHTTPRequestGetOrPostParameter( hRequest, key->GetName(), key->GetString() );
        }
    }

    // Set the body only if we're POSTing some data
    if ( method == k_EHTTPMethodPOST )
    {
        CUtlBuffer postDataRaw;
        postDataRaw.PutString( requestBody );

        pHTTP->SetHTTPRequestRawPostBody( hRequest, contentType, ( uint8 * )postDataRaw.Base(), postDataRaw.TellPut() );
    }

    SteamAPICall_t hCall;
    if ( !pHTTP->SendHTTPRequestAndStreamResponse( hRequest, &hCall ) )
    {
        pHTTP->ReleaseHTTPRequest( hRequest );

        lua_pushboolean( L, false );
        return 1;
    }

    int successCallbackRef = LUA_NOREF;
    int failureCallbackRef = LUA_NOREF;

    if ( !lua_isnoneornil( L, successCallbackIndex ) )
    {
        lua_pushvalue( L, successCallbackIndex );
        successCallbackRef = luaL_ref( L, LUA_REGISTRYINDEX );
    }

    if ( !lua_isnoneornil( L, failureCallbackIndex ) )
    {
        lua_pushvalue( L, failureCallbackIndex );
        failureCallbackRef = luaL_ref( L, LUA_REGISTRYINDEX );
    }

    LuaWebConnectionHandle *callback = new LuaWebConnectionHandle(
        hRequest,
        L,
        successCallbackRef,
        failureCallbackRef );

    s_LuaWebConnectionManager.AddRequest( hRequest, callback );

    callback->m_CompleteResult = new CCallResult< CLuaWebConnectionManager, HTTPRequestCompleted_t >();
    callback->m_CompleteResult->Set(
        hCall,
        &s_LuaWebConnectionManager,
        &CLuaWebConnectionManager::OnHTTPComplete );

    lua_pushboolean( L, true );
    return 1;
}
LUA_BINDING_END()

LUALIB_API int luaopen_WebConnections( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( WebConnections );

    LUA_PUSH_NEW_METATABLE( L, WEBCONNECTIONSHEADERSMETANAME );

    LUA_REGISTRATION_COMMIT( WebConnectionHeadersHandle );

    lua_pushstring( L, WEBCONNECTIONSHEADERSMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "WebConnectionHeadersHandle" */

    return 1;
}
