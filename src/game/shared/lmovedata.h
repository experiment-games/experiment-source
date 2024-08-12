#ifndef LMOVEDATA_H
#define LMOVEDATA_H

#ifdef _WIN32
#pragma once
#endif


/* type for CMoveData functions */
typedef CMoveData lua_CMoveData;

/*
** access functions (stack -> C)
*/

LUA_API lua_CMoveData             *(lua_tomovedata) (lua_State *L, int idx);

/*
** push functions (C -> stack)
*/
LUA_API void  (lua_pushmovedata) (lua_State *L, lua_CMoveData *pHelper);

LUALIB_API lua_CMoveData *(luaL_checkmovedata) (lua_State *L, int narg);


#endif // LMOVEDATA_H
