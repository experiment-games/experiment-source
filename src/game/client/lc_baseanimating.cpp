//===== Copy	right © 1996-2005, Valve Corporation, All rights reserved. ==//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//
#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lc_baseanimating.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "mathlib/lvector.h"
#include "model_types.h"

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

LUA_REGISTRATION_INIT( CBaseAnimating  )

LUA_BINDING_BEGIN( CBaseAnimating, AddEntityToVisibleList, "class", "Add the entity to the visible entity list. Seems to only call CreateLightEffects internally." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->AddEntity();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, AddToClientSideAnimationList, "class", "Add the entity to the client side animation list." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->AddToClientSideAnimationList();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, BecomeRagdollOnClient, "class", "Become a ragdoll on the client." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pAnimating->BecomeRagdollOnClient() );

    return 1;
}
LUA_BINDING_END( "entity", "The ragdoll entity" )

LUA_BINDING_BEGIN( CBaseAnimating, CalculateIKLocks, "class", "Calculate IK locks." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "currentTime" );

    pAnimating->CalculateIKLocks( flTime );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ClampCycle, "class", "Clamp the cycle." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flCycle = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "cycle" );
    bool bIsLooping = LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "isLooping" );

    lua_pushnumber( L, pAnimating->ClampCycle( flCycle, bIsLooping ) );

    return 1;
}
LUA_BINDING_END( "number", "The clamped cycle" )

LUA_BINDING_BEGIN( CBaseAnimating, Clear, "class", "Resets internal fields." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->Clear();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ClearRagdoll, "class", "Clear/destroy the clientside ragdoll." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->ClearRagdoll();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ClientSideAnimationChanged, "class", "Called when the client side animation changes." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->ClientSideAnimationChanged();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ComputeClientSideAnimationFlags, "class", "Compute the client side animation flags." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushnumber( L, pAnimating->ComputeClientSideAnimationFlags() );

    return 1;
}
LUA_BINDING_END( "number", "The client side animation flags" )

LUA_BINDING_BEGIN( CBaseAnimating, ComputeEntitySpaceHitboxSurroundingBox, "class", "Compute the entity space hitbox surrounding box." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    Vector pVecWorldMins, pVecWorldMaxs;
    lua_pushboolean( L, pAnimating->ComputeEntitySpaceHitboxSurroundingBox( &pVecWorldMins, &pVecWorldMaxs ) );
    lua_pushvector( L, pVecWorldMins );
    lua_pushvector( L, pVecWorldMaxs );

    return 3;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "vector", "The world mins", "vector", "The world maxs" )

LUA_BINDING_BEGIN( CBaseAnimating, ComputeHitboxSurroundingBox, "class", "Compute the hitbox surrounding box." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    Vector pVecWorldMins, pVecWorldMaxs;
    lua_pushboolean( L, pAnimating->ComputeHitboxSurroundingBox( &pVecWorldMins, &pVecWorldMaxs ) );
    lua_pushvector( L, pVecWorldMins );
    lua_pushvector( L, pVecWorldMaxs );

    return 3;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "vector", "The world mins", "vector", "The world maxs" )

LUA_BINDING_BEGIN( CBaseAnimating, CreateRagdollCopy, "class", "Create a ragdoll copy of the entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pAnimating->CreateRagdollCopy() );

    return 1;
}
LUA_BINDING_END( "entity", "The ragdoll entity" )

LUA_BINDING_BEGIN( CBaseAnimating, CreateUnragdollInfo, "class", "Creates unragdoll info for the given ragdoll, setting it on the entity internally." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    lua_CBaseAnimating *pRagdoll = LUA_BINDING_ARGUMENT( luaL_checkanimating, 2, "ragdoll" );

    pAnimating->CreateUnragdollInfo( pRagdoll );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, DisableMuzzleFlash, "class", "Disable the muzzle flash." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->DisableMuzzleFlash();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, DispatchMuzzleEffect, "class", "Dispatch the muzzle effect." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszOptions = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "options" );
    bool bIsFirstPerson = LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "isFirstPerson" );

    lua_pushboolean( L, pAnimating->DispatchMuzzleEffect( pszOptions, bIsFirstPerson ) );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful" )

LUA_BINDING_BEGIN( CBaseAnimating, DoMuzzleFlash, "class", "Sets up muzzle flash parity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->DoMuzzleFlash();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, DrawClientHitboxes, "class", "Draw the client hitboxes." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flDuration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.0f, "duration" );
    bool bMonoColor = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "monoColor" );

    pAnimating->DrawClientHitboxes( flDuration, bMonoColor );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, DrawModel, "class", "Draw the model." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, STUDIO_RENDER, "flags" );

    lua_pushinteger( L, pAnimating->DrawModel( nFlags ) );

    return 1;
}
LUA_BINDING_END( "number", "The result of the operation" )

LUA_BINDING_BEGIN( CBaseAnimating, FindFollowedEntity, "class", "Find the followed entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pAnimating->FindFollowedEntity() );

    return 1;
}
LUA_BINDING_END( "entity", "The followed entity" )

LUA_BINDING_BEGIN( CBaseAnimating, FindTransitionSequence, "class", "Find the transition sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );
    int iGoalSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "goalSequence" );

    int piDir;
    lua_pushinteger( L, pAnimating->FindTransitionSequence( iSequence, iGoalSequence, &piDir ) );
    lua_pushinteger( L, piDir );

    return 2;
}
LUA_BINDING_END( "number", "The transition sequence", "number", "The direction" )

LUA_BINDING_BEGIN( CBaseAnimating, FireEvent, "class", "Fire an event." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    Vector pVecOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" );
    QAngle pAngAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "angles" );
    float iEvent = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "event" );
    const char *pszOptions = LUA_BINDING_ARGUMENT( luaL_checkstring, 5, "options" );

    pAnimating->FireEvent( pVecOrigin, pAngAngles, iEvent, pszOptions );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, FireObsoleteEvent, "class", "These events are all obsolete events, left here to support old games. Their systems have all been replaced with better ones." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    Vector pVecOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" );
    QAngle pAngAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "angles" );
    int iEvent = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "event" );
    const char *pszOptions = LUA_BINDING_ARGUMENT( luaL_checkstring, 5, "options" );

    pAnimating->FireObsoleteEvent( pVecOrigin, pAngAngles, iEvent, pszOptions );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ForceClientSideAnimationOn, "class", "This can be used to force client side animation to be on. Only use if you know what you're doing! Normally, the server entity should set this." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->ForceClientSideAnimationOn();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, FrameAdvance, "class", "Advance the frame." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flInterval = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.0f, "interval" );

    lua_pushnumber( L, pAnimating->FrameAdvance( flInterval ) );

    return 1;
}
LUA_BINDING_END( "number", "The new cycle" )

LUA_BINDING_BEGIN( CBaseAnimating, GetAimEntOrigin, "class", "Get the aim entity origin." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    lua_CBaseEntity *pAttachedTo = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "attachedTo" );
    Vector pVecOrigin;
    QAngle pAngAngles;

    pAnimating->GetAimEntOrigin( pAttachedTo, &pVecOrigin, &pAngAngles );

    lua_pushvector( L, pVecOrigin );
    lua_pushangle( L, pAngAngles );

    return 2;
}
LUA_BINDING_END( "vector", "The aim entity origin", "angle", "The aim entity angles" )

LUA_BINDING_BEGIN( CBaseAnimating, GetAnimTimeInterval, "class", "Get the animation time interval." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushnumber( L, pAnimating->GetAnimTimeInterval() );

    return 1;
}
LUA_BINDING_END( "number", "The animation time interval" )

LUA_BINDING_BEGIN( CBaseAnimating, GetAttachmentLocal, "class", "Get the local attachment." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iAttachment = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "attachment" );
    Vector pVecOrigin;
    QAngle pAngAngles;

    lua_pushboolean( L, pAnimating->GetAttachmentLocal( iAttachment, pVecOrigin, pAngAngles ) );
    lua_pushvector( L, pVecOrigin );
    lua_pushangle( L, pAngAngles );

    return 3;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "vector", "The origin", "angle", "The angles" )

LUA_BINDING_BEGIN( CBaseAnimating, GetAttachmentVelocity, "class", "Get the attachment velocity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iAttachment = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "attachment" );
    Vector originVel;
    Quaternion angleVel;

    lua_pushboolean( L, pAnimating->GetAttachmentVelocity( iAttachment, originVel, angleVel ) );
    lua_pushvector( L, originVel );
    // Todo: implement Quaternion class!!
    // lua_pushquaternion(L, &angleVel);
    return 2;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "vector", "The origin velocity" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBaseAnimating, "class", "Get the base animating entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pAnimating->GetBaseAnimating() );

    return 1;
}
LUA_BINDING_END( "entity", "The base animating entity" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBlendedLinearVelocity, "class", "Get the blended linear velocity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    Vector pVec;

    pAnimating->GetBlendedLinearVelocity( &pVec );

    lua_pushvector( L, pVec );

    return 1;
}
LUA_BINDING_END( "vector", "The blended linear velocity" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBody, "class", "Get the body (internally described as 'Object bodygroup'?)." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetBody() );

    return 1;
}
LUA_BINDING_END( "number", "The body" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBoneControllers, "class", "Get all bone controller values." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float controllers[MAXSTUDIOBONECTRLS];

    pAnimating->GetBoneControllers( controllers );

    for ( int i = 0; i < MAXSTUDIOBONECTRLS; i++ )
    {
        lua_pushnumber( L, controllers[i] );
    }

    return MAXSTUDIOBONECTRLS;
}
LUA_BINDING_END( "number", "The bone controllers" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBonePosition, "class", "Get the bone position." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBone = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bone" );
    Vector pVecOrigin;
    QAngle pAngAngles;

    pAnimating->GetBonePosition( iBone, pVecOrigin, pAngAngles );

    lua_pushvector( L, pVecOrigin );
    lua_pushangle( L, pAngAngles );

    return 2;
}
LUA_BINDING_END( "vector", "The origin", "angle", "The angles" )

LUA_BINDING_BEGIN( CBaseAnimating, GetClientSideFade, "class", "Get the client side fade." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetClientSideFade() );

    return 1;
}
LUA_BINDING_END( "number", "The client side fade" )

LUA_BINDING_BEGIN( CBaseAnimating, GetCollideType, "class", "Get the collide type." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetCollideType() );

    return 1;
}
LUA_BINDING_END( "integer|CollideType_t", "The collide type" )

LUA_BINDING_BEGIN( CBaseAnimating, GetCycle, "class", "Get the current cycle." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushnumber( L, pAnimating->GetCycle() );

    return 1;
}
LUA_BINDING_END( "number", "The cycle" )

LUA_BINDING_BEGIN( CBaseAnimating, GetFlexControllerName, "class", "Get the flex controller name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iFlexControllerIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flexControllerIndex" );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )iFlexControllerIndex;

    lua_pushstring( L, pAnimating->GetFlexControllerName( iFlexController ) );

    return 1;
}
LUA_BINDING_END( "string", "The flex controller name" )

LUA_BINDING_BEGIN( CBaseAnimating, GetFlexControllerType, "class", "Get the flex controller type." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iFlexControllerIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flexControllerIndex" );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )iFlexControllerIndex;

    lua_pushstring( L, pAnimating->GetFlexControllerType( iFlexController ) );

    return 1;
}
LUA_BINDING_END( "string", "The flex controller type" )

LUA_BINDING_BEGIN( CBaseAnimating, GetFlexDescFacs, "class", "Get the flex desc facs." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iFlexDescIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flexDescIndex" );

    lua_pushstring( L, pAnimating->GetFlexDescFacs( iFlexDescIndex ) );

    return 1;
}
LUA_BINDING_END( "string", "The flex desc facs" )

LUA_BINDING_BEGIN( CBaseAnimating, GetHitboxSet, "class", "Get the index of the hitbox set in use." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetHitboxSet() );

    return 1;
}
LUA_BINDING_END( "number", "The hitbox set" )

LUA_BINDING_BEGIN( CBaseAnimating, GetHitboxSetCount, "class", "Get the hitbox set count." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetHitboxSetCount() );

    return 1;
}
LUA_BINDING_END( "number", "The hitbox set count" )

LUA_BINDING_BEGIN( CBaseAnimating, GetHitboxSetName, "class", "Get the hitbox set name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushstring( L, pAnimating->GetHitboxSetName() );

    return 1;
}
LUA_BINDING_END( "string", "The hitbox set name" )

// static int CBaseAnimating_GetModelWidthScale (lua_State *L) {
//   lua_pushnumber(L, luaL_checkanimating(L, 1)->GetModelWidthScale());
//   return 1;
// }

LUA_BINDING_BEGIN( CBaseAnimating, GetNumFlexControllers, "class", "Get the number of flex controllers." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetNumFlexControllers() );

    return 1;
}
LUA_BINDING_END( "number", "The number of flex controllers" )

LUA_BINDING_BEGIN( CBaseAnimating, GetPlaybackRate, "class", "Get the playback rate." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushnumber( L, pAnimating->GetPlaybackRate() );

    return 1;
}
LUA_BINDING_END( "number", "The playback rate" )

LUA_BINDING_BEGIN( CBaseAnimating, GetPoseParameter, "class", "Get the pose parameter." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iPoseParameter = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "poseParameter" );

    lua_pushnumber( L, pAnimating->GetPoseParameter( iPoseParameter ) );

    return 1;
}
LUA_BINDING_END( "number", "The pose parameter" )

LUA_BINDING_BEGIN( CBaseAnimating, GetPoseParameterRange, "class", "Get the pose parameter range." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iPoseParameter = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "poseParameter" );
    float minValue, maxValue;

    lua_pushboolean( L, pAnimating->GetPoseParameterRange( iPoseParameter, minValue, maxValue ) );
    lua_pushnumber( L, minValue );
    lua_pushnumber( L, maxValue );

    return 3;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "number", "The minimum value", "number", "The maximum value" )

LUA_BINDING_BEGIN( CBaseAnimating, GetRenderAngles, "class", "Get the render angles." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    QAngle v = pAnimating->GetRenderAngles();

    lua_pushangle( L, v );

    return 1;
}
LUA_BINDING_END( "angle", "The render angles" )

LUA_BINDING_BEGIN( CBaseAnimating, GetRenderBounds, "class", "Get the render bounds." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    Vector theMins, theMaxs;

    pAnimating->GetRenderBounds( theMins, theMaxs );

    lua_pushvector( L, theMins );
    lua_pushvector( L, theMaxs );

    return 2;
}
LUA_BINDING_END( "vector", "The minimum bounds", "vector", "The maximum bounds" )

LUA_BINDING_BEGIN( CBaseAnimating, GetRenderOrigin, "class", "Get the render origin." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    Vector v = pAnimating->GetRenderOrigin();

    lua_pushvector( L, v );

    return 1;
}
LUA_BINDING_END( "vector", "The render origin" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequence, "class", "Get the sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetSequence() );

    return 1;
}
LUA_BINDING_END( "number", "The sequence" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceActivity, "class", "Get the sequence activity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushinteger( L, pAnimating->GetSequenceActivity( iSequence ) );

    return 1;
}
LUA_BINDING_END( "number", "The sequence activity" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceActivityName, "class", "Get the sequence activity name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushstring( L, pAnimating->GetSequenceActivityName( iSequence ) );

    return 1;
}
LUA_BINDING_END( "string", "The sequence activity name" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceGroundSpeed, "class", "Get the sequence ground speed." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushnumber( L, pAnimating->GetSequenceGroundSpeed( iSequence ) );

    return 1;
}
LUA_BINDING_END( "number", "The sequence ground speed" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceLinearMotion, "class", "Get the sequence linear motion." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );
    Vector pVec;

    pAnimating->GetSequenceLinearMotion( iSequence, &pVec );

    lua_pushvector( L, pVec );

    return 1;
}
LUA_BINDING_END( "vector", "The sequence linear motion" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceName, "class", "Get the sequence name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushstring( L, pAnimating->GetSequenceName( iSequence ) );

    return 1;
}
LUA_BINDING_END( "string", "The sequence name" )

LUA_BINDING_BEGIN( CBaseAnimating, GetServerIntendedCycle, "class", "Get the server intended cycle." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushnumber( L, pAnimating->GetServerIntendedCycle() );

    return 1;
}
LUA_BINDING_END( "number", "The server intended cycle" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSkin, "class", "Get the skin." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetSkin() );

    return 1;
}
LUA_BINDING_END( "number", "The skin" )

LUA_BINDING_BEGIN( CBaseAnimating, IgniteRagdoll, "class", "Ignite the ragdoll." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    lua_CBaseAnimating *pIgniter = LUA_BINDING_ARGUMENT( luaL_checkanimating, 2, "igniter" );

    pAnimating->IgniteRagdoll( pIgniter );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, InitBoneSetupThreadPool, "class|static", "Initialize the bone setup thread pool." )
{
    CBaseAnimating::InitBoneSetupThreadPool();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, InitModelEffects, "class", "Initialize the model effects." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->InitModelEffects();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, InternalDrawModel, "class", "Internal draw the model." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int flags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );

    lua_pushinteger( L, pAnimating->InternalDrawModel( flags ) );

    return 1;
}
LUA_BINDING_END( "number", "The resulting draw flag" )

LUA_BINDING_BEGIN( CBaseAnimating, Interpolate, "class", "Interpolate the entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flCurrentTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "currentTime" );

    lua_pushboolean( L, pAnimating->Interpolate( flCurrentTime ) );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful" )

LUA_BINDING_BEGIN( CBaseAnimating, InvalidateBoneCache, "class", "Invalidate the bone cache." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->InvalidateBoneCache();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, InvalidateBoneCaches, "class|static", "Invalidate the bone caches." )
{
    CBaseAnimating::InvalidateBoneCaches();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, InvalidateMdlCache, "class", "Invalidate the MDL cache." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->InvalidateMdlCache();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, IsActivityFinished, "class", "Check if the activity is finished." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsActivityFinished() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the activity is finished" )

LUA_BINDING_BEGIN( CBaseAnimating, IsBoneCacheValid, "class", "Check if the bone cache is valid." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsBoneCacheValid() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the bone cache is valid" )

LUA_BINDING_BEGIN( CBaseAnimating, IsOnFire, "class", "Check if the entity is on fire." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsOnFire() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity is on fire" )

LUA_BINDING_BEGIN( CBaseAnimating, IsRagdoll, "class", "Check if the entity is a ragdoll." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsRagdoll() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity is a ragdoll" )

LUA_BINDING_BEGIN( CBaseAnimating, IsSelfAnimating, "class", "Check if the entity is self animating." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsSelfAnimating() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity is self animating" )

LUA_BINDING_BEGIN( CBaseAnimating, IsSequenceFinished, "class", "Check if the sequence is finished." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsSequenceFinished() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the sequence is finished" )

LUA_BINDING_BEGIN( CBaseAnimating, IsSequenceLooping, "class", "Check if the sequence is looping." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushboolean( L, pAnimating->IsSequenceLooping( iSequence ) );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the sequence is looping" )

LUA_BINDING_BEGIN( CBaseAnimating, IsViewModel, "class", "Check if the entity is a view model." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsViewModel() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity is a view model" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupActivity, "class", "Looks up an activity by name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszActivityName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "activityName" );

    lua_pushinteger( L, pAnimating->LookupActivity( pszActivityName ) );

    return 1;
}
LUA_BINDING_END( "integer|ACT", "The activity" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupAttachment, "class", "Lookup the attachment." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszAttachmentName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "attachmentName" );

    lua_pushinteger( L, pAnimating->LookupAttachment( pszAttachmentName ) );

    return 1;
}
LUA_BINDING_END( "number", "The attachment" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupBone, "class", "Lookup the bone." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszBoneName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "boneName" );

    lua_pushinteger( L, pAnimating->LookupBone( pszBoneName ) );

    return 1;
}
LUA_BINDING_END( "number", "The bone" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupPoseParameter, "class", "Lookup the pose parameter." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszPoseParameterName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "poseParameterName" );

    lua_pushinteger( L, pAnimating->LookupPoseParameter( pszPoseParameterName ) );

    return 1;
}
LUA_BINDING_END( "number", "The pose parameter" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupRandomAttachment, "class", "Get a random index of an attachment point with the specified substring in its name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszAttachmentName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "attachmentNameSubstring" );

    lua_pushinteger( L, pAnimating->LookupRandomAttachment( pszAttachmentName ) );

    return 1;
}
LUA_BINDING_END( "number", "The random attachment" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupSequence, "class", "Lookup the sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszSequenceName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "sequenceName" );

    lua_pushinteger( L, pAnimating->LookupSequence( pszSequenceName ) );

    return 1;
}
LUA_BINDING_END( "number", "The sequence" )

LUA_BINDING_BEGIN( CBaseAnimating, NotifyShouldTransmit, "class", "Notify should transmit." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iState = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "state" );
    ShouldTransmitState_t state = ( ShouldTransmitState_t )iState;

    pAnimating->NotifyShouldTransmit( state );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, OnDataChanged, "class", "Call to inform that data changed. Creates/removes ragdolls, sets internal variables + other things that happen to an entity when data changes." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iUpdateType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "updateType" );
    DataUpdateType_t updateType = ( DataUpdateType_t )iUpdateType;

    pAnimating->OnDataChanged( updateType );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, OnPreDataChanged, "class", "Internally sets up 'old*' and 'last*' variables to track changes." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iUpdateType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "updateType" );
    DataUpdateType_t updateType = ( DataUpdateType_t )iUpdateType;

    pAnimating->OnPreDataChanged( updateType );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, PopBoneAccess, "class|static", "Pop bone access." )
{
    C_BaseAnimating::PopBoneAccess( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, PostDataUpdate, "class", "Post data update." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iUpdateType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "updateType" );
    DataUpdateType_t updateType = ( DataUpdateType_t )iUpdateType;

    pAnimating->PostDataUpdate( updateType );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, PreDataUpdate, "class", "Pre data update." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iUpdateType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "updateType" );
    DataUpdateType_t updateType = ( DataUpdateType_t )iUpdateType;

    pAnimating->PreDataUpdate( updateType );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ProcessMuzzleFlashEvent, "class", "Performs the muzzle flash effect." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->ProcessMuzzleFlashEvent();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, PushAllowBoneAccess, "class|static", "Push allow bone access." )
{
    C_BaseAnimating::PushAllowBoneAccess( LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "allowForNormalModels" ),
                                          LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "allowForViewModels" ),
                                          LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "tagPush" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, RagdollMoved, "class", "Call this when the ragdoll moved to update the entity's position, angles and bounds to that of its ragdoll." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->RagdollMoved();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, Release, "class", "Release the entity. This clears the ragdoll and destroys the entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->Release();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, RemoveFromClientSideAnimationList, "class", "Remove from the client side animation list." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->RemoveFromClientSideAnimationList();

    return 0;
}
LUA_BINDING_END()

// static int CBaseAnimating_ResetEventsParity (lua_State *L) {
//   luaL_checkanimating(L, 1)->ResetEventsParity();
//   return 0;
// }

LUA_BINDING_BEGIN( CBaseAnimating, ResetLatched, "class", "Reset the latched." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->ResetLatched();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ResetSequence, "class", "Reset the sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    auto model = pAnimating->GetModelPtr();

    if ( !model )
    {
        Warning( "CBaseAnimating::ResetSequence failed: no model\n" );
        return 0;
    }

    if ( iSequence >= model->GetNumSeq() )
    {
        Warning( "CBaseAnimating::ResetSequence failed: invalid sequence %d\n", iSequence );
        return 0;
    }

    pAnimating->ResetSequence( iSequence );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ResetSequenceInfo, "class", "Reset the sequence info." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->ResetSequenceInfo();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, RetrieveRagdollInfo, "class", "Retrieve the ragdoll info." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    Vector pos;
    Quaternion q;

    lua_pushboolean( L, pAnimating->RetrieveRagdollInfo( &pos, &q ) );
    lua_pushvector( L, pos );
    // Todo: implement Quaternion class!!
    // lua_pushquaternion(L, &q);

    return 2;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "vector", "The position" )

LUA_BINDING_BEGIN( CBaseAnimating, SelectWeightedSequence, "class", "Select the weighted sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iActivity = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "activity" );

    lua_pushinteger( L, pAnimating->SelectWeightedSequence( iActivity ) );

    return 1;
}
LUA_BINDING_END( "number", "The weighted sequence" )

LUA_BINDING_BEGIN( CBaseAnimating, SequenceDuration, "class", "Get the sequence duration." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    if ( lua_isnoneornil( L, 2 ) )
        lua_pushnumber( L, pAnimating->SequenceDuration() );
    else
        lua_pushnumber( L, pAnimating->SequenceDuration( LUA_BINDING_ARGUMENT_NILLABLE( luaL_checknumber, 2, "sequence" ) ) );

    return 1;
}
LUA_BINDING_END( "number", "The sequence duration" )

LUA_BINDING_BEGIN( CBaseAnimating, SequenceLoops, "class", "Check if the sequence loops." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->SequenceLoops() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the sequence loops" )

LUA_BINDING_BEGIN( CBaseAnimating, SetBoneController, "class", "Set the value for the specified bone controller." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBoneController = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "boneController" );
    float flValue = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );

    lua_pushnumber( L, pAnimating->SetBoneController( iBoneController, flValue ) );

    return 1;
}
LUA_BINDING_END( "number", "The bone controller" )

LUA_BINDING_BEGIN( CBaseAnimating, SetCycle, "class", "Set the cycle." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flCycle = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "cycle" );

    pAnimating->SetCycle( flCycle );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetHitboxSet, "class", "Set the hitbox set." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iHitboxSet = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "hitboxSet" );

    pAnimating->SetHitboxSet( iHitboxSet );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetHitboxSetByName, "class", "Set the hitbox set by name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszHitboxSetName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "hitboxSetName" );

    pAnimating->SetHitboxSetByName( pszHitboxSetName );

    return 0;
}
LUA_BINDING_END()

// TODO:
// static int CBaseAnimating_SetModelWidthScale (lua_State *L) {
//   luaL_checkanimating(L, 1)->SetModelWidthScale(luaL_checknumber(L, 2));
//   return 0;
// }

LUA_BINDING_BEGIN( CBaseAnimating, SetPoseParameter, "class", "Set the pose parameter." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            lua_pushnumber( L, pAnimating->SetPoseParameter( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "poseParameter" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" ) ) );
            break;
        case LUA_TSTRING:
        default:
            lua_pushnumber( L, pAnimating->SetPoseParameter( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "poseParameter" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" ) ) );
            break;
    }

    return 1;
}
LUA_BINDING_END( "number", "The pose parameter" )

LUA_BINDING_BEGIN( CBaseAnimating, SetPredictable, "class", "Set predictable." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    bool bPredictable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "predictable" );

    pAnimating->SetPredictable( bPredictable );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetPredictionEligible, "class", "Set prediction eligible." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    bool bCanPredict = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "canPredict" );

    pAnimating->SetPredictionEligible( bCanPredict );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetPredictionPlayer, "class|static", "Set prediction player locally." )
{
    C_BasePlayer *pPlayer = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

    lua_CBaseAnimating::SetPredictionPlayer( pPlayer );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetReceivedSequence, "class", "Set the received sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->SetReceivedSequence();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetSequence, "class", "Set the sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    pAnimating->SetSequence( iSequence );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetServerIntendedCycle, "class", "Set the server intended cycle." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flCycle = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "cycle" );

    pAnimating->SetServerIntendedCycle( flCycle );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ShadowCastType, "class", "Should this object cast render-to-texture shadows?." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->ShadowCastType() );

    return 1;
}
LUA_BINDING_END( "integer|ShadowType_t", "The shadow cast type" )

LUA_BINDING_BEGIN( CBaseAnimating, ShouldMuzzleFlash, "class", "Is the muzzle flash event on?" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->ShouldMuzzleFlash() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity should muzzle flash" )

LUA_BINDING_BEGIN( CBaseAnimating, ShouldResetSequenceOnNewModel, "class", "Check if the sequence should reset on a new model." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->ShouldResetSequenceOnNewModel() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the sequence should reset on a new model" )

LUA_BINDING_BEGIN( CBaseAnimating, ShutdownBoneSetupThreadPool, "class|static", "Shutdown the bone setup thread pool." )
{
    CBaseAnimating::ShutdownBoneSetupThreadPool();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, Simulate, "class", "Simulate." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->Simulate();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, StudioFrameAdvance, "class", "Advances the animation frame." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->StudioFrameAdvance();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ThreadedBoneSetup, "class|static", "Threaded bone setup." )
{
    CBaseAnimating::ThreadedBoneSetup();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, TransferDissolveFrom, "class", "Transfer the dissolve from." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    C_BaseAnimating *pAnimatingFrom = LUA_BINDING_ARGUMENT( luaL_checkanimating, 2, "entityFrom" );

    pAnimating->TransferDissolveFrom( pAnimatingFrom );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, UncorrectViewModelAttachment, "class", "All view model attachments origins are stretched so you can place entities at them an dthey will match up with where the attachment winds up being drawn on the view model, since the view models are drawn with a different FOV.\n\nIf you're drawing something inside of a view model's DrawModel() function, then you want the original attachment origin instead of the adjusted one. To get that, call this on the adjusted attachment origin. \nThis only works for C_BaseViewModel entities." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    Vector vecOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" );

    pAnimating->UncorrectViewModelAttachment( vecOrigin );
    lua_pushvector( L, vecOrigin );

    return 1;
}
LUA_BINDING_END( "vector", "The uncorrected view model attachment origin" )

LUA_BINDING_BEGIN( CBaseAnimating, UpdateClientSideAnimation, "class", "Update the client side animation for the given entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->UpdateClientSideAnimation();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, UpdateClientSideAnimations, "class|static", "Update the client side animations for all in the g_ClientSideAnimationList." )
{
    CBaseAnimating::UpdateClientSideAnimations();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, UpdateIKLocks, "class", "Update the IK locks." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "`currentTime" );

    pAnimating->UpdateIKLocks( flTime );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, UseClientSideAnimation, "class", "Use the client side animation." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->UseClientSideAnimation();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, UsesPowerOfTwoFrameBufferTexture, "class", "Check if the entity uses a power of two frame buffer texture." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->UsesPowerOfTwoFrameBufferTexture() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity uses a power of two frame buffer texture" )

/*
** Open CBaseAnimating object
*/
LUALIB_API int luaopen_CBaseAnimating( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEANIMATINGLIBNAME );
    //luaL_register( L, NULL, CBaseAnimatingmeta );
    //lua_pushstring( L, "Entity" );
    //lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */ // moved to lbaseanimating_shared
    LUA_REGISTRATION_COMMIT( CBaseAnimating );

    return 1;
}
