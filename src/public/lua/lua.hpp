// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

/*
 * Compatibility with Lua 5.1
 */

#ifndef LUA_HPP_COMPAT_LOADED
#define LUA_HPP_COMPAT_LOADED

#define LUA_QL(x)	"'" x "'"
#define LUA_QS		LUA_QL("%s")

lua_State *lua_open() {
    return luaL_newstate();
}

void luaL_register(lua_State *L, const char *libname,
                          const luaL_Reg *l) {
    if (libname) {
        // Create new table for the library
        luaL_newlib(L, l);
        // Set the table as a global variable
        lua_setglobal(L, libname);
    } else {
        // Register functions into the existing table on the stack
        luaL_setfuncs(L, l, 0);
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

#endif // LUA_HPP_COMPAT_LOADED
