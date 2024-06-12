#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "linetchannelinfo.h"
#include "mathlib/lvector.h"
#include "usermessages.h"
#include "lnetwork.h"
#include <thread>
#include <chrono>

#ifndef CLIENT_DLL
#include "lrecipientfilter.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar lua_umsg_size( "lua_umsg_size", "255", FCVAR_ARCHIVE | FCVAR_REPLICATED, "The size of the usermessage buffer" );

/*
** umsg library
*/
#ifdef CLIENT_DLL
/// <summary>
/// Calls umsg.OnMessageReceived( messageName, message )
/// </summary>
/// <param name="L"></param>
/// <param name="messageName"></param>
/// <param name="message"></param>
static void umsg_CallOnMessageReceived( lua_State *L, const char *messageName, bf_read *message )
{
    lua_getglobal( L, "umsg" );
    lua_getfield( L, -1, "OnMessageReceived" );
    lua_pushstring( L, messageName );
    lua_pushbf_read( L, message );
    lua_call( L, 2, 0 );
}

static void umsg_HandleReceiveMessage( bf_read &msg )
{
    Warning( "umsg_HandleReceiveMessage\n" );
    char name[2048];
    msg.ReadString( name, sizeof( name ) );
    // umsg_CallOnMessageReceived( L, name, &msg );
}
#else
static bf_write *g_pMsgBuffer = NULL;

static int umsg_Start( lua_State *L )
{
    // TODO: This is only to debug
    CSingleUserRecipientFilter filter( UTIL_PlayerByIndex( 1 ) );
    filter.MakeReliable();
    const char *messageName = luaL_checkstring( L, 1 );

    // If the filter is nil or empty, create one with all players
    if ( !lua_isnoneornil( L, 2 ) )
    {
        //filter = lua_torecipientfilter( L, 2 );
    }

    if ( g_pMsgBuffer )
    {
        Warning( "umsg.Start: Already started, resetting it\n" );
        g_pMsgBuffer->Reset();
    }
    else
    {
        int msg_type = usermessages->LookupUserMessage( "_LuaMessage" );

        if ( msg_type == -1 )
        {
            Error( "umsg.Start:  Unregistered message '%s'\n", "_LuaMessage" );
        }

        g_pMsgBuffer = engine->UserMessageBegin( &filter, msg_type );
    }

    // Write the message name the user wants to send
    g_pMsgBuffer->WriteString( messageName );

    return 0;
}

static int umsg_MessageEnd( lua_State *L )
{
    if ( !g_pMsgBuffer )
    {
        Error( "umsg.MessageEnd called with no active message\n" );
        return 0;
    }

    // Get how many bytes we've written, fill the rest with 0
    int bytesLeft = g_pMsgBuffer->GetNumBytesLeft() - 1; // Why -1?

    for ( int i = 0; i < bytesLeft; i++ )
    {
        g_pMsgBuffer->WriteByte( 0 );
    }

    Warning( "umsg.MessageEnd: %i bytes left\n", bytesLeft );

    engine->MessageEnd();

    g_pMsgBuffer = NULL;

    return 0;
}

static int umsg_WriteByte( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "umsg.WriteByte called with no active message\n" );

    g_pMsgBuffer->WriteByte( luaL_checkint( L, 1 ) );
    return 0;
}

static int umsg_WriteChar( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "umsg.WriteChar called with no active message\n" );

    g_pMsgBuffer->WriteChar( luaL_checkint( L, 1 ) );
    return 0;
}

static int umsg_WriteShort( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "umsg.WriteShort called with no active message\n" );

    g_pMsgBuffer->WriteShort( luaL_checkint( L, 1 ) );

    return 0;
}

static int umsg_WriteWord( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "umsg.WriteWord called with no active message\n" );

    g_pMsgBuffer->WriteWord( luaL_checkint( L, 1 ) );

    return 0;
}

static int umsg_WriteLong( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "umsg.WriteLong called with no active message\n" );

    g_pMsgBuffer->WriteLong( luaL_checkint( L, 1 ) );

    return 0;
}

static int umsg_WriteFloat( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "umsg.WriteFloat called with no active message\n" );

    g_pMsgBuffer->WriteFloat( luaL_checknumber( L, 1 ) );

    return 0;
}

static int umsg_WriteVector( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "umsg.WriteVector called with no active message\n" );

    g_pMsgBuffer->WriteBitVec3Coord( luaL_checkvector( L, 1 ) );

    return 0;
}

static int umsg_WriteNormal( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "umsg.WriteNormal called with no active message\n" );

    g_pMsgBuffer->WriteBitVec3Normal( luaL_checkvector( L, 1 ) );

    return 0;
}

static int umsg_WriteAngle( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "WriteAngle called with no active message\n" );

    g_pMsgBuffer->WriteBitAngles( luaL_checkangle( L, 1 ) );

    return 0;
}

static int umsg_WriteString( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "WriteString called with no active message\n" );

    g_pMsgBuffer->WriteString( luaL_checkstring( L, 1 ) );

    return 0;
}

static int umsg_WriteEntity( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "WriteEHandle called with no active message\n" );

    long iEncodedEHandle;

    if ( luaL_checkentity( L, 1 ) )
    {
        EHANDLE hEnt = lua_toentity( L, 1 );

        int iSerialNum = hEnt.GetSerialNumber() &
                         ( ( 1 << NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS ) - 1 );
        iEncodedEHandle = hEnt.GetEntryIndex() | ( iSerialNum << MAX_EDICT_BITS );
    }
    else
    {
        iEncodedEHandle = INVALID_NETWORKED_EHANDLE_VALUE;
    }

    g_pMsgBuffer->WriteLong( iEncodedEHandle );

    return 0;
}

static int umsg_WriteBool( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "WriteBool called with no active message\n" );

    g_pMsgBuffer->WriteOneBit( lua_toboolean( L, 1 ) ? 1 : 0 );

    return 0;
}

static int umsg_WriteUBitLong( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "WriteUBitLong called with no active message\n" );

    g_pMsgBuffer->WriteUBitLong( luaL_checkint( L, 1 ), luaL_checkint( L, 2 ) );

    return 0;
}

static int umsg_WriteSBitLong( lua_State *L )
{
    if ( !g_pMsgBuffer )
        Error( "WriteSBitLong called with no active message\n" );

    g_pMsgBuffer->WriteSBitLong( luaL_checkint( L, 1 ), luaL_checkint( L, 2 ) );

    return 0;
}
#endif

static const luaL_Reg umsgLib[] = {
#ifndef CLIENT_DLL
    { "Start", umsg_Start },
    { "MessageEnd", umsg_MessageEnd },
    { "WriteAngle", umsg_WriteAngle },
    { "WriteAngle", umsg_WriteAngle },
    { "WriteBool", umsg_WriteBool },
    { "WriteByte", umsg_WriteByte },
    { "WriteChar", umsg_WriteChar },
    { "WriteEntity", umsg_WriteEntity },
    { "WriteFloat", umsg_WriteFloat },
    { "WriteLong", umsg_WriteLong },
    { "WriteNormal", umsg_WriteNormal },
    { "WriteSBitLong", umsg_WriteSBitLong },
    { "WriteShort", umsg_WriteShort },
    { "WriteString", umsg_WriteString },
    { "WriteUBitLong", umsg_WriteUBitLong },
    { "WriteVector", umsg_WriteVector },
    { "WriteWord", umsg_WriteWord },
#endif
    { NULL, NULL } };

/// <summary>
/// Called from RegisterUserMessages when the usermessages library is registered
/// by the engine.
/// </summary>
/// <param name=""></param>
void RegisterLuaUserMessages()
{
    if ( usermessages->LookupUserMessage( "_LuaMessage" ) != -1 )
    {
        return;
    }

    // Register a single message that carries all other messages, It's a bit of a hack but it works
    usermessages->Register( "_LuaMessage", lua_umsg_size.GetInt() );

#ifdef CLIENT_DLL
    usermessages->HookMessage( "_LuaMessage", umsg_HandleReceiveMessage );
#endif
}

/*
** net library
*/
static const luaL_Reg netLib[] = {
    { NULL, NULL } };

/*
** bf_read methods
*/
#ifdef CLIENT_DLL
LUA_API void lua_pushbf_read( lua_State *L, bf_read *message )
{
    bf_read **ud = ( bf_read ** )lua_newuserdata( L, sizeof( bf_read * ) );
    *ud = message;
    luaL_getmetatable( L, LUA_BFREADLIBNAME );
    lua_setmetatable( L, -2 );
}

LUALIB_API bf_read *&luaL_checkbf_read( lua_State *L, int narg )
{
    bf_read **ud = ( bf_read ** )luaL_checkudata( L, narg, LUA_BFREADLIBNAME );
    if ( !ud )
    {
        luaL_typerror( L, narg, LUA_BFREADLIBNAME );
    }
    return *ud;
}

static int bfRead_ReadBit( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    lua_pushinteger( L, bf->ReadOneBit() );
    return 1;
}

static int bfRead_ReadByte( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    lua_pushinteger( L, bf->ReadByte() );
    return 1;
}

static int bfRead_ReadChar( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    lua_pushinteger( L, bf->ReadChar() );
    return 1;
}

static int bfRead_ReadShort( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    lua_pushinteger( L, bf->ReadShort() );
    return 1;
}

static int bfRead_ReadWord( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    lua_pushinteger( L, bf->ReadWord() );
    return 1;
}

static int bfRead_ReadLong( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    lua_pushinteger( L, bf->ReadBitLong( luaL_checkint( L, 2 ), lua_toboolean( L, 3 ) ) );
    return 1;
}

static int bfRead_ReadFloat( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    lua_pushnumber( L, bf->ReadBitFloat() );
    return 1;
}

static int bfRead_ReadVector( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    Vector vec;
    bf->ReadBitVec3Coord( vec );
    lua_pushvector( L, vec );
    return 1;
}

static int bfRead_ReadVectorNormal( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    Vector vec;
    bf->ReadBitVec3Normal( vec );
    lua_pushvector( L, vec );
    return 1;
}

static int bfRead_ReadAngle( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    QAngle ang;
    bf->ReadBitAngles( ang );
    lua_pushangle( L, ang );
    return 1;
}

static int bfRead_ReadString( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    char str[2048];
    bf->ReadString( str, sizeof( str ) );
    lua_pushstring( L, str );
    return 1;
}

static int bfRead_ReadBytes( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    int len = luaL_checkint( L, 2 );
    char *buf = new char[len];
    bf->ReadBytes( buf, len );
    lua_pushlstring( L, buf, len );
    delete[] buf;
    return 1;
}

static int bfRead_ReadEntity( lua_State *L )
{
    bf_read *bf = luaL_checkbf_read( L, 1 );
    unsigned int iEncodedEHandle = bf->ReadLong();

    if ( iEncodedEHandle == INVALID_NETWORKED_EHANDLE_VALUE )
    {
        lua_pushnil( L );
        return 1;
    }

    int iSerialNum = iEncodedEHandle >> MAX_EDICT_BITS;

    CBaseEntity *pEntity = CBaseEntity::Instance( iEncodedEHandle & ( ( 1 << MAX_EDICT_BITS ) - 1 ) );

    if ( !pEntity )
    {
        lua_pushnil( L );
        return 1;
    }

    CBaseHandle handle = pEntity->GetRefEHandle();

    if ( handle.GetSerialNumber() != iSerialNum )
    {
        lua_pushnil( L );
        return 1;
    }

    lua_pushentity( L, pEntity );

    return 1;
}

static const luaL_Reg bfReadLib[] = {
    { "ReadBit", bfRead_ReadBit },
    { "ReadByte", bfRead_ReadByte },
    { "ReadChar", bfRead_ReadChar },
    { "ReadShort", bfRead_ReadShort },
    { "ReadWord", bfRead_ReadWord },
    { "ReadLong", bfRead_ReadLong },
    { "ReadFloat", bfRead_ReadFloat },
    { "ReadVector", bfRead_ReadVector },
    { "ReadVectorNormal", bfRead_ReadVectorNormal },
    { "ReadAngle", bfRead_ReadAngle },
    { "ReadString", bfRead_ReadString },
    { "ReadBytes", bfRead_ReadBytes },
    { "ReadEntity", bfRead_ReadEntity },
    { NULL, NULL } };
#endif

/*
** Open networking libraries
*/
LUALIB_API int luaopen_umsg( lua_State *L )
{
    luaL_register( L, LUA_UMSGLIBNAME, umsgLib );
    return 1;
}

LUALIB_API int luaopen_net( lua_State *L )
{
    luaL_register( L, LUA_NETLIBNAME, netLib );
    return 1;
}

#ifdef CLIENT_DLL
LUALIB_API int luaopen_bf_read( lua_State *L )
{
    luaL_getmetatable( L, LUA_BFREADLIBNAME );
    if ( lua_isnoneornil( L, -1 ) )
    {
        lua_pop( L, 1 );
        luaL_newmetatable( L, LUA_BFREADLIBNAME );
    }
    luaL_register( L, NULL, bfReadLib );
    return 1;
}
#endif
