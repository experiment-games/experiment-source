//========= Copyright Valve Corporation, All rights reserved. ============//
#ifndef Experiment_PLAYERANIMSTATE_H
#define Experiment_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif

#include "convar.h"
#include "multiplayer_animstate.h"

#if defined( CLIENT_DLL )
class C_Experiment_Player;
#define CExperiment_Player C_Experiment_Player
#else
class CExperiment_Player;
#endif

// ------------------------------------------------------------------------------------------------ //
// CPlayerAnimState declaration.
// ------------------------------------------------------------------------------------------------ //
class CExperimentPlayerAnimState : public CMultiPlayerAnimState
{
   public:
    DECLARE_CLASS( CExperimentPlayerAnimState, CMultiPlayerAnimState );

    CExperimentPlayerAnimState();
    CExperimentPlayerAnimState( CBasePlayer *pPlayer, MultiPlayerMovementData_t &movementData );
    ~CExperimentPlayerAnimState();

    void InitExperimentAnimState( CExperiment_Player *pPlayer );
    CExperiment_Player *GetExperimentPlayer( void )
    {
        return m_pExperimentPlayer;
    }

    virtual void ClearAnimationState();
    virtual Activity TranslateActivity( Activity actDesired );
    virtual void Update( float eyeYaw, float eyePitch );

    void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
    virtual Activity CalcMainActivity();

   private:
    CExperiment_Player *m_pExperimentPlayer;

    bool m_bFreshJump;
    bool m_bWasJumping;
};

CExperimentPlayerAnimState *CreateExperimentPlayerAnimState( CExperiment_Player *pPlayer );

#endif  // Experiment_PLAYERANIMSTATE_H
