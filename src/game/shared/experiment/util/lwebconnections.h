#ifndef LWEBCONNECTIONS_H
#define LWEBCONNECTIONS_H

#ifdef _WIN32
#pragma once
#endif

#include <steam/isteamhttp.h>
#include "utlbuffer.h"

class CLuaWebConnectionManager;  // Forward declaration

class LuaWebConnectionHandle
{
    public:
    LuaWebConnectionHandle(
        HTTPRequestHandle handle,
        lua_State *L,
        int successCallback,
        int failureCallback )
        : m_L( L ),
        m_Handle( handle ),
        m_SuccessCallback( successCallback ),
        m_FailureCallback( failureCallback ) {}
    ~LuaWebConnectionHandle();

    CCallResult< CLuaWebConnectionManager, HTTPRequestCompleted_t > *m_CompleteResult;

    void SetResponseBufferSize( int size )
    {
        m_ResponseBuffer.EnsureCapacity( size );
    }

    void PutInResponseBuffer( const void *data, int size )
    {
        m_ResponseBuffer.Put( data, size );
    }

    int GetResponseBufferSize() const
    {
        return m_ResponseBuffer.TellPut();
    }

    const char *GetResponseBuffer() const
    {
        return reinterpret_cast< const char * >( m_ResponseBuffer.Base() );
    }

    void CallSuccessCallback( EHTTPStatusCode statusCode, const char *body, int bodySize );
    void CallFailureCallback( const char *message );

    private:
    HTTPRequestHandle m_Handle;
    lua_State *m_L;

    int m_SuccessCallback;
    int m_FailureCallback;

    CUtlBuffer m_ResponseBuffer;
};

class CLuaWebConnectionManager
{
    public:
    CLuaWebConnectionManager();

    void AddRequest( HTTPRequestHandle handle, LuaWebConnectionHandle *connectionHandle )
    {
        m_WebConnectionsMap.Insert( handle, connectionHandle );
    }

    void RemoveRequest( HTTPRequestHandle handle )
    {
        int index = m_WebConnectionsMap.Find( handle );

        if ( index != m_WebConnectionsMap.InvalidIndex() )
        {
            m_WebConnectionsMap.RemoveAt( index );
        }
    }

    LuaWebConnectionHandle *GetRequest( HTTPRequestHandle handle )
    {
        int index = m_WebConnectionsMap.Find( handle );

        if ( index != m_WebConnectionsMap.InvalidIndex() )
        {
            return m_WebConnectionsMap[index];
        }

        return nullptr;
    }

    // Call this before closing the Lua state
    void PurgeAllRequests()
    {
        m_WebConnectionsMap.PurgeAndDeleteElements();
    }

    STEAM_CALLBACK( CLuaWebConnectionManager, OnHTTPHeader, HTTPRequestHeadersReceived_t );
    STEAM_CALLBACK( CLuaWebConnectionManager, OnHTTPData, HTTPRequestDataReceived_t );
    void OnHTTPComplete( HTTPRequestCompleted_t *pParam, bool bIO );

    private:
    CUtlMap< HTTPRequestHandle, LuaWebConnectionHandle * > m_WebConnectionsMap;
};

extern CLuaWebConnectionManager *g_pLuaWebConnectionManager;

#endif  // LWEBCONNECTIONS_H
