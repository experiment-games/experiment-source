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

#ifndef Experiment_GAMERULES_H
#define Experiment_GAMERULES_H
#pragma once

#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "gamevars_shared.h"
#include "GameEventListener.h"

#ifndef CLIENT_DLL
#include "experiment_player.h"
#endif

#define VEC_CROUCH_TRACE_MIN ExperimentRules()->GetExperimentViewVectors()->m_vCrouchTraceMin
#define VEC_CROUCH_TRACE_MAX ExperimentRules()->GetExperimentViewVectors()->m_vCrouchTraceMax

enum
{
    TEAM_COMBINE = 2,
    TEAM_REBELS,
};

#ifdef CLIENT_DLL
#define CExperimentRules C_ExperimentRules
#define CExperimentGameRulesProxy C_ExperimentGameRulesProxy
#endif

class CExperimentGameRulesProxy : public CGameRulesProxy
{
    public:
    DECLARE_CLASS( CExperimentGameRulesProxy, CGameRulesProxy );
    DECLARE_NETWORKCLASS();
};

class ExperimentViewVectors : public CViewVectors
{
    public:
    ExperimentViewVectors(
        Vector vView,
        Vector vHullMin,
        Vector vHullMax,
        Vector vDuckHullMin,
        Vector vDuckHullMax,
        Vector vDuckView,
        Vector vObsHullMin,
        Vector vObsHullMax,
        Vector vDeadViewHeight,
        Vector vCrouchTraceMin,
        Vector vCrouchTraceMax )
        : CViewVectors(
            vView,
            vHullMin,
            vHullMax,
            vDuckHullMin,
            vDuckHullMax,
            vDuckView,
            vObsHullMin,
            vObsHullMax,
            vDeadViewHeight )
    {
        m_vCrouchTraceMin = vCrouchTraceMin;
        m_vCrouchTraceMax = vCrouchTraceMax;
    }

    Vector m_vCrouchTraceMin;
    Vector m_vCrouchTraceMax;
};

class CExperimentRules : public CTeamplayRules
{
    public:
    DECLARE_CLASS( CExperimentRules, CTeamplayRules );

#ifdef CLIENT_DLL

    DECLARE_CLIENTCLASS_NOBASE();  // This makes datatables able to access our private vars.

#else

    DECLARE_SERVERCLASS_NOBASE();  // This makes datatables able to access our private vars.
#endif

    CExperimentRules();
    virtual ~CExperimentRules();

    virtual void Precache( void );
    virtual bool ShouldCollide( int collisionGroup0, int collisionGroup1 );
    virtual bool ClientCommand( CBaseEntity *pEdict, const CCommand &args );

    virtual float FlWeaponRespawnTime( CBaseCombatWeapon *pWeapon );
    virtual float FlWeaponTryRespawn( CBaseCombatWeapon *pWeapon );
    virtual Vector VecWeaponRespawnSpot( CBaseCombatWeapon *pWeapon );
    virtual int WeaponShouldRespawn( CBaseCombatWeapon *pWeapon );
    virtual void Think( void );
    virtual void CreateStandardEntities( void );
    virtual void ClientSettingsChanged( CBasePlayer *pPlayer );
    virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );

#ifdef LUA_SDK
#ifndef CLIENT_DLL
    virtual bool PlayerCanHearChat( CBasePlayer *pListener,
                                    CBasePlayer *pSpeaker );
    virtual bool ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
    virtual void InitHUD( CBasePlayer *pl );
#endif
#endif

    virtual void GoToIntermission( void );
    virtual void DeathNotice( CBasePlayer *pVictim, const CTakeDamageInfo &info );
    virtual const char *GetGameDescription( void );
    // derive this function if you mod uses encrypted weapon info files
    virtual const unsigned char *GetEncryptionKey( void )
    {
        return ( unsigned char * )"x9Ke0BY7";
    }
    virtual const CViewVectors *GetViewVectors() const;
    const ExperimentViewVectors *GetExperimentViewVectors() const;

    float GetMapRemainingTime();
    void CleanUpMap();
    void CheckRestartGame();
    void RestartGame();

#ifndef CLIENT_DLL
    virtual Vector VecItemRespawnSpot( CItem *pItem );
    virtual QAngle VecItemRespawnAngles( CItem *pItem );
    virtual float FlItemRespawnTime( CItem *pItem );

#ifdef LUA_SDK
    virtual void PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem );
    virtual int ItemShouldRespawn( CItem *pItem );
#endif

    virtual bool CanHavePlayerItem( CBasePlayer *pPlayer, CBaseCombatWeapon *pItem );
    virtual bool FShouldSwitchWeapon( CBasePlayer *pPlayer, CBaseCombatWeapon *pWeapon );

    void AddLevelDesignerPlacedObject( CBaseEntity *pEntity );
    void RemoveLevelDesignerPlacedObject( CBaseEntity *pEntity );
    void ManageObjectRelocation( void );
    void CheckChatForReadySignal( CExperiment_Player *pPlayer, const char *chatmsg );
    const char *GetChatFormat( bool bTeamOnly, CBasePlayer *pPlayer );
#endif

    virtual void ClientDisconnected( edict_t *pClient );

#ifdef LUA_SDK
#ifndef CLIENT_DLL
    virtual float FlPlayerFallDamage( CBasePlayer *pPlayer );
#endif
#endif

    bool CheckGameOver( void );
    bool IsIntermission( void );

    void PlayerKilled( CBasePlayer *pVictim, const CTakeDamageInfo &info );

    bool IsTeamplay( void )
    {
        return m_bTeamPlayEnabled;
    }

#ifdef LUA_SDK
#ifndef CLIENT_DLL
    bool FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
    bool AllowDamage( CBaseEntity *pVictim, const CTakeDamageInfo &info );

    void PlayerSpawn( CBasePlayer *pPlayer );
    void PlayerThink( CBasePlayer *pPlayer );
    bool FPlayerCanRespawn( CBasePlayer *pPlayer );
    float FlPlayerSpawnTime( CBasePlayer *pPlayer );
#endif
#endif

#ifdef EXPERIMENT_SOURCE
#ifndef CLIENT_DLL
    bool NPC_ShouldDropGrenade( CBasePlayer *pRecipient );
    bool NPC_ShouldDropHealth( CBasePlayer *pRecipient );
    void NPC_DroppedHealth( void );
    void NPC_DroppedGrenade( void );
    /*bool MegaPhyscannonActive(void) {
        return m_bMegaPhysgun;
    }

    virtual bool IsAlyxInDarknessMode();*/
#endif
#endif

    void CheckAllPlayersReady( void );

    virtual bool IsConnectedUserInfoChangeAllowed( CBasePlayer *pPlayer );

    private:
#ifdef EXPERIMENT_SOURCE
#ifndef CLIENT_DLL
    float m_flLastHealthDropTime;
    float m_flLastGrenadeDropTime;
#endif
#endif

    CNetworkVar( bool, m_bTeamPlayEnabled );
    CNetworkVar( float, m_flGameStartTime );
    CUtlVector< EHANDLE > m_hRespawnableItemsAndWeapons;
    float m_tmNextPeriodicThink;
    float m_flRestartGameTime;
    bool m_bCompleteReset;
    bool m_bAwaitingReadyRestart;
    bool m_bHeardAllPlayersReady;

#ifndef CLIENT_DLL
    bool m_bChangelevelDone;
#endif
};

inline CExperimentRules *ExperimentRules()
{
    return static_cast< CExperimentRules * >( g_pGameRules );
}

#endif  // Experiment_GAMERULES_H
