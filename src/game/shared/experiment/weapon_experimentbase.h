//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef WEAPON_ExperimentBASE_H
#define WEAPON_ExperimentBASE_H
#ifdef _WIN32
#pragma once
#endif

#include "experiment_player_shared.h"
#include "basecombatweapon_shared.h"
#include "experiment_weapon_parse.h"

#if defined( CLIENT_DLL )
#include "c_experiment_player.h"
#define CWeaponExperimentBase C_WeaponExperimentBase
void UTIL_ClipPunchAngleOffset( QAngle &in, const QAngle &punch, const QAngle &clip );
#endif

class CExperiment_Player;

#if defined( CLIENT_DLL )
#define CExperiment_Player C_Experiment_Player
#endif

// These are the names of the ammo types that go in the CAmmoDefs and that the
// weapon script files reference.

// Given an ammo type (like from a weapon's GetPrimaryAmmoType()), this compares
// it against the ammo name you specify. MIKETODO: this should use indexing
// instead of searching and strcmp()'ing all the time.
bool IsAmmoType( int iAmmoType, const char *pAmmoName );

class CWeaponExperimentBase : public CBaseCombatWeapon
{
    public:
    DECLARE_CLASS( CWeaponExperimentBase, CBaseCombatWeapon );
    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();

    CWeaponExperimentBase();

#ifdef GAME_DLL
    DECLARE_DATADESC();

    void Materialize( void );
    virtual int ObjectCaps( void );
#endif

    // All predicted weapons need to implement and return true
    virtual bool IsPredicted() const;

    CBasePlayer *GetPlayerOwner() const;
    CExperiment_Player *GetExperimentPlayerOwner() const;

    void WeaponSound( WeaponSound_t sound_type, float soundtime = 0.0f );

    CExperimentSWeaponInfo const &GetExperimentWpnData() const;

    virtual void FireBullets( const FireBulletsInfo_t &info );
    virtual void FallInit( void );
    virtual bool Reload();

    public:
#if defined( CLIENT_DLL )

    virtual bool ShouldPredict();
    virtual void OnDataChanged( DataUpdateType_t type );

    virtual bool OnFireEvent( C_BaseViewModel *pViewModel, const Vector &origin, const QAngle &angles, int event, const char *options );

#else

    virtual void Spawn();

#endif

    float m_flPrevAnimTime;
    float m_flNextResetCheckTime;

    Vector GetOriginalSpawnOrigin( void )
    {
        return m_vOriginalSpawnOrigin;
    }
    QAngle GetOriginalSpawnAngles( void )
    {
        return m_vOriginalSpawnAngles;
    }

    private:
    CWeaponExperimentBase( const CWeaponExperimentBase & );

    Vector m_vOriginalSpawnOrigin;
    QAngle m_vOriginalSpawnAngles;
};

#endif  // WEAPON_ExperimentBASE_H
