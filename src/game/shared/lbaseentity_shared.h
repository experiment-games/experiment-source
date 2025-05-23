#ifndef LBASEENTITY_SHARED_H
#define LBASEENTITY_SHARED_H
#ifdef _WIN32
#pragma once
#endif

/* type for CBaseEntity functions */
typedef CBaseEntity lua_CBaseEntity;

/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseEntity *( lua_toentity )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CBaseEntity *( luaL_checkentity )( lua_State *L, int narg );
LUALIB_API lua_CBaseEntity *( luaL_optentity )( lua_State *L, int narg, lua_CBaseEntity *def );

#endif  // LBASEENTITY_SHARED_H
