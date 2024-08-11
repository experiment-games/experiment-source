//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef LANGLE_H
#define LANGLE_H

#ifdef _WIN32
#pragma once
#endif

/* type for QAngle functions */
typedef QAngle lua_QAngle;

/*
** access functions (stack -> C)
*/

LUA_API lua_QAngle     &(lua_toangle) (lua_State *L, int idx);
LUA_API bool(lua_isangle) (lua_State *L, int idx);

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushangle )( lua_State *L, lua_QAngle &v );
LUA_API void lua_pushangle( lua_State *L, const lua_QAngle &v );


LUALIB_API lua_QAngle &(luaL_checkangle) (lua_State *L, int narg);
LUALIB_API lua_QAngle &(luaL_optangle) (lua_State *L, int narg, lua_QAngle *def);

#define LUA_BINDING_PUSH_RESULT_ANGLE( ReturnPosition, Value ) \
    LUA_BINDING_PUSH_RESULT( lua_pushangle, Value.x, Value.y, Value.z );

#endif // LANGLE_H

