#include "cbase.h"
#include <lsingleluainstance.h>

/* Resolve actual userdata instance we've stored. */
void lua_pushuserdatainstance( lua_State *L, void *p )
{
    luaL_getmetatable( L, PUDATA_RIDX );
    lua_pushlightuserdata( L, p );
    lua_rawget( L, -2 );
    lua_replace( L, -2 );
}

/* Associate userdata at top of stack with p. */
void lua_pushuserdata_associateinstance( lua_State *L, void *p )
{
    if ( luaL_newmetatable( L, PUDATA_RIDX ) )
    { /* Create if missing. */
        lua_pushvalue( L, -1 );
        lua_setmetatable( L, -2 ); /* Point to self. */
        lua_pushliteral( L, "v" );
        lua_setfield( L, -2, "__mode" ); /* Weak values (udata). */
    }
    lua_pushlightuserdata( L, p ); /* Key. */
    lua_pushvalue( L, -3 );        /* Value. */
    lua_rawset( L, -3 );           /* Associate. */
    lua_pop( L, 1 );               /* Pop metatable. */
}

/* Creates and pushes new user data like normally, except it now stores the instance
in the registry for later retrieval. */
void *lua_newuserdatainstance( lua_State *L, size_t sz )
{
    void *p = lua_newuserdata( L, sz );
    lua_pushuserdata_associateinstance( L, p );
    return p;
}
