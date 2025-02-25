#ifndef EXPERIMENT_PLAYER_H
#define EXPERIMENT_PLAYER_H
#pragma once

class CExperiment_Player;

#include "basemultiplayerplayer.h"
#include "hl2_playerlocaldata.h"
#include "hl2_player.h"
#include "simtimer.h"
#include "soundenvelope.h"
#include "experiment_playeranimstate.h"
#include "experiment_player_shared.h"
#include "experiment_gamerules.h"
#include "utldict.h"
#include "steam/steam_gameserver.h"

//=============================================================================
// >> Experiment_Player
//=============================================================================
class CExperimentPlayerStateInfo
{
   public:
    ExperimentPlayerState m_iPlayerState;
    const char *m_pStateName;

    void ( CExperiment_Player::*pfnEnterState )();  // Init and deinit the state.
    void ( CExperiment_Player::*pfnLeaveState )();

    void ( CExperiment_Player::*pfnPreThink )();  // Do a PreThink() in this state.
};

class CExperiment_Player : public CHL2_Player
{
#ifdef LUA_SDK
    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( CExperiment_Player, LUA_EXPERIMENTPLAYERLIBNAME )
#endif

   public:
    DECLARE_CLASS( CExperiment_Player, CHL2_Player );

    CExperiment_Player();
    ~CExperiment_Player( void );

    static CExperiment_Player *CreatePlayer( const char *className, edict_t *ed )
    {
        CExperiment_Player::s_PlayerEdict = ed;
        return ( CExperiment_Player * )CreateEntityByName( className );
    }

    DECLARE_SERVERCLASS();
    DECLARE_DATADESC();
    DECLARE_PREDICTABLE();

    // This passes the event to the client's and server's CExperimentPlayerAnimState.
    void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
    void SetupBones( matrix3x4_t *pBoneToWorld, int boneMask );
    bool KeyDown( int buttonCode );

    virtual void Precache( void );
    virtual void Spawn( void );
    virtual void PostThink( void );
    virtual void PreThink( void );
    virtual void PlayerDeathThink( void );
    virtual bool HandleCommand_JoinTeam( int team );
    virtual bool ClientCommand( const CCommand &args );
    virtual void CreateViewModel( int viewmodelindex = 0 );
    virtual bool BecomeRagdollOnClient( const Vector &force );
    virtual void Event_Killed( const CTakeDamageInfo &info );
    virtual int OnTakeDamage( const CTakeDamageInfo &inputInfo );
    virtual bool WantsLagCompensationOnEntity(
        const CBasePlayer *pPlayer,
        const CUserCmd *pCmd,
        const CBitVec< MAX_EDICTS > *pEntityTransmitBits ) const;
    virtual void FireBullets( const FireBulletsInfo_t &info );
    virtual bool Weapon_Switch( CBaseCombatWeapon *pWeapon,
                                int viewmodelindex = 0 );
    virtual bool BumpWeapon( CBaseCombatWeapon *pWeapon );
    virtual void ChangeTeam( int iTeam );
    virtual void PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize );
    virtual void PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force );
    virtual void Weapon_Drop( CBaseCombatWeapon *pWeapon,
                              const Vector *pvecTarget = NULL,
                              const Vector *pVelocity = NULL );
    virtual void UpdateOnRemove( void );
    virtual void DeathSound( const CTakeDamageInfo &info );
    virtual CBaseEntity *EntSelectSpawnPoint( void );

    bool FlashlightIsOn( void );
    void FlashlightTurnOn( void );
    void FlashlightTurnOff( void );
    void PrecacheFootStepSounds( void );
    bool ValidatePlayerModel( const char *pModel );

    Vector GetAttackSpread( CBaseCombatWeapon *pWeapon,
                            CBaseEntity *pTarget = NULL );
    virtual Vector GetAutoaimVector( float flDelta );

    void CheatImpulseCommands( int iImpulse );
    void CreateRagdollEntity( void );
    void GiveAllItems( void );
    void GiveDefaultItems( void );

    void NoteWeaponFired( void );

    void SetAnimation( PLAYER_ANIM playerAnim );
    void SetPlayerModel( void );
    void SetPlayerTeamModel( void );
    Activity TranslateTeamActivity( Activity ActToTranslate );

    float GetNextModelChangeTime( void )
    {
        return m_flNextModelChangeTime;
    }
    float GetNextTeamChangeTime( void )
    {
        return m_flNextTeamChangeTime;
    }
    void PickDefaultSpawnTeam( void );
    void SetupPlayerSoundsByModel( const char *pModelName );
    const char *GetPlayerModelSoundPrefix( void );
    int GetPlayerModelType( void )
    {
        return m_iPlayerSoundType;
    }

    void DetonateTripmines( void );

    void Reset();

    bool IsReady();
    void SetReady( bool bReady );

    void CheckChatText( char *p, int bufsize );

    void State_Transition( ExperimentPlayerState newState );
    void State_Enter( ExperimentPlayerState newState );
    void State_Leave();
    void State_PreThink();
    CExperimentPlayerStateInfo *State_LookupInfo( ExperimentPlayerState state );

    void State_Enter_ACTIVE();
    void State_PreThink_ACTIVE();
    void State_Enter_OBSERVER_MODE();
    void State_PreThink_OBSERVER_MODE();

    virtual bool StartObserverMode( int mode );
    virtual void StopObserverMode( void );

    Vector m_vecTotalBulletForce;  // Accumulator for bullet force in a single
                                   // frame

    // Tracks our ragdoll entity.
    CNetworkHandle( CBaseEntity, m_hRagdoll );  // networked entity handle

    virtual bool CanHearAndReadChatFrom( CBasePlayer *pPlayer );

    bool IsAirborne() const
    {
        return ( !( GetFlags() & FL_ONGROUND ) );
    }

    CExperimentPlayerAnimState *GetAnimState() const
    {
        return m_PlayerAnimState;
    }

    CBaseEntity *GetRagdollEntity() const
    {
        return m_hRagdoll;
    }

   private:
    CExperimentPlayerAnimState *m_PlayerAnimState;

    CNetworkQAngle( m_angEyeAngles );

    int m_iLastWeaponFireUsercmd;
    int m_iModelType;
    CNetworkVar( int, m_iSpawnInterpCounter );
    CNetworkVar( int, m_iPlayerSoundType );

    float m_flNextModelChangeTime;
    float m_flNextTeamChangeTime;

    float m_flSlamProtectTime;

    ExperimentPlayerState m_iPlayerState;
    CExperimentPlayerStateInfo *m_pCurStateInfo;

    bool ShouldRunRateLimitedCommand( const CCommand &args );

    // This lets us rate limit the commands the players can execute so they
    // don't overflow things like reliable buffers.
    CUtlDict< float, int > m_RateLimitLastCommandTimes;

    bool m_bEnterObserver;
    bool m_bReady;

    CNetworkVar( int, m_cycleLatch );  // Network the cycle to clients periodically
    CountdownTimer m_cycleLatchTimer;

#ifndef NO_STEAM
    STEAM_GAMESERVER_CALLBACK( CExperiment_Player, OnValidateAuthTicketResponse, ValidateAuthTicketResponse_t, m_CallbackValidateAuthTicketResponse );
#endif
};

class CExperimentRagdoll : public CBaseAnimatingOverlay
{
   public:
    DECLARE_CLASS( CExperimentRagdoll, CBaseAnimatingOverlay );
    DECLARE_SERVERCLASS();

    // Transmit ragdolls to everyone.
    virtual int UpdateTransmitState()
    {
        return SetTransmitState( FL_EDICT_ALWAYS );
    }

   public:
    // In case the client has the player entity, we transmit the player index.
    // In case the client doesn't have it, we transmit the player's model index,
    // origin, and angles so they can create a ragdoll in the right place.
    CNetworkHandle( CBaseEntity, m_hPlayer );  // networked entity handle
    CNetworkVector( m_vecRagdollVelocity );
    CNetworkVector( m_vecRagdollOrigin );

    CBasePlayer *GetRagdollPlayer() const
    {
        return dynamic_cast< CBasePlayer * >( m_hPlayer.Get() );
    }
};

inline CExperiment_Player *ToExperimentPlayer( CBaseEntity *pEntity )
{
    if ( !pEntity || !pEntity->IsPlayer() )
        return NULL;

    return dynamic_cast< CExperiment_Player * >( pEntity );
}

#endif  // EXPERIMENT_PLAYER_H
