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

#ifndef LUA_COMPAT_LOADED_HPP
#define LUA_COMPAT_LOADED_HPP
#ifdef _WIN32
#pragma once
#endif

#define LUA_ENAME   "_E"
#define LUA_RNAME   "_R"

#define LUA_QL(x)	"'" x "'"
#define LUA_QS		LUA_QL("%s")

#define luaL_checkint(L, n) ((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L, n, d) ((int)luaL_optinteger(L, (n), (d)))
#define luaL_checklong(L, n) ((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L, n, d) ((long)luaL_optinteger(L, (n), (d)))

lua_State *lua_open();

void luaL_register(lua_State *L, const char *libname, const luaL_Reg *l);

int luaL_typerror(lua_State *L, int narg, const char *tname);

void *luaL_checkudata(lua_State *L, int ud, const char *tname);

const char *luaL_optlstring(lua_State *L, int numArg, const char *def, size_t *l);

void lua_getref(lua_State *L, int ref);

void lua_unref(lua_State *L, int ref);

bool lua_isrefvalid(lua_State *L, int ref);

// Copied from https://github.com/lua/lua/blob/bdc85357aa41a9610498232c2cffe7aa191e5cf6/lbaselib.c#L432
// since they're static in the original file
int luaB_error(lua_State *L);
int luaB_ipairs(lua_State *L);
int luaB_next(lua_State *L);
int luaB_pairs(lua_State *L);
int luaB_select(lua_State *L);
int luaB_tonumber(lua_State *L);
int luaB_tostring(lua_State *L);
int luaB_type(lua_State *L);

#endif // LUA_COMPAT_LOADED_HPP
