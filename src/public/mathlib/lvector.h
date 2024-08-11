//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef LVECTOR_H
#define LVECTOR_H

#ifdef _WIN32
#pragma once
#endif

/* type for Vector functions */
typedef Vector lua_Vector;

/*
** access functions (stack -> C)
*/

LUA_API lua_Vector     &(lua_tovector) (lua_State *L, int idx);
LUA_API bool(lua_isvector) (lua_State *L, int idx);

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushvector )( lua_State *L, lua_Vector &v );
LUA_API void lua_pushvector( lua_State *L, const lua_Vector &v );

LUALIB_API lua_Vector &(luaL_checkvector) (lua_State *L, int narg);
LUALIB_API lua_Vector &(luaL_optvector) (lua_State *L, int narg, lua_Vector *def);

#define LUA_BINDING_PUSH_RESULT_VECTOR( ReturnPosition, Value ) \
    LUA_BINDING_PUSH_RESULT( lua_pushvector, Value.x, Value.y, Value.z );

#define LUA_BINDING_PUSH_RESULT_VECTOR4D( ReturnPosition, Value ) \
    LUA_BINDING_PUSH_RESULT( lua_pushvector4d, Value.x, Value.y, Value.z, Value.w );

#endif // LVECTOR_H
