//========= Copyright Valve Corporation, All rights reserved. ============//
#include "cbase.h"
#include "base_playeranimstate.h"
#include "tier0/vprof.h"
#include "animation.h"
#include "studio.h"
#include "apparent_velocity_helper.h"
#include "utldict.h"

#include "experiment_playeranimstate.h"
#include "base_playeranimstate.h"
#include "datacache/imdlcache.h"
#include <mathlib/lvector.h>

#ifdef CLIENT_DLL
#include "c_experiment_player.h"
#include "voice_status.h"
#else
#include "experiment_player.h"
#endif

// Experiment; somehow get which type of leganim we're using
// e.g: m_AnimConfig.m_LegAnimType == LEGANIM_9WAY;
ConVar cl_leganimtype(
    "cl_leganimtype9",
    "1",
    FCVAR_ARCHIVE | FCVAR_REPLICATED,
    "Set to 1 to use 9-way leg animations, 0 to use 8-way" );

CExperimentPlayerAnimState *CreateExperimentPlayerAnimState( CExperiment_Player *pPlayer )
{
    MDLCACHE_CRITICAL_SECTION();

    // Setup the movement data.
    MultiPlayerMovementData_t movementData;
    movementData.m_flBodyYawRate = 720.0f;
    movementData.m_flRunSpeed = pPlayer ? pPlayer->GetRunSpeed() : EXPERIMENT_RUN_SPEED;
    movementData.m_flWalkSpeed = pPlayer ? pPlayer->GetWalkSpeed() : EXPERIMENT_WALK_SPEED;
    movementData.m_flSprintSpeed = -1.0f;  // TODO: ? This is running and m_flRunSpeed above is normal speed? Where is this struct used?

    // Create animation state for this player.
    CExperimentPlayerAnimState *pRet =
        new CExperimentPlayerAnimState( pPlayer, movementData );

    // Specific Experiment player initialization.
    pRet->InitExperimentAnimState( pPlayer );

    return pRet;
}

CExperimentPlayerAnimState::CExperimentPlayerAnimState()
{
    m_pExperimentPlayer = NULL;

    // Don't initialize Experiment specific variables here. Init them in
    // InitExperimentAnimState()
}

CExperimentPlayerAnimState::CExperimentPlayerAnimState(
    CBasePlayer *pPlayer,
    MultiPlayerMovementData_t &movementData )
    : CMultiPlayerAnimState( pPlayer, movementData )
{
    m_pExperimentPlayer = NULL;

    // Don't initialize Experiment specific variables here. Init them in
    // InitExperimentAnimState()
}

CExperimentPlayerAnimState::~CExperimentPlayerAnimState()
{
}

void CExperimentPlayerAnimState::InitExperimentAnimState( CExperiment_Player *pPlayer )
{
    m_pExperimentPlayer = pPlayer;
    m_bFreshJump = false;
    m_bWasJumping = false;
}

void CExperimentPlayerAnimState::ClearAnimationState( void )
{
    BaseClass::ClearAnimationState();
}

Activity CExperimentPlayerAnimState::TranslateActivity( Activity actDesired )
{
    CExperiment_Player *pPlayer = GetExperimentPlayer();

#ifdef LUA_SDK
    LUA_CALL_HOOK_BEGIN( "TranslateActivity" );
    CExperiment_Player::PushLuaInstanceSafe( L, pPlayer );
    lua_pushinteger( L, actDesired );
    LUA_CALL_HOOK_END( 2, 1 );

    if ( lua_isnumber( L, -1 ) )  // The new, translated activity
    {
        Activity iActivity = ( Activity )( int )lua_tonumber( L, -1 );
        lua_pop( L, 1 );
        return iActivity;
    }
    lua_pop( L, 1 );
#endif

    // Hook into baseclass when / if experiment player models get swim animations.
    Activity translateActivity =
        actDesired;  // BaseClass::TranslateActivity( actDesired );

    if ( pPlayer->GetActiveWeapon() )
    {
        translateActivity =
            pPlayer->GetActiveWeapon()->ActivityOverride(
                translateActivity, false );
    }

    return translateActivity;
}

void CExperimentPlayerAnimState::Update( float eyeYaw, float eyePitch )
{
    BaseClass::Update( eyeYaw, eyePitch );

#ifdef LUA_SDK
    Vector vecVelocity;
    GetOuterAbsVelocity( vecVelocity );

    LUA_CALL_HOOK_BEGIN( "UpdateAnimation" );
    CExperiment_Player::PushLuaInstanceSafe( L, GetExperimentPlayer() );
    lua_pushvector( L, vecVelocity );
    lua_pushnumber( L, m_flMaxGroundSpeed );
    LUA_CALL_HOOK_END( 3, 0 );
#endif
}

void CExperimentPlayerAnimState::DoAnimationEvent( PlayerAnimEvent_t event,
                                                   int nData )
{
    Activity iGestureActivity = ACT_INVALID;

    CExperiment_Player *pPlayer = GetExperimentPlayer();

    switch ( event )
    {
        case PLAYERANIMEVENT_ATTACK_PRIMARY:
        {
            // Weapon primary fire.
            if ( pPlayer->GetFlags() & FL_DUCKING )
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_ATTACK_CROUCH_PRIMARYFIRE );
            else
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_ATTACK_STAND_PRIMARYFIRE );

            iGestureActivity = ACT_VM_PRIMARYATTACK;
            break;
        }

        case PLAYERANIMEVENT_VOICE_COMMAND_GESTURE:
        {
            if ( !IsGestureSlotActive( GESTURE_SLOT_ATTACK_AND_RELOAD ) )
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ( Activity )nData );

            break;
        }
        case PLAYERANIMEVENT_ATTACK_SECONDARY:
        {
            // Weapon secondary fire.
            if ( pPlayer->GetFlags() & FL_DUCKING )
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_ATTACK_CROUCH_SECONDARYFIRE );
            else
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_ATTACK_STAND_SECONDARYFIRE );

            iGestureActivity = ACT_VM_PRIMARYATTACK;
            break;
        }
        case PLAYERANIMEVENT_ATTACK_PRE:
        {
            if ( pPlayer->GetFlags() & FL_DUCKING )
            {
                // Weapon pre-fire. Used for minigun windup, sniper aiming
                // start, etc in crouch.
                iGestureActivity = ACT_MP_ATTACK_CROUCH_PREFIRE;
            }
            else
            {
                // Weapon pre-fire. Used for minigun windup, sniper aiming
                // start, etc.
                iGestureActivity = ACT_MP_ATTACK_STAND_PREFIRE;
            }

            RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, iGestureActivity, false );

            break;
        }
        case PLAYERANIMEVENT_ATTACK_POST:
        {
            RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                            ACT_MP_ATTACK_STAND_POSTFIRE );
            break;
        }

        case PLAYERANIMEVENT_RELOAD:
        {
            // Weapon reload.
            if ( pPlayer->GetFlags() & FL_DUCKING )
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_RELOAD_CROUCH );
            else
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_RELOAD_STAND );
            break;
        }
        case PLAYERANIMEVENT_RELOAD_LOOP:
        {
            // Weapon reload.
            if ( pPlayer->GetFlags() & FL_DUCKING )
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_RELOAD_CROUCH_LOOP );
            else
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_RELOAD_STAND_LOOP );
            break;
        }
        case PLAYERANIMEVENT_RELOAD_END:
        {
            // Weapon reload.
            if ( pPlayer->GetFlags() & FL_DUCKING )
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_RELOAD_CROUCH_END );
            else
                RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD,
                                ACT_MP_RELOAD_STAND_END );
            break;
        }
        case PLAYERANIMEVENT_JUMP:
        {
            m_bFreshJump = true;
        }
        default:
        {
            BaseClass::DoAnimationEvent( event, nData );
            break;
        }
    }

#ifdef LUA_SDK
    LUA_CALL_HOOK_BEGIN( "DoAnimationEvent" );
    CExperiment_Player::PushLuaInstanceSafe( L, pPlayer );
    lua_pushinteger( L, ( int )event );
    lua_pushinteger( L, nData );
    LUA_CALL_HOOK_END( 3, 1 );

    if ( lua_isnumber( L, -1 ) )  // The translated activity to send to the weapon
    {
        iGestureActivity = ( Activity )( int )lua_tonumber( L, -2 );
    }

    lua_pop( L, 1 );
#endif

#ifdef CLIENT_DLL
    // Make the weapon play the animation as well
    if ( iGestureActivity != ACT_INVALID )
    {
        CBaseCombatWeapon *pWeapon = GetExperimentPlayer()->GetActiveWeapon();
        if ( pWeapon )
        {
            //			pWeapon->EnsureCorrectRenderingModel();
            pWeapon->SendWeaponAnim( iGestureActivity );
            //			// Force animation events!
            //			pWeapon->ResetEventsParity();		// reset
            // event parity so the animation events will occur on the weapon.
            pWeapon->DoAnimationEvents( pWeapon->GetModelPtr() );
        }
    }
#endif
}

Activity CExperimentPlayerAnimState::CalcMainActivity()
{
    Activity idealActivity = ACT_INVALID;

#ifdef LUA_SDK
    CExperiment_Player *pPlayer = GetExperimentPlayer();
    Vector vecVelocity;
    GetOuterAbsVelocity( vecVelocity );

    LUA_CALL_HOOK_BEGIN( "CalcMainActivity" );
    CExperiment_Player::PushLuaInstanceSafe( L, pPlayer );
    lua_pushvector( L, vecVelocity );
    LUA_CALL_HOOK_END( 2, 2 );

    if ( lua_isnumber( L, -2 ) )  // Activity
    {
        idealActivity = ( Activity )( int )lua_tonumber( L, -2 );
    }

    if ( lua_isnumber( L, -1 ) )  // Sequence
    {
        int sequence = ( int )lua_tonumber( L, -1 );

        if ( sequence != -1 )
            m_nSpecificMainSequence = sequence;
    }

    lua_pop( L, 2 );
#endif

    idealActivity = BaseClass::CalcMainActivity();

    return idealActivity;
}
