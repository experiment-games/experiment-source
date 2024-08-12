#include "cbase.h"
#include "igamemovement.h"
#include "lua.hpp"
#include "lmovedata.h"
#include "lbaseentity_shared.h"
#include "lgametrace.h"
#include "mathlib/lvector.h"
#include "lvphysics_interface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CMoveData *lua_tomovedata( lua_State *L, int idx )
{
    lua_CMoveData **pMoveData = ( lua_CMoveData ** )lua_touserdata( L, idx );
    return *pMoveData;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushmovedata( lua_State *L, lua_CMoveData *pMoveData )
{
    lua_CMoveData **pp = ( lua_CMoveData ** )lua_newuserdata( L, sizeof( pMoveData ) );
    *pp = pMoveData;
    LUA_SAFE_SET_METATABLE( L, LUA_MOVEDATALIBNAME );
}

LUALIB_API lua_CMoveData *luaL_checkmovedata( lua_State *L, int narg )
{
    lua_CMoveData **ppData = ( lua_CMoveData ** )luaL_checkudata( L, narg, LUA_MOVEDATALIBNAME );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CMoveData expected, got NULL" );

    return *ppData;
}

static int CMoveData_AddKey( lua_State *L )
{
    int key = luaL_checknumber( L, 2 );
    luaL_checkmovedata( L, 1 )->m_nButtons |= key;
    return 0;
}

static int CMoveData_ClearButtons( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_nButtons = 0;
    return 0;
}

static int CMoveData_ClearMovement( lua_State *L )
{
    CMoveData *cmd = luaL_checkmovedata( L, 1 );
    cmd->m_flForwardMove = 0.0f;
    cmd->m_flSideMove = 0.0f;
    cmd->m_flUpMove = 0.0f;
    return 0;
}

static int CMoveData_GetAbsMoveAngles( lua_State *L )
{
    lua_pushangle( L, luaL_checkmovedata( L, 1 )->m_vecAbsViewAngles );
    return 1;
}

static int CMoveData_GetAngles( lua_State *L )
{
    lua_pushangle( L, luaL_checkmovedata( L, 1 )->m_vecViewAngles );
    return 1;
}

static int CMoveData_GetButtons( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_nButtons );
    return 1;
}

static int CMoveData_GetConstraintCenter( lua_State *L )
{
    lua_pushvector( L, luaL_checkmovedata( L, 1 )->m_vecConstraintCenter );
    return 1;
}

static int CMoveData_GetConstraintRadius( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_flConstraintRadius );
    return 1;
}

static int CMoveData_GetConstraintSpeedScale( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_flConstraintSpeedFactor );
    return 1;
}

static int CMoveData_GetConstraintWidth( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_flConstraintWidth );
    return 1;
}

static int CMoveData_GetFinalIdealVelocity( lua_State *L )
{
    lua_pushvector( L, luaL_checkmovedata( L, 1 )->m_outWishVel );
    return 1;
}

static int CMoveData_GetFinalJumpVelocity( lua_State *L )
{
    lua_pushvector( L, luaL_checkmovedata( L, 1 )->m_outJumpVel );
    return 1;
}

static int CMoveData_GetFinalStepHeight( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_outStepHeight );
    return 1;
}

static int CMoveData_GetForwardSpeed( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_flForwardMove );
    return 1;
}

static int CMoveData_GetImpulseCommand( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_nImpulseCommand );
    return 1;
}

static int CMoveData_GetMaxClientSpeed( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_flClientMaxSpeed );
    return 1;
}

static int CMoveData_GetMaxSpeed( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_flMaxSpeed );
    return 1;
}

static int CMoveData_GetMoveAngles( lua_State *L )
{
    lua_pushangle( L, luaL_checkmovedata( L, 1 )->m_vecAngles );
    return 1;
}

static int CMoveData_GetOldAngles( lua_State *L )
{
    lua_pushangle( L, luaL_checkmovedata( L, 1 )->m_vecOldAngles );
    return 1;
}

static int CMoveData_GetOldButtons( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_nOldButtons );
    return 1;
}

static int CMoveData_GetOrigin( lua_State *L )
{
    Vector origin = luaL_checkmovedata( L, 1 )->GetAbsOrigin();
    lua_pushvector( L, origin );
    return 1;
}

static int CMoveData_GetSideSpeed( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_flSideMove );
    return 1;
}

static int CMoveData_GetUpSpeed( lua_State *L )
{
    lua_pushnumber( L, luaL_checkmovedata( L, 1 )->m_flUpMove );
    return 1;
}

static int CMoveData_GetVelocity( lua_State *L )
{
    lua_pushvector( L, luaL_checkmovedata( L, 1 )->m_vecVelocity );
    return 1;
}

static int CMoveData_KeyDown( lua_State *L )
{
    int key = luaL_checknumber( L, 2 );
    lua_pushboolean( L, ( luaL_checkmovedata( L, 1 )->m_nButtons & key ) != 0 );
    return 1;
}

static int CMoveData_KeyPressed( lua_State *L )
{
    int key = luaL_checknumber( L, 2 );
    lua_pushboolean( L, ( luaL_checkmovedata( L, 1 )->m_nOldButtons & key ) == 0 && ( luaL_checkmovedata( L, 1 )->m_nButtons & key ) != 0 );
    return 1;
}

static int CMoveData_KeyReleased( lua_State *L )
{
    int key = luaL_checknumber( L, 2 );
    lua_pushboolean( L, ( luaL_checkmovedata( L, 1 )->m_nOldButtons & key ) != 0 && ( luaL_checkmovedata( L, 1 )->m_nButtons & key ) == 0 );
    return 1;
}

static int CMoveData_KeyWasDown( lua_State *L )
{
    int key = luaL_checknumber( L, 2 );
    lua_pushboolean( L, ( luaL_checkmovedata( L, 1 )->m_nOldButtons & key ) != 0 );
    return 1;
}

static int CMoveData_SetAbsMoveAngles( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_vecAbsViewAngles = luaL_checkangle( L, 2 );
    return 0;
}

static int CMoveData_SetAngles( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_vecViewAngles = luaL_checkangle( L, 2 );
    return 0;
}

static int CMoveData_SetButtons( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_nButtons = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetConstraintCenter( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_vecConstraintCenter = luaL_checkvector( L, 2 );
    return 0;
}

static int CMoveData_SetConstraintRadius( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_flConstraintRadius = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetConstraintSpeedScale( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_flConstraintSpeedFactor = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetConstraintWidth( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_flConstraintWidth = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetFinalIdealVelocity( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->SetAbsOrigin( luaL_checkvector( L, 2 ) );
    return 0;
}

static int CMoveData_SetFinalJumpVelocity( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_outJumpVel = luaL_checkvector( L, 2 );
    return 0;
}

static int CMoveData_SetFinalStepHeight( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_outJumpVel = luaL_checkvector( L, 2 );
    return 0;
}

static int CMoveData_SetForwardSpeed( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_flForwardMove = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetImpulseCommand( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_nImpulseCommand = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetMaxClientSpeed( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_flClientMaxSpeed = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetMaxSpeed( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_flMaxSpeed = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetMoveAngles( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_vecAngles = luaL_checkangle( L, 2 );
    return 0;
}

static int CMoveData_SetOldAngles( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_vecOldAngles = luaL_checkangle( L, 2 );
    return 0;
}

static int CMoveData_SetOldButtons( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_nOldButtons = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetOrigin( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->SetAbsOrigin( luaL_checkvector( L, 2 ) );
    return 0;
}

static int CMoveData_SetSideSpeed( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_flSideMove = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetUpSpeed( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_flUpMove = luaL_checknumber( L, 2 );
    return 0;
}

static int CMoveData_SetVelocity( lua_State *L )
{
    luaL_checkmovedata( L, 1 )->m_vecVelocity = luaL_checkvector( L, 2 );
    return 0;
}

static int CMoveData___tostring( lua_State *L )
{
    lua_pushfstring( L, "CMoveData: %p", luaL_checkudata( L, 1, LUA_MOVEDATALIBNAME ) );
    return 1;
}

static const luaL_Reg CMoveDatameta[] = {
    { "AddKey", CMoveData_AddKey },
    { "ClearButtons", CMoveData_ClearButtons },
    { "ClearMovement", CMoveData_ClearMovement },
    { "GetAbsMoveAngles", CMoveData_GetAbsMoveAngles },
    { "GetAngles", CMoveData_GetAngles },
    { "GetButtons", CMoveData_GetButtons },
    { "GetConstraintCenter", CMoveData_GetConstraintCenter },
    { "GetConstraintRadius", CMoveData_GetConstraintRadius },
    { "GetConstraintSpeedScale", CMoveData_GetConstraintSpeedScale },
    { "GetConstraintWidth", CMoveData_GetConstraintWidth },
    { "GetFinalIdealVelocity", CMoveData_GetFinalIdealVelocity },
    { "GetFinalJumpVelocity", CMoveData_GetFinalJumpVelocity },
    { "GetFinalStepHeight", CMoveData_GetFinalStepHeight },
    { "GetForwardSpeed", CMoveData_GetForwardSpeed },
    { "GetImpulseCommand", CMoveData_GetImpulseCommand },
    { "GetMaxClientSpeed", CMoveData_GetMaxClientSpeed },
    { "GetMaxSpeed", CMoveData_GetMaxSpeed },
    { "GetMoveAngles", CMoveData_GetMoveAngles },
    { "GetOldAngles", CMoveData_GetOldAngles },
    { "GetOldButtons", CMoveData_GetOldButtons },
    { "GetOrigin", CMoveData_GetOrigin },
    { "GetSideSpeed", CMoveData_GetSideSpeed },
    { "GetUpSpeed", CMoveData_GetUpSpeed },
    { "GetVelocity", CMoveData_GetVelocity },
    { "KeyDown", CMoveData_KeyDown },
    { "KeyPressed", CMoveData_KeyPressed },
    { "KeyReleased", CMoveData_KeyReleased },
    { "KeyWasDown", CMoveData_KeyWasDown },
    { "SetAbsMoveAngles", CMoveData_SetAbsMoveAngles },
    { "SetAngles", CMoveData_SetAngles },
    { "SetButtons", CMoveData_SetButtons },
    { "SetConstraintCenter", CMoveData_SetConstraintCenter },
    { "SetConstraintRadius", CMoveData_SetConstraintRadius },
    { "SetConstraintSpeedScale", CMoveData_SetConstraintSpeedScale },
    { "SetConstraintWidth", CMoveData_SetConstraintWidth },
    { "SetFinalIdealVelocity", CMoveData_SetFinalIdealVelocity },
    { "SetFinalJumpVelocity", CMoveData_SetFinalJumpVelocity },
    { "SetFinalStepHeight", CMoveData_SetFinalStepHeight },
    { "SetForwardSpeed", CMoveData_SetForwardSpeed },
    { "SetImpulseCommand", CMoveData_SetImpulseCommand },
    { "SetMaxClientSpeed", CMoveData_SetMaxClientSpeed },
    { "SetMaxSpeed", CMoveData_SetMaxSpeed },
    { "SetMoveAngles", CMoveData_SetMoveAngles },
    { "SetOldAngles", CMoveData_SetOldAngles },
    { "SetOldButtons", CMoveData_SetOldButtons },
    { "SetOrigin", CMoveData_SetOrigin },
    { "SetSideSpeed", CMoveData_SetSideSpeed },
    { "SetUpSpeed", CMoveData_SetUpSpeed },
    { "SetVelocity", CMoveData_SetVelocity },
    { "__tostring", CMoveData___tostring },
    { NULL, NULL } };

/*
** Open CMoveData object
*/
LUALIB_API int luaopen_CMoveData( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_MOVEDATALIBNAME );
    luaL_register( L, NULL, CMoveDatameta );
    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, "MoveData" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "MoveData" */
    lua_pop( L, 1 );
    return 1;
}
