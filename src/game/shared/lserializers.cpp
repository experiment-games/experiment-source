#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "tier3/tier3.h"
#include "base64.h"
#include "lzma/lzma.h"
#include <lzmaDecoder.h>
#include "utlbuffer.h"

#include "tier0/valve_minmax_off.h"
// This is wrapped by minmax_off due to Valve making a macro for min and max...
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
// Now we can unwrap
#include "tier0/valve_minmax_on.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace rapidjson;

LUA_REGISTRATION_INIT( Serializers )

//
// CRC32
//

LUA_BINDING_BEGIN( Serializers, Crc32, "library", "Compute the CRC32 of a string" )
{
    const char *pszString = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "string" );

    lua_pushinteger( L, CRC32_ProcessSingleBuffer( pszString, Q_strlen( pszString ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "CRC32 of the string." )

//
// Base64
//

LUA_BINDING_BEGIN( Serializers, Base64Encode, "library", "Encode a string to base64" )
{
    const char *pszString = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "string" );
    bool bDisableRfc2045Compliance = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "disableRfc2045Compliance" );

    std::string encodedString;

    if ( bDisableRfc2045Compliance )
        encodedString = base64_encode( pszString, false );
    else
        encodedString = base64_encode_mime( pszString );

    lua_pushstring( L, encodedString.c_str() );

    return 1;
}
LUA_BINDING_END( "string", "Base64 encoded string." )

LUA_BINDING_BEGIN( Serializers, Base64Decode, "library", "Decode a base64 encoded string" )
{
    const char *pszString = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "string" );
    std::string decodedString = base64_decode( pszString );

    lua_pushstring( L, decodedString.c_str() );

    return 1;
}
LUA_BINDING_END( "string", "Decoded string." )

//
// JSON
//

// TODO: Test and expand this for all datatypes.
static void WriteLuaTableToJson( lua_State *L, PrettyWriter< StringBuffer > &writer, int index )
{
    lua_pushnil( L );

    while ( lua_next( L, index ) != 0 )
    {
        if ( lua_type( L, -2 ) == LUA_TSTRING )
        {
            writer.Key( lua_tostring( L, -2 ) );
        }
        else
        {
            writer.Key( "" );
        }

        switch ( lua_type( L, -1 ) )
        {
            case LUA_TNUMBER:
                writer.Double( lua_tonumber( L, -1 ) );
                break;
            case LUA_TSTRING:
                writer.String( lua_tostring( L, -1 ) );
                break;
            case LUA_TBOOLEAN:
                writer.Bool( lua_toboolean( L, -1 ) );
                break;
            case LUA_TTABLE:
                writer.StartObject();
                WriteLuaTableToJson( L, writer, lua_gettop( L ) );
                writer.EndObject();
                break;
            default:
                DevWarning( "Unsupported type %s\n", lua_typename( L, lua_type( L, -1 ) ) );
                writer.Null();
                break;
        }

        lua_pop( L, 1 );  // Pop the value, keep the key
    }
}

LUA_BINDING_BEGIN( Serializers, JsonEncode, "library", "Encode a lua table to JSON" )
{
    luaL_checktype( L, 1, LUA_TTABLE );

    StringBuffer buffer;
    buffer.Clear();
    PrettyWriter< StringBuffer > writer( buffer );

    WriteLuaTableToJson( L, writer, 1 );

    lua_pushstring( L, buffer.GetString() );
    return 1;
}
LUA_BINDING_END( "string", "JSON encoded string." )

//
// LZMA
//

LUA_BINDING_BEGIN( Serializers, LzmaCompress, "library", "Compress a string using LZMA" )
{
    const char *inputString = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "string" );
    unsigned int outCompressedSize;
    CUtlBuffer inputBuffer = CUtlBuffer( inputString, Q_strlen( inputString ), CUtlBuffer::READ_ONLY );

    unsigned char *compressedString = LZMA_Compress(
        ( unsigned char * )inputBuffer.Base(),
        inputBuffer.TellMaxPut(),
        &outCompressedSize );

    lua_pushlstring( L, ( char * )compressedString, outCompressedSize );
    delete[] compressedString;
    return 1;
}
LUA_BINDING_END( "string", "Compressed string." )

LUA_BINDING_BEGIN( Serializers, LzmaDecompress, "library", "Decompress a string using LZMA" )
{
    const char *compressedString = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "compressedString" );

    if ( !LZMA_IsCompressed( ( unsigned char * )compressedString ) )
    {
        luaL_argerror( L, 2, "Our LZMA header is missing: Cannot decompress the string if is not compressed through this library. Prepend the length of the uncompressed data to its compressed form as an 8-byte little endian integer." );
        return 0;
    }

    unsigned char *outputString;
    unsigned int outUncompressedSize;
    bool success = LZMA_Uncompress(
        ( unsigned char * )compressedString,
        &outputString,
        &outUncompressedSize );

    if ( !success )
    {
        delete[] outputString;
        luaL_error( L, "Failed to decompress string." );
    }

    // Note:    If you set a breakpoint here and inspect outputString, you may find that additional
    //          junk data is present at the end of the string. This is because the LZMA decompression
    //          algorithm does not null-terminate the output string. This is not a bug, since we now
    //          only push the length of the decompressed string to Lua, and Lua strings are null-terminated.
    lua_pushlstring( L, ( char * )outputString, outUncompressedSize );
    delete[] outputString;
    return 1;
}
LUA_BINDING_END( "string", "Decompressed string." )

//
// Open the library
//

LUALIB_API int luaopen_Serializers( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Serializers );
    return 1;
}
