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
    LUA_SAFE_SET_METATABLE( L, LUA_MOVEHELPERMETANAME );
}

LUALIB_API lua_IMoveHelper *luaL_checkmovehelper( lua_State *L, int narg )
{
    lua_IMoveHelper **ppData = ( lua_IMoveHelper ** )luaL_checkudata( L, narg, LUA_MOVEHELPERMETANAME );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "MoveHelper expected, got NULL" );

    return *ppData;
}

LUA_REGISTRATION_INIT( MoveHelper );

LUA_BINDING_BEGIN( MoveHelper, ResetTouchList, "class", "Resets the touch list" )
{
    lua_IMoveHelper *moveHelper = LUA_BINDING_ARGUMENT( luaL_checkmovehelper, 1, "moveHelper" );
    moveHelper->ResetTouchList();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveHelper, AddToTouched, "class", "Adds the trace result to touch list, if contact is not already in list" )
{
    lua_IMoveHelper *moveHelper = LUA_BINDING_ARGUMENT( luaL_checkmovehelper, 1, "moveHelper" );
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( luaL_checktrace, 2, "trace" );
    lua_Vector impactVelocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "impactVelocity" );
    lua_pushboolean( L, moveHelper->AddToTouched( trace, impactVelocity ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns true if the contact was added to the touch list, false if it already exists" )

LUA_BINDING_BEGIN( MoveHelper, ProcessImpacts, "class", "Processes all impacts" )
{
    lua_IMoveHelper *moveHelper = LUA_BINDING_ARGUMENT( luaL_checkmovehelper, 1, "moveHelper" );
    moveHelper->ProcessImpacts();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveHelper, ConsoleLog, "class", "Prints a message to the console at a specific line" )
{
    lua_IMoveHelper *moveHelper = LUA_BINDING_ARGUMENT( luaL_checkmovehelper, 1, "moveHelper" );
    int line = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "line" );
    const char *message = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "message" );
    moveHelper->Con_NPrintf( line, "%s", message );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveHelper, PlayerFallingDamage, "class", "Apply falling damage to host player based on set fall velocity" )
{
    lua_IMoveHelper *moveHelper = LUA_BINDING_ARGUMENT( luaL_checkmovehelper, 1, "moveHelper" );
    lua_pushboolean( L, moveHelper->PlayerFallingDamage() );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns true if the player survived the fall, false if they died" )

LUA_BINDING_BEGIN( MoveHelper, PlayerSetAnimation, "class", "Sets player animation" )
{
    lua_IMoveHelper *moveHelper = LUA_BINDING_ARGUMENT( luaL_checkmovehelper, 1, "moveHelper" );
    PLAYER_ANIM animation = ( PLAYER_ANIM )( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "animation" );
    moveHelper->PlayerSetAnimation( animation );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( MoveHelper, GetSurfaceProperties, "class", "Gets the physics surface properties" )
{
    lua_IMoveHelper *moveHelper = LUA_BINDING_ARGUMENT( luaL_checkmovehelper, 1, "moveHelper" );
    lua_pushphysicssurfaceprops( L, moveHelper->GetSurfaceProps() );
    return 1;
}
LUA_BINDING_END( "PhysicsSurfaceProps", "Physics surface properties" )

LUA_BINDING_BEGIN( MoveHelper, __tostring, "class", "Converts MoveHelper to a string" )
{
    lua_IMoveHelper *moveHelper = LUA_BINDING_ARGUMENT( luaL_checkmovehelper, 1, "moveHelper" );
    lua_pushfstring( L, "MoveHelper: %p", moveHelper );
    return 1;
}
LUA_BINDING_END( "string", "MoveHelper as a string" )

LUA_REGISTRATION_INIT( MoveHelpers );

LUA_BINDING_BEGIN( MoveHelpers, Create, "library", "Creates a new MoveHelper object" )
{
    lua_pushmovehelper( L, MoveHelper() );
    return 1;
}
LUA_BINDING_END( "MoveHelper", "New MoveHelper object" )

/*
** Open MoveHelper object
*/
LUALIB_API int luaopen_IMoveHelper( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_MOVEHELPERMETANAME );

    LUA_REGISTRATION_COMMIT( MoveHelper );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_MOVEHELPERMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "MoveHelper" */

    LUA_REGISTRATION_COMMIT_LIBRARY( MoveHelpers );

    return 1;
}
