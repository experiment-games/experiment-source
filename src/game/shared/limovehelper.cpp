//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "imovehelper.h"
#include "lua.hpp"
#include "limovehelper.h"
#include "lbaseentity_shared.h"
#include "lgametrace.h"
#include "mathlib/lvector.h"
#include "lvphysics_interface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_IMoveHelper *lua_tomovehelper( lua_State *L, int idx )
{
    lua_IMoveHelper **ppHelper = ( lua_IMoveHelper ** )lua_touserdata( L, idx );
    return *ppHelper;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushmovehelper( lua_State *L, lua_IMoveHelper *pHelper )
{
    lua_IMoveHelper **ppHelper = ( lua_IMoveHelper ** )lua_newuserdata( L, sizeof( lua_IMoveHelper ) );
    *ppHelper = pHelper;
    LUA_SAFE_SET_METATABLE( L, "IMoveHelper" );
}

LUALIB_API lua_IMoveHelper *luaL_checkmovehelper( lua_State *L, int narg )
{
    lua_IMoveHelper **ppData = ( lua_IMoveHelper ** )luaL_checkudata( L, narg, "IMoveHelper" );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "IMoveHelper expected, got NULL" );

    return *ppData;
}

static int IMoveHelper_ResetTouchList( lua_State *L )
{
    luaL_checkmovehelper( L, 1 )->ResetTouchList();
    return 0;
}

static int IMoveHelper_AddToTouched( lua_State *L )
{
    lua_pushboolean( L, luaL_checkmovehelper( L, 1 )->AddToTouched( luaL_checktrace( L, 2 ), luaL_checkvector( L, 3 ) ) );
    return 1;
}

static int IMoveHelper_ProcessImpacts( lua_State *L )
{
    luaL_checkmovehelper( L, 1 )->ProcessImpacts();
    return 0;
}

static int IMoveHelper_Con_NPrintf( lua_State *L )
{
    luaL_checkmovehelper( L, 1 )->Con_NPrintf( luaL_checknumber( L, 2 ), luaL_checkstring( L, 3 ) );
    return 0;
}

static int IMoveHelper_PlayerFallingDamage( lua_State *L )
{
    lua_pushboolean( L, luaL_checkmovehelper( L, 1 )->PlayerFallingDamage() );
    return 1;
}

static int IMoveHelper_PlayerSetAnimation( lua_State *L )
{
    luaL_checkmovehelper( L, 1 )->PlayerSetAnimation( ( PLAYER_ANIM )luaL_checkinteger( L, 2 ) );
    return 0;
}

static int IMoveHelper_GetSurfaceProps( lua_State *L )
{
    lua_pushphysicssurfaceprops( L, luaL_checkmovehelper( L, 1 )->GetSurfaceProps() );
    return 1;
}

static int IMoveHelper___tostring( lua_State *L )
{
    lua_pushfstring( L, "IMoveHelper: %p", luaL_checkudata( L, 1, "IMoveHelper" ) );
    return 1;
}

static const luaL_Reg IMoveHelpermeta[] = {
    { "ResetTouchList", IMoveHelper_ResetTouchList },
    { "AddToTouched", IMoveHelper_AddToTouched },
    { "ProcessImpacts", IMoveHelper_ProcessImpacts },
    { "Con_NPrintf", IMoveHelper_Con_NPrintf },
    { "PlayerSetAnimation", IMoveHelper_PlayerSetAnimation },
    { "GetSurfaceProps", IMoveHelper_GetSurfaceProps },
    { "__tostring", IMoveHelper___tostring },
    { NULL, NULL } };

static int luasrc_MoveHelper( lua_State *L )
{
    lua_pushmovehelper( L, MoveHelper() );
    return 1;
}

static const luaL_Reg IMoveHelper_funcs[] = {
    { "MoveHelper", luasrc_MoveHelper },
    { NULL, NULL } };

/*
** Open IMoveHelper object
*/
LUALIB_API int luaopen_IMoveHelper( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "IMoveHelper" );
    luaL_register( L, NULL, IMoveHelpermeta );
    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, "MoveHelper" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "MoveHelper" */
    luaL_register( L, LUA_GNAME, IMoveHelper_funcs );
    lua_pop( L, 1 );
    return 1;
}
