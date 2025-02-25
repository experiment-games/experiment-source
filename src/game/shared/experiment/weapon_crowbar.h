//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef Experiment_WEAPON_CROWBAR_H
#define Experiment_WEAPON_CROWBAR_H
#pragma once

#include "weapon_experimentbasehlmpcombatweapon.h"
#include "weapon_experimentbasebasebludgeon.h"

#ifdef CLIENT_DLL
#define CWeaponCrowbar C_WeaponCrowbar
#endif

//-----------------------------------------------------------------------------
// CWeaponCrowbar
//-----------------------------------------------------------------------------

class CWeaponCrowbar : public CBaseExperimentBludgeonWeapon
{
    public:
    DECLARE_CLASS( CWeaponCrowbar, CBaseExperimentBludgeonWeapon );

    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();
    DECLARE_ACTTABLE();

    CWeaponCrowbar();

    float GetRange( void );
    float GetFireRate( void );

    void AddViewKick( void );
    float GetDamageForActivity( Activity hitActivity );
    void SecondaryAttack( void )
    {
        return;
    }

    void Drop( const Vector &vecVelocity );

    // Animation event
#ifndef CLIENT_DLL
    virtual void Operator_HandleAnimEvent( animevent_t *pEvent,
                                            CBaseCombatCharacter *pOperator );
    void HandleAnimEventMeleeHit( animevent_t *pEvent,
                                CBaseCombatCharacter *pOperator );
    int WeaponMeleeAttack1Condition( float flDot, float flDist );
#endif

    CWeaponCrowbar( const CWeaponCrowbar & );

    private:
};

#endif  // Experiment_WEAPON_CROWBAR_H
