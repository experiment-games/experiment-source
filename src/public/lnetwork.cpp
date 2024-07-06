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
    lua_getglobal( L, LUA_UMSGLIBNAME );
    lua_getfield( L, -1, "OnMessageReceived" );
    lua_pushstring( L, messageName );
    lua_pushbf_read( L, message );
    lua_remove( L, -4 );  // Remove the umsg library
    int result = lua_pcall( L, 2, 0, 0 );

    if ( result != 0 )
    {
        Warning( "[Lua] umsg.umsg_CallOnMessageReceived: %s\n", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
    }
}

static void umsg_HandleReceiveMessage( bf_read &msg )
{
    char name[2048];
    msg.ReadString( name, sizeof( name ) );
    umsg_CallOnMessageReceived( L, name, &msg );
}

#else

static bool isMessageQueued = false;
static bf_write queuedMessageBuffer;
static CRecipientFilter queuedRecipientFilter;
static int luaMessageType = 0;
byte byteBuffer[PAD_NUMBER( MAX_USER_MSG_DATA, 4 )];

static int umsg_Start( lua_State *L )
{
    if ( isMessageQueued )
    {
        Warning( "umsg.Start called with an active message! Resetting that message.\n" );
    }

    const char *messageName = luaL_checkstring( L, 1 );

    isMessageQueued = true;

    queuedMessageBuffer.StartWriting( byteBuffer, sizeof( byteBuffer ) );
    queuedMessageBuffer.Reset();

    // If the filter is nil or empty, create one with all players
    if ( lua_isnoneornil( L, 2 ) )
    {
        queuedRecipientFilter.AddAllPlayers();
    }
    else
    {
        queuedRecipientFilter = lua_torecipientfilter( L, 2 );
    }

    queuedRecipientFilter.MakeReliable();

    // Write the message name the user wants to send
    queuedMessageBuffer.WriteString( messageName );

    return 0;
}

static int umsg_MessageEnd( lua_State *L )
{
    if ( !isMessageQueued )
    {
        Warning( "[Lua] umsg.MessageEnd called with no active message\n" );
        return 0;
    }

    isMessageQueued = false;

    // We have to create and send the message in one go, or we'll get a
    // access violation in the engine (from  othermessages getting in the way)
    bf_write *sendBuffer = engine->UserMessageBegin( &queuedRecipientFilter, luaMessageType );
    sendBuffer->Reset();

    // Copy the prepared buffer over to the user message send buffer
    sendBuffer->WriteBits( queuedMessageBuffer.GetData(), queuedMessageBuffer.GetNumBitsWritten() );

    engine->MessageEnd();

    queuedRecipientFilter.Reset();

    return 0;
}

static int umsg_WriteByte( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteByte called with no active message\n" );

    queuedMessageBuffer.WriteByte( luaL_checkint( L, 1 ) );
    return 0;
}

static int umsg_WriteChar( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteChar called with no active message\n" );

    queuedMessageBuffer.WriteChar( luaL_checkint( L, 1 ) );
    return 0;
}

static int umsg_WriteShort( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteShort called with no active message\n" );

    queuedMessageBuffer.WriteShort( luaL_checkint( L, 1 ) );

    return 0;
}

static int umsg_WriteWord( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteWord called with no active message\n" );

    queuedMessageBuffer.WriteWord( luaL_checkint( L, 1 ) );

    return 0;
}

static int umsg_WriteLong( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteLong called with no active message\n" );

    queuedMessageBuffer.WriteLong( luaL_checkint( L, 1 ) );

    return 0;
}

static int umsg_WriteFloat( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteFloat called with no active message\n" );

    queuedMessageBuffer.WriteFloat( luaL_checknumber( L, 1 ) );

    return 0;
}

static int umsg_WriteVector( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteVector called with no active message\n" );

    queuedMessageBuffer.WriteBitVec3Coord( luaL_checkvector( L, 1 ) );

    return 0;
}

static int umsg_WriteNormal( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteNormal called with no active message\n" );

    queuedMessageBuffer.WriteBitVec3Normal( luaL_checkvector( L, 1 ) );

    return 0;
}

static int umsg_WriteAngle( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteAngle called with no active message\n" );

    queuedMessageBuffer.WriteBitAngles( luaL_checkangle( L, 1 ) );

    return 0;
}

static int umsg_WriteString( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteString called with no active message\n" );

    queuedMessageBuffer.WriteString( luaL_checkstring( L, 1 ) );

    return 0;
}

static int umsg_WriteEntity( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteEHandle called with no active message\n" );

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

    queuedMessageBuffer.WriteLong( iEncodedEHandle );

    return 0;
}

static int umsg_WriteBool( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteBool called with no active message\n" );

    queuedMessageBuffer.WriteOneBit( lua_toboolean( L, 1 ) ? 1 : 0 );

    return 0;
}

static int umsg_WriteUBitLong( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteUBitLong called with no active message\n" );

    queuedMessageBuffer.WriteUBitLong( luaL_checkint( L, 1 ), luaL_checkint( L, 2 ) );

    return 0;
}

static int umsg_WriteSBitLong( lua_State *L )
{
    if ( !isMessageQueued )
        Warning( "[Lua] umsg.WriteSBitLong called with no active message\n" );

    queuedMessageBuffer.WriteSBitLong( luaL_checkint( L, 1 ), luaL_checkint( L, 2 ) );

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
    usermessages->Register( "_LuaMessage", -1 );

#ifdef CLIENT_DLL
    usermessages->HookMessage( "_LuaMessage", umsg_HandleReceiveMessage );
#else
    luaMessageType = usermessages->LookupUserMessage( "_LuaMessage" );
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
    lua_pushlightuserdata( L, message );
    luaL_getmetatable( L, LUA_BFREADLIBNAME );
    lua_setmetatable( L, -2 );
}

LUALIB_API bf_read *luaL_checkbf_read( lua_State *L, int narg )
{
    bf_read *bfRead = ( bf_read * )luaL_checkudata( L, narg, LUA_BFREADLIBNAME );
    if ( !bfRead )
    {
        luaL_typerror( L, narg, LUA_BFREADLIBNAME );
    }

    return bfRead;
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

static int bfRead___index( lua_State *L )
{
    luaL_getmetatable( L, LUA_BFREADLIBNAME );
    lua_pushvalue( L, 2 );
    lua_gettable( L, -2 );

    if ( lua_isnil( L, -1 ) )
    {
        lua_pop( L, 2 );
        lua_pushvalue( L, 2 );
        lua_gettable( L, 1 );
    }

    return 1;
}

static int bfRead___tostring( lua_State *L )
{
    lua_pushfstring( L, "bf_read: %p", luaL_checkbf_read( L, 1 ) );
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
    { "__index", bfRead___index },
    { "__tostring", bfRead___tostring },
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

#ifdef CLIENT_DLL
LUALIB_API int luaopen_bf_read( lua_State *L )
{
    luaL_newmetatable( L, LUA_BFREADLIBNAME );
    luaL_register( L, NULL, bfReadLib );
    lua_pushstring( L, "bf_read" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "entity" */
    lua_pop( L, 1 );
    return 1;
}
#endif
