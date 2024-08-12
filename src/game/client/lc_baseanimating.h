#ifndef LC_BASEANIMATING_H
#define LC_BASEANIMATING_H

#ifdef _WIN32
#pragma once
#endif

/* type for C_BaseAnimating functions */
typedef C_BaseAnimating lua_CBaseAnimating;



/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseAnimating     *(lua_toanimating) (lua_State *L, int idx);


/*
** push functions (C -> stack)
*/
LUALIB_API lua_CBaseAnimating *(luaL_checkanimating) (lua_State *L, int narg);


#endif // LC_BASEANIMATING_H
