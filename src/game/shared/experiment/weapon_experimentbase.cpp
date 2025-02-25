//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "in_buttons.h"
#include "takedamageinfo.h"
#include "ammodef.h"
#include "experiment_gamerules.h"

#ifdef CLIENT_DLL
extern IVModelInfoClient *modelinfo;
#else
extern IVModelInfo *modelinfo;
#endif

#if defined( CLIENT_DLL )

#include "vgui/ISurface.h"
#include "vgui_controls/Controls.h"
#include "c_experiment_player.h"
#include "hud_crosshair.h"

#else

#include "experiment_player.h"
#include "vphysics/constraints.h"

#endif

#include "weapon_experimentbase.h"

#if defined( CLIENT_DLL )
#define CExperiment_Player C_Experiment_Player
#endif

// -----------------------------------------------------------------------------
// // Global functions.
// -----------------------------------------------------------------------------
// //

bool IsAmmoType( int iAmmoType, const char *pAmmoName )
{
    return GetAmmoDef()->Index( pAmmoName ) == iAmmoType;
}

static const char *s_WeaponAliasInfo[] = {
    "none",  //	WEAPON_NONE = 0,

    // Melee
    "shotgun",  // WEAPON_AMERKNIFE,

    NULL,  // end of list marker
};

// -----------------------------------------------------------------------------
// // CWeaponExperimentBase tables.
// -----------------------------------------------------------------------------
// //

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponExperimentBase, DT_WeaponExperimentBase )

BEGIN_NETWORK_TABLE( CWeaponExperimentBase, DT_WeaponExperimentBase )

#ifdef CLIENT_DLL

#else
// world weapon models have no aminations
//	SendPropExclude( "DT_AnimTimeMustBeFirst", "m_flAnimTime" ),
//	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
//	SendPropExclude( "DT_LocalActiveWeaponData", "m_flTimeWeaponIdle" ),
#endif

END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponExperimentBase )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_experiment_base, CWeaponExperimentBase );

#ifdef GAME_DLL

BEGIN_DATADESC( CWeaponExperimentBase )

END_DATADESC()

#endif

// -----------------------------------------------------------------------------
// // CWeaponExperimentBase implementation.
// -----------------------------------------------------------------------------
// //
CWeaponExperimentBase::CWeaponExperimentBase()
{
    SetPredictionEligible( true );
    AddSolidFlags(
        FSOLID_TRIGGER );  // Nothing collides with these but it gets touches.

    m_flNextResetCheckTime = 0.0f;
}

bool CWeaponExperimentBase::IsPredicted() const
{
    return true;
}

// Tony; override for animation purposes.
bool CWeaponExperimentBase::Reload( void )
{
    bool fRet = DefaultReload( GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD );
    if ( fRet )
    {
        //		WeaponSound( RELOAD );
        ToExperimentPlayer( GetOwner() )->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );
    }
    return fRet;
}

void CWeaponExperimentBase::WeaponSound( WeaponSound_t sound_type,
                                        float soundtime /* = 0.0f */ )
{
#ifdef CLIENT_DLL

    // If we have some sounds from the weapon classname.txt file, play a random
    // one of them
    const char *shootsound = GetWpnData().aShootSounds[sound_type];
    if ( !shootsound || !shootsound[0] )
        return;

    CBroadcastRecipientFilter filter;  // this is client side only
    if ( !te->CanPredict() )
        return;

    CBaseEntity::EmitSound( filter, GetPlayerOwner()->entindex(), shootsound, &GetPlayerOwner()->GetAbsOrigin() );
#else
    BaseClass::WeaponSound( sound_type, soundtime );
#endif
}

CBasePlayer *CWeaponExperimentBase::GetPlayerOwner() const
{
    return dynamic_cast< CBasePlayer * >( GetOwner() );
}

CExperiment_Player *CWeaponExperimentBase::GetExperimentPlayerOwner() const
{
    return dynamic_cast< CExperiment_Player * >( GetOwner() );
}

#ifdef CLIENT_DLL

void CWeaponExperimentBase::OnDataChanged( DataUpdateType_t type )
{
    BaseClass::OnDataChanged( type );

    if ( GetPredictable() && !ShouldPredict() )
        ShutdownPredictable();
}

bool CWeaponExperimentBase::ShouldPredict()
{
    if ( GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer() )
        return true;

    return BaseClass::ShouldPredict();
}

#else

void CWeaponExperimentBase::Spawn()
{
    BaseClass::Spawn();

    // Set this here to allow players to shoot dropped weapons
    SetCollisionGroup( COLLISION_GROUP_WEAPON );
}

void CWeaponExperimentBase::Materialize( void )
{
    if ( IsEffectActive( EF_NODRAW ) )
    {
        // changing from invisible state to visible.
        EmitSound( "AlyxEmp.Charge" );

        RemoveEffects( EF_NODRAW );
        DoMuzzleFlash();
    }

    if ( HasSpawnFlags( SF_NORESPAWN ) == false )
    {
        VPhysicsInitNormal( SOLID_BBOX, GetSolidFlags() | FSOLID_TRIGGER, false );
        SetMoveType( MOVETYPE_VPHYSICS );

        ExperimentRules()->AddLevelDesignerPlacedObject( this );
    }

    if ( HasSpawnFlags( SF_NORESPAWN ) == false )
    {
        if ( GetOriginalSpawnOrigin() == vec3_origin )
        {
            m_vOriginalSpawnOrigin = GetAbsOrigin();
            m_vOriginalSpawnAngles = GetAbsAngles();
        }
    }

    SetPickupTouch();

    SetThink( NULL );
}

int CWeaponExperimentBase::ObjectCaps()
{
    return BaseClass::ObjectCaps() & ~FCAP_IMPULSE_USE;
}

#endif

void CWeaponExperimentBase::FallInit( void )
{
#ifndef CLIENT_DLL
    SetModel( GetWorldModel() );
    VPhysicsDestroyObject();

    if ( HasSpawnFlags( SF_NORESPAWN ) == false )
    {
        SetMoveType( MOVETYPE_NONE );
        SetSolid( SOLID_BBOX );
        AddSolidFlags( FSOLID_TRIGGER );

        UTIL_DropToFloor( this, MASK_SOLID );
    }
    else
    {
        if ( !VPhysicsInitNormal( SOLID_BBOX, GetSolidFlags() | FSOLID_TRIGGER, false ) )
        {
            SetMoveType( MOVETYPE_NONE );
            SetSolid( SOLID_BBOX );
            AddSolidFlags( FSOLID_TRIGGER );
        }
        else
        {
#if !defined( CLIENT_DLL )
            // Constrained start?
            if ( HasSpawnFlags( SF_WEAPON_START_CONSTRAINED ) )
            {
                // Constrain the weapon in place
                IPhysicsObject *pReferenceObject, *pAttachedObject;

                pReferenceObject = g_PhysWorldObject;
                pAttachedObject = VPhysicsGetObject();

                if ( pReferenceObject && pAttachedObject )
                {
                    constraint_fixedparams_t fixed;
                    fixed.Defaults();
                    fixed.InitWithCurrentObjectState( pReferenceObject,
                                                    pAttachedObject );

                    fixed.constraint.forceLimit = lbs2kg( 10000 );
                    fixed.constraint.torqueLimit = lbs2kg( 10000 );

                    IPhysicsConstraint *pConstraint = GetConstraint();

                    pConstraint = physenv->CreateFixedConstraint(
                        pReferenceObject, pAttachedObject, NULL, fixed );

                    pConstraint->SetGameData( ( void * )this );
                }
            }
#endif  // CLIENT_DLL
        }
    }

    SetPickupTouch();

    SetThink( &CBaseCombatWeapon::FallThink );

    SetNextThink( gpGlobals->curtime + 0.1f );

#endif
}

const CExperimentSWeaponInfo &CWeaponExperimentBase::GetExperimentWpnData() const
{
    const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
    const CExperimentSWeaponInfo *pExperimentInfo;

#ifdef _DEBUG
    pExperimentInfo = dynamic_cast< const CExperimentSWeaponInfo * >( pWeaponInfo );
    Assert( pExperimentInfo );
#else
    pExperimentInfo = static_cast< const CExperimentSWeaponInfo * >( pWeaponInfo );
#endif

    return *pExperimentInfo;
}
void CWeaponExperimentBase::FireBullets( const FireBulletsInfo_t &info )
{
    FireBulletsInfo_t modinfo = info;

    modinfo.m_iPlayerDamage = GetExperimentWpnData().m_iPlayerDamage;

    BaseClass::FireBullets( modinfo );
}

#if defined( CLIENT_DLL )

#include "c_te_effect_dispatch.h"

#define NUM_MUZZLE_FLASH_TYPES 4

bool CWeaponExperimentBase::OnFireEvent( C_BaseViewModel *pViewModel,
                                        const Vector &origin,
                                        const QAngle &angles,
                                        int event,
                                        const char *options )
{
    return BaseClass::OnFireEvent( pViewModel, origin, angles, event, options );
}

void UTIL_ClipPunchAngleOffset( QAngle &in, const QAngle &punch, const QAngle &clip )
{
    QAngle final = in + punch;

    // Clip each component
    for ( int i = 0; i < 3; i++ )
    {
        if ( final[i] > clip[i] )
        {
            final[i] = clip[i];
        }
        else if ( final[i] < -clip[i] )
        {
            final[i] = -clip[i];
        }

        // Return the result
        in[i] = final[i] - punch[i];
    }
}

#endif
