//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "lgametrace.h"
#include "mathlib/lvector.h"
#include "collisionutils.h"
#include "util_shared.h"
#include "datacache/imdlcache.h"
#include "tier3/tier3.h"
#include "base64.h"

#ifdef CLIENT_DLL
#include "cdll_util.h"
#include "c_world.h"
#else
#include "world.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// TODO: Test if this function works as expected
static int luasrc_Util_IntersectRayWithOBB( lua_State *L )
{
    Vector vecRayStart = luaL_checkvector( L, 1 );
    Vector vecRayDelta = luaL_checkvector( L, 2 );
    Vector vecBoxOrigin = luaL_checkvector( L, 3 );
    QAngle angBoxAngles = luaL_checkangle( L, 4 );
    Vector vecBoxMins = luaL_checkvector( L, 5 );
    Vector vecBoxMaxs = luaL_checkvector( L, 6 );

    matrix3x4_t matOBBToWorld;
    AngleMatrix( angBoxAngles, vecBoxOrigin, matOBBToWorld );

    CBaseTrace tr;
    if ( IntersectRayWithOBB( vecRayStart, vecRayDelta, matOBBToWorld, vecBoxMins, vecBoxMaxs, 0.0f, &tr ) )
    {
        lua_pushvector( L, tr.endpos );
        lua_pushvector( L, tr.plane.normal );
        lua_pushnumber( L, tr.fraction );
    }
    else
    {
        lua_pushnil( L );
        lua_pushnil( L );
        lua_pushnil( L );
    }

    return 3;
}

static int luasrc_Util_IntersectRayWithPlane( lua_State *L )
{
    Vector vecRayOrigin = luaL_checkvector( L, 1 );
    Vector vecRayDirection = luaL_checkvector( L, 2 );
    Vector vecPlaneOrigin = luaL_checkvector( L, 3 );
    Vector vecPlaneNormal = luaL_checkvector( L, 4 );

    float flDot = DotProduct( vecPlaneNormal, vecRayDirection );

    // If the ray is parallel to the plane, there is no intersection.
    if ( flDot == 0.0f )
    {
        lua_pushnil( L );
        return 1;
    }

    float flDistance = DotProduct( vecPlaneOrigin, vecPlaneNormal ) - DotProduct( vecRayOrigin, vecPlaneNormal );

    Vector vecHitPos = vecRayOrigin + vecRayDirection * ( flDistance / flDot );

    lua_pushvector( L, vecHitPos );

    return 1;
}

static int lua_Util_IntersectRayWithSphere( lua_State *L )
{
    Vector vecRayOrigin = luaL_checkvector( L, 1 );
    Vector vecRayDelta = luaL_checkvector( L, 2 );
    Vector vecSphereCenter = luaL_checkvector( L, 3 );
    float flRadius = luaL_checknumber( L, 4 );

    float t1, t2;
    if ( IntersectRayWithSphere( vecRayOrigin, vecRayDelta, vecSphereCenter, flRadius, &t1, &t2 ) )
    {
        lua_pushnumber( L, t1 );
        lua_pushnumber( L, t2 );
        return 2;
    }

    lua_pushnil( L );
    lua_pushnil( L );
    return 2;
}

static int lua_Util_IsBoxIntersectingSphere( lua_State *L )
{
    Vector boxMin = luaL_checkvector( L, 1 );
    Vector boxMax = luaL_checkvector( L, 2 );
    Vector spherePosition = luaL_checkvector( L, 3 );
    float sphereRadius = luaL_checknumber( L, 4 );

    lua_pushboolean( L, IsBoxIntersectingSphere( boxMin, boxMax, spherePosition, sphereRadius ) );
    return 1;
}

static int lua_Util_IsOBBIntersectingOBB( lua_State *L )
{
    Vector vecOrigin1 = luaL_checkvector( L, 1 );
    QAngle vecAngles1 = luaL_checkangle( L, 2 );
    Vector boxMin1 = luaL_checkvector( L, 3 );
    Vector boxMax1 = luaL_checkvector( L, 4 );

    Vector vecOrigin2 = luaL_checkvector( L, 5 );
    QAngle vecAngles2 = luaL_checkangle( L, 6 );
    Vector boxMin2 = luaL_checkvector( L, 7 );
    Vector boxMax2 = luaL_checkvector( L, 8 );

    float flTolerance = luaL_optnumber( L, 9, 0.0f );

    lua_pushboolean( L, IsOBBIntersectingOBB( vecOrigin1, vecAngles1, boxMin1, boxMax1, vecOrigin2, vecAngles2, boxMin2, boxMax2, flTolerance ) );
    return 1;
}

static int lua_Util_IsSphereIntersectingCone( lua_State *L )
{
    Vector sphereCenter = luaL_checkvector( L, 1 );
    float sphereRadius = luaL_checknumber( L, 2 );
    Vector coneOrigin = luaL_checkvector( L, 3 );
    Vector coneAxis = luaL_checkvector( L, 4 );
    float coneSine = luaL_checknumber( L, 5 );
    float coneCosine = luaL_checknumber( L, 6 );

    lua_pushboolean( L, IsSphereIntersectingCone( sphereCenter, sphereRadius, coneOrigin, coneAxis, coneSine, coneCosine ) );
    return 1;
}

static int lua_Util_IsSphereIntersectingSphere( lua_State *L )
{
    Vector sphereCenter1 = luaL_checkvector( L, 1 );
    float sphereRadius1 = luaL_checknumber( L, 2 );
    Vector sphereCenter2 = luaL_checkvector( L, 3 );
    float sphereRadius2 = luaL_checknumber( L, 4 );

    lua_pushboolean( L, IsSphereIntersectingSphere( sphereCenter1, sphereRadius1, sphereCenter2, sphereRadius2 ) );
    return 1;
}

static int luasrc_Util_VecToYaw( lua_State *L )
{
    lua_pushnumber( L, UTIL_VecToYaw( luaL_checkvector( L, 1 ) ) );
    return 1;
}

static int luasrc_Util_VecToPitch( lua_State *L )
{
    lua_pushnumber( L, UTIL_VecToPitch( luaL_checkvector( L, 1 ) ) );
    return 1;
}

static int luasrc_Util_YawToVector( lua_State *L )
{
    Vector v = UTIL_YawToVector( luaL_checknumber( L, 1 ) );
    lua_pushvector( L, v );
    return 1;
}

static int luasrc_SharedRandomFloat( lua_State *L )
{
    lua_pushnumber( L, SharedRandomFloat( luaL_checkstring( L, 1 ), luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ), luaL_optint( L, 4, 0 ) ) );
    return 1;
}

static int luasrc_SharedRandomInt( lua_State *L )
{
    lua_pushinteger( L, SharedRandomInt( luaL_checkstring( L, 1 ), luaL_checkint( L, 2 ), luaL_checkint( L, 3 ), luaL_optint( L, 4, 0 ) ) );
    return 1;
}

static int luasrc_SharedRandomVector( lua_State *L )
{
    lua_pushvector( L, SharedRandomVector( luaL_checkstring( L, 1 ), luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ), luaL_optint( L, 4, 0 ) ) );
    return 1;
}

static int luasrc_SharedRandomAngle( lua_State *L )
{
    lua_pushangle( L, SharedRandomAngle( luaL_checkstring( L, 1 ), luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ), luaL_optint( L, 4, 0 ) ) );
    return 1;
}

// TODO: Move this to seperate file and trace library
class CTraceLuaFilter : public CTraceFilterSimple
{
   public:
    CTraceLuaFilter( lua_State *L, int filterArg, int collisionGroup, bool bIgnoreWorld, bool bFilterTableInverted = false )
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

    virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
    {
        if ( m_bIgnoreWorld && pHandleEntity == GetWorldEntity() )
            return false;

        // Do not hit entities returned by the function, or have the function return a
        // boolean to determine if the entity should be hit
        if ( m_iFilterType == LUA_TFUNCTION )
        {
            lua_pushvalue( m_pLuaState, m_iFilterArgIndex );
            lua_pushentity( m_pLuaState, EntityFromEntityHandle( pHandleEntity ) );
            lua_call( m_pLuaState, 1, 1 );

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

            lua_pushnil( m_pLuaState ); // Push a nil key to start iteration
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

                    if ( !pEntity )
                    {
                        luaL_typeerror( m_pLuaState, -1, "Entity or string" );
                    }
                    else if ( pEntity == EntityFromEntityHandle( pHandleEntity ) )
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

   private:
    const IHandleEntity *m_pPassEntity;
    lua_State *m_pLuaState;
    int m_iFilterType;
    int m_iFilterArgIndex;
    bool m_bIgnoreWorld;
    bool m_bFilterTableInverted;

#ifdef CLIENT_DLL  // Server has its own GetWorldEntity in world.h
    C_BaseEntity *GetWorldEntity( void )
    {
        return GetClientWorldEntity();
    }
#endif
};

static int luasrc_Util_TraceLine( lua_State *L )
{
    // For quick compatibility with gmod we use its inconsistent naming in the trace table
    luaL_checktype( L, 1, LUA_TTABLE );

    lua_getfield( L, 1, "start" );
    Vector vecStart = Vector( 0, 0, 0 );
    vecStart = luaL_optvector( L, -1, &vecStart );
    lua_pop( L, 1 );

    lua_getfield( L, 1, "endpos" );
    Vector vecEnd = Vector( 0, 0, 0 );
    vecEnd = luaL_optvector( L, -1, &vecEnd );
    lua_pop( L, 1 );

    lua_getfield( L, 1, "mask" );
    int mask = luaL_optnumber( L, -1, MASK_SOLID );
    lua_pop( L, 1 );

    lua_getfield( L, 1, "collisiongroup" );
    int collisionGroup = luaL_optnumber( L, -1, COLLISION_GROUP_NONE );
    lua_pop( L, 1 );

    lua_getfield( L, 1, "ignoreworld" );
    bool bIgnoreWorld = luaL_optboolean( L, -1, false );
    lua_pop( L, 1 );

    lua_getfield( L, 1, "whitelist" );
    bool bFilterTableInverted = luaL_optboolean( L, -1, false );
    lua_pop( L, 1 );

    lua_getfield( L, 1, "filter" );
    CTraceLuaFilter traceFilter( L, -1, collisionGroup, bIgnoreWorld, bFilterTableInverted );

    trace_t gameTrace;
    Ray_t ray;
    ray.Init( vecStart, vecEnd );
    enginetrace->TraceRay( ray, mask, &traceFilter, &gameTrace );

    // For gmod compatibility we create a table with more values
    const surfacedata_t *pSurfaceData = physprops->GetSurfaceData( gameTrace.surface.surfaceProps );

    // Let users pass a table to get the output placed in
    lua_getfield( L, 1, "output" );

    if ( lua_isnil( L, -1 ) )
    {
        lua_pop( L, 1 ); // Pop the nil value
        lua_newtable( L );
    }
    else
    {
        luaL_checktype( L, -1, LUA_TTABLE );
    }

    lua_pushstring( L, "Entity" );
    lua_pushentity( L, gameTrace.m_pEnt );
    lua_settable( L, -3 );

    lua_pushstring( L, "Fraction" );
    lua_pushnumber( L, gameTrace.fraction );
    lua_settable( L, -3 );

    lua_pushstring( L, "FractionLeftSolid" );
    lua_pushnumber( L, gameTrace.fractionleftsolid );
    lua_settable( L, -3 );

    lua_pushstring( L, "Hit" );
    lua_pushboolean( L, gameTrace.DidHit() );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitBox" );
    lua_pushinteger( L, gameTrace.hitbox );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitGroup" );
    lua_pushinteger( L, gameTrace.hitgroup );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitNoDraw" );
    lua_pushboolean( L, gameTrace.DidHitNonWorldEntity() );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitNonWorld" );
    lua_pushboolean( L, gameTrace.DidHitNonWorldEntity() );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitNormal" );
    lua_pushvector( L, gameTrace.plane.normal );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitPos" );
    lua_pushvector( L, gameTrace.endpos );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitSky" );
    lua_pushboolean( L, gameTrace.surface.flags & SURF_SKY );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitTexture" );
    lua_pushstring( L, gameTrace.surface.name );
    lua_settable( L, -3 );

    lua_pushstring( L, "HitWorld" );
    lua_pushboolean( L, gameTrace.DidHitWorld() );
    lua_settable( L, -3 );

    lua_pushstring( L, "MatType" );
    lua_pushinteger( L, pSurfaceData->game.material );
    lua_settable( L, -3 );

    Vector normal = gameTrace.endpos - gameTrace.startpos;
    VectorNormalizeFast( normal );
    lua_pushstring( L, "Normal" );
    lua_pushvector( L, normal );
    lua_settable( L, -3 );

    lua_pushstring( L, "PhysicsBone" );
    lua_pushinteger( L, gameTrace.physicsbone );
    lua_settable( L, -3 );

    lua_pushstring( L, "StartPos" );
    lua_pushvector( L, gameTrace.startpos );
    lua_settable( L, -3 );

    lua_pushstring( L, "SurfaceProps" );
    lua_pushinteger( L, gameTrace.surface.surfaceProps );
    lua_settable( L, -3 );

    lua_pushstring( L, "StartSolid" );
    lua_pushboolean( L, gameTrace.startsolid );
    lua_settable( L, -3 );

    lua_pushstring( L, "AllSolid" );
    lua_pushboolean( L, gameTrace.allsolid );
    lua_settable( L, -3 );

    lua_pushstring( L, "SurfaceFlags" );
    lua_pushinteger( L, gameTrace.surface.flags );
    lua_settable( L, -3 );

    lua_pushstring( L, "DispFlags" );
    lua_pushinteger( L, gameTrace.dispFlags );
    lua_settable( L, -3 );

    lua_pushstring( L, "Contents" );
    lua_pushinteger( L, gameTrace.contents );
    lua_settable( L, -3 );

    return 1;
}

static int luasrc_Util_TraceHull( lua_State *L )
{
    UTIL_TraceHull( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ), luaL_checkvector( L, 3 ), luaL_checkvector( L, 4 ), luaL_checkint( L, 5 ), luaL_checkentity( L, 6 ), luaL_checkint( L, 7 ), &luaL_checktrace( L, 8 ) );
    return 0;
}

static int luasrc_Util_TraceEntity( lua_State *L )
{
    UTIL_TraceEntity( luaL_checkentity( L, 1 ), luaL_checkvector( L, 2 ), luaL_checkvector( L, 3 ), luaL_checkint( L, 4 ), luaL_checkentity( L, 5 ), luaL_checkint( L, 5 ), &luaL_checktrace( L, 6 ) );
    return 0;
}

static int luasrc_Util_EntityHasMatchingRootParent( lua_State *L )
{
    lua_pushboolean( L, UTIL_EntityHasMatchingRootParent( luaL_checkentity( L, 1 ), luaL_checkentity( L, 2 ) ) );
    return 1;
}

static int luasrc_Util_PointContents( lua_State *L )
{
    lua_pushinteger( L, UTIL_PointContents( luaL_checkvector( L, 1 ) ) );
    return 1;
}

static int luasrc_Util_TraceModel( lua_State *L )
{
    UTIL_TraceModel( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ), luaL_checkvector( L, 3 ), luaL_checkvector( L, 4 ), luaL_checkentity( L, 5 ), luaL_checkint( L, 6 ), &luaL_checktrace( L, 7 ) );
    return 0;
}

static int luasrc_Util_ParticleTracer( lua_State *L )
{
    UTIL_ParticleTracer( luaL_checkstring( L, 1 ), luaL_checkvector( L, 2 ), luaL_checkvector( L, 3 ), luaL_optint( L, 4, 0 ), luaL_optint( L, 5, 0 ), luaL_optboolean( L, 6, 0 ) );
    return 0;
}

static int luasrc_Util_Tracer( lua_State *L )
{
    UTIL_Tracer( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ), luaL_optint( L, 3, 0 ), luaL_optint( L, 4, -1 ), luaL_optnumber( L, 5, 0 ), luaL_optboolean( L, 6, 0 ), luaL_optstring( L, 7, 0 ), luaL_optint( L, 8, 0 ) );
    return 0;
}

static int luasrc_Util_BloodDrips( lua_State *L )
{
    UTIL_BloodDrips( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ), luaL_checkint( L, 3 ), luaL_checkint( L, 4 ) );
    return 0;
}

static int luasrc_Util_IsLowViolence( lua_State *L )
{
    lua_pushboolean( L, UTIL_IsLowViolence() );
    return 1;
}

static int luasrc_Util_IsValidModel( lua_State *L )
{
    const char *pszModel = luaL_checkstring( L, 1 );

    // Invalid if the string starts with space or maps
    if ( pszModel[0] == ' ' || !Q_strncmp( pszModel, "maps", 4 ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    // Invalid if the string doesn't start with models
    if ( Q_strncmp( pszModel, "models", 6 ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    // Invalid if the string contains any of the following
    if ( Q_strstr( pszModel, "_gestures" ) || Q_strstr( pszModel, "_animations" ) || Q_strstr( pszModel, "_postures" ) || Q_strstr( pszModel, "_gst" ) || Q_strstr( pszModel, "_pst" ) || Q_strstr( pszModel, "_shd" ) || Q_strstr( pszModel, "_ss" ) || Q_strstr( pszModel, "_anm" ) || Q_strstr( pszModel, ".bsp" ) || Q_strstr( pszModel, "cs_fix" ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    int iModelIndex = CBaseEntity::PrecacheModel( pszModel );

    if ( iModelIndex == MDLHANDLE_INVALID )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    const model_t *pModel = modelinfo->GetModel( iModelIndex );

    if ( !pModel )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    studiohdr_t *pStudioModel = modelinfo->GetStudiomodel( pModel );

    if ( !pStudioModel || Q_strcmp( pStudioModel->name, "error.mdl" ) == 0 )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    lua_pushboolean( L, true );
    return 1;
}

static int luasrc_Util_IsValidPhysicsProp( lua_State *L )
{
    const char *pszModel = luaL_checkstring( L, 1 );

    const MDLHandle_t modelHandle = g_pMDLCache->FindMDL( pszModel );

    if ( modelHandle == MDLHANDLE_INVALID )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    studiohdr_t *pStudioModel = g_pMDLCache->GetStudioHdr( modelHandle );

    if ( !pStudioModel )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    if ( !mdlcache->GetVCollide( modelHandle ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    lua_pushboolean( L, true );
    return 1;
}

static int luasrc_Util_ShouldShowBlood( lua_State *L )
{
    lua_pushboolean( L, UTIL_ShouldShowBlood( luaL_checkint( L, 1 ) ) );
    return 1;
}

static int luasrc_Util_BloodImpact( lua_State *L )
{
    UTIL_BloodImpact( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ), luaL_checkint( L, 3 ), luaL_checkint( L, 4 ) );
    return 0;
}

static int luasrc_Util_BloodDecalTrace( lua_State *L )
{
    UTIL_BloodDecalTrace( &luaL_checktrace( L, 1 ), luaL_checkint( L, 2 ) );
    return 0;
}

static int luasrc_Util_DecalTrace( lua_State *L )
{
    UTIL_DecalTrace( &luaL_checktrace( L, 1 ), luaL_checkstring( L, 2 ) );
    return 0;
}

static int luasrc_Util_IsSpaceEmpty( lua_State *L )
{
    lua_pushboolean( L, UTIL_IsSpaceEmpty( luaL_checkentity( L, 1 ), luaL_checkvector( L, 2 ), luaL_checkvector( L, 3 ) ) );
    return 1;
}

static int CBasePlayer_GetAllBots( lua_State *L )
{
    CUtlVector< CBasePlayer * > curPlayers;
    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
        if ( pPlayer && ( pPlayer->GetFlags() & FL_FAKECLIENT ) )
        {
            curPlayers.AddToTail( pPlayer );
        }
    }
    lua_newtable( L );
    for ( int i = 0; i < curPlayers.Count(); i++ )
    {
        lua_pushplayer( L, curPlayers[i] );
        lua_rawseti( L, -2, i + 1 );
    }
    return 1;
}

static int CBasePlayer_GetAllHumans( lua_State *L )
{
    CUtlVector< CBasePlayer * > curPlayers;
    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
        if ( pPlayer && !( pPlayer->GetFlags() & FL_FAKECLIENT ) )
        {
            curPlayers.AddToTail( pPlayer );
        }
    }
    lua_newtable( L );
    for ( int i = 0; i < curPlayers.Count(); i++ )
    {
        lua_pushplayer( L, curPlayers[i] );
        lua_rawseti( L, -2, i + 1 );
    }
    return 1;
}

static int CBasePlayer_GetAllPlayers( lua_State *L )
{
    CUtlVector< CBasePlayer * > curPlayers;
    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
        if ( pPlayer )
        {
            curPlayers.AddToTail( pPlayer );
        }
    }
    lua_newtable( L );
    for ( int i = 0; i < curPlayers.Count(); i++ )
    {
        lua_pushplayer( L, curPlayers[i] );
        lua_rawseti( L, -2, i + 1 );
    }
    return 1;
}

static int luasrc_Util_PlayerByIndex( lua_State *L )
{
    lua_pushplayer( L, UTIL_PlayerByIndex( luaL_checkinteger( L, 1 ) ) );
    return 1;
}

static int luasrc_Util_PlayerByUserId( lua_State *L )
{
    lua_pushplayer( L, UTIL_PlayerByUserId( luaL_checkinteger( L, 1 ) ) );
    return 1;
}

static int luasrc_Util_PlayerByName( lua_State *L )
{
    lua_pushplayer( L, UTIL_PlayerByName( luaL_checkstring( L, 1 ) ) );
    return 1;
}

#define MAX_ENTITYARRAY 1024

static int luasrc_Util_EntitiesAlongRay( lua_State *L )
{
    CBaseEntity *pList[MAX_ENTITYARRAY];

    Ray_t ray;
    ray.Init( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ), luaL_checkvector( L, 3 ), luaL_checkvector( L, 4 ) );
    int count = UTIL_EntitiesAlongRay( pList, MAX_ENTITYARRAY, ray, UINT_MAX );
    lua_pushinteger( L, count );
    lua_newtable( L );
    for ( int i = 0; i < count; i++ )
    {
        lua_pushinteger( L, i );
        lua_pushentity( L, pList[i] );
        lua_settable( L, -3 );
    }
    return 2;
}

static int luasrc_Util_EntitiesInBox( lua_State *L )
{
    CBaseEntity *pList[MAX_ENTITYARRAY];

    int count = UTIL_EntitiesInBox( pList, luaL_checkinteger( L, 1 ), luaL_checkvector( L, 2 ), luaL_checkvector( L, 3 ), luaL_checkinteger( L, 4 ) );
    lua_pushinteger( L, count );
    lua_newtable( L );
    for ( int i = 0; i < count; i++ )
    {
        lua_pushinteger( L, i );
        lua_pushentity( L, pList[i] );
        lua_settable( L, -3 );
    }
    return 2;
}

static int luasrc_Util_EntitiesInSphere( lua_State *L )
{
    CBaseEntity *pList[MAX_ENTITYARRAY];

    int count = UTIL_EntitiesInSphere( pList, luaL_checkinteger( L, 1 ), luaL_checkvector( L, 2 ), luaL_checknumber( L, 3 ), luaL_checkinteger( L, 4 ) );
    lua_pushinteger( L, count );
    lua_newtable( L );
    for ( int i = 0; i < count; i++ )
    {
        lua_pushinteger( L, i );
        lua_pushentity( L, pList[i] );
        lua_settable( L, -3 );
    }
    return 2;
}

#ifndef CLIENT_DLL
/// <summary>
/// Goes through the entities and find the ones that are in the PVS of the provided vector.
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
static int luasrc_Util_EntitiesInPVS( lua_State *L )
{
    Vector vecViewOrigin = luaL_checkvector( L, 1 );

    static byte pvs[MAX_MAP_CLUSTERS / 8];
    int clusterIndex = engine->GetClusterForOrigin( vecViewOrigin );
    Assert( clusterIndex >= 0 );
    engine->GetPVSForCluster( clusterIndex, sizeof( pvs ), pvs );

    int count = 0;

    lua_newtable( L );

    for ( CBaseEntity *pEntity = gEntList.NextEnt( NULL ); pEntity; pEntity = gEntList.NextEnt( pEntity ) )
    {
        // Only return attached ents.
        if ( !pEntity->edict() )
            continue;

        CBaseEntity *pParent = pEntity->GetRootMoveParent();

        Vector vecSurroundMins, vecSurroundMaxs;
        pParent->CollisionProp()->WorldSpaceSurroundingBounds( &vecSurroundMins, &vecSurroundMaxs );
        if ( !engine->CheckBoxInPVS( vecSurroundMins, vecSurroundMaxs, pvs, sizeof( pvs ) ) )
            continue;

        lua_pushinteger( L, count );
        lua_pushentity( L, pEntity );
        lua_settable( L, -3 );
        count++;

        if ( count >= MAX_ENTITYARRAY )
        {
            DevWarning( "UTIL_EntitiesInPVS: Reached max (%d) entities in PVS (skipping the rest)\n", MAX_ENTITYARRAY );
            break;
        }
    }

    // insert the count before the table
    lua_pushinteger( L, count );
    lua_insert( L, -2 );

    return 2;
}
#endif

static int luasrc_Util_WaterLevel( lua_State *L )
{
    lua_pushnumber( L, UTIL_WaterLevel( luaL_checkvector( L, 1 ), luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) ) );
    return 1;
}

static int luasrc_Util_CRC32( lua_State *L )
{
    const char *pszString = luaL_checkstring( L, 1 );

    lua_pushinteger( L, CRC32_ProcessSingleBuffer( pszString, Q_strlen( pszString ) ) );
    return 1;
}

static int luasrc_Util_Base64Encode( lua_State *L )
{
    const char *pszString = luaL_checkstring( L, 1 );
    bool bDisableRfc2045Compliance = luaL_optboolean( L, 2, false );

    std::string encodedString;

    if ( bDisableRfc2045Compliance )
        encodedString = base64_encode( pszString, false );
    else
        encodedString = base64_encode_mime( pszString );

    lua_pushstring( L, encodedString.c_str() );

    return 1;
}

static int luasrc_Util_Base64Decode( lua_State *L )
{
    const char *pszString = luaL_checkstring( L, 1 );
    std::string decodedString = base64_decode( pszString );

    lua_pushstring( L, decodedString.c_str() );

    return 1;
}

static const luaL_Reg util_funcs[] = {
    { "IntersectRayWithPlane", luasrc_Util_IntersectRayWithPlane },
    { "IntersectRayWithOBB", luasrc_Util_IntersectRayWithOBB },
    { "IntersectRayWithSphere", lua_Util_IntersectRayWithSphere },
    { "IsBoxIntersectingSphere", lua_Util_IsBoxIntersectingSphere },
    { "IsOBBIntersectingOBB", lua_Util_IsOBBIntersectingOBB },
    { "IsSphereIntersectingCone", lua_Util_IsSphereIntersectingCone },
    { "IsSphereIntersectingSphere", lua_Util_IsSphereIntersectingSphere },
    { "VecToYaw", luasrc_Util_VecToYaw },
    { "VecToPitch", luasrc_Util_VecToPitch },
    { "YawToVector", luasrc_Util_YawToVector },
    { "SharedRandomFloat", luasrc_SharedRandomFloat },
    { "SharedRandomInt", luasrc_SharedRandomInt },
    { "SharedRandomVector", luasrc_SharedRandomVector },
    { "SharedRandomAngle", luasrc_SharedRandomAngle },
    { "TraceLine", luasrc_Util_TraceLine },
    { "TraceHull", luasrc_Util_TraceHull },
    { "TraceEntity", luasrc_Util_TraceEntity },
    { "EntityHasMatchingRootParent", luasrc_Util_EntityHasMatchingRootParent },
    { "PointContents", luasrc_Util_PointContents },
    { "TraceModel", luasrc_Util_TraceModel },
    { "ParticleTracer", luasrc_Util_ParticleTracer },
    { "Tracer", luasrc_Util_Tracer },
    { "IsLowViolence", luasrc_Util_IsLowViolence },
    { "IsValidModel", luasrc_Util_IsValidModel },
    { "IsValidPhysicsProp", luasrc_Util_IsValidPhysicsProp },
    { "ShouldShowBlood", luasrc_Util_ShouldShowBlood },
    { "BloodDrips", luasrc_Util_BloodDrips },
    { "BloodImpact", luasrc_Util_BloodImpact },
    { "BloodDecalTrace", luasrc_Util_BloodDecalTrace },
    { "DecalTrace", luasrc_Util_DecalTrace },
    { "IsSpaceEmpty", luasrc_Util_IsSpaceEmpty },
    { "GetAllPlayers", CBasePlayer_GetAllPlayers },
    { "GetAllBots", CBasePlayer_GetAllBots },
    { "GetAllHumans", CBasePlayer_GetAllHumans },
    { "PlayerByIndex", luasrc_Util_PlayerByIndex },
    { "PlayerByUserId", luasrc_Util_PlayerByUserId },
    { "PlayerByName", luasrc_Util_PlayerByName },
    { "PlayerByUserId", luasrc_Util_PlayerByUserId },
    { "PlayerByName", luasrc_Util_PlayerByName },
    { "EntitiesAlongRay", luasrc_Util_EntitiesAlongRay },
    { "EntitiesInBox", luasrc_Util_EntitiesInBox },
    { "EntitiesInSphere", luasrc_Util_EntitiesInSphere },
#ifndef CLIENT_DLL
    { "EntitiesInPVS", luasrc_Util_EntitiesInPVS },
#endif
    { "WaterLevel", luasrc_Util_WaterLevel },
    { "CRC32", luasrc_Util_CRC32 },
    { "Base64Encode", luasrc_Util_Base64Encode },
    { "Base64Decode", luasrc_Util_Base64Decode },
    { NULL, NULL } };

LUALIB_API int luaopen_Util_shared( lua_State *L )
{
    // luaL_register(L, LUA_GNAME, util_funcs);
    luaL_register( L, LUA_UTILLIBNAME, util_funcs );
    return 1;
}
