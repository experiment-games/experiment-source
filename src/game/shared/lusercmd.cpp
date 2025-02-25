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
    LUA_SAFE_SET_METATABLE( L, LUA_USERCMDMETANAME );
}

LUALIB_API lua_CUserCmd *luaL_checkusercmd( lua_State *L, int narg )
{
    lua_CUserCmd **ppData = ( lua_CUserCmd ** )luaL_checkudata( L, narg, LUA_USERCMDMETANAME );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CUserCmd expected, got NULL" );

    return *ppData;
}

LUA_REGISTRATION_INIT( UserCommand );

LUA_BINDING_BEGIN( UserCommand, AddKey, "class", "Adds a key to the buttons" )
{
    int key = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "buttonCode" );
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->buttons |= key;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, ClearButtons, "class", "Clears the buttons" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->buttons = 0;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, ClearMovement, "class", "Clears the movement" )
{
    lua_CUserCmd *cmd = LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" );
    cmd->forwardmove = 0.0f;
    cmd->sidemove = 0.0f;
    cmd->upmove = 0.0f;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, CommandNumber, "class", "Gets the command number" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->command_number );
    return 1;
}
LUA_BINDING_END( "integer", "The command number" )

LUA_BINDING_BEGIN( UserCommand, GetButtons, "class", "Gets the button states" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->buttons );
    return 1;
}
LUA_BINDING_END( "integer", "The button states" )

LUA_BINDING_BEGIN( UserCommand, GetForwardMove, "class", "Gets forward movement value" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->forwardmove );
    return 1;
}
LUA_BINDING_END( "number", "The forward movement value" )

LUA_BINDING_BEGIN( UserCommand, GetImpulse, "class", "Gets the impulse value" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->impulse );
    return 1;
}
LUA_BINDING_END( "integer", "The impulse value" )

LUA_BINDING_BEGIN( UserCommand, GetMouseWheel, "class", "Gets the mouse wheel delta" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->mousedx );
    return 1;
}
LUA_BINDING_END( "integer", "The mouse wheel delta" )

LUA_BINDING_BEGIN( UserCommand, GetMouseX, "class", "Gets mouse X movement" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->mousedx );
    return 1;
}
LUA_BINDING_END( "integer", "The mouse X movement" )

LUA_BINDING_BEGIN( UserCommand, GetMouseY, "class", "Gets mouse Y movement" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->mousedy );
    return 1;
}
LUA_BINDING_END( "integer", "The mouse Y movement" )

LUA_BINDING_BEGIN( UserCommand, GetSideMove, "class", "Gets side movement value" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->sidemove );
    return 1;
}
LUA_BINDING_END( "number", "The side movement value" )

LUA_BINDING_BEGIN( UserCommand, GetUpMove, "class", "Gets upward movement value" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->upmove );
    return 1;
}
LUA_BINDING_END( "number", "The upward movement value" )

LUA_BINDING_BEGIN( UserCommand, GetViewAngles, "class", "Gets the view angles" )
{
    lua_pushangle( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->viewangles );
    return 1;
}
LUA_BINDING_END( "Angle", "The view angles" )

LUA_BINDING_BEGIN( UserCommand, IsNew, "class", "Whether this command is repeated (likely due to lag) and it isn't new." )
{
    // Experiment; TODO: This doesn't work as expected lastcommand is set too early
    //    lua_CUserCmd *cmd = LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" );
    //    CBasePlayer *player = CBaseEntity::GetPredictionPlayer();
    //    Assert( player );
    //
    // #ifdef CLIENT_DLL
    //    // Here to mimic gmod behavior, the client always forms new commands.
    //    int lastPlayerCommandNumber = player->CurrentCommandNumber() - 1;
    // #else
    //    int lastPlayerCommandNumber = player->LastUserCommandNumber();
    // #endif
    //
    //    if ( cmd->command_number == lastPlayerCommandNumber )
    //    {
    //        // Experiment; Let's see if this happens often.
    //        DevMsg( "[Experiment debug] UserCommand number mismatch: %i == %i (IsNew = false)\n", cmd->command_number, lastPlayerCommandNumber );
    //    }
    //
    //    lua_pushboolean( L, cmd->command_number != lastPlayerCommandNumber );
    lua_pushboolean( L, true );  // TODO: Implement
    return 1;
}
LUA_BINDING_END( "boolean", "True if the command is new, false otherwise" )

LUA_BINDING_BEGIN( UserCommand, KeyDown, "class", "Checks if a key is down" )
{
    int key = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" );
    lua_pushboolean( L, ( LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->buttons & key ) != 0 );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the key is down, false otherwise" )

LUA_BINDING_BEGIN( UserCommand, RemoveKey, "class", "Removes a key from the buttons" )
{
    int key = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" );
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->buttons &= ~key;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, SelectWeapon, "class", "Selects a weapon" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->weaponselect = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" )->entindex();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, SetButtons, "class", "Sets the button states" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->buttons = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "buttons" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, SetForwardMove, "class", "Sets forward movement" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->forwardmove = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "forwardMove" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, SetImpulse, "class", "Sets the impulse value" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->impulse = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "impulse" );
    return 0;
}
LUA_BINDING_END()

// static int CUserCmd_SetMouseWheel( lua_State *L )
//{
//     LUA_BINDING_ARGUMENT(luaL_checkusercmd, 1, "userCmd")->??? = LUA_BINDING_ARGUMENT(luaL_checknumber, 2, "???");
//     return 0;
// }

LUA_BINDING_BEGIN( UserCommand, SetMouseX, "class", "Sets mouse X movement" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->mousedx = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "mouseDx" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, SetMouseY, "class", "Sets mouse Y movement" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->mousedy = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "mouseDy" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, SetSideMove, "class", "Sets side movement" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->sidemove = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sideMove" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, SetUpMove, "class", "Sets upward movement" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->upmove = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "upMove" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, SetViewAngles, "class", "Sets the view angles" )
{
    LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->viewangles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "viewAngles" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( UserCommand, TickCount, "class", "Gets the tick count" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkusercmd, 1, "userCmd" )->tick_count );
    return 1;
}
LUA_BINDING_END( "integer", "The tick count" )

LUA_BINDING_BEGIN( UserCommand, __tostring, "class", "__tostring method for UserCommand" )
{
    lua_pushfstring( L, "UserCmd: %p", LUA_BINDING_ARGUMENT( lua_tousercmd, 1, LUA_USERCMDMETANAME ) );
    return 1;
}
LUA_BINDING_END( "string", "The string representation" )

/*
** Open CUserCmd object
*/
LUALIB_API int luaopen_CUserCmd( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_USERCMDMETANAME );

    LUA_REGISTRATION_COMMIT( UserCommand );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_USERCMDMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "UserCommand" */

    lua_pop( L, 1 );
    return 1;
}
