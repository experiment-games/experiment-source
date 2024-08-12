#ifndef LUSERCMD_H
#define LUSERCMD_H

#ifdef _WIN32
#pragma once
#endif

/* type for CUserCmd functions */
typedef CUserCmd lua_CUserCmd;

/*
** access functions (stack -> C)
*/

LUA_API lua_CUserCmd             *(lua_tousercmd) (lua_State *L, int idx);

/*
** push functions (C -> stack)
*/
LUA_API void  (lua_pushusercmd) (lua_State *L, lua_CUserCmd *pHelper);

LUALIB_API lua_CUserCmd *(luaL_checkusercmd) (lua_State *L, int narg);

#endif // LUSERCMD_H
