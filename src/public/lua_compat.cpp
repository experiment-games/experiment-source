/*
 * Compatibility with Lua 5.1
 */

#include "cbase.h"
#include "lua.hpp"

lua_State *lua_open()
{
    return luaL_newstate();
}

void luaL_register( lua_State *L, const char *libname, const luaL_Reg *l )
{
    if ( libname )
    {
        luaL_getsubtable( L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE );
        lua_getfield( L, -1, libname );  // get package.loaded[libname]
        if ( !lua_istable( L, -1 ) )
        {
            lua_pop( L, 1 );  // remove previous result
            lua_newtable( L );
            lua_pushvalue( L, -1 );
            lua_setfield( L, -3,
                        libname );  // package.loaded[libname] = new table
        }
        lua_remove( L, -2 );  // remove package.loaded
        lua_pushvalue( L, -1 );
        lua_setglobal( L, libname );  // _G[libname] = new table
    }

    for ( ; l->name != NULL; l++ )
    {
        lua_pushcfunction( L, l->func );
        lua_setfield( L, -2, l->name );
    }
}

int luaL_typerror( lua_State *L, int narg, const char *tname )
{
    const char *msg = lua_pushfstring( L, "%s expected, got %s", tname, luaL_typename( L, narg ) );
    return luaL_argerror( L, narg, msg );
}

void lua_getref( lua_State *L, int ref )
{
    lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
}

void lua_unref( lua_State *L, int ref )
{
    luaL_unref( L, LUA_REGISTRYINDEX, ref );
}

bool lua_isrefvalid( lua_State *L, int ref )
{
    // ref being 0 indicates we forgot to set `m_nTableReference = LUA_NOREF`
    // in a constructor somewhere.
    Assert( ref != 0 );

    return ref != LUA_REFNIL && ref != LUA_NOREF;
}
