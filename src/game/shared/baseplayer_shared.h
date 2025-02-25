//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef BASEPLAYER_SHARED_H
#define BASEPLAYER_SHARED_H
#ifdef _WIN32
#pragma once
#endif

// PlayerUse defines
#define PLAYER_USE_RADIUS 80.f
#define CONE_45_DEGREES 0.707f
#define CONE_15_DEGREES 0.9659258f
#define CONE_90_DEGREES 0

#define TRAIN_ACTIVE 0x80
#define TRAIN_NEW 0xc0
#define TRAIN_OFF 0x00
#define TRAIN_NEUTRAL 0x01
#define TRAIN_SLOW 0x02
#define TRAIN_MEDIUM 0x03
#define TRAIN_FAST 0x04
#define TRAIN_BACK 0x05

// entity messages
#define PLAY_PLAYER_JINGLE 1
#define UPDATE_PLAYER_RADAR 2

#define DEATH_ANIMATION_TIME 3.0f

#if defined( EXPERIMENT_SOURCE )
#define EXPERIMENT_WALK_SPEED 90.0f           // ALT-key (slow walk)
#define EXPERIMENT_NORMAL_SPEED 150.0f        // no modifiers (light jog)
#define EXPERIMENT_RUN_SPEED 320.0f           // SHIFT-key (full sprint)
#define EXPERIMENT_CROUCH_WALK_FRACTION 0.5f  // CTRL-key (crouch walk)
#elif defined( HL2MP )
#define HL2_WALK_SPEED 150
#define HL2_NORM_SPEED 190
#define HL2_SPRINT_SPEED 320
#else
extern ConVar hl2_walkspeed;
extern ConVar hl2_normspeed;
extern ConVar hl2_sprintspeed;
#define HL2_WALK_SPEED hl2_walkspeed.GetFloat()
#define HL2_NORM_SPEED hl2_normspeed.GetFloat()
#define HL2_SPRINT_SPEED hl2_sprintspeed.GetFloat()
#endif

// When moving this fast, we play run anim.
#define ARBITRARY_RUN_SPEED 175.0f

typedef struct
{
    Vector m_vecAutoAimDir;    // The direction autoaim wishes to point.
    Vector m_vecAutoAimPoint;  // The point (world space) that autoaim is aiming at.
    EHANDLE m_hAutoAimEntity;  // The entity that autoaim is aiming at.
    bool m_bAutoAimAssisting;  // If this is true, autoaim is aiming at the target. If false, the player is naturally aiming.
    bool m_bOnTargetNatural;
    float m_fScale;
    float m_fMaxDist;
} autoaim_params_t;

enum stepsoundtimes_t
{
    STEPSOUNDTIME_NORMAL = 0,
    STEPSOUNDTIME_ON_LADDER,
    STEPSOUNDTIME_WATER_KNEE,
    STEPSOUNDTIME_WATER_FOOT,
};

void CopySoundNameWithModifierToken( char *pchDest, const char *pchSource, int nMaxLenInChars, const char *pchToken );

// Shared header file for players
#if defined( CLIENT_DLL )
#define CBasePlayer C_BasePlayer
#include "c_baseplayer.h"
#else
#include "player.h"
#endif

#endif  // BASEPLAYER_SHARED_H
