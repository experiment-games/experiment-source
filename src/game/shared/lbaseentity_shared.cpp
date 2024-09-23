#include "cbase.h"
#ifdef CLIENT_DLL
#include "c_recipientfilter.h"
#define CRecipientFilter C_RecipientFilter
#else
#include "recipientfilter.h"
#endif
#include "takedamageinfo.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "lrecipientfilter.h"
#include "weapon_experimentbase_scriptedweapon.h"
#ifndef CLIENT_DLL
#include "basescripted.h"
#include "iservervehicle.h"
#endif
#include "lbaseplayer_shared.h"
#include "lgametrace.h"
#include "SoundEmitterSystem/lisoundemittersystembase.h"
#include "lshareddefs.h"
#include "ltakedamageinfo.h"
#include "mathlib/lvector.h"
#include "lvphysics_interface.h"
#include "engine/IEngineSound.h"
#include <lbasecombatweapon_shared.h>
#include "vcollide_parse.h"
#include <gamestringpool.h>
#include <basescripted.h>
#include <lColor.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseEntity *lua_toentity( lua_State *L, int idx )
{
    CBaseHandle *hEntity =
        dynamic_cast< CBaseHandle * >( ( CBaseHandle * )lua_touserdata( L, idx ) );
    if ( hEntity == NULL )
        return NULL;
    return dynamic_cast< lua_CBaseEntity * >( hEntity->Get() );
}

/*
** push functions (C -> stack)
*/

LUALIB_API lua_CBaseEntity *luaL_checkentity( lua_State *L, int narg )
{
    lua_CBaseEntity *d = lua_toentity( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "Entity expected, got NULL entity" );
    return d;
}

LUALIB_API lua_CBaseEntity *luaL_optentity( lua_State *L, int narg, CBaseEntity *def )
{
    return luaL_opt( L, luaL_checkentity, narg, def );
}

LUA_REGISTRATION_INIT( Entity )

LUA_BINDING_BEGIN( Entity, Activate, "class", "Activate the entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->Activate();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, AddEffects, "class", "Add effects." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nEffects = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "effects" );

    pEntity->AddEffects( nEffects );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, AddDataObjectType, "class", "Add data object type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "type" );

    pEntity->AddDataObjectType( nType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, AddEntityFlags, "class", "Add entity flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );

    pEntity->AddEFlags( nFlags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, AddFlag, "class", "Add flag to entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlag = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flag" );

    pEntity->AddFlag( nFlag );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, AddSolidFlags, "class", "Add solid flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );

    pEntity->AddSolidFlags( nFlags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, ApplyAbsoluteVelocityImpulse, "class", "Apply absolute velocity impulse." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vImpulse = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "impulse" );

    pEntity->ApplyAbsVelocityImpulse( vImpulse );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, ApplyLocalAngularVelocityImpulse, "class", "Apply local angular velocity impulse." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    AngularImpulse angImpulse = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "impulse" );

    pEntity->ApplyLocalAngularVelocityImpulse( angImpulse );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetBlocksLineOfSight, "class", "Whether this entity blocks line of sight." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->BlocksLOS() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the entity blocks line of sight, false otherwise." )

LUA_BINDING_BEGIN( Entity, GetBloodColorType, "class", "Blood type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->BloodColor() );
    return 1;
}
LUA_BINDING_END( "enumeration/BLOOD_COLOR", "The blood type." )

LUA_BINDING_BEGIN( Entity, GetBoundingRadius, "class", "Gets the bounding radius." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushnumber( L, pEntity->BoundingRadius() );
    return 1;
}
LUA_BINDING_END( "number", "The bounding radius." )

LUA_BINDING_BEGIN( Entity, CalculateNearestPoint, "class", "Computes the nearest point in the OBB to a point specified in world space." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vPoint = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "point" );

    Vector vNearestPoint;
    pEntity->CollisionProp()->CalcNearestPoint( vPoint, &vNearestPoint );
    lua_pushvector( L, vNearestPoint );

    return 1;
}
LUA_BINDING_END( "Vector", "The nearest point." )

LUA_BINDING_BEGIN( Entity, ChangeTeam, "class", "Change the team of the entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nTeam = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "team" );

    pEntity->ChangeTeam( nTeam );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, ClearEffects, "class", "Clear effects." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->ClearEffects();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, ClearFlags, "class", "Clear flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->ClearFlags();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, CollisionRulesChanged, "class", "Collision rules changed." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->CollisionRulesChanged();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, ComputeAbsoluteDirection, "class", "Compute absolute direction." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vLocalDir = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "localDir" );

    Vector vAbsDir;
    pEntity->ComputeAbsDirection( vLocalDir, &vAbsDir );
    lua_pushvector( L, vAbsDir );

    return 1;
}
LUA_BINDING_END( "Vector", "The absolute direction." )

LUA_BINDING_BEGIN( Entity, ComputeAbsolutePosition, "class", "Compute absolute position." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vLocalPos = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "localPos" );

    Vector vAbsPos;
    pEntity->ComputeAbsPosition( vLocalPos, &vAbsPos );
    lua_pushvector( L, vAbsPos );

    return 1;
}
LUA_BINDING_END( "Vector", "The absolute position." )

LUA_BINDING_BEGIN( Entity, ComputeWorldSpaceSurroundingBox, "class", "Compute world space surrounding box." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vMin, vMax;
    pEntity->ComputeWorldSpaceSurroundingBox( &vMin, &vMax );

    lua_pushvector( L, vMin );
    lua_pushvector( L, vMax );
    return 2;
}
LUA_BINDING_END( "Vector", "The minimum vector.", "Vector", "The maximum vector." )

LUA_BINDING_BEGIN( Entity, CreateDataObject, "class", "Create data object." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "type" );

    pEntity->CreateDataObject( nType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, CreatePredictedEntityByName, "class", "Create predicted entity by name." )
{
    const char *pszClassName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "className" );
    const char *pszModule = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "module" );
    int nLine = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "line" );
    bool bPersists = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 5, false, "shouldPersist" );

    CBaseEntity::PushLuaInstanceSafe( L, CBaseEntity::CreatePredictedEntityByName( pszClassName, pszModule, nLine, bPersists ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The predicted entity." )

LUA_BINDING_BEGIN( Entity, CreateVPhysics, "class", "Create VPhysics." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->CreateVPhysics() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if VPhysics was created, false otherwise." )

LUA_BINDING_BEGIN( Entity, DamageDecal, "class", "Damage decal." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nDamageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damageType" );
    int nDamage = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "damage" );

    lua_pushstring( L, pEntity->DamageDecal( nDamageType, nDamage ) );
    return 1;
}
LUA_BINDING_END( "string", "The decal name." )

LUA_BINDING_BEGIN( Entity, DecalTrace, "class", "Decal trace." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CGameTrace tr = LUA_BINDING_ARGUMENT( luaL_checktrace, 2, "trace" );
    const char *pszDecalName = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "decalName" );

    pEntity->DecalTrace( &tr, pszDecalName );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, DestroyAllDataObjects, "class", "Destroy all data objects." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->DestroyAllDataObjects();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, DestroyDataObject, "class", "Destroy data object." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "type" );

    pEntity->DestroyDataObject( nType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, DispatchTraceAttack, "class", "Dispatch trace attack." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CTakeDamageInfo info = LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 2, "damageInfo" );
    Vector vDir = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "direction" );
    CGameTrace tr = LUA_BINDING_ARGUMENT( luaL_checktrace, 4, "trace" );

    pEntity->DispatchTraceAttack( info, vDir, &tr );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, DoImpactEffect, "class", "Do impact effect." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CGameTrace tr = LUA_BINDING_ARGUMENT( luaL_checktrace, 2, "trace" );
    int nDamageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "damageType" );

    pEntity->DoImpactEffect( tr, nDamageType );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetEarPosition, "class", "Get ear position." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushvector( L, pEntity->EarPosition() );
    return 1;
}
LUA_BINDING_END( "Vector", "The ear position." )

LUA_BINDING_BEGIN( Entity, EmitAmbientSound, "class|static", "Emit ambient sound." )
{
    int nSoundIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "soundIndex" );
    Vector vOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" );
    const char *pszSoundName = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "soundName" );
    int nFlags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 0, "flags" );
    float flSoundTime = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 0, "soundTime" );

    float flDuration = 0;
    CBaseEntity::EmitAmbientSound( nSoundIndex, vOrigin, pszSoundName, nFlags, flSoundTime, &flDuration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, EmitSound, "class", "Emit sound." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    const char *pszSoundName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "soundName" );
    soundlevel_t soundLevel = LUA_BINDING_ARGUMENT_ENUM_WITH_DEFAULT( soundlevel_t, 3, SNDLVL_NORM, "soundLevel" );
    float flPitchPercent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 100, "pitchPercent" );
    float flVolume = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 1, "volume" );
    int nChannel = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 6, pEntity->IsWeapon() ? CHAN_WEAPON : CHAN_AUTO, "channel" );
    int nSoundFlags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 7, 0, "soundFlags" );
    int nDSP = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 8, 0, "dsp" );

    CRecipientFilter filter;

    if ( lua_isrecipientfilter( L, 9 ) )
        filter = LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkrecipientfilter, 9, "filter" );
    else
    {
        filter = CPASAttenuationFilter( pEntity, soundLevel );
    }

    float duration = 0;
    int iEntIndex = pEntity->entindex();

    EmitSound_t params;
    params.m_pSoundName = pszSoundName;
    params.m_nChannel = nChannel;
    params.m_flVolume = flVolume;
    params.m_SoundLevel = soundLevel;
    params.m_nFlags = nSoundFlags;
    params.m_nPitch = flPitchPercent;
    params.m_pOrigin = &pEntity->GetAbsOrigin();
    params.m_nSpecialDSP = nDSP;
    params.m_flSoundTime = 0;
    params.m_nSpeakerEntity = iEntIndex;
    params.m_pflSoundDuration = &duration;
    params.m_bWarnOnDirectWaveReference = false;

    pEntity->EmitSound( filter, iEntIndex, params );

    lua_pushnumber( L, duration );

    return 1;
}
LUA_BINDING_END( "number", "The sound duration." )

LUA_BINDING_BEGIN( Entity, EndGroundContact, "class", "End ground contact." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pGround = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "ground" );

    pEntity->EndGroundContact( pGround );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, EndTouch, "class", "End touch." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "other" );

    pEntity->EndTouch( pOther );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetEntityIndex, "class", "Get entity index. Returns 0 for NULL entities." )
{
    // Note:    instead of luaL_checkentity, we soft-check here so NULL values can
    //          have a 0 returned as their index
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );

    if ( pEntity == NULL )
    {
        lua_pushinteger( L, 0 );
    }
    else
    {
        lua_pushinteger( L, pEntity->entindex() );
    }

    return 1;
}
LUA_BINDING_END( "integer", "The entity index." )

LUA_BINDING_BEGIN( Entity, EntityToWorldSpace, "class", "Entity to world space." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vLocal = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "local" );

    Vector vWorld;
    pEntity->EntityToWorldSpace( vLocal, &vWorld );
    lua_pushvector( L, vWorld );

    return 1;
}
LUA_BINDING_END( "Vector", "The world space vector." )

LUA_BINDING_BEGIN( Entity, GetChildren, "class", "Get the moveparent children." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_newtable( L );

    int i = 1;  // Lua tables are 1-based.
    for ( CBaseEntity *pChild = pEntity->FirstMoveChild(); pChild; pChild = pChild->NextMovePeer() )
    {
        CBaseEntity::PushLuaInstanceSafe( L, pChild );
        lua_rawseti( L, -2, i++ );
    }

    return 1;
}
LUA_BINDING_END( "table", "The moveparent children." )

LUA_BINDING_BEGIN( Entity, GetParent, "class", "Get the moveparent." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pEntity->GetMoveParent() );
    return 1;
}
LUA_BINDING_END( "Entity", "The moveparent." )

LUA_BINDING_BEGIN( Entity, GetEyeAngles, "class", "Get eye angles." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    QAngle v = pEntity->EyeAngles();
    lua_pushangle( L, v );
    return 1;
}
LUA_BINDING_END( "Angle", "The eye angles." )

LUA_BINDING_BEGIN( Entity, GetEyePosition, "class", "Get eye position." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushvector( L, pEntity->EyePosition() );
    return 1;
}
LUA_BINDING_END( "Vector", "The eye position." )

LUA_BINDING_BEGIN( Entity, GetLifeState, "class", "Get the life state." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->m_lifeState );
    return 1;
}
LUA_BINDING_END( "enumeration/LIFE", "The life state." )

LUA_BINDING_BEGIN( Entity, SetKeyValue, "class", "Set key value." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszKey = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" );
    const char *pszValue = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "value" );

    pEntity->KeyValue( pszKey, pszValue );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetLifeState, "class", "Set the life state." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nLifeState = LUA_BINDING_ARGUMENT_ENUM_DEFINE( LIFE, 2, "lifeState" );

    pEntity->m_lifeState = nLifeState;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, FireBullets, "class", "Fire bullets." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    FireBulletsInfo_t info = LUA_BINDING_ARGUMENT_WITH_EXTRA( lua_tofirebulletsinfo, 2, pEntity, "info" );

    pEntity->FireBullets( info );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetFirstMoveChild, "class", "Get first move child." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pEntity->FirstMoveChild() );
    return 1;
}
LUA_BINDING_END( "Entity", "The first move child." )

LUA_BINDING_BEGIN( Entity, FollowEntity, "class", "Follow entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pTarget = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "target" );
    bool bBoneMerge = LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "boneMerge" );

    pEntity->FollowEntity( pTarget, bBoneMerge );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GenderExpandString, "class", "Expands any $gender tags in the string into male or female tags based on the gender of the actor (based on his/her .mdl). Used for filesnames only. The returned string is at most 260 characters long." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszString = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "string" );

    char out[MAX_PATH];
    out[0] = '\0';
    pEntity->GenderExpandString( pszString, out, sizeof( out ) );
    lua_pushstring( L, out );

    return 1;
}
LUA_BINDING_END( "string", "The expanded string." )

LUA_BINDING_BEGIN( Entity, GetAngles, "class", "Get absolute angles." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    QAngle v = pEntity->GetAbsAngles();
    lua_pushangle( L, v );
    return 1;
}
LUA_BINDING_END( "Angle", "The absolute angles." )

LUA_BINDING_BEGIN( Entity, GetPosition, "class", "Get absolute origin." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->GetAbsOrigin();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The absolute origin." )

LUA_BINDING_BEGIN( Entity, GetVelocity, "class", "Get absolute velocity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->GetAbsVelocity();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The absolute velocity." )

LUA_BINDING_BEGIN( Entity, GetAnimationTime, "class", "Get animation time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushnumber( L, pEntity->GetAnimTime() );
    return 1;
}
LUA_BINDING_END( "number", "The animation time." )

// Experiment; Not very useful in Lua, so disabled.
// LUA_BINDING_BEGIN( Entity, GetBaseAnimating, "class", "Get base animating." )
//{
//    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
//
//    CBaseAnimating::PushLuaInstanceSafe( L, pEntity->GetBaseAnimating() );
//    return 1;
//}
// LUA_BINDING_END( "Entity", "The base animating." )

// Experiment; Not very useful in Lua, so disabled.
// LUA_BINDING_BEGIN( Entity, GetBaseEntity, "class", "Get base entity." )
//{
//    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
//
//    CBaseEntity::PushLuaInstanceSafe( L, pEntity->GetBaseEntity() );
//    return 1;
//}
// LUA_BINDING_END( "Entity", "The base entity." )

LUA_BINDING_BEGIN( Entity, GetBaseVelocity, "class", "Get base velocity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->GetBaseVelocity();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The base velocity." )

LUA_BINDING_BEGIN( Entity, GetCheckUntouch, "class", "Get check untouch." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->GetCheckUntouch() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if check untouch, false otherwise." )

LUA_BINDING_BEGIN( Entity, GetClass, "class", "Get class name." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

#ifdef CLIENT_DLL
    if ( pEntity->IsScripted() )
    {
        CBaseScripted *pScripted = dynamic_cast< CBaseScripted * >( pEntity );

        if ( pScripted )
            lua_pushstring( L, pScripted->GetScriptedClassname() );
        else
        {
            CExperimentScriptedWeapon *pWeapon = dynamic_cast< CExperimentScriptedWeapon * >( pEntity );

            AssertMsg( pWeapon, "Entity is not a scripted entity, nor weapon." );  // which other scripted entity is there?

            lua_pushstring( L, pWeapon->GetScriptedClassname() );
        }
    }
    else
#endif
        lua_pushstring( L, pEntity->GetClassname() );

    return 1;
}
LUA_BINDING_END( "string", "The class name." )

LUA_BINDING_BEGIN( Entity, GetCollisionGroup, "class", "Get collision group." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetCollisionGroup() );
    return 1;
}
LUA_BINDING_END( "enumeration/COLLISION_GROUP", "The collision group." )

LUA_BINDING_BEGIN( Entity, GetDataObject, "class", "Get data object." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "type" );

    pEntity->GetDataObject( nType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetDebugName, "class", "Get debug name." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushstring( L, pEntity->GetDebugName() );
    return 1;
}
LUA_BINDING_END( "string", "The debug name." )

LUA_BINDING_BEGIN( Entity, GetDLLType, "class|static", "Get DLL type, either client or server depending on where its called." )
{
    lua_pushstring( L, CBaseEntity::GetDLLType() );
    return 1;
}
LUA_BINDING_END( "string", "The DLL type: client or server, depending on where it's called." )

LUA_BINDING_BEGIN( Entity, GetEffectEntity, "class", "Get effect entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pEntity->GetEffectEntity() );
    return 1;
}
LUA_BINDING_END( "Entity", "The effect entity." )

LUA_BINDING_BEGIN( Entity, GetEffects, "class", "Get effects." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetEffects() );
    return 1;
}
LUA_BINDING_END( "integer", "The ENTITY_EFFECT flags." )

LUA_BINDING_BEGIN( Entity, GetEntityFlags, "class", "Get Entity Flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetEFlags() );
    return 1;
}
LUA_BINDING_END( "integer", "The ENTITY_FLAG flags." )

LUA_BINDING_BEGIN( Entity, GetElasticity, "class", "Get elasticity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushnumber( L, pEntity->GetElasticity() );
    return 1;
}
LUA_BINDING_END( "number", "The elasticity." )

LUA_BINDING_BEGIN( Entity, GetFirstThinkTick, "class", "Get first think tick." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetFirstThinkTick() );
    return 1;
}
LUA_BINDING_END( "integer", "The first think tick." )

LUA_BINDING_BEGIN( Entity, GetFlags, "class", "Get flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetFlags() );
    return 1;
}
LUA_BINDING_END( "integer", "The flags." )

LUA_BINDING_BEGIN( Entity, GetFollowedEntity, "class", "Get followed entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pEntity->GetFollowedEntity() );
    return 1;
}
LUA_BINDING_END( "Entity", "The followed entity." )

LUA_BINDING_BEGIN( Entity, GetGravity, "class", "Get gravity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushnumber( L, pEntity->GetGravity() );
    return 1;
}
LUA_BINDING_END( "number", "The gravity." )

LUA_BINDING_BEGIN( Entity, GetGroundChangeTime, "class", "Get ground change time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushnumber( L, pEntity->GetGroundChangeTime() );
    return 1;
}
LUA_BINDING_END( "number", "The ground change time." )

LUA_BINDING_BEGIN( Entity, GetGroundEntity, "class", "Get ground entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pEntity->GetGroundEntity() );
    return 1;
}
LUA_BINDING_END( "Entity", "The ground entity." )

LUA_BINDING_BEGIN( Entity, GetHealth, "class", "Get health." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetHealth() );
    return 1;
}
LUA_BINDING_END( "integer", "The health." )

LUA_BINDING_BEGIN( Entity, GetKeyValue, "class", "Get key value (the value is limited to 256 bytes)." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszKey = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" );

    char szValue[256];
    szValue[0] = '\0';
    lua_pushboolean( L, pEntity->GetKeyValue( pszKey, szValue, sizeof( szValue ) ) );
    lua_pushstring( L, szValue );

    return 2;
}
LUA_BINDING_END( "boolean", "True if key value exists, false otherwise.", "string", "The key value." )

LUA_BINDING_BEGIN( Entity, GetLastThink, "class", "Get last think." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszName = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, nullptr, "name" );

    lua_pushnumber( L, pEntity->GetLastThink( pszName ) );
    return 1;
}
LUA_BINDING_END( "number", "The last think." )

LUA_BINDING_BEGIN( Entity, GetLastThinkTick, "class", "Get last think tick." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" );

    lua_pushinteger( L, pEntity->GetLastThinkTick( pszName ) );
    return 1;
}
LUA_BINDING_END( "integer", "The last think tick." )

LUA_BINDING_BEGIN( Entity, GetLocalAngles, "class", "Get local angles." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    QAngle dst;
    VectorCopy( pEntity->GetLocalAngles(), dst );
    lua_pushangle( L, dst );
    return 1;
}
LUA_BINDING_END( "Angle", "The local angles." )

LUA_BINDING_BEGIN( Entity, GetLocalAngularVelocity, "class", "Get local angular velocity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    QAngle v = pEntity->GetLocalAngularVelocity();
    lua_pushangle( L, v );
    return 1;
}
LUA_BINDING_END( "Angle", "The local angular velocity." )

LUA_BINDING_BEGIN( Entity, GetLocalOrigin, "class", "Get local origin." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->GetLocalOrigin();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The local origin." )

LUA_BINDING_BEGIN( Entity, GetLocalVelocity, "class", "Get local velocity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->GetLocalVelocity();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The local velocity." )

LUA_BINDING_BEGIN( Entity, GetMaxHealth, "class", "Get max health." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetMaxHealth() );
    return 1;
}
LUA_BINDING_END( "integer", "The max health." )

LUA_BINDING_BEGIN( Entity, GetModelBounds, "class", "Get model bounds." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector mins, maxs;
    modelinfo->GetModelBounds( pEntity->GetModel(), mins, maxs );
    lua_pushvector( L, mins );
    lua_pushvector( L, maxs );

    return 2;
}
LUA_BINDING_END( "Vector", "The minimum bounds.", "Vector", "The maximum bounds." )

LUA_BINDING_BEGIN( Entity, GetModelIndex, "class", "Get model index." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetModelIndex() );
    return 1;
}
LUA_BINDING_END( "integer", "The model index." )

LUA_BINDING_BEGIN( Entity, GetMoveParent, "class", "Get move parent." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pEntity->GetMoveParent() );
    return 1;
}
LUA_BINDING_END( "Entity", "The move parent." )

LUA_BINDING_BEGIN( Entity, GetMoveType, "class", "Get move type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, ( MoveType_t )pEntity->GetMoveType() );
    return 1;
}
LUA_BINDING_END( "enumeration/MOVE_TYPE", "The move type." )

LUA_BINDING_BEGIN( Entity, GetOBBCenter, "class", "Get OBB center." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CCollisionProperty *pCollisionProp = pEntity->CollisionProp();

    if ( !pCollisionProp )
    {
        luaL_typeerror( L, 1, "Entity with collision" );
        return 0;
    }

    Vector v = pCollisionProp->OBBCenter();
    lua_pushvector( L, v );

    return 1;
}
LUA_BINDING_END( "Vector", "The OBB center." )

LUA_BINDING_BEGIN( Entity, GetOBBMaxs, "class", "Get OBB maxs." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CCollisionProperty *pCollisionProp = pEntity->CollisionProp();

    if ( !pCollisionProp )
    {
        luaL_typeerror( L, 1, "Entity with collision" );
        return 0;
    }

    Vector v = pCollisionProp->OBBMaxs();
    lua_pushvector( L, v );

    return 1;
}
LUA_BINDING_END( "Vector", "The OBB maxs." )

LUA_BINDING_BEGIN( Entity, GetOBBMins, "class", "Get OBB mins." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CCollisionProperty *pCollisionProp = pEntity->CollisionProp();

    if ( !pCollisionProp )
    {
        luaL_typeerror( L, 1, "Entity with collision" );
        return 0;
    }

    Vector v = pCollisionProp->OBBMins();
    lua_pushvector( L, v );

    return 1;
}
LUA_BINDING_END( "Vector", "The OBB mins." )

LUA_BINDING_BEGIN( Entity, GetOwnerEntity, "class", "Get owner entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pEntity->GetOwnerEntity() );
    return 1;
}
LUA_BINDING_END( "Entity", "The owner entity." )

LUA_BINDING_BEGIN( Entity, GetParametersForSound, "class|static", "Get parameters for sound." )
{
    const char *pszSound = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sound" );
    const char *pszActor = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, nullptr, "actor" );

    CSoundParameters params;
    lua_pushboolean( L, CBaseEntity::GetParametersForSound( pszSound, params, pszActor ) );
    lua_pushsoundparameters( L, params );

    return 2;
}
LUA_BINDING_END( "boolean", "True if parameters for sound exist, false otherwise.", "soundparameters", "The sound parameters." )

LUA_BINDING_BEGIN( Entity, GetRefTable, "class", "Get reference table." )
{
    // Note:    instead of luaL_checkentity, we soft-check here so NULL values can
    //          have a nil reference table returned.
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );

    if ( pEntity == NULL )
    {
        lua_pushnil( L );
    }
    else
    {
        LUA_GET_REF_TABLE( L, pEntity );
    }

    return 1;
}
LUA_BINDING_END( "table", "The reference table." )

LUA_BINDING_BEGIN( Entity, GetSimulatingPlayer, "class", "Get simulating player." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    CBasePlayer::PushLuaInstanceSafe( L, pEntity->GetSimulatingPlayer() );
    return 1;
}
LUA_BINDING_END( "Player", "The simulating player." )

LUA_BINDING_BEGIN( Entity, GetSimulationTime, "class", "Get simulation time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushnumber( L, pEntity->GetSimulationTime() );
    return 1;
}
LUA_BINDING_END( "number", "The simulation time." )

LUA_BINDING_BEGIN( Entity, GetSkinCount, "class", "Get skin count." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    studiohdr_t *pStudioModel = modelinfo->GetStudiomodel( pEntity->GetModel() );

    if ( pStudioModel )
    {
        lua_pushinteger( L, pStudioModel->numskinfamilies );
    }
    else
    {
        DevWarning( "Entity::GetSkinCount: Failed to get studio model\n" );
        lua_pushinteger( L, 0 );
    }

    return 1;
}
LUA_BINDING_END( "integer", "The skin count." )

LUA_BINDING_BEGIN( Entity, GetSolid, "class", "Get solid." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetSolid() );
    return 1;
}
LUA_BINDING_END( "integer", "The solid." )

LUA_BINDING_BEGIN( Entity, GetSolidFlags, "class", "Get solid flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetSolidFlags() );
    return 1;
}
LUA_BINDING_END( "integer", "The SOLID_FLAG flags." )

LUA_BINDING_BEGIN( Entity, GetSoundDuration, "class|static", "Get sound duration." )
{
    const char *pszSound = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sound" );
    const char *pszSoundType = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "sound type" );

    lua_pushnumber( L, CBaseEntity::GetSoundDuration( pszSound, pszSoundType ) );
    return 1;
}
LUA_BINDING_END( "number", "The sound duration." )

LUA_BINDING_BEGIN( Entity, GetSoundSourceIndex, "class", "Get sound source index." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetSoundSourceIndex() );
    return 1;
}
LUA_BINDING_END( "integer", "The sound source index." )

LUA_BINDING_BEGIN( Entity, GetTeamNumber, "class", "Get team number." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetTeamNumber() );
    return 1;
}
LUA_BINDING_END( "integer", "The team number." )

LUA_BINDING_BEGIN( Entity, GetNoCollidingWithTeammates, "class", "Get no colliding with teammates." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->GetNoCollidingWithTeammates() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if no colliding with teammates, false otherwise." )

LUA_BINDING_BEGIN( Entity, SetNoCollidingWithTeammates, "class", "Set no colliding with teammates." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bNoCollide = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "noCollide" );

    pEntity->SetNoCollidingWithTeammates( bNoCollide );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetTextureFrameIndex, "class", "Get texture frame index." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetTextureFrameIndex() );
    return 1;
}
LUA_BINDING_END( "integer", "The texture frame index." )

LUA_BINDING_BEGIN( Entity, GetTouchTrace, "class|static", "Get touch trace most recently relevant." )
{
    trace_t trace = CBaseEntity::GetTouchTrace();
    lua_pushtrace( L, trace );
    return 1;
}
LUA_BINDING_END( "trace", "The most recent touch trace." )

LUA_BINDING_BEGIN( Entity, GetTracerAttachment, "class", "Get tracer attachment." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetTracerAttachment() );
    return 1;
}
LUA_BINDING_END( "integer", "The tracer attachment." )

LUA_BINDING_BEGIN( Entity, GetTracerType, "class", "Get tracer type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushstring( L, pEntity->GetTracerType() );
    return 1;
}
LUA_BINDING_END( "string", "The tracer type." )

LUA_BINDING_BEGIN( Entity, GetVectors, "class", "Get vectors." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector forward, right, up;
    pEntity->GetVectors( &forward, &right, &up );
    lua_pushvector( L, forward );
    lua_pushvector( L, right );
    lua_pushvector( L, up );

    return 3;
}
LUA_BINDING_END( "Vector", "The forward vector.", "Vector", "The right vector.", "Vector", "The up vector." )

LUA_BINDING_BEGIN( Entity, GetViewOffset, "class", "Get view offset." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->GetViewOffset();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The view offset." )

// TODO: Expose the WL_ enums
LUA_BINDING_BEGIN( Entity, GetWaterLevel, "class", "Get water level enum." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetWaterLevel() );
    return 1;
}
LUA_BINDING_END( "integer", "The water level." )

LUA_BINDING_BEGIN( Entity, GetWaterType, "class", "Get water type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->GetWaterType() );
    return 1;
}
LUA_BINDING_END( "integer", "The water type flags." )

LUA_BINDING_BEGIN( Entity, HasMatchingRootParent, "class", "Checks if the given entity has a matching root parent." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "other" );

    lua_pushboolean( L, UTIL_EntityHasMatchingRootParent( pEntity, pOther ) );
    return 1;
}
LUA_BINDING_END( "boolean", "The given entity has a matching root parent with this entity." )

LUA_BINDING_BEGIN( Entity, HasDataObjectType, "class", "Has data object type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "type" );

    lua_pushboolean( L, pEntity->HasDataObjectType( nType ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if has data object type, false otherwise." )

LUA_BINDING_BEGIN( Entity, HasNPCsOnIt, "class", "Has NPCs on it." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->HasNPCsOnIt() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if has NPCs on it, false otherwise." )

LUA_BINDING_BEGIN( Entity, ImpactTrace, "class", "Impact trace." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    trace_t tr = LUA_BINDING_ARGUMENT( luaL_checktrace, 2, "trace" );
    int nDamageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "damage type" );
    const char *pszCustomImpactName = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 4, nullptr, "custom impact name" );

    pEntity->ImpactTrace( &tr, nDamageType, pszCustomImpactName );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, InSameTeam, "class", "In same team." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "other" );

    lua_pushboolean( L, pEntity->InSameTeam( pOther ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if in same team, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsAbsQueriesValid, "class|static", "Is abs queries valid." )
{
    lua_pushboolean( L, CBaseEntity::IsAbsQueriesValid() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if abs queries are valid, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsAIWalkable, "class", "Is AI walkable." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsAIWalkable() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if AI walkable, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsAlive, "class", "Is alive" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsAlive() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if alive, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsAnimatedEveryTick, "class", "Is animated every tick." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsAnimatedEveryTick() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if animated every tick, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsBaseObject, "class", "Is base object." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsBaseObject() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if base object, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsBaseTrain, "class", "Is base train." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsBaseTrain() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if base train, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsBSPModel, "class", "Is BSP model." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsBSPModel() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if BSP model, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsClient, "class|static", "Returns true if called on the client." )
{
    lua_pushboolean( L, CBaseEntity::IsClient() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if client, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsCurrentlyTouching, "class", "Is currently touching." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsCurrentlyTouching() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if currently touching, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsDormant, "class", "Is dormant." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsDormant() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if dormant, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsEffectActive, "class", "Is effect active." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nEffect = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "effect" );

    lua_pushboolean( L, pEntity->IsEffectActive( nEffect ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if effect active, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsEntityFlagSet, "class", "Is Entity Flag set." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlag = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flag" );

    lua_pushboolean( L, pEntity->IsEFlagSet( nFlag ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if Entity Flag set, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsFloating, "class", "Is floating." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsFloating() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if floating, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsFollowingEntity, "class", "Is following entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsFollowingEntity() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if following entity, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsInWorld, "class", "Is in world." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsInWorld() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if in world, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsMarkedForDeletion, "class", "Is marked for deletion." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsMarkedForDeletion() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if marked for deletion, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsNpc, "class", "Is NPC." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );

    if ( !pEntity )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    lua_pushboolean( L, pEntity->IsNPC() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if NPC, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsPlayer, "class", "Is player." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );

    if ( !pEntity )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    lua_pushboolean( L, pEntity->IsPlayer() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if player, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsPlayerSimulated, "class", "Is player simulated." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsPlayerSimulated() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if player simulated, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsPointInBounds, "class", "Is point in bounds." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector v = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "point" );

    CCollisionProperty *pCollisionProp = pEntity->CollisionProp();

    if ( !pCollisionProp )
    {
        luaL_typeerror( L, 1, "Entity with collision" );
        return 0;
    }

    lua_pushboolean( L, pCollisionProp->IsPointInBounds( v ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if point in bounds, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsPointSized, "class", "Is point sized." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsPointSized() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if point sized, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsPrecacheAllowed, "class|static", "Is precache allowed." )
{
    lua_pushboolean( L, CBaseEntity::IsPrecacheAllowed() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if precache allowed, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsServer, "class|static", "Returns true if called on the server." )
{
    lua_pushboolean( L, CBaseEntity::IsServer() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if server, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsSimulatedEveryTick, "class", "Is simulated every tick." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsSimulatedEveryTick() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if simulated every tick, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsSimulatingOnAlternateTicks, "class|static", "Is simulating on alternate ticks." )
{
    lua_pushboolean( L, CBaseEntity::IsSimulatingOnAlternateTicks() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if simulating on alternate ticks, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsSolid, "class", "Is solid" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsSolid() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if solid, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsSolidFlagSet, "class", "Is solid flag set." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlag = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flag" );

    lua_pushboolean( L, pEntity->IsSolidFlagSet( nFlag ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if solid flag set, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsStandable, "class", "Is standable." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsStandable() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if standable, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsTransparent, "class", "Is transparent." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsTransparent() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if transparent, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsWeapon, "class", "Is weapon." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->IsWeapon() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if weapon, false otherwise." )

LUA_BINDING_BEGIN( Entity, IsWorld, "class", "Whether entity is world." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsWorld() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if world, false otherwise." )

LUA_BINDING_BEGIN( Entity, KeyValue, "class", "Key value." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszKey = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" );

    switch ( lua_type( L, 3 ) )
    {
        case LUA_TNUMBER:
        {
            float flValue = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );
            lua_pushboolean( L, pEntity->KeyValue( pszKey, flValue ) );
            break;
        }
        case LUA_TSTRING:
        default:
        {
            const char *pszValue = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "value" );
            lua_pushboolean( L, pEntity->KeyValue( pszKey, pszValue ) );
            break;
        }
        case LUA_TUSERDATA:
        {
            if ( luaL_checkudata( L, 3, LUA_VECTORMETANAME ) )
            {
                Vector v = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "value" );
                lua_pushboolean( L, pEntity->KeyValue( pszKey, v ) );
            }
            else
            {
                luaL_typerror( L, 3, LUA_VECTORMETANAME );
            }
            break;
        }
    }

    return 1;
}
LUA_BINDING_END( "boolean", "True if key value, false otherwise." )

LUA_BINDING_BEGIN( Entity, LocalEyeAngles, "class", "Local eye angles." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    QAngle v = pEntity->LocalEyeAngles();
    lua_pushangle( L, v );
    return 1;
}
LUA_BINDING_END( "Angle", "The local eye angles." )

LUA_BINDING_BEGIN( Entity, NextMovePeer, "class", "Next move peer." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    CBaseEntity::PushLuaInstanceSafe( L, pEntity->NextMovePeer() );
    return 1;
}
LUA_BINDING_END( "Entity", "The next move peer." )

LUA_BINDING_BEGIN( Entity, ObjectCapabilities, "class", "Object capabilities for save/restore." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->ObjectCaps() );
    return 1;
}
LUA_BINDING_END( "integer", "The object y flags." )

LUA_BINDING_BEGIN( Entity, OnRestore, "class", "On restore." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->OnRestore();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsCheckForEntityUntouch, "class", "Physics check for entity untouch." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->PhysicsCheckForEntityUntouch();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsCheckWater, "class", "Physics check water." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->PhysicsCheckWater() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if physics check water, false otherwise." )

LUA_BINDING_BEGIN( Entity, PhysicsCheckWaterTransition, "class", "Physics check water transition." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->PhysicsCheckWaterTransition();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsImpact, "class", "Physics impact." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "other" );
    trace_t tr = LUA_BINDING_ARGUMENT( luaL_checktrace, 3, "trace" );

    pEntity->PhysicsImpact( pOther, tr );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsMarkEntitiesAsTouching, "class", "Physics mark entities as touching." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "other" );
    trace_t tr = LUA_BINDING_ARGUMENT( luaL_checktrace, 3, "trace" );

    pEntity->PhysicsMarkEntitiesAsTouching( pOther, tr );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsNotifyOtherOfGroundRemoval, "class|static", "Physics notify other of ground removal." )
{
    CBaseEntity *pGround = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "ground" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "other" );

    CBaseEntity::PhysicsNotifyOtherOfGroundRemoval( pGround, pOther );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsNotifyOtherOfUntouch, "class|static", "Physics notify other of untouch." )
{
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "other" );
    CBaseEntity *pGround = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "ground" );

    CBaseEntity::PhysicsNotifyOtherOfUntouch( pOther, pGround );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsRemoveGroundList, "class|static", "Physics remove ground list." )
{
    CBaseEntity *pGround = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "ground" );

    CBaseEntity::PhysicsRemoveGroundList( pGround );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsRemoveTouchedList, "class|static", "Physics remove touched list." )
{
    CBaseEntity *pGround = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "ground" );

    CBaseEntity::PhysicsRemoveTouchedList( pGround );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsSimulate, "class", "Physics simulate." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->PhysicsSimulate();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsSolidMaskForEntity, "class", "Physics solid mask for entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, pEntity->PhysicsSolidMaskForEntity() );
    return 1;
}
LUA_BINDING_END( "integer", "The physics solid mask for entity." )

LUA_BINDING_BEGIN( Entity, PhysicsStartGroundContact, "class", "Physics start ground contact." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pGround = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "ground" );

    pEntity->PhysicsStartGroundContact( pGround );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, Precache, "class", "Precache." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->Precache();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PrecacheModel, "class|static", "Precache model." )
{
    const char *pszModel = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "model" );

    lua_pushinteger( L, CBaseEntity::PrecacheModel( pszModel ) );
    return 1;
}
LUA_BINDING_END( "integer", "The precached model." )

LUA_BINDING_BEGIN( Entity, PrecacheScriptSound, "class|static", "Precache script sound." )
{
    const char *pszSound = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sound" );

    lua_pushinteger( L, CBaseEntity::PrecacheScriptSound( pszSound ) );
    return 1;
}
LUA_BINDING_END( "integer", "The precached script sound." )

LUA_BINDING_BEGIN( Entity, PrecacheSound, "class|static", "Precache sound." )
{
    const char *pszSound = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sound" );

    // Copied from PrecacheSound so the "Direct precache of %s" warnings arent shown
    if ( !CBaseEntity::IsPrecacheAllowed() )
    {
        if ( !enginesound->IsSoundPrecached( pszSound ) )
        {
            Warning( "Late precache of %s\n", pszSound );
        }
    }

    lua_pushboolean( L, enginesound->PrecacheSound( pszSound, true ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if precached sound, false otherwise." )

LUA_BINDING_BEGIN( Entity, PrefetchScriptSound, "class|static", "Prefetch script sound." )
{
    const char *pszSound = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sound" );

    CBaseEntity::PrefetchScriptSound( pszSound );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PrefetchSound, "class|static", "Prefetch sound." )
{
    const char *pszSound = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sound" );

    CBaseEntity::PrefetchSound( pszSound );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, RegisterThinkContext, "class", "Register think context." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszContext = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "context" );

    lua_pushinteger( L, pEntity->RegisterThinkContext( pszContext ) );
    return 1;
}
LUA_BINDING_END( "integer", "The registered think context." )

LUA_BINDING_BEGIN( Entity, Remove, "class", "Remove." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

#ifdef GAME_DLL
    // On the server we need to kick out passengers or removing the parent will also take the passenger with it
    if ( pEntity->IsVehicle() )
    {
        IServerVehicle *pServerVehicle = pEntity->GetServerVehicle();
        CBaseCombatCharacter *pPassenger = pServerVehicle->GetPassenger();

        if ( pPassenger )
        {
            CBasePlayer *pPlayer = ToBasePlayer( pPassenger );

            if ( pPlayer )
            {
                pPlayer->LeaveVehicle();
            }
            else
            {
                pPassenger->ExitVehicle();
            }
        }
    }
#endif

    pEntity->Remove();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, RemoveAllDecals, "class", "Remove all decals." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->RemoveAllDecals();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, RemoveDataObjectType, "class", "Remove data object type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "type" );

    pEntity->RemoveDataObjectType( nType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, RemoveEffects, "class", "Remove effects." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nEffects = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "effects" );

    pEntity->RemoveEffects( nEffects );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, RemoveEntityFlags, "class", "Remove Entity Flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );

    pEntity->RemoveEFlags( nFlags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, RemoveFlag, "class", "Remove flag." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlag = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flag" );

    pEntity->RemoveFlag( nFlag );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, RemoveSolidFlags, "class", "Remove solid flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );

    pEntity->RemoveSolidFlags( nFlags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetAngles, "class", "Set absolute angles." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    QAngle v = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angles" );

    pEntity->SetAbsAngles( v );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetPosition, "class", "Set absolute origin." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector v = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" );

    pEntity->SetAbsOrigin( v );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetAbsoluteQueriesValid, "class|static", "Set absolute queries valid." )
{
    bool bValid = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "valid" );

    CBaseEntity::SetAbsQueriesValid( bValid );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetVelocity, "class", "Set absolute velocity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector v = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "velocity" );

    pEntity->SetAbsVelocity( v );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetAIWalkable, "class", "Set AI walkable." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bWalkable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "walkable" );

    pEntity->SetAIWalkable( bWalkable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetAllowPrecache, "class|static", "Set allow precache." )
{
    bool bAllow = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "allow" );

    CBaseEntity::SetAllowPrecache( bAllow );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetAnimatedEveryTick, "class", "Set animated every tick." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bAnimated = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "animated" );

    pEntity->SetAnimatedEveryTick( bAnimated );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetAnimTime, "class", "Set animation time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" );

    pEntity->SetAnimTime( flTime );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetBaseVelocity, "class", "Set base velocity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector v = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "velocity" );

    pEntity->SetBaseVelocity( v );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetBlocksLineOfSight, "class", "Set blocks Lin eOf Sight." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bBlocksLOS = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "blocksLOS" );

    pEntity->SetBlocksLOS( bBlocksLOS );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetCheckUntouch, "class", "Set check untouch." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bCheckUntouch = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "checkUntouch" );

    pEntity->SetCheckUntouch( bCheckUntouch );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetClass, "class", "Set the class name." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszClassname = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "classname" );

    pEntity->SetClassname( pszClassname );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetCollisionBounds, "class", "Set collision bounds." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vMins = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "mins" );
    Vector vMaxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "maxs" );

    pEntity->SetCollisionBounds( vMins, vMaxs );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetCollisionGroup, "class", "Set collision group." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nGroup = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "group" );

    pEntity->SetCollisionGroup( nGroup );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetEffectEntity, "class", "Set effect entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pEffect = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "effect" );

    pEntity->SetEffectEntity( pEffect );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetEffects, "class", "Set effects." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nEffects = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "effects" );

    pEntity->SetEffects( nEffects );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetEntityFlags, "class", "Set entity flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );

    pEntity->SetEFlags( nFlags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetFriction, "class", "Set friction." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flFriction = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "friction" );

    pEntity->SetFriction( flFriction );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetGravity, "class", "Set gravity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flGravity = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "gravity" );

    pEntity->SetGravity( flGravity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetGroundChangeTime, "class", "Set ground change time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" );

    pEntity->SetGroundChangeTime( flTime );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetGroundEntity, "class", "Set ground entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pGround = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "ground" );

    pEntity->SetGroundEntity( pGround );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetHealth, "class", "Set health." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flHealth = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "health" );

    pEntity->SetHealth( flHealth );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetLocalAngles, "class", "Set local angles." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    QAngle v = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angles" );

    pEntity->SetLocalAngles( v );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetLocalAngularVelocity, "class", "Set local angular velocity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    QAngle v = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "velocity" );

    pEntity->SetLocalAngularVelocity( v );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetLocalOrigin, "class", "Set local origin." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector v = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" );

    pEntity->SetLocalOrigin( v );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetLocalVelocity, "class", "Set local velocity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector v = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "velocity" );

    pEntity->SetLocalVelocity( v );
    return 0;
}
LUA_BINDING_END()

#ifdef CLIENT_DLL
LUA_BINDING_BEGIN( Entity, SetModel, "class", "Set model.", "client" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszModel = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "model" );

    // Precache if not yet done
    int i = modelinfo->GetModelIndex( pszModel );
    if ( i == -1 )
    {
        CBaseEntity::PrecacheModel( pszModel );
    }

    lua_pushboolean( L, pEntity->SetModel( pszModel ) );
    return 1;
}
LUA_BINDING_END( "boolean", "False if the name is bogus or otherwise can't be loaded." )
#else
LUA_BINDING_BEGIN( Entity, SetModel, "class", "Set model.", "server" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    const char *pszModel = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "model" );

    // Precache if not yet done
    int i = modelinfo->GetModelIndex( pszModel );
    if ( i == -1 )
    {
        CBaseEntity::PrecacheModel( pszModel );
    }

    pEntity->SetModel( pszModel );
    return 0;
}
LUA_BINDING_END()
#endif

LUA_BINDING_BEGIN( Entity, SetModelIndex, "class", "Set model index." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" );

    pEntity->SetModelIndex( nIndex );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetModelName, "class", "Set model name." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    string_t szName = AllocPooledString( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ) );

    pEntity->SetModelName( szName );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetMoveCollide, "class", "Set move collide." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    MoveCollide_t nCollide = LUA_BINDING_ARGUMENT_ENUM( MoveCollide_t, 2, "collide" );

    pEntity->SetMoveCollide( nCollide );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetMoveType, "class", "Set move type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    MoveType_t nType = LUA_BINDING_ARGUMENT_ENUM( MoveType_t, 2, "type" );
    MoveCollide_t nCollide = LUA_BINDING_ARGUMENT_ENUM_WITH_DEFAULT( MoveCollide_t, 3, MOVECOLLIDE_DEFAULT, "collide" );

    pEntity->SetMoveType( nType, nCollide );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetNextThink, "class", "Set next think." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" );

    pEntity->SetNextThink( flTime );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetOwnerEntity, "class", "Set owner entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOwner = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "owner" );

    pEntity->SetOwnerEntity( pOwner );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetParent, "class", "Set parent." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pParent = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "parent" );
    int iAttachment = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "attachment" );

    pEntity->SetParent( pParent, iAttachment );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetPlayerSimulated, "class", "Set player simulated." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBasePlayer *pPlayer = LUA_BINDING_ARGUMENT( luaL_checkplayer, 2, "player" );

    pEntity->SetPlayerSimulated( pPlayer );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetPredictionEligible, "class", "Set prediction eligible." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool canPredict = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "canPredict" );

    pEntity->SetPredictionEligible( canPredict );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetPredictionPlayer, "class|static", "Set prediction player." )
{
    CBasePlayer *pPlayer = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

    CBaseEntity::SetPredictionPlayer( pPlayer );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetRenderColor, "class", "Get the entity render color." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    color32 rawColor = pEntity->GetRenderColor();
    lua_Color color( rawColor.r, rawColor.g, rawColor.b, rawColor.a );
    lua_pushcolor( L, color );
    return 1;
}
LUA_BINDING_END( "Color", "The entity render color." )

LUA_BINDING_BEGIN( Entity, SetRenderColor, "class", "Set the entity render color." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_Color &color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 2, "color" );
    int r = color.r();
    int g = color.g();
    int b = color.b();
    int a = color.a();

    pEntity->SetRenderColor( r, g, b, a );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetRenderColorA, "class", "Set render color alpha." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int a = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "alpha" );

    pEntity->SetRenderColorA( a );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetRenderColorB, "class", "Set render color blue." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int b = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "blue" );

    pEntity->SetRenderColorB( b );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetRenderColorG, "class", "Set render color green." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int g = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "green" );

    pEntity->SetRenderColorG( g );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetRenderColorR, "class", "Set render color red." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int r = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "red" );

    pEntity->SetRenderColorR( r );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetRenderMode, "class", "Set render mode." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    RenderMode_t nMode = LUA_BINDING_ARGUMENT_ENUM( RenderMode_t, 2, "mode" );

#ifdef CLIENT_DLL
    pEntity->SetRenderMode( nMode, true );
#else
    pEntity->SetRenderMode( nMode );
#endif

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetSimulatedEveryTick, "class", "Set simulated every tick." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bSimulated = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "simulated" );

    pEntity->SetSimulatedEveryTick( bSimulated );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetSimulationTime, "class", "Set simulation time." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" );

    pEntity->SetSimulationTime( flTime );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetSize, "class", "Set size of bounding box." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vMins = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "mins" );
    Vector vMaxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "maxs" );

    pEntity->SetSize( vMins, vMaxs );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetSolid, "class", "Set solid type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    SolidType_t nType = LUA_BINDING_ARGUMENT_ENUM( SolidType_t, 2, "type" );

    pEntity->SetSolid( nType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetSolidFlags, "class", "Set solid flags." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );

    pEntity->SetSolidFlags( nFlags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetTextureFrameIndex, "class", "Set texture frame index." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" );

    pEntity->SetTextureFrameIndex( nIndex );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetTransmitWithParent, "class", "Set transmit with parent." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bTransmit = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "transmit" );

    pEntity->SetTransmitWithParent( bTransmit );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetTransmitWithParent, "class", "Get transmit with parent." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->GetTransmitWithParent() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetViewOffset, "class", "Set view offset." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector v = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "offset" );

    pEntity->SetViewOffset( v );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetWaterLevel, "class", "Set water level enum." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nLevel = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "level" );

    pEntity->SetWaterLevel( nLevel );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetWaterType, "class", "Set water type." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "type" );

    pEntity->SetWaterType( nType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, ShouldCollide, "class", "Check if entity should collide." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nCollisionGroup = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "collisionGroup" );
    int nContentsMask = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "contentsMask" );

    lua_pushboolean( L, pEntity->ShouldCollide( nCollisionGroup, nContentsMask ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, ShouldSavePhysics, "class", "Check if entity should save physics." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->ShouldSavePhysics() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, Spawn, "class", "Spawn entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->Spawn();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, StartGroundContact, "class", "Start ground contact." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pGround = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "ground" );

    pEntity->StartGroundContact( pGround );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, StartTouch, "class", "Start touch." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "other" );

    pEntity->StartTouch( pOther );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, StopFollowingEntity, "class", "Stop following entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->StopFollowingEntity();
    return 0;
}
LUA_BINDING_END()

// LUA_BINDING_BEGIN( Entity, SUB_Remove, "class", "Remove entity." )
//{
//     lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
//
//     pEntity->SUB_Remove();
//     return 0;
// }
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, Think, "class", "Think." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->Think();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, ToggleFlag, "class", "Toggle flag." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nFlag = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flag" );

    pEntity->ToggleFlag( nFlag );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, Touch, "class", "Touch." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "other" );

    pEntity->Touch( pOther );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, TraceBleed, "class", "Trace bleed." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int nDamage = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damage" );
    Vector vDir = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "direction" );
    trace_t trace = LUA_BINDING_ARGUMENT( luaL_checktrace, 4, "trace" );
    int nType = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "type" );

    pEntity->TraceBleed( nDamage, vDir, &trace, nType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, UnsetPlayerSimulated, "class", "Unset player simulated." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->UnsetPlayerSimulated();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, UpdateOnRemove, "class", "Update on remove." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->UpdateOnRemove();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, DestroyPhysicsObject, "class", "Destroy physics object." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->VPhysicsDestroyObject();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetPhysicsObject, "class", "Get physics object." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushphysicsobject( L, pEntity->VPhysicsGetObject() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetPhysicsObjects, "class", "Get physics object list." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    IPhysicsObject *pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
    int count = pEntity->VPhysicsGetObjectList( pList, ARRAYSIZE( pList ) );

    lua_pushinteger( L, count );
    lua_newtable( L );
    for ( int i = 0; i < count; i++ )
    {
        lua_pushinteger( L, i );
        lua_pushphysicsobject( L, pList[i] );
        lua_settable( L, -3 );
    }

    return 2;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsInit, "class", "Initialize physics object." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    SolidType_t nSolidType = ( SolidType_t )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 2, SOLID_VPHYSICS, "solidType" );
    int nSolidFlags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "solidFlags" );
    bool bCreateAsleep = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "createAsleep" );

    // TODO: Implement a Lua solid_t object
    lua_pushphysicsobject( L, pEntity->VPhysicsInitNormal( nSolidType, nSolidFlags, bCreateAsleep ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsInitBox, "class", "Initialize physics object as box." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vMins = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "mins" );
    Vector vMaxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "maxs" );
    const char *pszSurfaceProperty = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 4, "default", "surfaceProperty" );

    // Copied mostly from PhysModelCreateBox
    solid_t solid;
    PhysGetDefaultAABBSolid( solid );
    Vector vDims = vMaxs - vMins;
    solid.params.volume = vDims.x * vDims.y * vDims.z;
    Q_strncpy( solid.surfaceprop, pszSurfaceProperty, sizeof( solid.surfaceprop ) );

    CPhysCollide *pCollide = PhysCreateBbox( vMins, vMaxs );
    if ( !pCollide )
        return 0;

    const bool bStatic = false;
    // TODO: Check if origin needs to be in world space (like it is now)
    IPhysicsObject *pPhysicsObject = PhysModelCreateCustom(
        pEntity,
        pCollide,
        pEntity->GetAbsOrigin(),
        vec3_angle,  // AABB
        STRING( pEntity->GetModelName() ),
        bStatic,
        &solid );
    pEntity->VPhysicsSetObject( pPhysicsObject );

    pEntity->SetCollisionBounds( vMins, vMaxs );

    lua_pushphysicsobject( L, pPhysicsObject );

    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsInitStatic, "class", "Initialize physics object as static." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    SolidType_t nSolidType = ( SolidType_t )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 2, SOLID_VPHYSICS, "solidType" );

    if ( pEntity->GetSolid() != nSolidType )
    {
        pEntity->SetSolid( nSolidType );
    }

    lua_pushphysicsobject( L, pEntity->VPhysicsInitStatic() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsIsFlesh, "class", "Check if physics is flesh." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushboolean( L, pEntity->VPhysicsIsFlesh() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, PhysicsSetObject, "class", "Set physics object." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    IPhysicsObject *pObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 2, "object" );

    pEntity->VPhysicsSetObject( pObject );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, UpdatePhysics, "class", "Update physics object." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    IPhysicsObject *pObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 2, "object" );

    pEntity->VPhysicsUpdate( pObject );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, WakeRestingObjects, "class", "Wake resting objects." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    pEntity->WakeRestingObjects();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, WorldAlignMaxs, "class", "Get world aligned maxs." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->WorldAlignMaxs();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, WorldAlignMins, "class", "Get world aligned mins." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->WorldAlignMins();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, WorldAlignSize, "class", "Get world aligned size." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->WorldAlignSize();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, WorldSpaceCenter, "class", "Get world space center." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    Vector v = pEntity->WorldSpaceCenter();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, WorldToEntitySpace, "class", "Convert world to entity space." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector vWorld = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "world" );
    Vector vEntity = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "entity" );

    pEntity->WorldToEntitySpace( vWorld, &vEntity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, SetNetworkDataValue, "class", "Sets a data table variable of the given type, in the given slot, to the given value. Slots start at 0 and there's 32 slots per type (except for strings, which have 4 slots and a limit of 512 characters per string)." )
{
    lua_CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    NETWORK_VARIABLE networkVarType = LUA_BINDING_ARGUMENT_ENUM( NETWORK_VARIABLE, 2, "type" );
    int slot = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "slot" );

    if ( slot < 0 || ( networkVarType == TYPE_STRING && slot >= LUA_MAX_NETWORK_VARIABLES_STRING ) ||
         ( networkVarType != TYPE_STRING && slot >= LUA_MAX_NETWORK_VARIABLES ) )
    {
        luaL_argerror( L, 3, "Invalid slot index" );
        return 0;
    }

    int oldValueIndex = lua_gettop( L );

    switch ( networkVarType )
    {
        case TYPE_BOOLEAN:
        {
            bool newValueBool = LUA_BINDING_ARGUMENT( luaL_checkboolean, 4, "value" );
            lua_pushboolean( L, entity->m_LuaVariables_bool[slot] );
            lua_pushboolean( L, newValueBool );
#ifdef CLIENT_DLL
            entity->m_LuaVariables_bool[slot] = newValueBool;
#else
            entity->m_LuaVariables_bool.GetForModify( slot ) = newValueBool;
#endif
            break;
        }
        case TYPE_INTEGER:
        {
            int newValueInt = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "value" );
            lua_pushinteger( L, entity->m_LuaVariables_int[slot] );
            lua_pushinteger( L, newValueInt );
#ifdef CLIENT_DLL
            entity->m_LuaVariables_int[slot] = newValueInt;
#else
            entity->m_LuaVariables_int.GetForModify( slot ) = newValueInt;
#endif
            break;
        }
        case TYPE_FLOAT:
        {
            float newValueFloat = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "value" );
            lua_pushnumber( L, entity->m_LuaVariables_float[slot] );
            lua_pushnumber( L, newValueFloat );
            if ( !IsFinite( newValueFloat ) )
            {
                newValueFloat = FLT_MAX;
                Assert( IsFinite( newValueFloat ) );
            }
#ifdef CLIENT_DLL
            entity->m_LuaVariables_float[slot] = newValueFloat;
#else
            entity->m_LuaVariables_float.GetForModify( slot ) = newValueFloat;
#endif
            break;
        }
        case TYPE_VECTOR:
        {
            Vector &newValueVector = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "value" );
            lua_pushvector( L, entity->m_LuaVariables_Vector[slot] );
            lua_pushvector( L, newValueVector );
#ifdef CLIENT_DLL
            entity->m_LuaVariables_Vector[slot] = newValueVector;
#else
            entity->m_LuaVariables_Vector.GetForModify( slot ) = newValueVector;
#endif
            break;
        }
        case TYPE_ANGLE:
        {
            QAngle &newValueAngle = LUA_BINDING_ARGUMENT( luaL_checkangle, 4, "value" );
            lua_pushangle( L, entity->m_LuaVariables_QAngle[slot] );
            lua_pushangle( L, newValueAngle );
#ifdef CLIENT_DLL
            entity->m_LuaVariables_QAngle[slot] = newValueAngle;
#else
            entity->m_LuaVariables_QAngle.GetForModify( slot ) = newValueAngle;
#endif
            break;
        }
        case TYPE_STRING:
        {
            const char *newValueString = strdup( LUA_BINDING_ARGUMENT( luaL_checkstring, 4, "value" ) );
#ifdef CLIENT_DLL
            lua_pushstring( L, entity->m_LuaVariables_String[slot] );
            lua_pushstring( L, newValueString );
            Q_strncpy( entity->m_LuaVariables_String[slot], newValueString, sizeof( entity->m_LuaVariables_String[slot] ) );
#else
            lua_pushstring( L, STRING( entity->m_LuaVariables_String[slot] ) );
            lua_pushstring( L, newValueString );
            entity->m_LuaVariables_String.GetForModify( slot ) = MAKE_STRING( newValueString );
#endif
            break;
        }
        case TYPE_ENTITY:
        {
            CBaseEntity *newValueEntity = LUA_BINDING_ARGUMENT( lua_toentity, 4, "value" );
            CBaseEntity::PushLuaInstanceSafe( L, CBaseEntity::Instance( entity->m_LuaVariables_Entity[slot] ) );
            CBaseEntity::PushLuaInstanceSafe( L, newValueEntity );
#ifdef CLIENT_DLL
            entity->m_LuaVariables_Entity[slot] = newValueEntity;
#else
            entity->m_LuaVariables_Entity.GetForModify( slot ) = newValueEntity;
#endif

            break;
        }
        default:
            luaL_argerror( L, 2, "Invalid type" );
            return 0;
    }

    // Commented, lets use the macro we have for this (also because it'll show the hook as shared then instead
    // of client AND shared in the docs.
    // LUA_CALL_HOOK_BEGIN( "EntityNetworkVariableChanging", "Called just before a network variable is changed" );
    // CBaseEntity::PushLuaInstanceSafe( L, entity ); // doc: entity
    // lua_pushinteger( L, slot ); // doc: slot (which network variable)
    // lua_pushvalue( L, oldValueIndex + 1 ); // doc: newValue
    // lua_pushvalue( L, oldValueIndex ); // doc: oldValue
    // LUA_CALL_HOOK_END( 4, 0 );
    LUA_CALL_NETWORK_VARIABLE_CHANGING_HOOK( entity, slot, lua_pushvalue, oldValueIndex + 1, oldValueIndex );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, GetNetworkDataValue, "class", "Gets a data table variable of the given type, in the given slot." )
{
    lua_CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    NETWORK_VARIABLE networkVarType = LUA_BINDING_ARGUMENT_ENUM( NETWORK_VARIABLE, 2, "type" );
    int slot = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "slot" );

    if ( slot < 0 || ( networkVarType == TYPE_STRING && slot >= LUA_MAX_NETWORK_VARIABLES_STRING ) ||
         ( networkVarType != TYPE_STRING && slot >= LUA_MAX_NETWORK_VARIABLES ) )
    {
        luaL_argerror( L, 3, "Invalid slot index" );
        return 0;
    }

    switch ( networkVarType )
    {
        case TYPE_BOOLEAN:
            lua_pushboolean( L, entity->m_LuaVariables_bool[slot] );
            break;
        case TYPE_INTEGER:
            lua_pushinteger( L, entity->m_LuaVariables_int[slot] );
            break;
        case TYPE_FLOAT:
            lua_pushnumber( L, entity->m_LuaVariables_float[slot] );
            break;
        case TYPE_VECTOR:
            lua_pushvector( L, entity->m_LuaVariables_Vector[slot] );
            break;
        case TYPE_ANGLE:
            lua_pushangle( L, entity->m_LuaVariables_QAngle[slot] );
            break;
        case TYPE_STRING:
#ifdef CLIENT_DLL
            lua_pushstring( L, entity->m_LuaVariables_String[slot] );
#else
            lua_pushstring( L, STRING( entity->m_LuaVariables_String[slot] ) );
#endif
            break;
        case TYPE_ENTITY:
            CBaseEntity::PushLuaInstanceSafe( L, CBaseEntity::Instance( entity->m_LuaVariables_Entity[slot] ) );
            break;
        default:
            luaL_argerror( L, 2, "Invalid type" );
            return 0;
    }

    return 1;
}
LUA_BINDING_END( "any", "The value of the network variable" )

// TODO: This is only for viewmodels, move it to a separate file for viewmodel bindings
LUA_BINDING_BEGIN( Entity, SendViewModelMatchingSequence, "class", "Send view model matching sequence. Only works on viewmodels." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    CBaseViewModel *pViewModel = dynamic_cast< CBaseViewModel * >( pEntity );
    int nSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    if ( !pViewModel )
    {
        luaL_argerror( L, 1, "Entity is not a view model" );
        return 0;
    }

    pViewModel->SendViewModelMatchingSequence( nSequence );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, IsValid, "class", "Check if entity is valid." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );

    if ( pEntity == NULL )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    CExperimentScriptedWeapon *pWeapon = dynamic_cast< CExperimentScriptedWeapon * >( pEntity );

    if ( pWeapon )
    {
        lua_pushboolean( L, pWeapon->IsValid() );
        return 1;
    }

    // Just like in Garry's Mod, we'll return false for the Worldspawn (0) entity
    if ( pEntity->entindex() == 0 )
    {
        lua_pushboolean( L, false );
        return 1;
    }

#if CLIENT_DLL
    lua_pushboolean( L, true );
#else
    lua_pushboolean( L, UTIL_IsValidEntity( pEntity ) );
#endif

    return 1;
}
LUA_BINDING_END()

// TODO:    This is overwritten by Garry's Mod in lua/includes/extensions/entity.lua
//          It is mentioned there that implementing it in Lua is faster, so perhaps we should do that too.
LUA_BINDING_BEGIN( Entity, __index, "class", "Metamethod that is called when a non existing field is indexed" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );

    LUA_METATABLE_INDEX_CHECK_VALID( L, Entity_IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pEntity );

    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( Q_strcmp( field, "m_bAllowPrecache" ) == 0 )
        lua_pushboolean( L, pEntity->m_bAllowPrecache );
    else if ( Q_strcmp( field, "m_flAnimTime" ) == 0 )
        lua_pushnumber( L, pEntity->m_flAnimTime );
    else if ( Q_strcmp( field, "m_flSimulationTime" ) == 0 )
        lua_pushnumber( L, pEntity->m_flSimulationTime );
    else if ( Q_strcmp( field, "m_flSpeed" ) == 0 )
        lua_pushnumber( L, pEntity->m_flSpeed );
    else if ( Q_strcmp( field, "m_iClassname" ) == 0 )
        lua_pushstring( L, STRING( pEntity->m_iClassname ) );
    else if ( Q_strcmp( field, "m_iHealth" ) == 0 )
        lua_pushinteger( L, pEntity->m_iHealth );
    else if ( Q_strcmp( field, "m_nLastThinkTick" ) == 0 )
        lua_pushinteger( L, pEntity->m_nLastThinkTick );
    else if ( Q_strcmp( field, "m_nModelIndex" ) == 0 )
        lua_pushinteger( L, pEntity->m_nModelIndex );
    else if ( Q_strcmp( field, "touchStamp" ) == 0 )
        lua_pushinteger( L, pEntity->touchStamp );
    else
    {
        LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pEntity );

        if ( lua_getmetatable( L, 1 ) )
        {
            LUA_METATABLE_INDEX_CHECK_TABLE( L );
        }

        luaL_getmetatable( L, LUA_BASEENTITYMETANAME );
        LUA_METATABLE_INDEX_CHECK_TABLE( L );

        lua_pushnil( L );
    }

    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( Entity, __newindex, "class", "Metamethod that is called a new field is added" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );

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

    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( Q_strcmp( field, "m_bAllowPrecache" ) == 0 )
        pEntity->m_bAllowPrecache = luaL_checkboolean( L, 3 );
    else if ( Q_strcmp( field, "m_flAnimTime" ) == 0 )
        pEntity->m_flAnimTime = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_flSimulationTime" ) == 0 )
        pEntity->m_flSimulationTime = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_flSpeed" ) == 0 )
        pEntity->m_flSpeed = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_iClassname" ) == 0 )
        pEntity->m_iClassname = MAKE_STRING( luaL_checkstring( L, 3 ) );
    else if ( Q_strcmp( field, "m_iHealth" ) == 0 )
        pEntity->m_iHealth = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_nLastThinkTick" ) == 0 )
        pEntity->m_nLastThinkTick = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_nModelIndex" ) == 0 )
        pEntity->m_nModelIndex = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "touchStamp" ) == 0 )
        pEntity->touchStamp = luaL_checknumber( L, 3 );
    else
    {
        LUA_GET_REF_TABLE( L, pEntity );
        lua_pushvalue( L, 3 );
        lua_setfield( L, -2, field );
        lua_pop( L, 1 );
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entity, __eq, "class", "Metamethod that is called when comparing two entities" )
{
    lua_CBaseEntity *pEntity1 = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );
    lua_CBaseEntity *pEntity2 = LUA_BINDING_ARGUMENT( lua_toentity, 2, "other" );

    lua_pushboolean( L, pEntity1 == pEntity2 );
    return 1;
}
LUA_BINDING_END( "boolean", "True if entities are equal." )

LUA_BINDING_BEGIN( Entity, __tostring, "class", "Metamethod that is called when converting entity to string" )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( lua_toentity, 1, "entity" );

    if ( pEntity == NULL )
        lua_pushstring( L, "NULL" );
    else
    {
#ifdef CLIENT_DLL
        if ( pEntity->IsScripted() )
        {
            CBaseScripted *pScripted = dynamic_cast< CBaseScripted * >( pEntity );

            if ( pScripted )
                lua_pushfstring( L, "Entity: %s (%d)", pScripted->GetScriptedClassname(), pEntity->entindex() );
            else
            {
                CExperimentScriptedWeapon *pWeapon = dynamic_cast< CExperimentScriptedWeapon * >( pEntity );

                AssertMsg( pWeapon, "Entity is not a scripted entity, nor weapon." );  // which other scripted entity is there?

                lua_pushstring( L, pWeapon->GetScriptedClassname() );
            }
        }
        else
#endif
            lua_pushfstring( L, "Entity: %s (%d)", pEntity->GetClassname(), pEntity->entindex() );
    }

    return 1;
}
LUA_BINDING_END( "string", "String representation of entity" )

/*
** Open CBaseEntity object
*/
LUALIB_API int luaopen_CBaseEntity_shared( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_BASEENTITYMETANAME );

    lua_pushstring( L, "Entity" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */

    LUA_REGISTRATION_COMMIT( Entity );

    lua_pop( L, 1 );
    CBaseEntity::PushLuaInstanceSafe( L, NULL );

    lua_setglobal( L, "NULL" ); /* set global NULL */
    return 1;
}
