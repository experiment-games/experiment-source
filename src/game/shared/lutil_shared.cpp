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
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Utilities )

LUA_BINDING_BEGIN( Utilities, AngleDifference, "library", "Compute the difference between two angles" )
{
    float srcAngle = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "srcAngle" );
    float dstAngle = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "dstAngle" );
    lua_pushnumber( L, UTIL_AngleDiff( srcAngle, dstAngle ) );
    return 1;
}
LUA_BINDING_END( "number", "Difference between the angles." )

LUA_BINDING_BEGIN( Utilities, CreateBubbles, "library", "Create bubbles at the specified position and range" )
{
    Vector minPos = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "minPos" );
    Vector maxPos = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "maxPos" );
    int count = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "count" );
    UTIL_Bubbles( minPos, maxPos, count );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, ShakeScreen, "library", "Apply screen shake effect" )
{
    Vector center = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "center" );
    float amplitude = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "amplitude" );
    float frequency = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "frequency" );
    float duration = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "duration" );
    float radius = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "radius" );
    ShakeCommand_t command = ( ShakeCommand_t )( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "command" );
    bool localAmplitude = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 7, false, "localAmplitude" );
    UTIL_ScreenShake( center, amplitude, frequency, duration, radius, command, localAmplitude );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, CreateTracer, "library", "Create a tracer from the specified start to end position" )
{
    Vector startPos = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "startPos" );
    Vector endPos = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "endPos" );
    int life = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "life" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "width" );
    int brightness = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "brightness" );
    bool noise = LUA_BINDING_ARGUMENT( luaL_checkboolean, 6, "noise" );
    const char *modelname = LUA_BINDING_ARGUMENT( luaL_checkstring, 7, "modelname" );
    UTIL_Tracer( startPos, endPos, life, width, brightness, noise, modelname );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, CreateSmoke, "library", "Create smoke effect at a position" )
{
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "position" );
    int modelIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "modelIndex" );
    float scale = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "scale" );
    UTIL_Smoke( position, modelIndex, scale );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, ImpactTrace, "library", "Generate an impact trace" )
{
    trace_t *trace = &LUA_BINDING_ARGUMENT( luaL_checktrace, 1, "trace" );
    int damageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damageType" );
    char *customImpactName = ( char * )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, 0, "customImpactName" );
    UTIL_ImpactTrace( trace, damageType, customImpactName );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, SetEntityOrigin, "library", "Set the origin for an entity" )
{
    CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" );
    UTIL_SetOrigin( entity, origin );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, PrecacheOther, "library", "Precache an entity by class name" )
{
    const char *className = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "className" );
    UTIL_PrecacheOther( className );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, ClientPrint, "library", "Print message to client" )
{
    CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int msgDest = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "msgDest" );
    const char *message = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "message" );
    const char *param1 = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 4, 0, "param1" );
    const char *param2 = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 5, 0, "param2" );
    const char *param3 = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 6, 0, "param3" );
    const char *param4 = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 7, 0, "param4" );
    ClientPrint( player, msgDest, message, param1, param2, param3, param4 );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, BoundToWorldSize, "library", "Bound the vector to the world size" )
{
    Vector *vec = &LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vec" );
    UTIL_BoundToWorldSize( vec );
    return 0;
}
LUA_BINDING_END()

// TODO: Test if this function works as expected
LUA_BINDING_BEGIN( Utilities, IntersectRayWithObb, "library", "Intersect a ray with an oriented bounding box" )
{
    Vector vecRayStart = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vecRayStart" );
    Vector vecRayDelta = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vecRayDelta" );
    Vector vecBoxOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "vecBoxOrigin" );
    QAngle angBoxAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 4, "angBoxAngles" );
    Vector vecBoxMins = LUA_BINDING_ARGUMENT( luaL_checkvector, 5, "vecBoxMins" );
    Vector vecBoxMaxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 6, "vecBoxMaxs" );

    matrix3x4_t matObbToWorld;
    AngleMatrix( angBoxAngles, vecBoxOrigin, matObbToWorld );

    CBaseTrace tr;
    if ( IntersectRayWithOBB( vecRayStart, vecRayDelta, matObbToWorld, vecBoxMins, vecBoxMaxs, 0.0f, &tr ) )
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
LUA_BINDING_END( "Vector", "End position of the intersection", "Vector", "Normal of the intersection", "number", "Fraction of the intersection" )

LUA_BINDING_BEGIN( Utilities, IntersectRayWithPlane, "library", "Intersect a ray with a plane" )
{
    Vector vecRayOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vecRayOrigin" );
    Vector vecRayDirection = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vecRayDirection" );
    Vector vecPlaneOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "vecPlaneOrigin" );
    Vector vecPlaneNormal = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "vecPlaneNormal" );

    float flDot = DotProduct( vecPlaneNormal, vecRayDirection );

    // If the ray is parallel to the plane, there is no intersection.
    if ( flDot == 0.0f )
    {
        lua_pushnil( L );
        return 1;
    }

    float flDistance = DotProduct( vecPlaneNormal, vecPlaneOrigin ) - DotProduct( vecRayOrigin, vecPlaneNormal );

    Vector vecHitPos = vecRayOrigin + vecRayDirection * ( flDistance / flDot );

    lua_pushvector( L, vecHitPos );

    return 1;
}
LUA_BINDING_END( "Vector", "Intersection position" )

LUA_BINDING_BEGIN( Utilities, IntersectRayWithSphere, "library", "Intersect a ray with a sphere" )
{
    Vector vecRayOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vecRayOrigin" );
    Vector vecRayDelta = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vecRayDelta" );
    Vector vecSphereCenter = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "vecSphereCenter" );
    float flRadius = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "flRadius" );

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
LUA_BINDING_END( "number", "First intersection point", "number", "Second intersection point" )

LUA_BINDING_BEGIN( Utilities, IsBoxIntersectingSphere, "library", "Check if a box is intersecting a sphere" )
{
    Vector boxMin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "boxMin" );
    Vector boxMax = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "boxMax" );
    Vector spherePosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "spherePosition" );
    float sphereRadius = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "sphereRadius" );

    lua_pushboolean( L, IsBoxIntersectingSphere( boxMin, boxMax, spherePosition, sphereRadius ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the box is intersecting the sphere" )

LUA_BINDING_BEGIN( Utilities, IsObbIntersectingObb, "library", "Check if an oriented bounding box is intersecting another oriented bounding box" )
{
    Vector vecOrigin1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vecOrigin1" );
    QAngle vecAngles1 = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "vecAngles1" );
    Vector boxMin1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "boxMin1" );
    Vector boxMax1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "boxMax1" );

    Vector vecOrigin2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 5, "vecOrigin2" );
    QAngle vecAngles2 = LUA_BINDING_ARGUMENT( luaL_checkangle, 6, "vecAngles2" );
    Vector boxMin2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 7, "boxMin2" );
    Vector boxMax2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 8, "boxMax2" );

    float flTolerance = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 9, 0.0f, "flTolerance" );

    lua_pushboolean( L, IsOBBIntersectingOBB( vecOrigin1, vecAngles1, boxMin1, boxMax1, vecOrigin2, vecAngles2, boxMin2, boxMax2, flTolerance ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the oriented bounding boxes are intersecting" )

LUA_BINDING_BEGIN( Utilities, IsSphereIntersectingCone, "library", "Check if a sphere is intersecting a cone" )
{
    Vector sphereCenter = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "sphereCenter" );
    float sphereRadius = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sphereRadius" );
    Vector coneOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "coneOrigin" );
    Vector coneAxis = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "coneAxis" );
    float coneSine = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "coneSine" );
    float coneCosine = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "coneCosine" );

    lua_pushboolean( L, IsSphereIntersectingCone( sphereCenter, sphereRadius, coneOrigin, coneAxis, coneSine, coneCosine ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the sphere is intersecting the cone" )

LUA_BINDING_BEGIN( Utilities, IsSphereIntersectingSphere, "library", "Check if a sphere is intersecting another sphere" )
{
    Vector sphereCenter1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "sphereCenter1" );
    float sphereRadius1 = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sphereRadius1" );
    Vector sphereCenter2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "sphereCenter2" );
    float sphereRadius2 = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "sphereRadius2" );

    lua_pushboolean( L, IsSphereIntersectingSphere( sphereCenter1, sphereRadius1, sphereCenter2, sphereRadius2 ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the spheres are intersecting" )

LUA_BINDING_BEGIN( Utilities, PointOnLineNearestPoint, "library", "Returns the nearest point on a line to a given point." )
{
    Vector point = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "point" );
    Vector lineStart = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "lineStart" );
    Vector lineEnd = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "lineEnd" );
    bool clamp = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "clamp" );

    lua_pushvector( L, UTIL_PointOnLineNearestPoint( point, lineStart, lineEnd, clamp ) );
    return 1;
}
LUA_BINDING_END( "Vector", "The nearest point on the line." )

LUA_BINDING_BEGIN( Utilities, IsSpaceEmpty, "library", "Check if the space is empty" )
{
    CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector minPos = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "minPos" );
    Vector maxPos = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "maxPos" );

    lua_pushboolean( L, UTIL_IsSpaceEmpty( entity, minPos, maxPos ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the space is empty" )

LUA_BINDING_BEGIN( Utilities, GetPointWaterLevel, "library", "Get the water level at the specified position" )
{
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "point" );
    float minz = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "minz" );
    float maxz = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxz" );

    lua_pushnumber( L, UTIL_WaterLevel( position, minz, maxz ) );
    return 1;
}
LUA_BINDING_END( "number", "Water level at the specified position" )

LUA_BINDING_BEGIN( Utilities, IsLowViolence, "library", "Check if the game is in low violence mode" )
{
    lua_pushboolean( L, UTIL_IsLowViolence() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the game is in low violence mode" )

LUA_BINDING_BEGIN( Utilities, IsValidModel, "library", "Check if the given string is a valid model" )
{
    const char *pszModel = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "model" );

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
LUA_BINDING_END( "boolean", "True if the model is valid" )

LUA_BINDING_BEGIN( Utilities, IsValidPhysicsProp, "library", "Check if the given string is a valid physics props" )
{
    const char *pszModel = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "model" );
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
LUA_BINDING_END( "boolean", "True if the model is a valid physics prop" )

LUA_BINDING_BEGIN( Utilities, ShouldShowBlood, "library", "Check if blood of the given color should be shown" )
{
    int bloodColor = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "bloodColor" );

    lua_pushboolean( L, UTIL_ShouldShowBlood( bloodColor ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if blood should be shown" )

LUALIB_API int luaopen_Util_shared( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Utilities )

    return 1;
}
