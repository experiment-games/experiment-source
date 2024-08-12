#ifndef LVMATRIX_H
#define LVMATRIX_H

#ifdef _WIN32
#pragma once
#endif

/* type for VMatrix functions */
typedef VMatrix lua_VMatrix;

/*
** access functions (stack -> C)
*/

LUA_API lua_VMatrix &( lua_tovmatrix )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushvmatrix )( lua_State *L, lua_VMatrix &matrix );

LUALIB_API lua_VMatrix &( luaL_checkvmatrix )( lua_State *L, int narg );

#endif
