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

bool CExperimentPlayerAnimState::IsAirborne()
{
    Assert( m_pExperimentPlayer );
    return m_pExperimentPlayer->IsAirborne();
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
    // Profile the animation update.
    VPROF( "CExperimentPlayerAnimState::Update" );

    // Get the Experiment player.
    CExperiment_Player *pExperimentPlayer = GetExperimentPlayer();
    if ( !pExperimentPlayer )
        return;

    // Get the studio header for the player.
    CStudioHdr *pStudioHdr = pExperimentPlayer->GetModelPtr();
    if ( !pStudioHdr )
        return;

    // Check to see if we should be updating the animation state - dead,
    // ragdolled?
    if ( !ShouldUpdateAnimState() )
    {
        ClearAnimationState();
        return;
    }

    // Store the eye angles.
    m_flEyeYaw = AngleNormalize( eyeYaw );
    m_flEyePitch = AngleNormalize( eyePitch );

    // Compute the player sequences.
    ComputeSequences( pStudioHdr );

    if ( SetupPoseParameters( pStudioHdr ) )
    {
        // Pose parameter - what direction are the player's legs running in.
        ComputePoseParam_MoveYaw( pStudioHdr );

        // Pose parameter - Torso aiming (up/down).
        ComputePoseParam_AimPitch( pStudioHdr );

        // Pose parameter - Torso aiming (rotation).
        ComputePoseParam_AimYaw( pStudioHdr );
    }

#ifdef CLIENT_DLL
    if ( C_BasePlayer::ShouldDrawLocalPlayer() )
    {
        m_pExperimentPlayer->SetPlaybackRate( 1.0f );
    }
#endif

#ifdef LUA_SDK
    Vector vecVelocity;
    GetOuterAbsVelocity( vecVelocity );

    LUA_CALL_HOOK_BEGIN( "UpdateAnimation" );
    CExperiment_Player::PushLuaInstanceSafe( L, pExperimentPlayer );
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

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *idealActivity -
//-----------------------------------------------------------------------------
bool CExperimentPlayerAnimState::HandleSwimming( Activity &idealActivity )
{
    bool bInWater = BaseClass::HandleSwimming( idealActivity );

    return bInWater;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *idealActivity -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CExperimentPlayerAnimState::HandleMoving( Activity &idealActivity )
{
    return BaseClass::HandleMoving( idealActivity );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *idealActivity -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CExperimentPlayerAnimState::HandleDucking( Activity &idealActivity )
{
    if ( m_pExperimentPlayer->GetFlags() & FL_DUCKING )
    {
        if ( GetOuterXYSpeed() < MOVING_MINIMUM_SPEED )
        {
            idealActivity = ACT_MP_CROUCH_IDLE;
        }
        else
        {
            idealActivity = ACT_MP_CROUCHWALK;
        }

        return true;
    }

    return false;
}

bool CExperimentPlayerAnimState::HandleJumping( Activity &idealActivity )
{
    if ( IsAirborne() )
    {
        // TODO: Fix which weapon suffix to use
        idealActivity = ACT_HL2MP_JUMP_PASSIVE;

        // We set this here (and not in DoAnimationEvent) because in
        // contrast to DoAnimationEvent, this function is called on
        // both the client and the server.
        m_bWasJumping = true;

        return true;
    }
    else if ( m_bWasJumping )
    {
        AddToGestureSlot( GESTURE_SLOT_JUMP, ACT_LAND, true );
        m_bWasJumping = false;
    }

    return false;
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

bool CExperimentPlayerAnimState::SetupPoseParameters( CStudioHdr *pStudioHdr )
{
    // Check to see if this has already been done.
    if ( m_bPoseParameterInit )
        return true;

    // Save off the pose parameter indices.
    if ( !pStudioHdr )
        return false;

    if ( cl_leganimtype.GetBool() )
    {
        m_PoseParameterData.m_iMoveX =
            GetBasePlayer()->LookupPoseParameter( pStudioHdr, "move_x" );
        m_PoseParameterData.m_iMoveY =
            GetBasePlayer()->LookupPoseParameter( pStudioHdr, "move_y" );

        if ( ( m_PoseParameterData.m_iMoveX == 0 ) &&
            ( m_PoseParameterData.m_iMoveY == 0 ) )
            return false;
    }
    else
    {
        // Tony; just set them both to the same for now.
        m_PoseParameterData.m_iMoveX =
            GetBasePlayer()->LookupPoseParameter( pStudioHdr, "move_yaw" );
        m_PoseParameterData.m_iMoveY =
            GetBasePlayer()->LookupPoseParameter( pStudioHdr, "move_yaw" );

        if ( ( m_PoseParameterData.m_iMoveX < 0 ) ||
            ( m_PoseParameterData.m_iMoveY < 0 ) )
            return false;
    }

    // Look for the aim pitch blender.
    m_PoseParameterData.m_iAimPitch =
        GetBasePlayer()->LookupPoseParameter( pStudioHdr, "aim_pitch" );
    if ( m_PoseParameterData.m_iAimPitch < 0 )
        return false;

    // Look for aim yaw blender.
    m_PoseParameterData.m_iAimYaw =
        GetBasePlayer()->LookupPoseParameter( pStudioHdr, "aim_yaw" );
    if ( m_PoseParameterData.m_iAimYaw < 0 )
        return false;

    m_bPoseParameterInit = true;

    return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CExperimentPlayerAnimState::EstimateYaw( void )
{
    // Get the frame time.
    float flDeltaTime = gpGlobals->frametime;
    if ( flDeltaTime == 0.0f )
        return;

#if 0  // 9way
        // Get the player's velocity and angles.
    Vector vecEstVelocity;
    GetOuterAbsVelocity( vecEstVelocity );
    QAngle angles = GetBasePlayer()->GetLocalAngles();

    // If we are not moving, sync up the feet and eyes slowly.
    if ( vecEstVelocity.x == 0.0f && vecEstVelocity.y == 0.0f )
    {
        float flYawDelta = angles[YAW] - m_PoseParameterData.m_flEstimateYaw;
        flYawDelta = AngleNormalize( flYawDelta );

        if ( flDeltaTime < 0.25f )
        {
            flYawDelta *= ( flDeltaTime * 4.0f );
        }
        else
        {
            flYawDelta *= flDeltaTime;
        }

        m_PoseParameterData.m_flEstimateYaw += flYawDelta;
        AngleNormalize( m_PoseParameterData.m_flEstimateYaw );
    }
    else
    {
        m_PoseParameterData.m_flEstimateYaw = ( atan2( vecEstVelocity.y, vecEstVelocity.x ) * 180.0f / M_PI );
        m_PoseParameterData.m_flEstimateYaw = clamp( m_PoseParameterData.m_flEstimateYaw, -180.0f, 180.0f );
    }
#else
    float dt = gpGlobals->frametime;

    // Get the player's velocity and angles.
    Vector vecEstVelocity;
    GetOuterAbsVelocity( vecEstVelocity );
    QAngle angles = GetBasePlayer()->GetLocalAngles();

    if ( vecEstVelocity.y == 0 && vecEstVelocity.x == 0 )
    {
        float flYawDiff = angles[YAW] - m_PoseParameterData.m_flEstimateYaw;
        flYawDiff = flYawDiff - ( int )( flYawDiff / 360 ) * 360;
        if ( flYawDiff > 180 )
            flYawDiff -= 360;
        if ( flYawDiff < -180 )
            flYawDiff += 360;

        if ( dt < 0.25 )
            flYawDiff *= dt * 4;
        else
            flYawDiff *= dt;

        m_PoseParameterData.m_flEstimateYaw += flYawDiff;
        m_PoseParameterData.m_flEstimateYaw =
            m_PoseParameterData.m_flEstimateYaw -
            ( int )( m_PoseParameterData.m_flEstimateYaw / 360 ) * 360;
    }
    else
    {
        m_PoseParameterData.m_flEstimateYaw =
            ( atan2( vecEstVelocity.y, vecEstVelocity.x ) * 180 / M_PI );

        if ( m_PoseParameterData.m_flEstimateYaw > 180 )
            m_PoseParameterData.m_flEstimateYaw = 180;
        else if ( m_PoseParameterData.m_flEstimateYaw < -180 )
            m_PoseParameterData.m_flEstimateYaw = -180;
    }
#endif
}
//-----------------------------------------------------------------------------
// Purpose: Override for backpeddling
// Input  : dt -
//-----------------------------------------------------------------------------
void CExperimentPlayerAnimState::ComputePoseParam_MoveYaw( CStudioHdr *pStudioHdr )
{
    // Get the estimated movement yaw.
    EstimateYaw();

    // view direction relative to movement
    float flYaw;

    QAngle angles = GetBasePlayer()->GetLocalAngles();
    float ang = m_flEyeYaw;

    if ( ang > 180.0f )
    {
        ang -= 360.0f;
    }
    else if ( ang < -180.0f )
    {
        ang += 360.0f;
    }

    // calc side to side turning
    flYaw = ang - m_PoseParameterData.m_flEstimateYaw;
    // Invert for mapping into 8way blend
    flYaw = -flYaw;
    flYaw = flYaw - ( int )( flYaw / 360 ) * 360;

    if ( flYaw < -180 )
    {
        flYaw = flYaw + 360;
    }
    else if ( flYaw > 180 )
    {
        flYaw = flYaw - 360;
    }

    if ( cl_leganimtype.GetBool() )
    {
        // Get the current speed the character is running.
        bool bIsMoving;
        float flPlaybackRate = CalcMovementPlaybackRate( &bIsMoving );

        // Setup the 9-way blend parameters based on our speed and direction.
        Vector2D vecCurrentMoveYaw( 0.0f, 0.0f );
        if ( bIsMoving )
        {
            vecCurrentMoveYaw.x = cos( DEG2RAD( flYaw ) ) * flPlaybackRate;
            vecCurrentMoveYaw.y = -sin( DEG2RAD( flYaw ) ) * flPlaybackRate;
        }

        GetBasePlayer()->SetPoseParameter(
            pStudioHdr, m_PoseParameterData.m_iMoveX, vecCurrentMoveYaw.x );
        GetBasePlayer()->SetPoseParameter(
            pStudioHdr, m_PoseParameterData.m_iMoveY, vecCurrentMoveYaw.y );

        m_DebugAnimData.m_vecMoveYaw = vecCurrentMoveYaw;
    }
    else
    {
        // Tony; oops, i inverted this previously above.
        GetBasePlayer()->SetPoseParameter( pStudioHdr,
                                            m_PoseParameterData.m_iMoveY,
                                            flYaw );
    }
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CExperimentPlayerAnimState::ComputePoseParam_AimPitch( CStudioHdr *pStudioHdr )
{
    // Get the view pitch.
    float flAimPitch = m_flEyePitch;

    // Set the aim pitch pose parameter and save.
    GetBasePlayer()->SetPoseParameter(
        pStudioHdr, m_PoseParameterData.m_iAimPitch, flAimPitch );
    m_DebugAnimData.m_flAimPitch = flAimPitch;
}

void CExperimentPlayerAnimState::ComputePoseParam_AimYaw( CStudioHdr *pStudioHdr )
{
    // Get the movement velocity.
    Vector vecVelocity;
    GetOuterAbsVelocity( vecVelocity );

    // Check to see if we are moving.
    bool bMoving = ( vecVelocity.Length() > 1.0f ) ? true : false;

    // If we are moving or are prone and undeployed.
    if ( bMoving || m_bForceAimYaw )
    {
        // The feet match the eye direction when moving - the move yaw takes
        // care of the rest.
        m_flGoalFeetYaw = m_flEyeYaw;
    }
    // Else if we are not moving.
    else
    {
        // Initialize the feet.
        if ( m_PoseParameterData.m_flLastAimTurnTime <= 0.0f )
        {
            m_flGoalFeetYaw = m_flEyeYaw;
            m_flCurrentFeetYaw = m_flEyeYaw;
            m_PoseParameterData.m_flLastAimTurnTime = gpGlobals->curtime;
        }
        // Make sure the feet yaw isn't too far out of sync with the eye yaw.
        // TODO: Do something better here!
        else
        {
            float flYawDelta = AngleNormalize( m_flGoalFeetYaw - m_flEyeYaw );

            if ( fabs( flYawDelta ) > 45.0f )
            {
                float flSide = ( flYawDelta > 0.0f ) ? -1.0f : 1.0f;
                m_flGoalFeetYaw += ( 45.0f * flSide );
            }
        }
    }

    // Fix up the feet yaw.
    m_flGoalFeetYaw = AngleNormalize( m_flGoalFeetYaw );
    if ( m_flGoalFeetYaw != m_flCurrentFeetYaw )
    {
        if ( m_bForceAimYaw )
        {
            m_flCurrentFeetYaw = m_flGoalFeetYaw;
        }
        else
        {
            ConvergeYawAngles( m_flGoalFeetYaw, 720.0f, gpGlobals->frametime, m_flCurrentFeetYaw );
            m_flLastAimTurnTime = gpGlobals->curtime;
        }
    }

    // Rotate the body into position.
    m_angRender[YAW] = m_flCurrentFeetYaw;

    // Find the aim(torso) yaw base on the eye and feet yaws.
    float flAimYaw = m_flEyeYaw - m_flCurrentFeetYaw;
    flAimYaw = AngleNormalize( flAimYaw );

    // Set the aim yaw and save.
    GetBasePlayer()->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iAimYaw, flAimYaw );
    m_DebugAnimData.m_flAimYaw = flAimYaw;

    // Turn off a force aim yaw - either we have already updated or we don't
    // need to.
    m_bForceAimYaw = false;

#ifndef CLIENT_DLL
    QAngle angle = GetBasePlayer()->GetAbsAngles();
    angle[YAW] = m_flCurrentFeetYaw;

    GetBasePlayer()->SetAbsAngles( angle );
#endif
}

float CExperimentPlayerAnimState::GetCurrentMaxGroundSpeed()
{
    CStudioHdr *pStudioHdr = GetBasePlayer()->GetModelPtr();

    if ( pStudioHdr == NULL )
        return 1.0f;

    float prevY =
        GetBasePlayer()->GetPoseParameter( m_PoseParameterData.m_iMoveY );

    float d = sqrt( prevY * prevY );
    float newY;
    if ( d == 0.0 )
    {
        newY = 0.0;
    }
    else
    {
        newY = prevY / d;
    }

    GetBasePlayer()->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveY, newY );

    float speed =
        GetBasePlayer()->GetSequenceGroundSpeed( GetBasePlayer()->GetSequence() );

    GetBasePlayer()->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveY, prevY );

    return speed;
}

bool CExperimentPlayerAnimState::ShouldResetMainSequence( int iCurrentSequence,
                                                        int iNewSequence )
{
    if ( IsAirborne() )
    {
        if ( !m_bFreshJump )
        {
            // Only reset active mid-air jump sequence if we're transitioning
            // away from that animation
            return iNewSequence !=
                    SelectWeightedSequence( TranslateActivity( ACT_HL2MP_JUMP ) );
        }
        m_bFreshJump = false;
    }

    return BaseClass::ShouldResetMainSequence( iCurrentSequence, iNewSequence );
}
