//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"

#ifdef CLIENT_DLL
#include "c_experiment_player.h"
#include "prediction.h"
#define CRecipientFilter C_RecipientFilter
#else
#include "experiment_player.h"
#endif

#include "engine/IEngineSound.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"

#if defined( LUA_SDK )
#include "luamanager.h"
#include "lexperiment_player_shared.h"
#include "mathlib/lvector.h"
#include "lvphysics_interface.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sv_footsteps;

const char *g_ppszPlayerSoundPrefixNames[PLAYER_SOUNDS_MAX] = {
    "NPC_Citizen",
    "NPC_CombineS",
    "NPC_MetroPolice",
};

// Experiment; copied from NEO:
// https://github.com/AdamTadeusz/neo/blob/master/mp/src/game/server/neo/neo_player_shared.cpp
bool PlayerAnimToPlayerAnimEvent( const PLAYER_ANIM playerAnim,
                                PlayerAnimEvent_t &outAnimEvent )
{
    bool success = true;

    if ( playerAnim == PLAYER_ANIM::PLAYER_JUMP )
    {
        outAnimEvent = PlayerAnimEvent_t::PLAYERANIMEVENT_JUMP;
    }
    else if ( playerAnim == PLAYER_ANIM::PLAYER_RELOAD )
    {
        outAnimEvent = PlayerAnimEvent_t::PLAYERANIMEVENT_RELOAD;
    }
    else if ( playerAnim == PLAYER_ANIM::PLAYER_ATTACK1 )
    {
        outAnimEvent = PlayerAnimEvent_t::PLAYERANIMEVENT_ATTACK_PRIMARY;
    }
    else
    {
        success = false;
    }

    return success;
}

const char *CExperiment_Player::GetPlayerModelSoundPrefix( void )
{
    return g_ppszPlayerSoundPrefixNames[m_iPlayerSoundType];
}

void CExperiment_Player::PrecacheFootStepSounds( void )
{
    int iFootstepSounds = ARRAYSIZE( g_ppszPlayerSoundPrefixNames );
    int i;

    for ( i = 0; i < iFootstepSounds; ++i )
    {
        char szFootStepName[128];

        Q_snprintf( szFootStepName, sizeof( szFootStepName ), "%s.RunFootstepLeft", g_ppszPlayerSoundPrefixNames[i] );
        PrecacheScriptSound( szFootStepName );

        Q_snprintf( szFootStepName, sizeof( szFootStepName ), "%s.RunFootstepRight", g_ppszPlayerSoundPrefixNames[i] );
        PrecacheScriptSound( szFootStepName );
    }
}

//-----------------------------------------------------------------------------
// Consider the weapon's built-in accuracy, this character's proficiency with
// the weapon, and the status of the target. Use this information to determine
// how accurately to shoot at the target.
//-----------------------------------------------------------------------------
Vector CExperiment_Player::GetAttackSpread( CBaseCombatWeapon *pWeapon,
                                            CBaseEntity *pTarget )
{
    if ( pWeapon )
        return pWeapon->GetBulletSpread( WEAPON_PROFICIENCY_PERFECT );

    return VECTOR_CONE_15DEGREES;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : step -
//			fvol -
//			force - force sound to play
//-----------------------------------------------------------------------------
void CExperiment_Player::PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force )
{
#if defined( LUA_SDK )
    LUA_CALL_HOOK_BEGIN( "PlayerPlayStepSound" );
    CExperiment_Player::PushLuaInstanceSafe( L, this );
    lua_pushvector( L, vecOrigin );
    lua_pushsurfacedata( L, psurface );
    lua_pushnumber( L, fvol );
    lua_pushboolean( L, force );
    LUA_CALL_HOOK_END( 5, 1 );

    LUA_RETURN_NONE_IF_FALSE();
#endif

    if ( gpGlobals->maxClients > 1 && !sv_footsteps.GetFloat() )
        return;

#if defined( CLIENT_DLL )
    // during prediction play footstep sounds only once
    if ( !prediction->IsFirstTimePredicted() )
        return;
#endif

    if ( GetFlags() & FL_DUCKING )
        return;

    m_Local.m_nStepside = !m_Local.m_nStepside;

    char szStepSound[128];

    if ( m_Local.m_nStepside )
    {
        Q_snprintf( szStepSound, sizeof( szStepSound ), "%s.RunFootstepLeft", g_ppszPlayerSoundPrefixNames[m_iPlayerSoundType] );
    }
    else
    {
        Q_snprintf( szStepSound, sizeof( szStepSound ), "%s.RunFootstepRight", g_ppszPlayerSoundPrefixNames[m_iPlayerSoundType] );
    }

    CSoundParameters params;
    if ( GetParametersForSound( szStepSound, params, NULL ) == false )
        return;

    CRecipientFilter filter;
    filter.AddRecipientsByPAS( vecOrigin );

#ifndef CLIENT_DLL
    // im MP, server removed all players in origins PVS, these players
    // generate the footsteps clientside
    if ( gpGlobals->maxClients > 1 )
        filter.RemoveRecipientsByPVS( vecOrigin );
#endif

    EmitSound_t ep;
    ep.m_nChannel = CHAN_BODY;
    ep.m_pSoundName = params.soundname;
    ep.m_flVolume = fvol;
    ep.m_SoundLevel = params.soundlevel;
    ep.m_nFlags = 0;
    ep.m_nPitch = params.pitch;
    ep.m_pOrigin = &vecOrigin;

    EmitSound( filter, entindex(), ep );
}

void CExperiment_Player::SetAvoidPlayers( bool shouldAvoid )
{
#ifdef CLIENT_DLL
    m_bAvoidPlayers = shouldAvoid;
#else
    m_bAvoidPlayers.GetForModify() = shouldAvoid;
#endif
}
bool CExperiment_Player::GetAvoidPlayers()
{
    return m_bAvoidPlayers;
}
