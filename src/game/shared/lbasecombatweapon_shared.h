#ifndef LCOMBATWEAPON_SHARED_H
#define LCOMBATWEAPON_SHARED_H
#ifdef _WIN32
#pragma once
#endif

/* type for CBaseCombatWeapon functions */
typedef CBaseCombatWeapon lua_CBaseCombatWeapon;

/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseCombatWeapon     *(lua_toweapon) (lua_State *L, int idx);

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CBaseCombatWeapon *(luaL_checkweapon) (lua_State *L, int narg);


#endif // LCOMBATWEAPON_SHARED_H
