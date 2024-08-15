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
    LUA_SAFE_SET_METATABLE( L, LUA_MOVEDATAMETANAME );
}

LUALIB_API lua_CMoveData *luaL_checkmovedata( lua_State *L, int narg )
{
    lua_CMoveData **ppData = ( lua_CMoveData ** )luaL_checkudata( L, narg, LUA_MOVEDATAMETANAME );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CMoveData expected, got NULL" );

    return *ppData;
}

LUA_REGISTRATION_INIT( MoveData );

LUA_BINDING_BEGIN( MoveData, AddKey, "class", "Adds a key to the buttons" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    int key = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "buttonCode" );
    moveData->m_nButtons |= key;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, ClearButtons, "class", "Clears the buttons" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_nButtons = 0;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, ClearMovement, "class", "Clears the movement" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_flForwardMove = 0.0f;
    moveData->m_flSideMove = 0.0f;
    moveData->m_flUpMove = 0.0f;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, GetAbsoluteMoveAngles, "class", "Gets the absolute move angles" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushangle( L, moveData->m_vecAbsViewAngles );
    return 1;
}
LUA_BINDING_END( "Angle", "The absolute move angles" )

LUA_BINDING_BEGIN( MoveData, GetAngles, "class", "Gets the angles (local space)" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushangle( L, moveData->m_vecViewAngles );
    return 1;
}
LUA_BINDING_END( "Angle", "The angles (local space)" )

LUA_BINDING_BEGIN( MoveData, GetButtons, "class", "Gets the buttons" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushinteger( L, moveData->m_nButtons );
    return 1;
}
LUA_BINDING_END( "integer", "Bitmask of the buttons pressed" )

LUA_BINDING_BEGIN( MoveData, GetConstraintCenter, "class", "Gets the constraint center" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushvector( L, moveData->m_vecConstraintCenter );
    return 1;
}
LUA_BINDING_END( "Vector", "The constraint center" )

LUA_BINDING_BEGIN( MoveData, GetConstraintRadius, "class", "Gets the constraint radius" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushnumber( L, moveData->m_flConstraintRadius );
    return 1;
}
LUA_BINDING_END( "number", "The constraint radius" )

LUA_BINDING_BEGIN( MoveData, GetConstraintSpeedScale, "class", "Gets the constraint speed scale" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushnumber( L, moveData->m_flConstraintSpeedFactor );
    return 1;
}
LUA_BINDING_END( "number", "The constraint speed scale" )

LUA_BINDING_BEGIN( MoveData, GetConstraintWidth, "class", "Gets the constraint width" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushnumber( L, moveData->m_flConstraintWidth );
    return 1;
}
LUA_BINDING_END( "number", "The constraint width" )

LUA_BINDING_BEGIN( MoveData, GetFinalIdealVelocity, "class", "Gets the final ideal velocity" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushvector( L, moveData->m_outWishVel );
    return 1;
}
LUA_BINDING_END( "Vector", "The final ideal velocity" )

LUA_BINDING_BEGIN( MoveData, GetFinalJumpVelocity, "class", "Gets the final jump velocity" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushvector( L, moveData->m_outJumpVel );
    return 1;
}
LUA_BINDING_END( "Vector", "The final jump velocity" )

LUA_BINDING_BEGIN( MoveData, GetFinalStepHeight, "class", "Gets the final step height" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushnumber( L, moveData->m_outStepHeight );
    return 1;
}
LUA_BINDING_END( "number", "The final step height" )

LUA_BINDING_BEGIN( MoveData, GetForwardSpeed, "class", "Gets the forward speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushnumber( L, moveData->m_flForwardMove );
    return 1;
}
LUA_BINDING_END( "number", "The forward speed" )

LUA_BINDING_BEGIN( MoveData, GetImpulseCommand, "class", "Gets the impulse command" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushinteger( L, moveData->m_nImpulseCommand );
    return 1;
}
LUA_BINDING_END( "integer", "The impulse command" )

LUA_BINDING_BEGIN( MoveData, GetMaxClientSpeed, "class", "Gets the max client speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushnumber( L, moveData->m_flClientMaxSpeed );
    return 1;
}
LUA_BINDING_END( "number", "The max client speed" )

LUA_BINDING_BEGIN( MoveData, GetMaxSpeed, "class", "Gets the max speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushnumber( L, moveData->m_flMaxSpeed );
    return 1;
}
LUA_BINDING_END( "number", "The max speed" )

LUA_BINDING_BEGIN( MoveData, GetMoveAngles, "class", "Gets the move angles" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushangle( L, moveData->m_vecAngles );
    return 1;
}
LUA_BINDING_END( "Angle", "The move angles" )

LUA_BINDING_BEGIN( MoveData, GetOldAngles, "class", "Gets the old angles" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushangle( L, moveData->m_vecOldAngles );
    return 1;
}
LUA_BINDING_END( "Angle", "The old angles" )

LUA_BINDING_BEGIN( MoveData, GetOldButtons, "class", "Gets the old buttons" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushinteger( L, moveData->m_nOldButtons );
    return 1;
}
LUA_BINDING_END( "integer", "Bitmask of the old buttons" )

LUA_BINDING_BEGIN( MoveData, GetOrigin, "class", "Gets the origin" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    Vector origin = moveData->GetAbsOrigin();
    lua_pushvector( L, origin );
    return 1;
}
LUA_BINDING_END( "Vector", "The origin" )

LUA_BINDING_BEGIN( MoveData, GetSideSpeed, "class", "Gets the side speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushnumber( L, moveData->m_flSideMove );
    return 1;
}
LUA_BINDING_END( "number", "The side speed" )

LUA_BINDING_BEGIN( MoveData, GetUpSpeed, "class", "Gets the up speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushnumber( L, moveData->m_flUpMove );
    return 1;
}
LUA_BINDING_END( "number", "The up speed" )

LUA_BINDING_BEGIN( MoveData, GetVelocity, "class", "Gets the velocity" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    lua_pushvector( L, moveData->m_vecVelocity );
    return 1;
}
LUA_BINDING_END( "Vector", "The velocity" )

LUA_BINDING_BEGIN( MoveData, IsKeyDown, "class", "Checks if a key is down" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    int key = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "buttonCode" );
    lua_pushboolean( L, ( moveData->m_nButtons & key ) != 0 );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the key is down, false otherwise" )

LUA_BINDING_BEGIN( MoveData, WasKeyPressed, "class", "Checks if a key was pressed, where before it wasn't." )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    int key = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "buttonCode" );
    lua_pushboolean( L, ( moveData->m_nOldButtons & key ) == 0 && ( moveData->m_nButtons & key ) != 0 );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the key was pressed, false otherwise" )

LUA_BINDING_BEGIN( MoveData, WasKeyReleased, "class", "Checks if a key was released, where before it wasn't." )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    int key = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "buttonCode" );
    lua_pushboolean( L, ( moveData->m_nOldButtons & key ) != 0 && ( moveData->m_nButtons & key ) == 0 );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the key was released, false otherwise" )

LUA_BINDING_BEGIN( MoveData, WasKeyDown, "class", "Checks if a key was down before" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    int key = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "buttonCode" );
    lua_pushboolean( L, ( moveData->m_nOldButtons & key ) != 0 );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the key was down, false otherwise" )

LUA_BINDING_BEGIN( MoveData, SetAbsoluteMoveAngles, "class", "Sets the absolute move angles" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_vecAbsViewAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angles" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetAngles, "class", "Sets the angles (local space)" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_vecViewAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angles" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetButtons, "class", "Sets the buttons" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_nButtons = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "buttons" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetConstraintCenter, "class", "Sets the constraint center" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_vecConstraintCenter = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "center" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetConstraintRadius, "class", "Sets the constraint radius" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_flConstraintRadius = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "radius" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetConstraintSpeedScale, "class", "Sets the constraint speed scale" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_flConstraintSpeedFactor = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speedScale" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetConstraintWidth, "class", "Sets the constraint width" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_flConstraintWidth = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "width" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetFinalIdealVelocity, "class", "Sets the final ideal velocity" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->SetAbsOrigin( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "velocity" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetFinalJumpVelocity, "class", "Sets the final jump velocity" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_outJumpVel = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "velocity" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetFinalStepHeight, "class", "Sets the final step height" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_outJumpVel = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "height" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetForwardSpeed, "class", "Sets the forward speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_flForwardMove = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetImpulseCommand, "class", "Sets the impulse command" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_nImpulseCommand = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "command" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetMaxClientSpeed, "class", "Sets the max client speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_flClientMaxSpeed = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetMaxSpeed, "class", "Sets the max speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_flMaxSpeed = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetMoveAngles, "class", "Sets the move angles" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_vecAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angles" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetOldAngles, "class", "Sets the old angles" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_vecOldAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angles" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetOldButtons, "class", "Sets the old buttons" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_nOldButtons = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "buttons" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetOrigin, "class", "Sets the origin" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->SetAbsOrigin( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetSideSpeed, "class", "Sets the side speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_flSideMove = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetUpSpeed, "class", "Sets the up speed" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_flUpMove = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, SetVelocity, "class", "Sets the velocity" )
{
    lua_CMoveData *moveData = LUA_BINDING_ARGUMENT( luaL_checkmovedata, 1, "moveData" );
    moveData->m_vecVelocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "velocity" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveData, __tostring, "class", "Returns a string representation of the object" )
{
    lua_pushfstring( L, "MoveData: %p", luaL_checkudata( L, 1, LUA_MOVEDATAMETANAME ) );
    return 1;
}
LUA_BINDING_END()

/*
** Open CMoveData object
*/
LUALIB_API int luaopen_CMoveData( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_MOVEDATAMETANAME );

    LUA_REGISTRATION_COMMIT( MoveData );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_MOVEDATAMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "MoveData" */

    lua_pop( L, 1 );
    return 1;
}
