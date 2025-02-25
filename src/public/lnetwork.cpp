#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "linetchannelinfo.h"
#include "mathlib/lvector.h"
#include "usermessages.h"
#include "lnetwork.h"
#include "networksystem/isockets.h"
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

LUA_REGISTRATION_INIT( UserMessages );

#ifdef CLIENT_DLL

/// <summary>
/// Calls UserMessages.OnMessageReceived( messageName, message )
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
    lua_remove( L, -4 );  // Remove the UserMessages library
    int result = lua_pcall( L, 2, 0, 0 );

    if ( result != 0 )
    {
        Warning( "[Lua] UserMessages.CallOnMessageReceived: %s\n", lua_tostring( L, -1 ) );
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

LUA_BINDING_BEGIN( UserMessages, Start, "library", "Starts a user message.", "server" )
{
    // Check if a message is already queued
    if ( isMessageQueued )
    {
        Warning( "UserMessages.Start called with an active message! Resetting that message.\n" );
    }

    const char *messageName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "messageName" );

    isMessageQueued = true;

    queuedMessageBuffer.StartWriting( byteBuffer, sizeof( byteBuffer ) );
    queuedMessageBuffer.Reset();

    // Check if the filter is nil or empty, then add all players
    if ( lua_isnoneornil( L, 2 ) )
    {
        queuedRecipientFilter.AddAllPlayers();
    }
    else
    {
        queuedRecipientFilter = LUA_BINDING_ARGUMENT_NILLABLE( lua_torecipientfilter, 2, "recipientFilter" );
    }

    queuedRecipientFilter.MakeReliable();

    // Write the message name the user wants to send
    queuedMessageBuffer.WriteString( messageName );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, MessageEnd, "library", "Ends a user message.", "server" )
{
    if ( !isMessageQueued )
    {
        Warning( "[Lua] UserMessages.MessageEnd called with no active message\n" );
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
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteByte, "library", "Writes a byte.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteByte called with no active message\n" );

    queuedMessageBuffer.WriteByte( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "byte" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteChar, "library", "Writes a char.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteChar called with no active message\n" );

    queuedMessageBuffer.WriteChar( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "char" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteShort, "library", "Writes a short integer.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteShort called with no active message\n" );

    queuedMessageBuffer.WriteShort( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "short" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteWord, "library", "Writes a word.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteWord called with no active message\n" );

    queuedMessageBuffer.WriteWord( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "word" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteLong, "library", "Writes a long integer.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteLong called with no active message\n" );

    queuedMessageBuffer.WriteLong( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "long" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteFloat, "library", "Writes a float.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteFloat called with no active message\n" );

    queuedMessageBuffer.WriteFloat( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "float" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteVector, "library", "Writes a vector.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteVector called with no active message\n" );

    queuedMessageBuffer.WriteBitVec3Coord( LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteNormal, "library", "Writes a normal vector.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteNormal called with no active message\n" );

    queuedMessageBuffer.WriteBitVec3Normal( LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "normal" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteAngle, "library", "Writes an angle.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteAngle called with no active message\n" );

    queuedMessageBuffer.WriteBitAngles( LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteString, "library", "Writes a string.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteString called with no active message\n" );

    queuedMessageBuffer.WriteString( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "string" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteEntity, "library", "Writes an entity.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteEntity called with no active message\n" );

    long iEncodedEHandle;

    if ( LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" ) )
    {
        EHANDLE hEnt = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );

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
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteBool, "library", "Writes a boolean.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteBool called with no active message\n" );

    queuedMessageBuffer.WriteOneBit( LUA_BINDING_ARGUMENT( lua_toboolean, 1, "boolean" ) ? 1 : 0 );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteUInt, "library", "Writes an unsigned bit long.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteUInt called with no active message\n" );

    queuedMessageBuffer.WriteUBitLong( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "value" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "numBits" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserMessages, WriteSBitLong, "library", "Writes a signed bit long.", "server" )
{
    if ( !isMessageQueued )
        Warning( "[Lua] UserMessages.WriteSBitLong called with no active message\n" );

    queuedMessageBuffer.WriteSBitLong( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "value" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "numBits" ) );
    return 0;
}
LUA_BINDING_END()

#endif

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
** bf_read methods
*/
LUA_REGISTRATION_INIT( MessageReader );

LUA_API bf_read &lua_tobf_read( lua_State *L, int idx )
{
    bf_read *bfRead = ( bf_read * )lua_touserdata( L, idx );
    return *bfRead;
}

LUA_API void lua_pushbf_read( lua_State *L, bf_read *message )
{
    lua_pushlightuserdata( L, message );
    LUA_SAFE_SET_METATABLE( L, LUA_MESSAGEREADERMETANAME );
}

LUALIB_API bf_read *luaL_checkbf_read( lua_State *L, int narg )
{
    bf_read *bfRead = ( bf_read * )luaL_checkudata( L, narg, LUA_MESSAGEREADERMETANAME );
    if ( !bfRead )
    {
        luaL_typerror( L, narg, LUA_MESSAGEREADERMETANAME );
    }

    return bfRead;
}

LUA_BINDING_BEGIN( MessageReader, GetNumBytesLeft, "class", "The bytes left to read." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->GetNumBytesLeft() );
    return 1;
}
LUA_BINDING_END( "integer", "The bytes left to read." )

LUA_BINDING_BEGIN( MessageReader, GetNumBitsLeft, "class", "The bits left to read." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->GetNumBitsLeft() );
    return 1;
}
LUA_BINDING_END( "integer", "The bits left to read." )

LUA_BINDING_BEGIN( MessageReader, GetNumBytesRead, "class", "The bytes read." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->GetNumBytesRead() );
    return 1;
}
LUA_BINDING_END( "integer", "The bytes read." )

LUA_BINDING_BEGIN( MessageReader, GetNumBitsRead, "class", "The bits read." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->GetNumBitsRead() );
    return 1;
}
LUA_BINDING_END( "integer", "The bits read." )

LUA_BINDING_BEGIN( MessageReader, ReadData, "class", "Reads binary data, represented in Lua as a string" )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    int len = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "length" );
    char *buf = new char[len];
    bf->ReadBytes( buf, len );
    lua_pushlstring( L, buf, len );
    delete[] buf;
    return 1;
}
LUA_BINDING_END( "string", "The data read." )

LUA_BINDING_BEGIN( MessageReader, ReadBit, "class", "Reads a bit." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->ReadOneBit() );
    return 1;
}
LUA_BINDING_END( "integer", "The bit read." )

LUA_BINDING_BEGIN( MessageReader, ReadBool, "class", "Reads a boolean." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushboolean( L, bf->ReadOneBit() == 1 );
    return 1;
}
LUA_BINDING_END( "boolean", "The boolean read." )

LUA_BINDING_BEGIN( MessageReader, ReadByte, "class", "Reads a byte." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->ReadByte() );
    return 1;
}
LUA_BINDING_END( "integer", "The byte read." )

LUA_BINDING_BEGIN( MessageReader, ReadChar, "class", "Reads a char." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->ReadChar() );
    return 1;
}
LUA_BINDING_END( "integer", "The char read." )

LUA_BINDING_BEGIN( MessageReader, ReadShort, "class", "Reads a short integer." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->ReadShort() );
    return 1;
}
LUA_BINDING_END( "integer", "The short read." )

LUA_BINDING_BEGIN( MessageReader, ReadWord, "class", "Reads a word." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->ReadWord() );
    return 1;
}
LUA_BINDING_END( "integer", "The word read." )

LUA_BINDING_BEGIN( MessageReader, ReadLong, "class", "Reads a long integer." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L,
                    bf->ReadBitLong(
                        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "numBits" ),
                        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, true, "isSigned" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The long read." )

LUA_BINDING_BEGIN( MessageReader, ReadFloat, "class", "Reads a float." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushnumber( L, bf->ReadBitFloat() );
    return 1;
}
LUA_BINDING_END( "number", "The float read." )

LUA_BINDING_BEGIN( MessageReader, ReadVector, "class", "Reads a vector." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    Vector vec;
    bf->ReadBitVec3Coord( vec );
    lua_pushvector( L, vec );
    return 1;
}
LUA_BINDING_END( "Vector", "The vector read." )

LUA_BINDING_BEGIN( MessageReader, ReadVectorNormal, "class", "Reads a normal vector." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    Vector vec;
    bf->ReadBitVec3Normal( vec );
    lua_pushvector( L, vec );
    return 1;
}
LUA_BINDING_END( "Vector", "The normal vector read." )

LUA_BINDING_BEGIN( MessageReader, ReadAngle, "class", "Reads an angle." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    QAngle ang;
    bf->ReadBitAngles( ang );
    lua_pushangle( L, ang );
    return 1;
}
LUA_BINDING_END( "Angle", "The angle read." )

LUA_BINDING_BEGIN( MessageReader, ReadUInt, "class", "Reads an unsigned integer" )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    lua_pushinteger( L, bf->ReadUBitLong( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "numBits" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The integer read." )

LUA_BINDING_BEGIN( MessageReader, ReadString, "class", "Reads a string." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    char str[2048];
    bf->ReadString( str, sizeof( str ) );
    lua_pushstring( L, str );
    return 1;
}
LUA_BINDING_END( "string", "The string read." )

LUA_BINDING_BEGIN( MessageReader, ReadBytes, "class", "Reads a number of bytes." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    int len = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "length" );
    char *buf = new char[len];
    bf->ReadBytes( buf, len );
    lua_pushlstring( L, buf, len );
    delete[] buf;
    return 1;
}
LUA_BINDING_END( "string", "The bytes read." )

LUA_BINDING_BEGIN( MessageReader, ReadEntity, "class", "Reads an entity." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
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

    CBaseEntity::PushLuaInstanceSafe( L, pEntity );

    return 1;
}
LUA_BINDING_END( "Entity", "The entity read." )

LUA_BINDING_BEGIN( MessageReader, ReadBitLong, "class", "Reads an signed/unsigned number of the specified bits long." )
{
    bf_read *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    bool isSigned = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, true, "isSigned" );

    if ( isSigned )
    {
        lua_pushinteger( L, bf->ReadSBitLong( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "numBits" ) ) );
    }
    else
    {
        lua_pushinteger( L, bf->ReadUBitLong( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "numBits" ) ) );
    }

    return 1;
}
LUA_BINDING_END( "integer", "The number read." )

LUA_BINDING_BEGIN( MessageReader, __index, "class", "Returns nil." )
{
    bf_read *MessageReader = LUA_BINDING_ARGUMENT( luaL_checkbf_read, 1, "reader" );
    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageReader, __tostring, "class", "Returns a string representation of the reader." )
{
    lua_pushfstring( L, "MessageReader: %p", LUA_BINDING_ARGUMENT( lua_tobf_read, 1, "reader" ) );
    return 1;
}
LUA_BINDING_END()

/*
** bf_write methods
*/
LUA_REGISTRATION_INIT( MessageWriter );

LUA_API bf_write &lua_tobf_write( lua_State *L, int idx )
{
    bf_write *bfWrite = ( bf_write * )lua_touserdata( L, idx );
    return *bfWrite;
}

LUA_API void lua_pushbf_write( lua_State *L, bf_write *message )
{
    lua_pushlightuserdata( L, message );
    LUA_SAFE_SET_METATABLE( L, LUA_MESSAGEWRITERMETANAME );
}

LUALIB_API bf_write *luaL_checkbf_write( lua_State *L, int narg )
{
    bf_write *bfWrite = ( bf_write * )luaL_checkudata( L, narg, LUA_MESSAGEWRITERMETANAME );
    if ( !bfWrite )
    {
        luaL_typerror( L, narg, LUA_MESSAGEWRITERMETANAME );
    }

    return bfWrite;
}

LUA_BINDING_BEGIN( MessageWriter, WriteBit, "class", "Writes a bit." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteOneBit( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bit" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteByte, "class", "Writes a byte." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteByte( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "byte" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteChar, "class", "Writes a char." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteChar( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "char" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteShort, "class", "Writes a short integer." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteShort( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "short" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteWord, "class", "Writes a word." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteWord( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "word" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteLong, "class", "Writes a long integer." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteBitLong(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "long" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "numBits" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, true, "isSigned" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteFloat, "class", "Writes a float." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteBitFloat( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "float" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteVector, "class", "Writes a vector." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteBitVec3Coord( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteVectorNormal, "class", "Writes a normal vector." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteBitVec3Normal( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "normal" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteAngle, "class", "Writes an angle." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteBitAngles( LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteString, "class", "Writes a string." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteString( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "string" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteEntity, "class", "Writes an entity." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );

    CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" );

    if ( pEntity )
    {
        bf->WriteLong( pEntity->GetRefEHandle().ToInt() );
    }
    else
    {
        bf->WriteLong( INVALID_NETWORKED_EHANDLE_VALUE );
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteData, "class", "Writes binary data, represented in Lua as a string" )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    size_t len;
    const char *data = luaL_checklstring( L, 2, &len );
    bf->WriteBytes( data, len );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteBool, "class", "Writes a boolean." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bf->WriteOneBit( LUA_BINDING_ARGUMENT( lua_toboolean, 2, "boolean" ) ? 1 : 0 );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, WriteBitLong, "class", "Writes an signed/unsigned number of the specified bits long." )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    bool isSigned = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, true, "isSigned" );

    if ( isSigned )
    {
        bf->WriteSBitLong(
            LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "value" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "numBits" ) );
    }
    else
    {
        bf->WriteUBitLong(
            LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "value" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "numBits" ) );
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, __index, "class", "Returns nil." )
{
    bf_write *MessageWriter = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, __gc, "class", "Cleans up the buffer belonging to this writer" )
{
    bf_write *bf = LUA_BINDING_ARGUMENT( luaL_checkbf_write, 1, "writer" );
    free( bf->GetBasePointer() );
    free( bf );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MessageWriter, __tostring, "class", "Returns a string representation of the writer." )
{
    lua_pushfstring( L, "MessageWriter: %p", LUA_BINDING_ARGUMENT( lua_tobf_write, 1, "writer" ) );
    return 1;
}
LUA_BINDING_END()

LUA_REGISTRATION_INIT( MessageWriters )

LUA_BINDING_BEGIN( MessageWriters, Create, "library", "Creates a new MessageWriter for writing network messages" )
{
    void *buffer = malloc( NETWORK_MAX_MESSAGE_LENGTH );
    bf_write *writer = new bf_write( buffer, NETWORK_MAX_MESSAGE_LENGTH );
    lua_pushbf_write( L, writer );
    return 1;
}
LUA_BINDING_END( "MessageWriter", "The new MessageWriter" )

/*
** Open networking libraries
*/
LUALIB_API int luaopen_UserMessages( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( UserMessages );
    return 1;
}

LUALIB_API int luaopen_bf_read( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_MESSAGEREADERMETANAME );

    LUA_REGISTRATION_COMMIT( MessageReader );

    lua_pushstring( L, LUA_MESSAGEREADERMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "MessageReader" */
    lua_pop( L, 1 );

    return 1;
}

LUALIB_API int luaopen_bf_write( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_MESSAGEWRITERMETANAME );

    LUA_REGISTRATION_COMMIT( MessageWriter );

    lua_pushstring( L, LUA_MESSAGEWRITERMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "MessageWriter" */
    lua_pop( L, 1 );

    LUA_REGISTRATION_COMMIT_LIBRARY( MessageWriters );

    return 1;
}
