#ifndef LCONVAR_H
#define LCONVAR_H

#if _WIN32
#pragma once
#endif

/* type for ConVar functions */
typedef ConVar lua_ConVar;

/*
** access functions (stack -> C)
*/

LUA_API lua_ConVar *( lua_toconvar )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushconvar )( lua_State *L, lua_ConVar *pConVar );

LUALIB_API lua_ConVar *( luaL_checkconvar )( lua_State *L, int narg );

void ResetConVarDatabase( void );

#endif  // LCONVAR_H
