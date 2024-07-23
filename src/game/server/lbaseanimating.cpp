//========= Copyright © 1996-2005, Valve Corporation, All rights reserved.
//============//
//
// Purpose: Base class for all animating characters and objects.
//
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "lbaseanimating.h"
#include "mathlib/lvector.h"
#include "lbaseentity_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseAnimating *lua_toanimating( lua_State *L, int idx )
{
    CBaseHandle *hEntity =
        dynamic_cast< CBaseHandle * >( ( CBaseHandle * )lua_touserdata( L, idx ) );
    if ( hEntity == NULL )
        return NULL;
    return dynamic_cast< lua_CBaseAnimating * >( hEntity->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CBaseAnimating *luaL_checkanimating( lua_State *L, int narg )
{
    lua_CBaseAnimating *d = lua_toanimating( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CBaseAnimating expected, got NULL entity" );
    return d;
}

static int CBaseAnimating_CalculateIKLocks( lua_State *L )
{
    luaL_checkanimating( L, 1 )->CalculateIKLocks( luaL_checknumber( L, 2 ) );
    return 0;
}

static int CBaseAnimating_ComputeEntitySpaceHitboxSurroundingBox( lua_State *L )
{
    Vector pVecWorldMins, pVecWorldMaxs;
    lua_pushboolean(
        L, luaL_checkanimating( L, 1 )->ComputeEntitySpaceHitboxSurroundingBox( &pVecWorldMins, &pVecWorldMaxs ) );
    lua_pushvector( L, pVecWorldMins );
    lua_pushvector( L, pVecWorldMaxs );
    return 3;
}

static int CBaseAnimating_ComputeHitboxSurroundingBox( lua_State *L )
{
    Vector pVecWorldMins, pVecWorldMaxs;
    lua_pushboolean( L, luaL_checkanimating( L, 1 )->ComputeHitboxSurroundingBox( &pVecWorldMins, &pVecWorldMaxs ) );
    lua_pushvector( L, pVecWorldMins );
    lua_pushvector( L, pVecWorldMaxs );
    return 3;
}

static int CBaseAnimating_DoMuzzleFlash( lua_State *L )
{
    luaL_checkanimating( L, 1 )->DoMuzzleFlash();
    return 0;
}

static int CBaseAnimating_FindTransitionSequence( lua_State *L )
{
    lua_pushinteger(
        L, luaL_checkanimating( L, 1 )->FindTransitionSequence( luaL_checkinteger( L, 2 ), luaL_checkinteger( L, 3 ), NULL ) );
    return 1;
}

static int CBaseAnimating_GetAnimTimeInterval( lua_State *L )
{
    lua_pushnumber( L, luaL_checkanimating( L, 1 )->GetAnimTimeInterval() );
    return 1;
}

static int CBaseAnimating_GetAttachmentLocal( lua_State *L )
{
    lua_pushboolean( L, luaL_checkanimating( L, 1 )->GetAttachmentLocal( luaL_checknumber( L, 2 ), luaL_checkvector( L, 3 ), luaL_checkangle( L, 4 ) ) );
    return 1;
}

static int CBaseAnimating_GetBaseAnimating( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, luaL_checkanimating( L, 1 )->GetBaseAnimating() );
    return 1;
}

static int CBaseAnimating_GetBonePosition( lua_State *L )
{
    Vector origin;
    QAngle angles;
    luaL_checkanimating( L, 1 )->GetBonePosition( luaL_checkinteger( L, 2 ), origin, angles );
    lua_pushvector( L, origin );
    lua_pushangle( L, angles );
    return 2;
}

static int CBaseAnimating_GetCycle( lua_State *L )
{
    lua_pushnumber( L, luaL_checkanimating( L, 1 )->GetCycle() );
    return 1;
}

static int CBaseAnimating_GetFlexDescFacs( lua_State *L )
{
    lua_pushstring(
        L, luaL_checkanimating( L, 1 )->GetFlexDescFacs( luaL_checkinteger( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_GetHitboxSet( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->GetHitboxSet() );
    return 1;
}

static int CBaseAnimating_GetHitboxSetCount( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->GetHitboxSetCount() );
    return 1;
}

static int CBaseAnimating_GetHitboxSetName( lua_State *L )
{
    lua_pushstring( L, luaL_checkanimating( L, 1 )->GetHitboxSetName() );
    return 1;
}

// static int CBaseAnimating_GetModelWidthScale (lua_State *L) {
//   lua_pushnumber(L, luaL_checkanimating(L, 1)->GetModelWidthScale());
//   return 1;
// }

static int CBaseAnimating_GetNumFlexControllers( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->GetNumFlexControllers() );
    return 1;
}

static int CBaseAnimating_GetPlaybackRate( lua_State *L )
{
    lua_pushnumber( L, luaL_checkanimating( L, 1 )->GetPlaybackRate() );
    return 1;
}

static int CBaseAnimating_GetPoseParameter( lua_State *L )
{
    lua_pushnumber( L, luaL_checkanimating( L, 1 )->GetPoseParameter( luaL_checkinteger( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_GetPoseParameterRange( lua_State *L )
{
    float minValue, maxValue;
    lua_pushboolean( L, luaL_checkanimating( L, 1 )->GetPoseParameterRange( luaL_checkinteger( L, 2 ), minValue, maxValue ) );
    lua_pushnumber( L, minValue );
    lua_pushnumber( L, maxValue );
    return 3;
}

static int CBaseAnimating_GetSequence( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->GetSequence() );
    return 1;
}

static int CBaseAnimating_GetSequenceActivity( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->GetSequenceActivity( luaL_checkinteger( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_GetSequenceActivityName( lua_State *L )
{
    lua_pushstring( L, luaL_checkanimating( L, 1 )->GetSequenceActivityName( luaL_checkinteger( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_GetSequenceGroundSpeed( lua_State *L )
{
    lua_pushnumber( L, luaL_checkanimating( L, 1 )->GetSequenceGroundSpeed( luaL_checkinteger( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_GetSequenceLinearMotion( lua_State *L )
{
    Vector pVec;
    luaL_checkanimating( L, 1 )->GetSequenceLinearMotion( luaL_checkinteger( L, 2 ),
                                                          &pVec );
    lua_pushvector( L, pVec );
    return 1;
}

static int CBaseAnimating_GetSequenceName( lua_State *L )
{
    lua_pushstring(
        L, luaL_checkanimating( L, 1 )->GetSequenceName( luaL_checkinteger( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_InvalidateBoneCache( lua_State *L )
{
    luaL_checkanimating( L, 1 )->InvalidateBoneCache();
    return 0;
}

static int CBaseAnimating_InvalidateMdlCache( lua_State *L )
{
    luaL_checkanimating( L, 1 )->InvalidateMdlCache();
    return 0;
}

static int CBaseAnimating_IsActivityFinished( lua_State *L )
{
    lua_pushboolean( L, luaL_checkanimating( L, 1 )->IsActivityFinished() );
    return 1;
}

static int CBaseAnimating_IsOnFire( lua_State *L )
{
    lua_pushboolean( L, luaL_checkanimating( L, 1 )->IsOnFire() );
    return 1;
}

static int CBaseAnimating_IsRagdoll( lua_State *L )
{
    lua_pushboolean( L, luaL_checkanimating( L, 1 )->IsRagdoll() );
    return 1;
}

static int CBaseAnimating_IsSequenceFinished( lua_State *L )
{
    lua_pushboolean( L, luaL_checkanimating( L, 1 )->IsSequenceFinished() );
    return 1;
}

static int CBaseAnimating_IsSequenceLooping( lua_State *L )
{
    lua_pushboolean( L, luaL_checkanimating( L, 1 )->IsSequenceLooping( luaL_checkinteger( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_LookupActivity( lua_State *L )
{
    lua_pushinteger(
        L, luaL_checkanimating( L, 1 )->LookupActivity( luaL_checkstring( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_LookupAttachment( lua_State *L )
{
    lua_pushinteger(
        L, luaL_checkanimating( L, 1 )->LookupAttachment( luaL_checkstring( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_LookupBone( lua_State *L )
{
    lua_pushinteger(
        L, luaL_checkanimating( L, 1 )->LookupBone( luaL_checkstring( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_LookupPoseParameter( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->LookupPoseParameter( luaL_checkstring( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_LookupSequence( lua_State *L )
{
    lua_pushinteger(
        L, luaL_checkanimating( L, 1 )->LookupSequence( luaL_checkstring( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_ResetSequence( lua_State *L )
{
    luaL_checkanimating( L, 1 )->ResetSequence( luaL_checkinteger( L, 2 ) );
    return 0;
}

static int CBaseAnimating_ResetSequenceInfo( lua_State *L )
{
    luaL_checkanimating( L, 1 )->ResetSequenceInfo();
    return 0;
}

static int CBaseAnimating_SelectWeightedSequence( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->SelectWeightedSequence( ( Activity )luaL_checkinteger( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_SequenceDuration( lua_State *L )
{
    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNONE:
        default:
            lua_pushnumber( L, luaL_checkanimating( L, 1 )->SequenceDuration() );
            break;
        case LUA_TNUMBER:
            lua_pushnumber( L, luaL_checkanimating( L, 1 )->SequenceDuration( luaL_checknumber( L, 2 ) ) );
            break;
    }
    return 1;
}

static int CBaseAnimating_SequenceLoops( lua_State *L )
{
    lua_pushboolean( L, luaL_checkanimating( L, 1 )->SequenceLoops() );
    return 1;
}

static int CBaseAnimating_SetBoneController( lua_State *L )
{
    lua_pushnumber( L, luaL_checkanimating( L, 1 )->SetBoneController( luaL_checkinteger( L, 2 ), luaL_checknumber( L, 3 ) ) );
    return 1;
}

static int CBaseAnimating_SetCycle( lua_State *L )
{
    luaL_checkanimating( L, 1 )->SetCycle( luaL_checknumber( L, 2 ) );
    return 0;
}

static int CBaseAnimating_SetHitboxSet( lua_State *L )
{
    luaL_checkanimating( L, 1 )->SetHitboxSet( luaL_checkinteger( L, 2 ) );
    return 0;
}

static int CBaseAnimating_SetHitboxSetByName( lua_State *L )
{
    luaL_checkanimating( L, 1 )->SetHitboxSetByName( luaL_checkstring( L, 2 ) );
    return 0;
}

// static int CBaseAnimating_SetModelWidthScale (lua_State *L) {
//   luaL_checkanimating(L, 1)->SetModelWidthScale(luaL_checknumber(L, 2));
//   return 0;
// }

static int CBaseAnimating_SetPlaybackRate( lua_State *L )
{
    luaL_checkanimating( L, 1 )->SetPlaybackRate( luaL_checknumber( L, 2 ) );
    return 0;
}

static int CBaseAnimating_SetPoseParameter( lua_State *L )
{
    lua_pushnumber( L, luaL_checkanimating( L, 1 )->SetPoseParameter( luaL_checkinteger( L, 2 ), luaL_checknumber( L, 3 ) ) );
    return 1;
}

static int CBaseAnimating_SetSequence( lua_State *L )
{
    luaL_checkanimating( L, 1 )->SetSequence( luaL_checkinteger( L, 2 ) );
    return 0;
}

static int CBaseAnimating_StudioFrameAdvance( lua_State *L )
{
    luaL_checkanimating( L, 1 )->StudioFrameAdvance();
    return 0;
}

static int CBaseAnimating_TransferDissolveFrom( lua_State *L )
{
    luaL_checkanimating( L, 1 )->TransferDissolveFrom( luaL_checkanimating( L, 2 ) );
    return 0;
}

static int CBaseAnimating_UseClientSideAnimation( lua_State *L )
{
    luaL_checkanimating( L, 1 )->UseClientSideAnimation();
    return 0;
}

static int CBaseAnimating___index( lua_State *L )
{
    CBaseAnimating *pEntity = lua_toanimating( L, 1 );

    LUA_METATABLE_INDEX_CHECK_VALID( L, CBaseEntity_IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pEntity );

    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pEntity );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, LUA_BASEENTITYLIBNAME );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    lua_pushnil( L );
    return 1;
}

static int CBaseAnimating___newindex( lua_State *L )
{
    CBaseAnimating *pEntity = lua_toanimating( L, 1 );
    if ( pEntity == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index a NULL entity", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    const char *field = luaL_checkstring( L, 2 );

    if ( Q_strcmp( field, "m_bClientSideAnimation" ) == 0 )
        luaL_checkboolean( L, 3 );
    // TODO: m_bClientSideAnimation only exists on client?
    // pEntity->m_bClientSideAnimation = (bool)luaL_checkboolean(L, 3);
    else if ( Q_strcmp( field, "m_nBody" ) == 0 )
        pEntity->m_nBody = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_nHitboxSet" ) == 0 )
        pEntity->m_nHitboxSet = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_nSkin" ) == 0 )
        pEntity->m_nSkin = luaL_checknumber( L, 3 );
    else
    {
        LUA_GET_REF_TABLE( L, pEntity );
        lua_pushvalue( L, 3 );
        lua_setfield( L, -2, field );
        lua_pop( L, 1 );
    }

    return 0;
}

static int CBaseAnimating___eq( lua_State *L )
{
    lua_pushboolean( L, lua_toanimating( L, 1 ) == lua_toanimating( L, 2 ) );
    return 1;
}

static int CBaseAnimating___tostring( lua_State *L )
{
    CBaseAnimating *pEntity = lua_toanimating( L, 1 );
    if ( pEntity == NULL )
        lua_pushstring( L, "NULL" );
    else
        lua_pushfstring( L, "CBaseAnimating: %d \"%s\"", pEntity->entindex(), pEntity->GetClassname() );
    return 1;
}

static const luaL_Reg CBaseAnimatingmeta[] = {
    { "CalculateIKLocks", CBaseAnimating_CalculateIKLocks },
    { "ComputeEntitySpaceHitboxSurroundingBox",
      CBaseAnimating_ComputeEntitySpaceHitboxSurroundingBox },
    { "ComputeHitboxSurroundingBox", CBaseAnimating_ComputeHitboxSurroundingBox },
    { "DoMuzzleFlash", CBaseAnimating_DoMuzzleFlash },
    { "FindTransitionSequence", CBaseAnimating_FindTransitionSequence },
    { "GetAnimTimeInterval", CBaseAnimating_GetAnimTimeInterval },
    { "GetAttachmentLocal", CBaseAnimating_GetAttachmentLocal },
    { "GetBaseAnimating", CBaseAnimating_GetBaseAnimating },
    { "GetBonePosition", CBaseAnimating_GetBonePosition },
    { "GetCycle", CBaseAnimating_GetCycle },
    { "GetFlexDescFacs", CBaseAnimating_GetFlexDescFacs },
    { "GetHitboxSet", CBaseAnimating_GetHitboxSet },
    { "GetHitboxSetCount", CBaseAnimating_GetHitboxSetCount },
    { "GetHitboxSetName", CBaseAnimating_GetHitboxSetName },
    //{"GetModelWidthScale", CBaseAnimating_GetModelWidthScale},
    { "GetNumFlexControllers", CBaseAnimating_GetNumFlexControllers },
    { "GetPlaybackRate", CBaseAnimating_GetPlaybackRate },
    { "GetPoseParameter", CBaseAnimating_GetPoseParameter },
    { "GetPoseParameterRange", CBaseAnimating_GetPoseParameterRange },
    { "GetSequence", CBaseAnimating_GetSequence },
    { "GetSequenceActivity", CBaseAnimating_GetSequenceActivity },
    { "GetSequenceActivityName", CBaseAnimating_GetSequenceActivityName },
    { "GetSequenceGroundSpeed", CBaseAnimating_GetSequenceGroundSpeed },
    { "GetSequenceLinearMotion", CBaseAnimating_GetSequenceLinearMotion },
    { "GetSequenceName", CBaseAnimating_GetSequenceName },
    { "InvalidateBoneCache", CBaseAnimating_InvalidateBoneCache },
    { "InvalidateMdlCache", CBaseAnimating_InvalidateMdlCache },
    { "IsActivityFinished", CBaseAnimating_IsActivityFinished },
    { "IsOnFire", CBaseAnimating_IsOnFire },
    { "IsRagdoll", CBaseAnimating_IsRagdoll },
    { "IsSequenceFinished", CBaseAnimating_IsSequenceFinished },
    { "IsSequenceLooping", CBaseAnimating_IsSequenceLooping },
    { "LookupActivity", CBaseAnimating_LookupActivity },
    { "LookupAttachment", CBaseAnimating_LookupAttachment },
    { "LookupBone", CBaseAnimating_LookupBone },
    { "LookupPoseParameter", CBaseAnimating_LookupPoseParameter },
    { "LookupSequence", CBaseAnimating_LookupSequence },
    { "ResetSequence", CBaseAnimating_ResetSequence },
    { "ResetSequenceInfo", CBaseAnimating_ResetSequenceInfo },
    { "SelectWeightedSequence", CBaseAnimating_SelectWeightedSequence },
    { "SequenceDuration", CBaseAnimating_SequenceDuration },
    { "SequenceLoops", CBaseAnimating_SequenceLoops },
    { "SetBoneController", CBaseAnimating_SetBoneController },
    { "SetCycle", CBaseAnimating_SetCycle },
    { "SetHitboxSet", CBaseAnimating_SetHitboxSet },
    { "SetHitboxSetByName", CBaseAnimating_SetHitboxSetByName },
    //{"SetModelWidthScale", CBaseAnimating_SetModelWidthScale},
    { "SetPlaybackRate", CBaseAnimating_SetPlaybackRate },
    { "SetPoseParameter", CBaseAnimating_SetPoseParameter },
    { "SetSequence", CBaseAnimating_SetSequence },
    { "StudioFrameAdvance", CBaseAnimating_StudioFrameAdvance },
    { "TransferDissolveFrom", CBaseAnimating_TransferDissolveFrom },
    { "UseClientSideAnimation", CBaseAnimating_UseClientSideAnimation },
    { "__index", CBaseAnimating___index },
    { "__newindex", CBaseAnimating___newindex },
    { "__eq", CBaseAnimating___eq },
    { "__tostring", CBaseAnimating___tostring },
    { NULL, NULL } };

/*
** Open CBaseAnimating object
*/
LUALIB_API int luaopen_CBaseAnimating( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "CBaseAnimating" );
    luaL_register( L, NULL, CBaseAnimatingmeta );
    lua_pushstring( L, "Entity" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */
    lua_pop( L, 1 );
    return 1;
}
