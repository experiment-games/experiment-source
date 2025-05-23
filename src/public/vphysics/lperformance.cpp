#include "cbase.h"
#include "performance.h"
#include "lua.hpp"
#include "lperformance.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_Physics_performanceparams_t lua_toperformanceparams( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TTABLE );
    physics_performanceparams_t params;
    params.Defaults();
    lua_getfield( L, idx, "maxCollisionsPerObjectPerTimestep" );
    if ( !lua_isnil( L, -1 ) )
        params.maxCollisionsPerObjectPerTimestep = luaL_checknumber( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "maxCollisionChecksPerTimestep" );
    if ( !lua_isnil( L, -1 ) )
        params.maxCollisionChecksPerTimestep = luaL_checknumber( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "maxVelocity" );
    if ( !lua_isnil( L, -1 ) )
        params.maxVelocity = luaL_checknumber( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "maxAngularVelocity" );
    if ( !lua_isnil( L, -1 ) )
        params.maxAngularVelocity = luaL_checknumber( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "lookAheadTimeObjectsVsWorld" );
    if ( !lua_isnil( L, -1 ) )
        params.lookAheadTimeObjectsVsWorld = luaL_checknumber( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "lookAheadTimeObjectsVsObject" );
    if ( !lua_isnil( L, -1 ) )
        params.lookAheadTimeObjectsVsObject = luaL_checknumber( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "minFrictionMass" );
    if ( !lua_isnil( L, -1 ) )
        params.minFrictionMass = luaL_checknumber( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "maxFrictionMass" );
    if ( !lua_isnil( L, -1 ) )
        params.maxFrictionMass = luaL_checknumber( L, -1 );
    lua_pop( L, 1 );
    return params;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushperformanceparams( lua_State *L, lua_Physics_performanceparams_t *pOutput )
{
    lua_newtable( L );

    lua_pushnumber( L, pOutput->lookAheadTimeObjectsVsWorld );
    lua_setfield( L, -2, "LookAheadTimeObjectsVsWorld" );
    lua_pushnumber( L, pOutput->lookAheadTimeObjectsVsObject );
    lua_setfield( L, -2, "LookAheadTimeObjectsVsObject" );

    lua_pushinteger( L, pOutput->maxCollisionChecksPerTimestep );
    lua_setfield( L, -2, "MaximumCollisionsPerObjectPerTimestep" );
    lua_pushinteger( L, pOutput->maxCollisionChecksPerTimestep );
    lua_setfield( L, -2, "MaximumCollisionChecksPerTimestep" );
    lua_pushnumber( L, pOutput->maxAngularVelocity );
    lua_setfield( L, -2, "MaximumAngularVelocity" );
    lua_pushnumber( L, pOutput->maxVelocity );
    lua_setfield( L, -2, "MaximumVelocity" );
    lua_pushnumber( L, pOutput->minFrictionMass );
    lua_setfield( L, -2, "MinimumFrictionMass" );
    lua_pushnumber( L, pOutput->maxFrictionMass );
    lua_setfield( L, -2, "MaximumFrictionMass" );

    // GMOD compatibility naming:
    lua_pushinteger( L, pOutput->maxCollisionChecksPerTimestep );
    lua_setfield( L, -2, "MaxCollisionsPerObjectPerTimestep" );
    lua_pushinteger( L, pOutput->maxCollisionChecksPerTimestep );
    lua_setfield( L, -2, "MaxCollisionChecksPerTimestep" );
    lua_pushnumber( L, pOutput->maxAngularVelocity );
    lua_setfield( L, -2, "MaxAngularVelocity" );
    lua_pushnumber( L, pOutput->maxVelocity );
    lua_setfield( L, -2, "MaxVelocity" );
    lua_pushnumber( L, pOutput->minFrictionMass );
    lua_setfield( L, -2, "MinFrictionMass" );
    lua_pushnumber( L, pOutput->maxFrictionMass );
    lua_setfield( L, -2, "MaxFrictionMass" );
}
