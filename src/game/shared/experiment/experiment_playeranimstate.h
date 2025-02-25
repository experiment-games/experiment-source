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

    bool HandleMoving( Activity &idealActivity );
    bool HandleJumping( Activity &idealActivity );
    bool HandleDucking( Activity &idealActivity );
    bool HandleSwimming( Activity &idealActivity );

    virtual float GetCurrentMaxGroundSpeed();

   protected:
    virtual bool ShouldResetMainSequence( int iCurrentSequence,
                                          int iNewSequence ) OVERRIDE;

    bool IsAirborne();

   private:
    bool SetupPoseParameters( CStudioHdr *pStudioHdr );
    virtual void EstimateYaw( void );
    virtual void ComputePoseParam_MoveYaw( CStudioHdr *pStudioHdr );
    virtual void ComputePoseParam_AimPitch( CStudioHdr *pStudioHdr );
    virtual void ComputePoseParam_AimYaw( CStudioHdr *pStudioHdr );

    CExperiment_Player *m_pExperimentPlayer;
    bool m_bInAirWalk;
    float m_flHoldDeployedPoseUntilTime;

    bool m_bFreshJump;
    bool m_bWasJumping;
};

CExperimentPlayerAnimState *CreateExperimentPlayerAnimState( CExperiment_Player *pPlayer );

#endif  // Experiment_PLAYERANIMSTATE_H
