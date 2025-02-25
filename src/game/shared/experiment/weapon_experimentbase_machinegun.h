//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "weapon_experimentbase.h"

#ifndef BASEHLCOMBATWEAPON_H
#define BASEHLCOMBATWEAPON_H
#ifdef _WIN32
#pragma once
#endif

#if defined( CLIENT_DLL )
#define CExperimentMachineGun C_ExperimentMachineGun
#endif

//=========================================================
// Machine gun base class
//=========================================================
class CExperimentMachineGun : public CWeaponExperimentBase
{
   public:
    DECLARE_CLASS( CExperimentMachineGun, CWeaponExperimentBase );
    DECLARE_DATADESC();

    CExperimentMachineGun();

    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();

    void PrimaryAttack( void );

    // Default calls through to m_hOwner, but plasma weapons can override and shoot projectiles here.
    virtual void ItemPostFrame( void );
    virtual void FireBullets( const FireBulletsInfo_t &info );
    virtual bool Deploy( void );

    virtual const Vector &GetBulletSpread( void );

    int WeaponSoundRealtime( WeaponSound_t shoot_type );

    // utility function
    static void DoMachineGunKick( CBasePlayer *pPlayer, float dampEasy, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime );

   private:
    CExperimentMachineGun( const CExperimentMachineGun & );

   protected:
    int m_nShotsFired;  // Number of consecutive shots fired

    float m_flNextSoundTime;  // real-time clock of when to make next sound
};

#endif  // BASEHLCOMBATWEAPON_H
