#include "cbase.h"
#include "igamemovement.h"
#include "lua.hpp"
#include "lusercmd.h"
#include "lbaseentity_shared.h"
#include "lgametrace.h"
#include "mathlib/lvector.h"
#include "lvphysics_interface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CUserCmd *lua_tousercmd( lua_State *L, int idx )
{
    lua_CUserCmd **pUserCmd = ( lua_CUserCmd ** )lua_touserdata( L, idx );
    return *pUserCmd;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushusercmd( lua_State *L, lua_CUserCmd *pUserCmd )
{
    lua_CUserCmd **pp = ( lua_CUserCmd ** )lua_newuserdata( L, sizeof( pUserCmd ) );
    *pp = pUserCmd;
    LUA_SAFE_SET_METATABLE( L, LUA_USERCMDLIBNAME );
}

LUALIB_API lua_CUserCmd *luaL_checkusercmd( lua_State *L, int narg )
{
    lua_CUserCmd **ppData = ( lua_CUserCmd ** )luaL_checkudata( L, narg, LUA_USERCMDLIBNAME );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CUserCmd expected, got NULL" );

    return *ppData;
}

static int CUserCmd_AddKey( lua_State *L )
{
    int key = luaL_checknumber( L, 2 );
    luaL_checkusercmd( L, 1 )->buttons |= key;
    return 0;
}

static int CUserCmd_ClearButtons( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->buttons = 0;
    return 0;
}

static int CUserCmd_ClearMovement( lua_State *L )
{
    CUserCmd *cmd = luaL_checkusercmd( L, 1 );
    cmd->forwardmove = 0.0f;
    cmd->sidemove = 0.0f;
    cmd->upmove = 0.0f;
    return 0;
}

static int CUserCmd_CommandNumber( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->command_number );
    return 1;
}

static int CUserCmd_GetButtons( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->buttons );
    return 1;
}

static int CUserCmd_GetForwardMove( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->forwardmove );
    return 1;
}

static int CUserCmd_GetImpulse( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->impulse );
    return 1;
}

static int CUserCmd_GetMouseWheel( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->mousedx );
    return 1;
}

static int CUserCmd_GetMouseX( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->mousedx );
    return 1;
}

static int CUserCmd_GetMouseY( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->mousedy );
    return 1;
}

static int CUserCmd_GetSideMove( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->sidemove );
    return 1;
}

static int CUserCmd_GetUpMove( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->upmove );
    return 1;
}

static int CUserCmd_GetViewAngles( lua_State *L )
{
    lua_pushangle( L, luaL_checkusercmd( L, 1 )->viewangles );
    return 1;
}

//static int CUserCmd_IsForced( lua_State *L )
//{
//    lua_pushboolean( L, luaL_checkusercmd( L, 1 )->??? );
//    return 1;
//}

static int CUserCmd_KeyDown( lua_State *L )
{
    int key = luaL_checknumber( L, 2 );
    lua_pushboolean( L, ( luaL_checkusercmd( L, 1 )->buttons & key ) != 0 );
    return 1;
}

static int CUserCmd_RemoveKey( lua_State *L )
{
    int key = luaL_checknumber( L, 2 );
    luaL_checkusercmd( L, 1 )->buttons &= ~key;
    return 0;
}

static int CUserCmd_SelectWeapon( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->weaponselect = luaL_checkentity( L, 2 )->entindex();
    return 0;
}

static int CUserCmd_SetButtons( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->buttons = luaL_checknumber( L, 2 );
    return 0;
}

static int CUserCmd_SetForwardMove( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->forwardmove = luaL_checknumber( L, 2 );
    return 0;
}

static int CUserCmd_SetImpulse( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->impulse = luaL_checknumber( L, 2 );
    return 0;
}

//static int CUserCmd_SetMouseWheel( lua_State *L )
//{
//    luaL_checkusercmd( L, 1 )->???
//    return 0;
//}

static int CUserCmd_SetMouseX( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->mousedx = luaL_checknumber( L, 2 );
    return 0;
}

static int CUserCmd_SetMouseY( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->mousedy = luaL_checknumber( L, 2 );
    return 0;
}

static int CUserCmd_SetSideMove( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->sidemove = luaL_checknumber( L, 2 );
    return 0;
}

static int CUserCmd_SetUpMove( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->upmove = luaL_checknumber( L, 2 );
    return 0;
}

static int CUserCmd_SetViewAngles( lua_State *L )
{
    luaL_checkusercmd( L, 1 )->viewangles = luaL_checkangle( L, 2 );
    return 0;
}

static int CUserCmd_TickCount( lua_State *L )
{
    lua_pushnumber( L, luaL_checkusercmd( L, 1 )->tick_count );
    return 1;
}

static int CUserCmd___tostring( lua_State *L )
{
    lua_pushfstring( L, "CUserCmd: %p", luaL_checkudata( L, 1, LUA_USERCMDLIBNAME ) );
    return 1;
}

static const luaL_Reg CUserCmdmeta[] = {
    { "AddKey", CUserCmd_AddKey },
    { "ClearButtons", CUserCmd_ClearButtons },
    { "ClearMovement", CUserCmd_ClearMovement },
    { "CommandNumber", CUserCmd_CommandNumber },
    { "GetButtons", CUserCmd_GetButtons },
    { "GetForwardMove", CUserCmd_GetForwardMove },
    { "GetImpulse", CUserCmd_GetImpulse },
    { "GetMouseWheel", CUserCmd_GetMouseWheel },
    { "GetMouseX", CUserCmd_GetMouseX },
    { "GetMouseY", CUserCmd_GetMouseY },
    { "GetSideMove", CUserCmd_GetSideMove },
    { "GetUpMove", CUserCmd_GetUpMove },
    { "GetViewAngles", CUserCmd_GetViewAngles },
    // { "IsForced", CUserCmd_IsForced },
    { "KeyDown", CUserCmd_KeyDown },
    { "RemoveKey", CUserCmd_RemoveKey },
    { "SelectWeapon", CUserCmd_SelectWeapon },
    { "SetButtons", CUserCmd_SetButtons },
    { "SetForwardMove", CUserCmd_SetForwardMove },
    { "SetImpulse", CUserCmd_SetImpulse },
    // { "SetMouseWheel", CUserCmd_SetMouseWheel },
    { "SetMouseX", CUserCmd_SetMouseX },
    { "SetMouseY", CUserCmd_SetMouseY },
    { "SetSideMove", CUserCmd_SetSideMove },
    { "SetUpMove", CUserCmd_SetUpMove },
    { "SetViewAngles", CUserCmd_SetViewAngles },
    { "TickCount", CUserCmd_TickCount },
    { "__tostring", CUserCmd___tostring },
    { NULL, NULL } };

/*
** Open CUserCmd object
*/
LUALIB_API int luaopen_CUserCmd( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_USERCMDLIBNAME );
    luaL_register( L, NULL, CUserCmdmeta );
    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, "UserCmd" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "UserCmd" */
    lua_pop( L, 1 );
    return 1;
}
