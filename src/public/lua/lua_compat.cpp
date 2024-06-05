/*
 * Compatibility with Lua 5.1
 */

#include "cbase.h"
#include "lua.hpp"

lua_State *lua_open() {
    return luaL_newstate();
}

void luaL_register(lua_State *L, const char *libname, const luaL_Reg *l) {
    if (libname) {
        luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
        lua_getfield(L, -1, libname);  // get package.loaded[libname]
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);  // remove previous result
            lua_newtable(L);
            lua_pushvalue(L, -1);
            lua_setfield(L, -3,
                         libname);  // package.loaded[libname] = new table
        }
        lua_remove(L, -2);  // remove package.loaded
        lua_pushvalue(L, -1);
        lua_setglobal(L, libname);  // _G[libname] = new table
    }

    for (; l->name != NULL; l++) {
        lua_pushcfunction(L, l->func);
        lua_setfield(L, -2, l->name);
    }
}

int luaL_typerror(lua_State *L, int narg, const char *tname) {
    const char *msg = lua_pushfstring(L, "%s expected, got %s", tname,
                                      luaL_typename(L, narg));
    return luaL_argerror(L, narg, msg);
}

#define luaL_checkint(L, n) ((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L, n, d) ((int)luaL_optinteger(L, (n), (d)))
#define luaL_checklong(L, n) ((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L, n, d) ((long)luaL_optinteger(L, (n), (d)))

void *luaL_checkudata(lua_State *L, int ud, const char *tname) {
    void *p = lua_touserdata(L, ud);
    if (p == NULL)
        luaL_typerror(L, ud, tname);
    return p;
}

const char *luaL_optlstring(lua_State *L, int numArg, const char *def,
                            size_t *l) {
    if (lua_isnoneornil(L, numArg)) {
        if (l)
            *l = (def ? strlen(def) : 0);
        return def;
    }
    return lua_tolstring(L, numArg, l);
}

void lua_getref(lua_State *L, int ref) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
}

void lua_unref(lua_State *L, int ref) {
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
}
