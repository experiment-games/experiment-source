//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef Experiment_PLAYER_SHARED_H
#define Experiment_PLAYER_SHARED_H
#pragma once

#define Experiment_PUSHAWAY_THINK_INTERVAL ( 1.0f / 20.0f )
#include "studio.h"

enum
{
    PLAYER_SOUNDS_CITIZEN = 0,
    PLAYER_SOUNDS_COMBINESOLDIER,
    PLAYER_SOUNDS_METROPOLICE,
    PLAYER_SOUNDS_MAX,
};

enum ExperimentPlayerState
{
    // Happily running around in the game.
    STATE_ACTIVE = 0,
    STATE_OBSERVER_MODE,  // Noclipping around, watching players, etc.
    NUM_PLAYER_STATES
};

#if defined( CLIENT_DLL )
#define CExperiment_Player C_Experiment_Player
#endif

// Experiment; https://github.com/AdamTadeusz/neo/blob/master/mp/src/game/shared/neo/neo_player_shared.h#L267
// Temporary helper for converting between these. Should refactor this to use
// the same structure for both. Returns true on success. If returns false, the
// out value will not be set.
bool PlayerAnimToPlayerAnimEvent( const PLAYER_ANIM playerAnim,
                                PlayerAnimEvent_t& outAnimEvent );

#endif  // Experiment_PLAYER_SHARED_h
