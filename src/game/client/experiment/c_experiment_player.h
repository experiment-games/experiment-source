//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef Experiment_PLAYER_H
#define Experiment_PLAYER_H
#pragma once

#include "experiment_playeranimstate.h"
#include "c_basehlplayer.h"
#include "experiment_player_shared.h"
#include "beamdraw.h"

//=============================================================================
// >> Experiment_Player
//=============================================================================
class C_Experiment_Player : public C_BaseHLPlayer
{
#ifdef LUA_SDK
    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( C_Experiment_Player, LUA_EXPERIMENTPLAYERLIBNAME )
#endif

   public:
    DECLARE_CLASS( C_Experiment_Player, C_BaseHLPlayer );

    DECLARE_CLIENTCLASS();
    DECLARE_PREDICTABLE();
    DECLARE_INTERPOLATION();

    C_Experiment_Player();
    ~C_Experiment_Player( void );

    void ClientThink( void );

    static C_Experiment_Player *GetLocalExperimentPlayer();

    virtual int DrawModel( int flags );
    virtual void AddEntity( void );

    Vector GetAttackSpread( CBaseCombatWeapon *pWeapon,
                            CBaseEntity *pTarget = NULL );

    // Should this object cast shadows?
    virtual ShadowType_t ShadowCastType( void );
    virtual C_BaseAnimating *BecomeRagdollOnClient();
    virtual const QAngle &GetRenderAngles();
    virtual bool ShouldDraw( void );
    virtual void OnDataChanged( DataUpdateType_t type );
    virtual float GetFOV( void );
    virtual CStudioHdr *OnNewModel( void );
    virtual void TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator );
    virtual void ItemPreFrame( void );
    virtual void ItemPostFrame( void );
    virtual float GetMinFOV() const
    {
        return 5.0f;
    }
    virtual Vector GetAutoaimVector( float flDelta );
    virtual void NotifyShouldTransmit( ShouldTransmitState_t state );
    virtual void CreateLightEffects( void )
    {
    }
    virtual bool ShouldReceiveProjectedTextures( int flags );
    virtual void PostDataUpdate( DataUpdateType_t updateType );
    virtual void PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force );
    virtual void PreThink( void );
    virtual void DoImpactEffect( trace_t &tr, int nDamageType );
    IRagdoll *GetRepresentativeRagdoll() const;
    virtual void CalcView( CViewSetup &view );
    virtual const QAngle &EyeAngles( void );

    bool CanSprint( void );
    void StartSprinting( void );
    void StopSprinting( void );
    void HandleSpeedChanges( CMoveData *mv );
    void UpdateLookAt( void );
    void Initialize( void );
    int GetIDTarget() const;
    void UpdateIDTarget( void );
    void PrecacheFootStepSounds( void );
    const char *GetPlayerModelSoundPrefix( void );

    ExperimentPlayerState State_Get() const;

    // Walking
    void StartWalking( void );
    void StopWalking( void );
    bool IsWalking( void )
    {
        return m_fIsWalking;
    }

    virtual void UpdateClientSideAnimation();
    void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
    virtual void CalculateIKLocks( float currentTime );

    bool KeyDown( int buttonCode );

    static void RecvProxy_CycleLatch( const CRecvProxyData *pData, void *pStruct, void *pOut );

    virtual float GetServerIntendedCycle()
    {
        return m_flServerCycle;
    }
    virtual void SetServerIntendedCycle( float cycle )
    {
        m_flServerCycle = cycle;
    }

    bool IsAirborne() const
    {
        return ( !( GetFlags() & FL_ONGROUND ) );
    }

    CExperimentPlayerAnimState *GetAnimState() const
    {
        return m_PlayerAnimState;
    }

    EHANDLE GetRagdollEntity() const
    {
        return m_hRagdoll;
    }

    bool FlashlightIsOn() const
    {
        return IsEffectActive( EF_DIMLIGHT );
    }

   private:
    C_Experiment_Player( const C_Experiment_Player & );

    CExperimentPlayerAnimState *m_PlayerAnimState;

    QAngle m_angEyeAngles;

    CInterpolatedVar< QAngle > m_iv_angEyeAngles;

    EHANDLE m_hRagdoll;

    int m_headYawPoseParam;
    int m_headPitchPoseParam;
    float m_headYawMin;
    float m_headYawMax;
    float m_headPitchMin;
    float m_headPitchMax;

    bool m_isInit;
    Vector m_vLookAtTarget;

    float m_flLastBodyYaw;
    float m_flCurrentHeadYaw;
    float m_flCurrentHeadPitch;

    int m_iIDEntIndex;

    CountdownTimer m_blinkTimer;

    int m_iSpawnInterpCounter;
    int m_iSpawnInterpCounterCache;

    int m_iPlayerSoundType;

    void ReleaseFlashlight( void );
    Beam_t *m_pFlashlightBeam;

    CNetworkVar( ExperimentPlayerState, m_iPlayerState );

    bool m_fIsWalking;
    int m_cycleLatch;  // The animation cycle goes out of sync very easily. Mostly from the player entering/exiting PVS. Server will frequently update us with a new one.
    float m_flServerCycle;
};

inline C_Experiment_Player *ToExperimentPlayer( CBaseEntity *pEntity )
{
    if ( !pEntity || !pEntity->IsPlayer() )
        return NULL;

    return dynamic_cast< C_Experiment_Player * >( pEntity );
}

class C_ExperimentRagdoll : public C_BaseAnimatingOverlay
{
   public:
    DECLARE_CLASS( C_ExperimentRagdoll, C_BaseAnimatingOverlay );
    DECLARE_CLIENTCLASS();

    C_ExperimentRagdoll();
    ~C_ExperimentRagdoll();

    virtual void OnDataChanged( DataUpdateType_t type );

    int GetPlayerEntIndex() const;
    IRagdoll *GetIRagdoll() const;

    void ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName );
    void UpdateOnRemove( void );
    virtual void SetupWeights( const matrix3x4_t *pBoneToWorld,
                               int nFlexWeightCount,
                               float *pFlexWeights,
                               float *pFlexDelayedWeights );

    C_BasePlayer *GetRagdollPlayer() const
    {
        return dynamic_cast< C_BasePlayer * >( m_hPlayer.Get() );
    }

   private:
    C_ExperimentRagdoll( const C_ExperimentRagdoll & )
    {
    }

    void Interp_Copy( C_BaseAnimatingOverlay *pDestinationEntity );
    void CreateExperimentRagdoll( void );

   private:
    EHANDLE m_hPlayer;
    CNetworkVector( m_vecRagdollVelocity );
    CNetworkVector( m_vecRagdollOrigin );
};

#endif  // Experiment_PLAYER_H
