#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "ltakedamageinfo.h"
#include "mathlib/lvector.h"
#include "items.h"
#include <lrecipientfilter.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( CBaseEntity )

LUA_BINDING_BEGIN( CBaseEntity, RecalcHasPlayerChildBit, "class", "Recalculate has player child bit (EFL_HAS_PLAYER_CHILD)." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->RecalcHasPlayerChildBit();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, AddDeleteOnRemove, "class", "Add an entity to the list of entities being deleted when this entity is removed." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_CBaseEntity *pEntityToDelete = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entityToDelete" );
    pEntity->AddDeleteOnRemove( pEntityToDelete );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, RemoveDeleteOnRemove, "class", "Remove an entity from the list of entities being deleted when this entity is removed." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_CBaseEntity *pEntityToDelete = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entityToDelete" );
    pEntity->RemoveDeleteOnRemove( pEntityToDelete );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DoesHavePlayerChild, "class", "Whether this entity has a player child somewhere in the hierarchy (checks EFL_HAS_PLAYER_CHILD)." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->DoesHavePlayerChild() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if has player child, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, SetNavIgnore, "class", "Set nav ignore." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flDuration = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "duration" );
    pEntity->SetNavIgnore( flDuration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, ClearNavIgnore, "class", "Clear nav ignore." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->ClearNavIgnore();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, IsNavIgnored, "class", "Is nav ignored." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsNavIgnored() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if nav ignored, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, SetTransmitState, "class", "Set transmit state." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nState = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "state" );
    lua_pushinteger( L, pEntity->SetTransmitState( nState ) );
    return 1;
}
LUA_BINDING_END( "number", "The transmit state" )

LUA_BINDING_BEGIN( CBaseEntity, GetTransmitState, "class", "Get transmit state." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushinteger( L, pEntity->GetTransmitState() );
    return 1;
}
LUA_BINDING_END( "number", "The transmit state" )

LUA_BINDING_BEGIN( CBaseEntity, SetPreventTransmit, "class", "Set prevent transmit." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CRecipientFilter filter;

    if ( lua_isrecipientfilter( L, 2 ) )
    {
        filter.CopyFrom( LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 2, "filter" ) );
    }
    else if ( LUA_BINDING_ARGUMENT( lua_istable, 2, "filter" ) )
    {
        lua_pushnil( L );  // First key
        while ( lua_next( L, 2 ) != 0 )
        {
            const CBasePlayer *pPlayer = dynamic_cast< const CBasePlayer * >( luaL_checkentity( L, -1 ) );
            filter.AddRecipient( pPlayer );
            lua_pop( L, 1 );  // Remove the value, keep the key for the next iteration
        }
    }
    else
    {
        const CBasePlayer *pPlayer = dynamic_cast< const CBasePlayer * >( LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "filter" ) );
        filter.AddRecipient( pPlayer );
    }

    pEntity->SetPreventTransmit( filter, LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "shouldPreventTransmitting" ) );
    pEntity->DispatchUpdateTransmitState();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, IncrementTransmitStateOwnedCounter, "class", "Increment transmit state owned counter." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->IncrementTransmitStateOwnedCounter();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DecrementTransmitStateOwnedCounter, "class", "Decrement transmit state owned counter." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->DecrementTransmitStateOwnedCounter();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DetectInSkybox, "class", "This function finds out if the entity is in the 3D skybox. If so, it sets the EFL_IN_SKYBOX flag so the entity gets transmitted to all the clients. Entities usually call this during their Activate(). Returns true if the entity is in the skybox (and EFL_IN_SKYBOX was set).." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->DetectInSkybox() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if in skybox, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, GetEntitySkybox, "class", "Returns the sky camera of the skybox the entity is in." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity::PushLuaInstanceSafe( L, ( CBaseEntity * )pEntity->GetEntitySkybox() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, PostConstructor, "class", "Post constructor." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszClassname = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "className" );
    pEntity->PostConstructor( pszClassname );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, PostClientActive, "class", "Post client active." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->PostClientActive();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, ValidateEntityConnections, "class", "Validate entity connections." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->ValidateEntityConnections();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, SetName, "class", "Set name of entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" );
    pEntity->SetName( MAKE_STRING( pszName ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, GetEntityName, "class", "Get entity name." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushstring( L, STRING( pEntity->GetEntityName() ) );
    return 1;
}
LUA_BINDING_END( "string", "The entity name" )

LUA_BINDING_BEGIN( CBaseEntity, NameMatches, "class", "Name matches." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" );
    lua_pushboolean( L, pEntity->NameMatches( pszName ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if matches, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, ClassMatches, "class", "Class matches." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszClassname = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "classNameOrWildcard" );
    lua_pushboolean( L, pEntity->ClassMatches( pszClassname ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if matches, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, GetSpawnFlags, "class", "Get spawn flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushinteger( L, pEntity->GetSpawnFlags() );
    return 1;
}
LUA_BINDING_END( "number", "The spawn flags" )

LUA_BINDING_BEGIN( CBaseEntity, AddSpawnFlags, "class", "Add spawn flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "flags" );
    pEntity->AddSpawnFlags( nFlags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, RemoveSpawnFlags, "class", "Remove spawn flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "flags" );
    pEntity->RemoveSpawnFlags( nFlags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, ClearSpawnFlags, "class", "Clear spawn flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->ClearSpawnFlags();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, HasSpawnFlags, "class", "Has spawn flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "flags" );
    lua_pushboolean( L, pEntity->HasSpawnFlags( nFlags ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if has flags, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, MakeDormant, "class", "Makes the entity inactive." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->MakeDormant();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, RemoveDeferred, "class", "Sets the entity invisible, and makes it remove itself on the next frame." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->RemoveDeferred();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, OnAcceptInput, "class", "Called when the entity accepted input" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    variant_t emptyVariant;
    const char *pszInput = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "input" );
    CBaseEntity *pActivator = LUA_BINDING_ARGUMENT( lua_toentity, 3, "activator" );
    CBaseEntity *pCaller = LUA_BINDING_ARGUMENT( lua_toentity, 4, "caller" );
    int outputID = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "outputID" );
    lua_pushboolean( L, pEntity->AcceptInput( pszInput, pActivator, pCaller, emptyVariant, outputID ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if accepted, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, GetInputDispatchEffectPosition, "class", "Get input dispatch effect position." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vPosition;
    QAngle vAngles;
    const char *pszInput = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "input" );
    pEntity->GetInputDispatchEffectPosition( pszInput, vPosition, vAngles );
    lua_pushvector( L, vPosition );
    lua_pushangle( L, vAngles );
    return 2;
}
LUA_BINDING_END( "vector, angle", "The position and angles" )

LUA_BINDING_BEGIN( CBaseEntity, EntityText, "class", "Entity text." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int textOffset = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "textOffset" );
    const char *pszText = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "text" );
    float flDuration = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "duration" );
    int nR = LUA_BINDING_ARGUMENT( luaL_checkinteger, 5, "r" );
    int nG = LUA_BINDING_ARGUMENT( luaL_checkinteger, 6, "g" );
    int nB = LUA_BINDING_ARGUMENT( luaL_checkinteger, 7, "b" );
    int nA = LUA_BINDING_ARGUMENT( luaL_checkinteger, 8, "a" );
    pEntity->EntityText( textOffset, pszText, flDuration, nR, nG, nB, nA );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DrawDebugGeometryOverlays, "class", "Draw debug geometry overlays." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->DrawDebugGeometryOverlays();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DrawDebugTextOverlays, "class", "Draw debug text overlays." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushinteger( L, pEntity->DrawDebugTextOverlays() );
    return 1;
}
LUA_BINDING_END( "number", "The number of overlays" )

LUA_BINDING_BEGIN( CBaseEntity, DrawTimedOverlays, "class", "Draw timed overlays." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->DrawTimedOverlays();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DrawBBoxOverlay, "class", "Draw bounding box overlay." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flDuration = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "duration" );
    pEntity->DrawBBoxOverlay( flDuration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DrawAbsBoxOverlay, "class", "Draw absolute box overlay." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->DrawAbsBoxOverlay();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DrawRBoxOverlay, "class", "Draw rotated box overlay." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->DrawRBoxOverlay();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, SendDebugPivotOverlay, "class", "Send debug pivot overlay." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->SendDebugPivotOverlay();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, AddTimedOverlay, "class", "Add timed overlay." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszText = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "text" );
    int nDuration = LUA_BINDING_ARGUMENT( luaL_checkinteger, 3, "durationInSeconds" );
    pEntity->AddTimedOverlay( pszText, nDuration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, RequiredEdictIndex, "class", "Required edict index." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushinteger( L, pEntity->RequiredEdictIndex() );
    return 1;
}
LUA_BINDING_END( "number", "The edict index" )

LUA_BINDING_BEGIN( CBaseEntity, OnMoveDone, "class", "Called when an entity has finished moving." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->MoveDone();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, Instance, "class|static", "Get entity instance." )
{
    int nEdictIndex = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "edictIndex" );
    CBaseEntity::PushLuaInstanceSafe( L, CBaseEntity::Instance( nEdictIndex ) );
    return 1;
}
LUA_BINDING_END( "entity", "The entity instance" )

LUA_BINDING_BEGIN( CBaseEntity, AddContext, "class", "Add context." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszContext = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "nameAndValue" );
    pEntity->AddContext( pszContext );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DispatchResponse, "class", "Dispatch response." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszResponse = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "conceptName" );
    pEntity->DispatchResponse( pszResponse );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, Classify, "class", "Returns the type of group (i.e, 'houndeye', or 'human military' so that NPCs with different classnames still realize that they are teammates. (overridden for NPCs that form groups)." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushinteger( L, pEntity->Classify() );
    return 1;
}
LUA_BINDING_END( "integer|Class_T", "The classification" )

LUA_BINDING_BEGIN( CBaseEntity, DeathNotice, "class", "Death notice." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pKiller = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "killer" );
    pEntity->DeathNotice( pKiller );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, ShouldAttractAutoAim, "class", "Checks if the autoaim should be attracted towards the given entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pAimingEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "aimingEntity" );
    lua_pushboolean( L, pEntity->ShouldAttractAutoAim( pAimingEntity ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if should attract, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, GetAutoAimRadius, "class", "Get auto aim radius." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushnumber( L, pEntity->GetAutoAimRadius() );
    return 1;
}
LUA_BINDING_END( "number", "The auto aim radius" )

LUA_BINDING_BEGIN( CBaseEntity, GetAutoAimCenter, "class", "Get auto aim center." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushvector( L, pEntity->GetAutoAimCenter() );
    return 1;
}
LUA_BINDING_END( "vector", "The auto aim center" )

LUA_BINDING_BEGIN( CBaseEntity, PassesDamageFilter, "class", "Passes damage filter." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CTakeDamageInfo info = LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 2, "damageInfo" );
    lua_pushboolean( L, pEntity->PassesDamageFilter( info ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if passes, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, CanBeHitByMeleeAttack, "class", "Can be hit by melee attack." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pAttacker = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "attacker" );
    lua_pushboolean( L, pEntity->CanBeHitByMeleeAttack( pAttacker ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if can be hit, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, OnTakeDamage, "class", "On take damage." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CTakeDamageInfo info = LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 2, "damageInfo" );
    lua_pushinteger( L, pEntity->OnTakeDamage( info ) );
    return 1;
}
LUA_BINDING_END( "number", "The damage taken" )

LUA_BINDING_BEGIN( CBaseEntity, TakeDamage, "class", "Take damage." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CTakeDamageInfo info = LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 2, "damageInfo" );
    pEntity->TakeDamage( info );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, TakeHealth, "class", "Take health." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flHealth = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "health" );
    int nBitsDamageType = LUA_BINDING_ARGUMENT( luaL_checkinteger, 3, "bitsDamageType" );
    lua_pushinteger( L, pEntity->TakeHealth( flHealth, nBitsDamageType ) );
    return 1;
}
LUA_BINDING_END( "number", "The health taken" )

LUA_BINDING_BEGIN( CBaseEntity, OnEventKilled, "class", "Called when the 'Killed' event happened." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CTakeDamageInfo info = LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 2, "damageInfo" );
    pEntity->Event_Killed( info );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, OnEventKilledOther, "class", "Called when the 'KilledOther' event happened." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pVictim = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "victim" );
    CTakeDamageInfo info = LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 3, "damageInfo" );
    pEntity->Event_KilledOther( pVictim, info );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, IsTriggered, "class", "Is triggered." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pActivator = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "activator" );
    lua_pushboolean( L, pEntity->IsTriggered( pActivator ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if triggered, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, GetDelay, "class", "Get the delay for the entity. Used mostly for trigger entities." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushnumber( L, pEntity->GetDelay() );
    return 1;
}
LUA_BINDING_END( "number", "The delay" )

LUA_BINDING_BEGIN( CBaseEntity, IsMoving, "class", "Is moving." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsMoving() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if moving, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, IsWorld, "class", "Whether entity is world." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsWorld() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if world, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, AddPoints, "class", "Add points." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nPoints = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "points" );
    bool bAllowNegative = LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "allowNegative" );
    pEntity->AddPoints( nPoints, bAllowNegative );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, AddPointsToTeam, "class", "Add points to team." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nPoints = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "points" );
    bool bAllowNegative = LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "allowNegative" );
    pEntity->AddPointsToTeam( nPoints, bAllowNegative );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, IsControllingEntity, "class", "Whether we are controlling the given entity (train, tank, etc.) " )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pEntity2 = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "testEntityUnderControl" );
    lua_pushboolean( L, pEntity->OnControls( pEntity2 ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if on controls, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, HasTarget, "class", "Has target." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszTarget = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "target" );
    lua_pushboolean( L, pEntity->HasTarget( MAKE_STRING( pszTarget ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if has target, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, IsNetClient, "class", "Is net client." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsNetClient() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if net client, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, IsTemplate, "class", "Wheter this entity is a template. Template entities are marked as templates by the level designer. They do not spawn, but their keyvalues are saved for use by a template spawner." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsTemplate() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if template, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, IsViewable, "class", "Is this entity something that would be looked at (model, sprite, etc.)." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsViewable() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if viewable, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, IsInAnyTeam, "class", "Is in any team." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsInAnyTeam() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if in any team, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, GetTeamID, "class", "Team ID." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushstring( L, pEntity->TeamID() );
    return 1;
}
LUA_BINDING_END( "string", "The team ID" )

LUA_BINDING_BEGIN( CBaseEntity, CanStandOn, "class", "Whether this entity can stand on the given surface entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pGround = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "surfaceEntity" );
    lua_pushboolean( L, pEntity->CanStandOn( pGround ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if can stand on, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, GetEnemy, "class", "Get enemy. Probably for NPC's only to get their current enemy." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity::PushLuaInstanceSafe( L, pEntity->GetEnemy() );
    return 1;
}
LUA_BINDING_END( "entity", "The enemy" )

LUA_BINDING_BEGIN( CBaseEntity, VelocityPunch, "class", "Velocity punch." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vecForce = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "force" );
    pEntity->VelocityPunch( vecForce );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, GetNextTarget, "class", "Get next target." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity::PushLuaInstanceSafe( L, pEntity->GetNextTarget() );
    return 1;
}
LUA_BINDING_END( "entity", "The next target" )

LUA_BINDING_BEGIN( CBaseEntity, Use, "class", "Dispatches use events to this entity's use handler, set via SetUse." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pActivator = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "activator" );
    CBaseEntity *pCaller = LUA_BINDING_ARGUMENT( luaL_checkentity, 3, "caller" );
    USE_TYPE useType = ( USE_TYPE )LUA_BINDING_ARGUMENT( luaL_checkinteger, 4, "useType" );
    float value = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "value" );
    pEntity->Use( pActivator, pCaller, useType, value );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, StartBlockedByEntity, "class", "Start blocked." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "blockingEntity" );
    pEntity->StartBlocked( pOther );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, RunBlockedByEntityEvent, "class", "Dispatches blocked events to this entity's blocked handler, set via SetBlocked." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "blockingEntity" );
    pEntity->Blocked( pOther );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, EndBlockedByEntity, "class", "End blocked." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->EndBlocked();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, PhysicsTouchTriggers, "class", "Physics touch triggers." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->PhysicsTouchTriggers();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DoNothing, "class", "Described as 'Convenient way to explicitly do nothing (passed to functions that require a method)'" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->SUB_DoNothing();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, StartFadeOut, "class", "Slowly fades a entity out, then removes it." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flDuration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 10.0f, "duration" );
    bool bInstant = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, true, "instant" );
    pEntity->SUB_StartFadeOut( flDuration, bInstant );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, StartFadeOutInstant, "class", "Start fade out instant." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->SUB_StartFadeOutInstant();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, FadeOut, "class", "Fade out." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->SUB_FadeOut();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, Vanish, "class", "Vanish." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->SUB_Vanish();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, CallUseToggle, "class", "Call use toggle." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->SUB_CallUseToggle();
    return 0;
}
LUA_BINDING_END()
\
LUA_BINDING_BEGIN( CBaseEntity, PerformFadeOut, "class", "Perform fade out." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->SUB_PerformFadeOut();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, IsAllowedToFade, "class", "Allowed to fade." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->SUB_AllowedToFade() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if allowed to fade, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, Teleport, "class", "Teleport." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vPosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "newPosition" );
    QAngle qAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "newAngles" );
    Vector vVelocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "newVelocity" );
    pEntity->Teleport( &vPosition, &qAngles, &vVelocity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, ShouldToggle, "class", "Should toggle." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    USE_TYPE useType = ( USE_TYPE )LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "useType" );
    int iState = LUA_BINDING_ARGUMENT( luaL_checkinteger, 3, "currentState" );
    lua_pushinteger( L, pEntity->ShouldToggle( useType, iState ) );
    return 1;
}
LUA_BINDING_END( "number", "The toggle state" )

LUA_BINDING_BEGIN( CBaseEntity, Respawn, "class", "Respawn." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity::PushLuaInstanceSafe( L, pEntity->Respawn() );
    return 1;
}
LUA_BINDING_END( "entity", "The respawned entity" )

LUA_BINDING_BEGIN( CBaseEntity, TraceAttackToTriggers, "class", "Trace attack to triggers." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CTakeDamageInfo info = LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 2, "damageInfo" );
    Vector vStartPos = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "startPos" );
    Vector vEndPos = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "endPos" );
    Vector vDir = LUA_BINDING_ARGUMENT( luaL_checkvector, 5, "direction" );
    pEntity->TraceAttackToTriggers( info, vStartPos, vEndPos, vDir );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, Intersects, "class", "Determines if the bounding boxes of two entities intersect." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "other" );
    lua_pushboolean( L, pEntity->Intersects( pOther ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if intersects, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, IsLockedByMain, "class", "Whether this door is locked by the main door (described as 'master' door in keyvalues)." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsLockedByMaster() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if locked by main, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, SetMaxHealth, "class", "Set max health." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nMaxHealth = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "maxHealth" );
    pEntity->SetMaxHealth( nMaxHealth );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DumpResponseCriteria, "class", "Dump response criteria." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->DumpResponseCriteria();
    return 0;
}
LUA_BINDING_END()

//// Experiment; Disabled in favor of CreateEntityByName which also calls DispatchSpawn (that installs the correct ENT table)
//// LUA_BINDING_BEGIN( CBaseEntity, Create, "class", "Create entity." )
//// {
////     const char *pszClassName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "className" );
////     Vector vPosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
////     QAngle qAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "angles" );
////     CBaseEntity *pOwner = LUA_BINDING_ARGUMENT( luaL_optentity, 4, NULL, "owner" );
////     CBaseEntity::PushLuaInstanceSafe( L, CBaseEntity::Create( pszClassName, vPosition, qAngles, pOwner ) );
////     return 1;
//// }
//// LUA_BINDING_END( "entity", "The created entity" )
////
//// LUA_BINDING_BEGIN( CBaseEntity, CreateNoSpawn, "class", "Create entity without spawning it." )
//// {
////     const char *pszClassName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "className" );
////     Vector vPosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
////     QAngle qAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "angles" );
////     CBaseEntity *pOwner = LUA_BINDING_ARGUMENT( luaL_optentity, 4, NULL, "owner" );
////     CBaseEntity::PushLuaInstanceSafe( L, CBaseEntity::CreateNoSpawn( pszClassName, vPosition, qAngles, pOwner ) );
////     return 1;
//// }
//// LUA_BINDING_END( "entity", "The created entity" )

LUA_BINDING_BEGIN( CBaseEntity, GetDamageType, "class", "Get damage type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushinteger( L, pEntity->GetDamageType() );
    return 1;
}
LUA_BINDING_END( "number", "The damage type" )

LUA_BINDING_BEGIN( CBaseEntity, GetDamage, "class", "Get damage." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushnumber( L, pEntity->GetDamage() );
    return 1;
}
LUA_BINDING_END( "number", "The damage" )

LUA_BINDING_BEGIN( CBaseEntity, SetDamage, "class", "Set damage." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flDamage = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damage" );
    pEntity->SetDamage( flDamage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, BodyTarget, "class", "Body target." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vPosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
    bool bNoisy = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, true, "noisy" );
    lua_pushvector( L, pEntity->BodyTarget( vPosition, bNoisy ) );
    return 1;
}
LUA_BINDING_END( "vector", "The body target" )

LUA_BINDING_BEGIN( CBaseEntity, HeadTarget, "class", "Head target." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vPosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
    lua_pushvector( L, pEntity->HeadTarget( vPosition ) );
    return 1;
}
LUA_BINDING_END( "vector", "The head target" )

LUA_BINDING_BEGIN( CBaseEntity, GetSmoothedVelocity, "class", "Get smoothed velocity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushvector( L, pEntity->GetSmoothedVelocity() );
    return 1;
}
LUA_BINDING_END( "vector", "The smoothed velocity" )

LUA_BINDING_BEGIN( CBaseEntity, GetFriction, "class", "Get friction." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushnumber( L, pEntity->GetFriction() );
    return 1;
}
LUA_BINDING_END( "number", "The friction" )

LUA_BINDING_BEGIN( CBaseEntity, IsTargetVisible, "class", "Returns true if a line can be traced from the caller's eyes to the target" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    if ( lua_isuserdata( L, 2 ) && lua_isvector( L, 2 ) )
    {
        Vector vPosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
        int nMask = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 3, MASK_BLOCKLOS, "mask" );
        lua_pushboolean( L, pEntity->FVisible( vPosition, nMask ) );
    }
    else
    {
        CBaseEntity *pEntity2 = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "target" );
        int nMask = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 3, MASK_BLOCKLOS, "mask" );
        lua_pushboolean( L, pEntity->FVisible( pEntity2, nMask ) );
    }

    return 1;
}
LUA_BINDING_END( "boolean", "true if visible, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, GetReceivedDamageScale, "class", "Returns a value that scales all damage done to this entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pInflictor = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "inflictor" );
    lua_pushnumber( L, pEntity->GetReceivedDamageScale( pInflictor ) );
    return 1;
}
LUA_BINDING_END( "number", "The received damage scale" )

LUA_BINDING_BEGIN( CBaseEntity, GetGroundVelocityToApply, "class", "Gets the velocity we impart to a player standing on us." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vecGroundVel;
    pEntity->GetGroundVelocityToApply( vecGroundVel );
    lua_pushvector( L, vecGroundVel );
    return 1;
}
LUA_BINDING_END( "vector", "The ground velocity to apply" )

LUA_BINDING_BEGIN( CBaseEntity, PhysicsSplash, "class", "Physics splash." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vCenter = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "center" );
    Vector vNormal = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "normal" );
    float flSpeed = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "rawSpeed" );
    float scaledSpeed = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "scaledSpeed" );
    lua_pushboolean( L, pEntity->PhysicsSplash( vCenter, vNormal, flSpeed, scaledSpeed ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if splashed, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, Splash, "class", "Splash." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->Splash();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, ClearSolidFlags, "class", "Clear solid flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->ClearSolidFlags();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, SetElasticity, "class", "Set elasticity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flElasticity = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "elasticity" );
    pEntity->SetElasticity( flElasticity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, SetShadowCastDistance, "class", "Set shadow cast distance." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flDistance = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "distance" );

    if ( !lua_isnoneornil( L, 3 ) )
    {
        float flMaxDistance = LUA_BINDING_ARGUMENT_NILLABLE( luaL_checknumber, 3, "maxDistance" );
        pEntity->SetShadowCastDistance( flDistance, flMaxDistance );
    }
    else
    {
        pEntity->SetShadowCastDistance( flDistance );
    }
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, GetLocalTime, "class", "Get local time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushnumber( L, pEntity->GetLocalTime() );
    return 1;
}
LUA_BINDING_END( "number", "The local time" )

LUA_BINDING_BEGIN( CBaseEntity, IncrementLocalTime, "class", "Increment local time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" );
    pEntity->IncrementLocalTime( flTime );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, GetMoveDoneTime, "class", "Get move done time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushnumber( L, pEntity->GetMoveDoneTime() );
    return 1;
}
LUA_BINDING_END( "number", "The move done time" )

LUA_BINDING_BEGIN( CBaseEntity, SetMoveDoneTime, "class", "Set move done time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" );
    pEntity->SetMoveDoneTime( flTime );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, GetSoundEmissionOrigin, "class", "From a source SDK comment 'Used by the PAS filters to ask the entity where in world space the sounds it emits come from. This is used right now because if you have something sitting on an incline, using our axis-aligned bounding boxes can return a position in solid space, so you won't hear sounds emitted by the object. For now, we're hacking around it by moving the sound emission origin up on certain objects like vehicles..'" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushvector( L, pEntity->GetSoundEmissionOrigin() );
    return 1;
}
LUA_BINDING_END( "vector", "The sound emission origin" )

LUA_BINDING_BEGIN( CBaseEntity, IsItem, "class", "Whether this entity is an item." )
{
    CItem *pItem = dynamic_cast< CItem * >( LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" ) );

    if ( pItem )
    {
        lua_pushboolean( L, true );
    }
    else
    {
        lua_pushboolean( L, false );
    }
    return 1;
}
LUA_BINDING_END( "boolean", "true if item, false otherwise." )

/*
** Open CBaseEntity object
*/
LUALIB_API int luaopen_CBaseEntity( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEENTITYMETANAME );

    LUA_REGISTRATION_COMMIT( CBaseEntity );

    return 1;
}
