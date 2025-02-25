#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "lgametrace.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Utilities )

// Experiment; disabled as this is not useful to Lua
// static int luasrc_FStrEq( lua_State *L )
//{
//    lua_pushboolean( L, FStrEq( luaL_checkstring( L, 1 ), luaL_checkstring( L, 2 ) ) );
//    return 1;
//}

LUA_BINDING_BEGIN( Utilities, PrecacheDecal, "library", "Precaches a decal. If preload is specified, the file is loaded into the server/client's cache memory before level startup, otherwise it'll only load when actually used (which can cause a disk i/o hitch if it occurs during play of a level)." )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    bool preload = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "shouldPreload" );

    lua_pushinteger( L, UTIL_PrecacheDecal( name, preload ) );

    return 1;
}
LUA_BINDING_END( "integer", "The decal index." )

LUA_BINDING_BEGIN( Utilities, FindWaterSurface, "library", "Finds the water surface at the given position." )
{
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "position" );
    float minz = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "minZ" );
    float maxz = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxZ" );

    lua_pushnumber( L, UTIL_FindWaterSurface( position, minz, maxz ) );

    return 1;
}
LUA_BINDING_END( "number", "The water surface z position." )

LUA_BINDING_BEGIN( Utilities, ValidateSoundName, "library", "Validates a sound name." )
{
    string_t name = AllocPooledString( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" ) );
    const char *defaultStr = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "default" );

    UTIL_ValidateSoundName( name, defaultStr );

    lua_pushstring( L, STRING( name ) );

    return 1;
}
LUA_BINDING_END( "string", "The validated sound name." )

LUALIB_API int luaopen_Util( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Utilities );

    return 1;
}
