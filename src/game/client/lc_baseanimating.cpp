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

LUA_REGISTRATION_INIT( CBaseAnimating )

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
LUA_BINDING_END( "Entity", "The ragdoll entity" )

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

    lua_pushinteger( L, pAnimating->ComputeClientSideAnimationFlags() );

    return 1;
}
LUA_BINDING_END( "integer", "The client side animation flags" )

LUA_BINDING_BEGIN( CBaseAnimating, CreateRagdollCopy, "class", "Create a ragdoll copy of the entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pAnimating->CreateRagdollCopy() );

    return 1;
}
LUA_BINDING_END( "Entity", "The ragdoll entity" )

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

LUA_BINDING_BEGIN( CBaseAnimating, DrawClientHitboxes, "class", "Draw the client hitboxes." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flDuration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.0f, "duration" );
    bool bMonoColor = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "monoColor" );

    pAnimating->DrawClientHitboxes( flDuration, bMonoColor );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, FindFollowedEntity, "class", "Find the followed entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pAnimating->FindFollowedEntity() );

    return 1;
}
LUA_BINDING_END( "Entity", "The followed entity" )

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
LUA_BINDING_END( "Vector", "The aim entity origin", "Angle", "The aim entity angles" )

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
LUA_BINDING_END( "boolean", "Whether the operation was successful", "Vector", "The origin velocity" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBlendedLinearVelocity, "class", "Get the blended linear velocity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    Vector pVec;

    pAnimating->GetBlendedLinearVelocity( &pVec );

    lua_pushvector( L, pVec );

    return 1;
}
LUA_BINDING_END( "Vector", "The blended linear velocity" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBody, "class", "Get the body (internally described as 'Object bodygroup'?)." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetBody() );

    return 1;
}
LUA_BINDING_END( "integer", "The body" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBoneControllers, "class", "Get all bone controller values." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float controllers[MAXSTUDIOBONECTRLS];

    pAnimating->GetBoneControllers( controllers );

    lua_newtable( L );

    for ( int i = 0; i < MAXSTUDIOBONECTRLS; i++ )
    {
        lua_pushnumber( L, controllers[i] );
        lua_rawseti( L, -2, i + 1 ); // 1-based index
    }

    return MAXSTUDIOBONECTRLS;
}
LUA_BINDING_END( "table", "The bone controller values" )

LUA_BINDING_BEGIN( CBaseAnimating, GetClientSideFade, "class", "Get the client side fade." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetClientSideFade() );

    return 1;
}
LUA_BINDING_END( "integer", "The client side fade" )

LUA_BINDING_BEGIN( CBaseAnimating, GetCollideType, "class", "Get the collide type." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetCollideType() );

    return 1;
}
LUA_BINDING_END( "enumeration/COLLIDE_TYPE", "The collide type" )

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

LUA_BINDING_BEGIN( CBaseAnimating, GetRenderBounds, "class", "Get the render bounds." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    Vector theMins, theMaxs;

    pAnimating->GetRenderBounds( theMins, theMaxs );

    lua_pushvector( L, theMins );
    lua_pushvector( L, theMaxs );

    return 2;
}
LUA_BINDING_END( "Vector", "The minimum bounds", "Vector", "The maximum bounds" )

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
LUA_BINDING_END( "integer", "The skin" )

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
LUA_BINDING_END( "integer", "The resulting draw flag" )

LUA_BINDING_BEGIN( CBaseAnimating, Interpolate, "class", "Interpolate the entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flCurrentTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "currentTime" );

    lua_pushboolean( L, pAnimating->Interpolate( flCurrentTime ) );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful" )

LUA_BINDING_BEGIN( CBaseAnimating, InvalidateBoneCaches, "class|static", "Invalidate the bone caches." )
{
    CBaseAnimating::InvalidateBoneCaches();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, IsBoneCacheValid, "class", "Check if the bone cache is valid." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsBoneCacheValid() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the bone cache is valid" )

LUA_BINDING_BEGIN( CBaseAnimating, IsSelfAnimating, "class", "Check if the entity is self animating." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsSelfAnimating() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity is self animating" )

LUA_BINDING_BEGIN( CBaseAnimating, IsViewModel, "class", "Check if the entity is a view model." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsViewModel() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity is a view model" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupRandomAttachment, "class", "Get a random index of an attachment point with the specified substring in its name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszAttachmentName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "attachmentNameSubstring" );

    lua_pushinteger( L, pAnimating->LookupRandomAttachment( pszAttachmentName ) );

    return 1;
}
LUA_BINDING_END( "integer", "The random attachment" )

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
LUA_BINDING_END( "boolean", "Whether the operation was successful", "Vector", "The position" )

LUA_BINDING_BEGIN( CBaseAnimating, SetPredictable, "class", "Set predictable." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    bool bPredictable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "predictable" );

    pAnimating->SetPredictable( bPredictable );

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
LUA_BINDING_END( "enumeration/SHADOW_TYPE", "The shadow cast type" )

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

LUA_BINDING_BEGIN( CBaseAnimating, ThreadedBoneSetup, "class|static", "Threaded bone setup." )
{
    CBaseAnimating::ThreadedBoneSetup();

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
LUA_BINDING_END( "Vector", "The uncorrected view model attachment origin" )

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

/*
** Open CBaseAnimating object
*/
LUALIB_API int luaopen_CBaseAnimating( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEANIMATINGLIBNAME );
    
    //lua_pushstring( L, "Entity" );
    //lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */ // moved to lbaseanimating_shared
    LUA_REGISTRATION_COMMIT( CBaseAnimating );

    return 1;
}
