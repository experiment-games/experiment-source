#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lgametrace.h"
#include "lcmodel.h"
#include "lbaseentity_shared.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CGameTrace &lua_totrace( lua_State *L, int idx )
{
    lua_CGameTrace *tr = ( lua_CGameTrace * )lua_touserdata( L, idx );
    return *tr;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushtrace( lua_State *L, lua_CGameTrace &tr )
{
    lua_CGameTrace *ptr = ( lua_CGameTrace * )lua_newuserdata( L, sizeof( lua_CGameTrace ) );
    *ptr = tr;
    LUA_SAFE_SET_METATABLE( L, LUA_GAMETRACELIBNAME );
}

LUALIB_API lua_CGameTrace &luaL_checktrace( lua_State *L, int narg )
{
    lua_CGameTrace *d = ( lua_CGameTrace * )luaL_checkudata( L, narg, LUA_GAMETRACELIBNAME );
    return *d;
}

LUA_REGISTRATION_INIT( Trace )

LUA_BINDING_BEGIN( Trace, DidHit, "class", "Check if the trace hit anything." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    lua_pushboolean( L, trace.DidHit() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the trace hit something, false otherwise." )

LUA_BINDING_BEGIN( Trace, DidHitNonWorldEntity, "class", "Check if the trace hit a non-world entity." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    lua_pushboolean( L, trace.DidHitNonWorldEntity() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the trace hit a non-world entity, false otherwise." )

LUA_BINDING_BEGIN( Trace, DidHitWorld, "class", "Check if the trace hit the world." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    lua_pushboolean( L, trace.DidHitWorld() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the trace hit the world, false otherwise." )

LUA_BINDING_BEGIN( Trace, GetEntityIndex, "class", "Get the index of the entity that was hit." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    lua_pushinteger( L, trace.GetEntityIndex() );
    return 1;
}
LUA_BINDING_END( "number", "The index of the entity that was hit." )

LUA_BINDING_BEGIN( Trace, IsDisplacementSurface, "class", "Check if the trace hit a displacement surface." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    lua_pushboolean( L, trace.IsDispSurface() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the trace hit a displacement surface, false otherwise." )

LUA_BINDING_BEGIN( Trace, IsDisplacementSurfaceBuildable, "class", "Check if the trace hit a buildable displacement surface." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    lua_pushboolean( L, trace.IsDispSurfaceBuildable() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the trace hit a buildable displacement surface, false otherwise." )

LUA_BINDING_BEGIN( Trace, IsDisplacementSurfaceProp1, "class", "Check if the trace hit a displacement surface with prop1 physics." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    lua_pushboolean( L, trace.IsDispSurfaceProp1() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the trace hit a displacement surface with prop1 physics, false otherwise." )

LUA_BINDING_BEGIN( Trace, IsDisplacementSurfaceProp2, "class", "Check if the trace hit a displacement surface with prop2 physics." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    lua_pushboolean( L, trace.IsDispSurfaceProp2() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the trace hit a displacement surface with prop2 physics, false otherwise." )

LUA_BINDING_BEGIN( Trace, IsDisplacementSurfaceWalkable, "class", "Check if the trace hit a walkable displacement surface." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    lua_pushboolean( L, trace.IsDispSurfaceWalkable() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the trace hit a walkable displacement surface, false otherwise." )

LUA_BINDING_BEGIN( Trace, __index, "class", "Get a field from the trace." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    const char *field = LUA_BINDING_ARGUMENT( lua_tostring, 2, "field" );

    if ( Q_strcmp( field, "allsolid" ) == 0 )
        lua_pushboolean( L, trace.allsolid );
    else if ( Q_strcmp( field, "contents" ) == 0 )
        lua_pushinteger( L, trace.contents );
    else if ( Q_strcmp( field, "dispFlags" ) == 0 )
        lua_pushinteger( L, trace.dispFlags );
    else if ( Q_strcmp( field, "endpos" ) == 0 )
        lua_pushvector( L, trace.endpos );
    else if ( Q_strcmp( field, "fraction" ) == 0 )
        lua_pushnumber( L, trace.fraction );
    else if ( Q_strcmp( field, "fractionleftsolid" ) == 0 )
        lua_pushnumber( L, trace.fractionleftsolid );
    else if ( Q_strcmp( field, "hitbox" ) == 0 )
        lua_pushinteger( L, trace.hitbox );
    else if ( Q_strcmp( field, "hitgroup" ) == 0 )
        lua_pushinteger( L, trace.hitgroup );
    else if ( Q_strcmp( field, "m_pEnt" ) == 0 )
        CBaseEntity::PushLuaInstanceSafe( L, trace.m_pEnt );
    else if ( Q_strcmp( field, "physicsbone" ) == 0 )
        lua_pushinteger( L, trace.physicsbone );
    else if ( Q_strcmp( field, "startpos" ) == 0 )
        lua_pushvector( L, trace.startpos );
    else if ( Q_strcmp( field, "startsolid" ) == 0 )
        lua_pushboolean( L, trace.startsolid );
    else if ( Q_strcmp( field, "surface" ) == 0 )
        lua_pushcsurface( L, trace.surface );
    else
    {
        if ( lua_getmetatable( L, 1 ) )
        {
            LUA_METATABLE_INDEX_CHECK_TABLE( L );
        }

        lua_pushnil( L );
    }

    return 1;
}
LUA_BINDING_END( "any", "The value of the field." )

LUA_BINDING_BEGIN( Trace, __newindex, "class", "Set a field in the trace." )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" );
    const char *field = LUA_BINDING_ARGUMENT( lua_tostring, 2, "field" );

    if ( Q_strcmp( field, "allsolid" ) == 0 )
        trace.allsolid = luaL_checkboolean( L, 3 );
    else if ( Q_strcmp( field, "contents" ) == 0 )
        trace.contents = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "dispFlags" ) == 0 )
        trace.dispFlags = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "endpos" ) == 0 )
        trace.endpos = luaL_checkvector( L, 3 );
    else if ( Q_strcmp( field, "fraction" ) == 0 )
        trace.fraction = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "fractionleftsolid" ) == 0 )
        trace.fractionleftsolid = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "hitbox" ) == 0 )
        trace.hitbox = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "hitgroup" ) == 0 )
        trace.hitgroup = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_pEnt" ) == 0 )
        trace.m_pEnt = lua_toentity( L, 3 );
    else if ( Q_strcmp( field, "physicsbone" ) == 0 )
        trace.physicsbone = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "startpos" ) == 0 )
        trace.startpos = luaL_checkvector( L, 3 );
    else if ( Q_strcmp( field, "startsolid" ) == 0 )
        trace.startsolid = luaL_checkboolean( L, 3 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Trace, __tostring, "class", "Get a string representation of the trace." )
{
    lua_pushfstring( L, "CGameTrace: %p", LUA_BINDING_ARGUMENT( luaL_checktrace, 1, "trace" ) );
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the trace." )

LUA_REGISTRATION_INIT( _G )

LUA_BINDING_BEGIN( _G, CreateTrace, "library", "Create a new trace object." )
{
    lua_CGameTrace trace;
    lua_pushtrace( L, trace );
    return 1;
}
LUA_BINDING_END( "Trace", "The new trace object." )

/*
** Open CGameTrace object
*/
LUALIB_API int luaopen_CGameTrace( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_GAMETRACELIBNAME );

    LUA_REGISTRATION_COMMIT( Trace );

    lua_pushstring( L, LUA_GAMETRACELIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Trace" */
    lua_pop( L, 1 );                 /* pop metatable */

    LUA_REGISTRATION_COMMIT_LIBRARY( _G );

    return 1;
}
