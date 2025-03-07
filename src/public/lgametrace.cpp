#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lgametrace.h"
#include "lcmodel.h"
#include "lbaseentity_shared.h"
#include "mathlib/lvector.h"

#ifdef CLIENT_DLL
#include "c_world.h"
#else
#include "world.h"
#endif
#include "lbaseflex_shared.h"

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
    LUA_SAFE_SET_METATABLE( L, LUA_GAMETRACEMETANAME );
}

LUALIB_API lua_CGameTrace &luaL_checktrace( lua_State *L, int argIndex )
{
    lua_CGameTrace *d = ( lua_CGameTrace * )luaL_checkudata( L, argIndex, LUA_GAMETRACEMETANAME );
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
LUA_BINDING_END( "integer", "The index of the entity that was hit." )

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
    lua_pushfstring( L, "CGameTrace: %p", LUA_BINDING_ARGUMENT( lua_totrace, 1, "trace" ) );
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the trace." )

/*
** Traces Library functions
*/

LUA_REGISTRATION_INIT( Traces )

LUA_BINDING_BEGIN( Traces, Create, "library", "Create a new trace object." )
{
    lua_CGameTrace trace;
    lua_pushtrace( L, trace );
    return 1;
}
LUA_BINDING_END( "Trace", "The new trace object." )

CTraceLuaFilter::CTraceLuaFilter( lua_State *L, int filterArg, int collisionGroup, bool bIgnoreWorld, bool bFilterTableInverted /* = false */ )
    : CTraceFilterSimple( NULL, collisionGroup ),
    m_pLuaState( L ),
    m_bIgnoreWorld( bIgnoreWorld ),
    m_bFilterTableInverted( bFilterTableInverted )
{
    m_iFilterArgIndex = filterArg = lua_absindex( L, filterArg );
    m_pPassEntity = nullptr;

    if ( lua_isfunction( m_pLuaState, filterArg ) )
    {
        // Keep function arg on stack, ready to poll in ShouldHitEntity later
        m_iFilterType = LUA_TFUNCTION;
    }
    else if ( lua_toentity( m_pLuaState, filterArg ) )
    {
        m_pPassEntity = lua_toentity( m_pLuaState, filterArg );
        // No need to keep the entity on the stack
        lua_remove( m_pLuaState, filterArg );
    }
    else if ( lua_istable( m_pLuaState, filterArg ) )
    {
        // Keep table on stack, ready to check in ShouldHitEntity later
        // Might be a table of entities, or table of classnames
        m_iFilterType = LUA_TTABLE;
    }
    else if ( !lua_isnil( m_pLuaState, filterArg ) )
    {
        luaL_typeerror( m_pLuaState, filterArg, "Entity, function, or table" );
    }
}

bool CTraceLuaFilter::ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
{
    if ( m_bIgnoreWorld && pHandleEntity == GetWorldEntity() )
        return false;

    // Do not hit entities returned by the function, or have the function return a
    // boolean to determine if the entity should be hit
    if ( m_iFilterType == LUA_TFUNCTION )
    {
        lua_pushvalue( m_pLuaState, m_iFilterArgIndex );
        CBaseEntity::PushLuaInstanceSafe( m_pLuaState, EntityFromEntityHandle( pHandleEntity ) );
        luasrc_pcall( m_pLuaState, 1, 1 );

        if ( lua_isboolean( m_pLuaState, -1 ) )
        {
            bool bResult = lua_toboolean( m_pLuaState, -1 );
            lua_pop( m_pLuaState, 1 );  // Pop the result
            return bResult;
        }
        else if ( lua_toentity( m_pLuaState, -1 ) != NULL )
        {
            lua_CBaseEntity *pEntity = lua_toentity( m_pLuaState, -1 );
            lua_pop( m_pLuaState, 1 );  // Pop the result

            if ( pEntity == EntityFromEntityHandle( pHandleEntity ) )
            {
                return false;
            }

            return false;
        }

        luaL_error( m_pLuaState, "Filter function must return a boolean or valid entity" );
    }
    // Do not hit entities in the table, or entities that have one of the class names in the table
    else if ( m_iFilterType == LUA_TTABLE )
    {
        lua_pushvalue( m_pLuaState, m_iFilterArgIndex );

        lua_pushnil( m_pLuaState );  // Push a nil key to start iteration
        while ( lua_next( m_pLuaState, -2 ) )
        {
            if ( lua_type( m_pLuaState, -1 ) == LUA_TSTRING )
            {
                CBaseEntity *pEntity = EntityFromEntityHandle( pHandleEntity );
                if ( pEntity && FClassnameIs( pEntity, lua_tostring( m_pLuaState, -1 ) ) )
                {
                    lua_pop( m_pLuaState, 2 );  // Pop the value and the key
                    return m_bFilterTableInverted;
                }
            }
            else
            {
                lua_CBaseEntity *pEntity = lua_toentity( m_pLuaState, -1 );

                // Disabled for now, since NULL may cause this error
                /* if ( !pEntity )
                {
                    luaL_typeerror( m_pLuaState, -1, "Entity or string" );
                }
                else*/
                if ( pEntity == EntityFromEntityHandle( pHandleEntity ) )
                {
                    lua_pop( m_pLuaState, 2 );  // Pop the value and the key
                    return m_bFilterTableInverted;
                }
            }

            lua_pop( m_pLuaState, 1 );  // Pop the value, keep the key
        }

        lua_pop( m_pLuaState, 1 );  // Pop the table
        return CTraceFilterSimple::ShouldHitEntity( pHandleEntity, contentsMask ) != m_bFilterTableInverted;
    }
    else if ( m_pPassEntity && m_pPassEntity == EntityFromEntityHandle( pHandleEntity ) )
    {
        return false;
    }

    return CTraceFilterSimple::ShouldHitEntity( pHandleEntity, contentsMask );
}

#ifdef CLIENT_DLL
C_BaseEntity *CTraceLuaFilter::GetWorldEntity( void )
{
    return GetClientWorldEntity();
}
#endif

// TODO: Actually push trace_t so it can be used by the likes of UTIL_BloodDecalTrace
LUA_API void lua_pushtrace_t( lua_State *L, trace_t *trace, bool bNoNewTable /* = false*/ )
{
    if ( !bNoNewTable )
        lua_newtable( L );

    // For gmod compatibility we create a table with more values
    const surfacedata_t *pSurfaceData = physprops->GetSurfaceData( trace->surface.surfaceProps );

    lua_pushstring( L, "Entity" );
    if ( trace->m_pEnt && trace->m_pEnt->IsWorld() )
    {
        // Instead of NULL, we return the world entity (like gmod does)
#ifdef CLIENT_DLL
        CBaseEntity::PushLuaInstanceSafe( L, GetClientWorldEntity() );
#else
        CBaseEntity::PushLuaInstanceSafe( L, GetWorldEntity() );
#endif
    }
    else
    {
        CBaseEntity::PushLuaInstanceSafe( L, trace->m_pEnt );
    }
    lua_settable( L, -3 );

    lua_pushstring( L, "Fraction" );
    lua_pushnumber( L, trace->fraction );
    lua_settable( L, -3 );

    lua_pushstring( L, "FractionLeftSolid" );
    lua_pushnumber( L, trace->fractionleftsolid );
    lua_settable( L, -3 );

    lua_pushstring( L, "Hit" );
    lua_pushboolean( L, trace->DidHit() );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitBox" );
    lua_pushinteger( L, trace->hitbox );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitGroup" );
    lua_pushinteger( L, trace->hitgroup );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitNoDraw" );
    lua_pushboolean( L, trace->DidHitNonWorldEntity() );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitNonWorld" );
    lua_pushboolean( L, trace->DidHitNonWorldEntity() );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitNormal" );
    lua_pushvector( L, trace->plane.normal );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitPos" );
    lua_pushvector( L, trace->endpos );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitSky" );
    lua_pushboolean( L, trace->surface.flags & SURF_SKY );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitTexture" );
    lua_pushstring( L, trace->surface.name );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitWorld" );
    lua_pushboolean( L, trace->DidHitWorld() );
    lua_settable( L, -3 );

    lua_pushstring( L, "MatType" );
    lua_pushinteger( L, pSurfaceData->game.material );
    lua_settable( L, -3 );

    Vector normal = trace->endpos - trace->startpos;
    VectorNormalizeFast( normal );
    lua_pushstring( L, "Normal" );
    lua_pushvector( L, normal );
    lua_settable( L, -3 );

    lua_pushstring( L, "PhysicsBone" );
    lua_pushinteger( L, trace->physicsbone );
    lua_settable( L, -3 );

    lua_pushstring( L, "StartPos" );
    lua_pushvector( L, trace->startpos );
    lua_settable( L, -3 );

    lua_pushstring( L, "SurfaceProps" );
    lua_pushinteger( L, trace->surface.surfaceProps );
    lua_settable( L, -3 );

    lua_pushstring( L, "StartSolid" );
    lua_pushboolean( L, trace->startsolid );
    lua_settable( L, -3 );

    lua_pushstring( L, "AllSolid" );
    lua_pushboolean( L, trace->allsolid );
    lua_settable( L, -3 );

    lua_pushstring( L, "SurfaceFlags" );
    lua_pushinteger( L, trace->surface.flags );
    lua_settable( L, -3 );

    lua_pushstring( L, "DispFlags" );
    lua_pushinteger( L, trace->dispFlags );
    lua_settable( L, -3 );

    lua_pushstring( L, "Contents" );
    lua_pushinteger( L, trace->contents );
    lua_settable( L, -3 );
}

LUA_API void lua_checktracestruct( lua_State *L, int argIndex )
{
    luaL_checktype( L, 1, LUA_TTABLE );
}

/*
LUA_API lua_FireBulletsInfo_t lua_tofirebulletsinfo( lua_State *L, int argIndex, lua_CBaseEntity *self )
{
    // Let's ensure that the bullet info is a table
    luaL_checktype( L, argIndex, LUA_TTABLE );

    FireBulletsInfo_t info;

    // Internally this field seems to be used for game stats
    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "IsPrimaryAttack", "m_bPrimaryAttack" );
    info.m_bPrimaryAttack = lua_isnil( L, -1 ) ? true : luaL_checkboolean( L, -1 );
    lua_pop( L, 1 );  // pop the IsPrimaryAttack value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Force", "m_flDamageForceScale" );
    info.m_flDamageForceScale = lua_isnil( L, -1 ) ? 1.0f : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Force value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Distance", "m_flDistance" );
    info.m_flDistance = lua_isnil( L, -1 ) ? 56756.0f : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Distance value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "AmmoType", "m_iAmmoType" );
    if ( lua_isnumber( L, -1 ) )
        info.m_iAmmoType = luaL_checknumber( L, -1 );
    else if ( lua_isstring( L, -1 ) )
    {
        const char *ammoName = luaL_checkstring( L, -1 );
        info.m_iAmmoType = GetAmmoDef()->Index( ammoName );
    }
    else
        info.m_iAmmoType = 0;
    lua_pop( L, 1 );  // pop the AmmoType value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Damage", "m_flDamage" );
    info.m_flDamage = lua_isnil( L, -1 ) ? 1.0f : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Damage value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "PlayerDamage", "m_iPlayerDamage" );
    info.m_iPlayerDamage = lua_isnil( L, -1 ) ? info.m_flDamage : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the PlayerDamage value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "AmountOfBullets", "Num" );
    info.m_iShots = lua_isnil( L, -1 ) ? 1 : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the AmountOfBullets value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "TracerFrequency", "Tracer" );
    info.m_iTracerFreq = lua_isnil( L, -1 ) ? 1 : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Tracer value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "TracerName", "m_pszTracerType" );
    info.m_pszTracerType = lua_isnil( L, -1 ) ? NULL : strdup( luaL_checkstring( L, -1 ) );
    lua_pop( L, 1 );  // pop the TracerName value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Flags", "m_nFlags" );
    info.m_nFlags = lua_isnil( L, -1 ) ? 0 : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Flags value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "IgnoreEntity", "m_pAdditionalIgnoreEnt" );
    info.m_pAdditionalIgnoreEnt = lua_isnil( L, -1 ) ? NULL : lua_toentity( L, -1 );
    lua_pop( L, 1 );  // pop the IgnoreEntity value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Attacker", "m_pAttacker" );
    info.m_pAttacker = lua_isnil( L, -1 ) ? self : lua_toentity( L, -1 );
    lua_pop( L, 1 );  // pop the Attacker value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Direction", "Dir" );
    info.m_vecDirShooting = lua_isnil( L, -1 ) ? vec3_origin : luaL_checkvector( L, -1 );
    lua_pop( L, 1 );  // pop the Direction value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Spread", "m_vecSpread" );
    info.m_vecSpread = lua_isnil( L, -1 ) ? vec3_origin : luaL_checkvector( L, -1 );
    lua_pop( L, 1 );  // pop the Spread value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Origin", "Src" );
    info.m_vecSrc = lua_isnil( L, -1 ) ? vec3_origin : luaL_checkvector( L, -1 );
    lua_pop( L, 1 );  // pop the Origin value

    return info;
}*/

LUA_API void lua_checktracestruct( lua_State *L, int argIndex, Vector &start, Vector &end, int &mask, int &collisionGroup, bool &bIgnoreWorld, bool &bFilterTableInverted, bool &bOutput, CTraceLuaFilter **filter )
{
    lua_checktracestruct( L, argIndex );

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "StartPosition", "start" );
    start = Vector( 0, 0, 0 );
    start = luaL_optvector( L, -1, &start );
    lua_pop( L, 1 );  // pop the StartPosition value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "EndPosition", "endpos" );
    end = Vector( 0, 0, 0 );
    end = luaL_optvector( L, -1, &end );
    lua_pop( L, 1 );  // pop the EndPosition value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Mask", "mask" );
    mask = luaL_optnumber( L, -1, MASK_SOLID );
    lua_pop( L, 1 );  // pop the Mask value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "CollisionGroup", "collisiongroup" );
    collisionGroup = luaL_optnumber( L, -1, COLLISION_GROUP_NONE );
    lua_pop( L, 1 );  // pop the CollisionGroup value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "ShouldIgnoreWorld", "ignoreworld" );
    bIgnoreWorld = luaL_optboolean( L, -1, false );
    lua_pop( L, 1 );  // pop the ShouldIgnoreWorld value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "IsFilterAllowList", "whitelist" );
    bFilterTableInverted = luaL_optboolean( L, -1, false );
    lua_pop( L, 1 );  // pop the IsFilterAllowList value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Output", "output" );
    bOutput = !lua_isnil( L, -1 );
    lua_pop( L, 1 );  // pop the Output value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Filter", "filter" );
    *filter = new CTraceLuaFilter( L, -1, collisionGroup, bIgnoreWorld, bFilterTableInverted );
    // lua_pop( L, 1 ); // Don't pop the filter. CTraceLuaFilter will handle it, leaving the filter on the stack if it's a function
}

LUA_BINDING_BEGIN( Traces, TraceLine, "library", "Trace a line." )
{
    Vector vecStart, vecEnd;
    int mask, collisionGroup;
    bool bIgnoreWorld, bFilterTableInverted, bOutput;
    CTraceLuaFilter *filter = nullptr;

    LUA_BINDING_ARGUMENT( lua_checktracestruct, 1, "trace" );
    lua_checktracestruct( L, 1, vecStart, vecEnd, mask, collisionGroup, bIgnoreWorld, bFilterTableInverted, bOutput, &filter );

    trace_t gameTrace;
    Ray_t ray;
    ray.Init( vecStart, vecEnd );
    // enginetrace->TraceRay( ray, mask, filter, &gameTrace );
    UTIL_TraceLine( vecStart, vecEnd, mask, filter, &gameTrace );

    if ( bOutput )
    {
        // Let users pass a table to get the output placed in, instead of returning it
        lua_getfield( L, 1, "output" );
        lua_pushtrace_t( L, &gameTrace, true );
        return 0;
    }

    lua_newtable( L );
    lua_pushtrace_t( L, &gameTrace );

    return 1;
}
LUA_BINDING_END( "TraceResult", "The trace result." )

LUA_BINDING_BEGIN( Traces, TraceHull, "library", "Trace a hull." )
{
    Vector vecStart, vecEnd;
    Vector vecMins, vecMaxs;
    int mask, collisionGroup;
    bool bIgnoreWorld, bFilterTableInverted, bOutput;
    CTraceLuaFilter *filter = nullptr;

    LUA_BINDING_ARGUMENT( lua_checktracestruct, 1, "trace" );
    lua_checktracestruct( L, 1, vecStart, vecEnd, mask, collisionGroup, bIgnoreWorld, bFilterTableInverted, bOutput, &filter );

    trace_t gameTrace;
    Ray_t ray;
    ray.Init( vecStart, vecEnd, vecMins, vecMaxs );
    UTIL_TraceHull( vecStart, vecEnd, vecMins, vecMaxs, mask, filter, &gameTrace );

    if ( bOutput )
    {
        // Let users pass a table to get the output placed in, instead of returning it
        lua_getfield( L, 1, "output" );
        lua_pushtrace_t( L, &gameTrace, true );
        return 0;
    }

    lua_newtable( L );
    lua_pushtrace_t( L, &gameTrace );

    return 1;
}
LUA_BINDING_END( "TraceResult", "The trace result." )

LUA_BINDING_BEGIN( Traces, TraceEntity, "library", "Runs a trace using the entity's collisionmodel between two points. This does not take the entity's angles into account and will trace its unrotated collisionmodel." )
{
    lua_CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" );
    Vector vecStart, vecEnd;
    int mask, collisionGroup;
    bool bIgnoreWorld, bFilterTableInverted, bOutput;
    CTraceLuaFilter *filter = nullptr;

    LUA_BINDING_ARGUMENT( lua_checktracestruct, 1, "trace" );
    lua_checktracestruct( L, 1, vecStart, vecEnd, mask, collisionGroup, bIgnoreWorld, bFilterTableInverted, bOutput, &filter );

    trace_t gameTrace;
    UTIL_TraceEntity( entity, vecStart, vecEnd, mask, filter, &gameTrace );

    if ( bOutput )
    {
        // Let users pass a table to get the output placed in, instead of returning it
        lua_getfield( L, 2, "output" );
        lua_pushtrace_t( L, &gameTrace, true );
        return 0;
    }

    lua_newtable( L );
    lua_pushtrace_t( L, &gameTrace );

    return 1;
}
LUA_BINDING_END( "TraceResult", "The trace result." )

LUA_BINDING_BEGIN( Traces, PointContents, "library", "Returns the contents mask + entity at a particular world-space position." )
{
    lua_pushinteger( L, UTIL_PointContents( LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "point" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "Bitwise contents mask." )

LUA_BINDING_BEGIN( Traces, TraceModel, "library", "Sweeps against a particular model, using collision rules." )
{
    Vector vecStart, vecEnd, vecMins, vecMaxs;
    lua_CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 5, "entity" );

    vecStart = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    vecEnd = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    vecMins = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "hullMinimum" );
    vecMaxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "hullMaximum" );
    int collisionGroup = luaL_checknumber( L, 6 );

    trace_t gameTrace;
    UTIL_TraceModel( vecStart, vecEnd, vecMins, vecMaxs, entity, collisionGroup, &gameTrace );

    lua_pushtrace_t( L, &gameTrace );

    return 1;
}
LUA_BINDING_END( "TraceResult", "The trace result." )

#ifdef GAME_DLL

LUA_BINDING_BEGIN( Traces, ClearTrace, "library", "Clears the given trace object.", "server" )
{
    lua_CGameTrace &trace = LUA_BINDING_ARGUMENT( luaL_checktrace, 1, "trace" );
    UTIL_ClearTrace( trace );
    return 0;
}
LUA_BINDING_END()

#endif  // GAME_DLL

/*
** Open CGameTrace object
*/
LUALIB_API int luaopen_CGameTrace( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_GAMETRACEMETANAME );

    LUA_REGISTRATION_COMMIT( Trace );

    lua_pushstring( L, LUA_GAMETRACEMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Trace" */
    lua_pop( L, 1 );                 /* pop metatable */

    LUA_REGISTRATION_COMMIT_LIBRARY( Traces );

    return 1;
}
