#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "tier3/tier3.h"
#include "base64.h"

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

LUA_BINDING_BEGIN( Serializers, Crc32, "library", "Compute the CRC32 of a string" )
{
    const char *pszString = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "string" );

    lua_pushinteger( L, CRC32_ProcessSingleBuffer( pszString, Q_strlen( pszString ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "CRC32 of the string." )

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

LUALIB_API int luaopen_Serializers( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Serializers );
    return 1;
}
