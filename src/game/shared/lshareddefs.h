#ifndef LSHAREDDEFS_H
#define LSHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif

#include "lbaseentity_shared.h"

/* type for FireBulletsInfo_t in Lua */
typedef FireBulletsInfo_t lua_FireBulletsInfo_t;

/* type for EmitSound_t in Lua */
typedef EmitSound_t lua_EmitSound_t;

/*
** access functions (stack -> C)
*/

LUA_API lua_FireBulletsInfo_t( lua_tofirebulletsinfo )( lua_State *L, int argIndex, lua_CBaseEntity *self );
LUA_API void                       (lua_toemitsound) (lua_State *L, int argIndex, EmitSound_t &ep);

#endif // LSHAREDDEFS_H
