//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL2.
//
//=============================================================================//

#include "cbase.h"
#include "vcollide_parse.h"
#include "c_experiment_player.h"
#include "view.h"
#include "takedamageinfo.h"
#include "experiment_gamerules.h"
#include "in_buttons.h"
#include "iviewrender_beams.h"  // flashlight beam
#include "r_efx.h"
#include "dlight.h"
#include "c_basetempentity.h"
#include "prediction.h"
#include "bone_setup.h"
#include "c_team.h"
#include <collisionutils.h>

#ifdef LUA_SDK
#include "luamanager.h"
#include "lgametrace.h"
#include "lexperiment_player_shared.h"
#include "ltakedamageinfo.h"
#include "mathlib/lvector.h"
#endif

#if defined( ARGG )
#include "iclientmode.h"
#endif

// Don't alias here
#if defined( CExperiment_Player )
#undef CExperiment_Player
#endif

#define CYCLELATCH_TOLERANCE 0.15f

;  // clang-format off

LINK_ENTITY_TO_CLASS( player, C_Experiment_Player );

BEGIN_RECV_TABLE_NOBASE( C_Experiment_Player, DT_ExperimentLocalPlayerExclusive )
    RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),
    RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),
    //	RecvPropFloat( RECVINFO( m_angEyeAngles[1] ) ),
END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE( C_Experiment_Player, DT_ExperimentNonLocalPlayerExclusive )
    RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),
    RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),
    RecvPropFloat( RECVINFO( m_angEyeAngles[1] ) ),

    RecvPropInt( RECVINFO( m_cycleLatch ), 0, &C_Experiment_Player::RecvProxy_CycleLatch ),
    RecvPropBool( RECVINFO( m_bAvoidPlayers ) ),
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_Experiment_Player, DT_Experiment_Player, CExperiment_Player )
    RecvPropDataTable( "experimentlocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_ExperimentLocalPlayerExclusive ) ),
    RecvPropDataTable( "experimentnonlocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_ExperimentNonLocalPlayerExclusive ) ),

    RecvPropEHandle( RECVINFO( m_hRagdoll ) ),
    RecvPropInt( RECVINFO( m_iSpawnInterpCounter ) ),
    RecvPropInt( RECVINFO( m_iPlayerSoundType ) ),
    RecvPropInt( RECVINFO( m_ArmorValue ) ),
    RecvPropInt( RECVINFO( m_MaxArmorValue ) ),
    
    RecvPropBool( RECVINFO( m_fIsWalking ) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_Experiment_Player )
    DEFINE_PRED_FIELD( m_flCycle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
    DEFINE_PRED_FIELD( m_fIsWalking, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
    DEFINE_PRED_FIELD( m_nSequence, FIELD_INTEGER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
    DEFINE_PRED_FIELD( m_flPlaybackRate, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
    DEFINE_PRED_ARRAY_TOL( m_flEncodedController, FIELD_FLOAT, MAXSTUDIOBONECTRLS, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE, 0.02f ),
    DEFINE_PRED_FIELD( m_nNewSequenceParity, FIELD_INTEGER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
END_PREDICTION_DATA()

;  // clang-format on

static ConVar cl_playermodel( "cl_playermodel", "none", FCVAR_USERINFO | FCVAR_ARCHIVE | FCVAR_SERVER_CAN_EXECUTE, "Default Player Model" );
static ConVar cl_defaultweapon( "cl_defaultweapon", "weapon_physcannon", FCVAR_USERINFO | FCVAR_ARCHIVE, "Default Spawn Weapon" );

ConVar exp_max_separation_force( "exp_max_separation_force", "256", FCVAR_DEVELOPMENTONLY );
ConVar exp_clientsideeye_lookats( "exp_clientsideeye_lookats", "1", FCVAR_NONE, "When on, players will turn their pupils to look at nearby players." );

extern ConVar cl_forwardspeed;
extern ConVar cl_backspeed;
extern ConVar cl_sidespeed;

void SpawnBlood( Vector vecSpot, const Vector &vecDir, int bloodColor, float flDamage );

C_Experiment_Player::C_Experiment_Player()
    : m_iv_angEyeAngles( "C_Experiment_Player::m_iv_angEyeAngles" )
{
    m_iIDEntIndex = 0;
    m_iSpawnInterpCounterCache = 0;

    AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

    // m_EntClientFlags |= ENTCLIENTFLAG_DONTUSEIK;
    m_PlayerAnimState = CreateExperimentPlayerAnimState( this );

    m_blinkTimer.Invalidate();

    m_pFlashlightBeam = NULL;

    m_flServerCycle = -1.0f;
}

C_Experiment_Player::~C_Experiment_Player( void )
{
    ReleaseFlashlight();
    m_PlayerAnimState->Release();
}

int C_Experiment_Player::GetIDTarget() const
{
    return m_iIDEntIndex;
}

//-----------------------------------------------------------------------------
// Purpose: Update this client's target entity
//-----------------------------------------------------------------------------
void C_Experiment_Player::UpdateIDTarget()
{
    if ( !IsLocalPlayer() )
        return;

    // Clear old target and find a new one
    m_iIDEntIndex = 0;

    // don't show IDs in chase spec mode
    if ( GetObserverMode() == OBS_MODE_CHASE ||
         GetObserverMode() == OBS_MODE_DEATHCAM )
        return;

    trace_t tr;
    Vector vecStart, vecEnd;
    VectorMA( MainViewOrigin(), 1500, MainViewForward(), vecEnd );
    VectorMA( MainViewOrigin(), 10, MainViewForward(), vecStart );
    UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

    if ( !tr.startsolid && tr.DidHitNonWorldEntity() )
    {
        C_BaseEntity *pEntity = tr.m_pEnt;

        if ( pEntity && ( pEntity != this ) )
        {
            m_iIDEntIndex = pEntity->entindex();
        }
    }
}

void C_Experiment_Player::TraceAttack( const CTakeDamageInfo &info,
                                       const Vector &vecDir,
                                       trace_t *ptr,
                                       CDmgAccumulator *pAccumulator )
{
#ifdef LUA_SDK
    // Andrew; push a copy of the damageinfo/vector, bring the changes back out
    // of Lua and set info/vecDir to the new value if it's been modified.
    CTakeDamageInfo lInfo = info;
    Vector lvecDir = vecDir;

    LUA_CALL_HOOK_BEGIN( "PlayerTraceAttack" );
    C_Experiment_Player::PushLuaInstanceSafe( L, this );
    lua_pushdamageinfo( L, lInfo );
    lua_pushvector( L, lvecDir );
    lua_pushtrace( L, *ptr );
    LUA_CALL_HOOK_END( 4, 1 );

    LUA_RETURN_NONE_IF_FALSE();

    Vector vecOrigin = ptr->endpos - lvecDir * 4;
#else
    Vector vecOrigin = ptr->endpos - vecDir * 4;
#endif

    float flDistance = 0.0f;

#ifdef LUA_SDK
    if ( lInfo.GetAttacker() )
    {
        flDistance =
            ( ptr->endpos - lInfo.GetAttacker()->GetAbsOrigin() ).Length();
    }
#else
    if ( info.GetAttacker() )
    {
        flDistance =
            ( ptr->endpos - info.GetAttacker()->GetAbsOrigin() ).Length();
    }
#endif

    if ( m_takedamage )
    {
#ifdef LUA_SDK
        AddMultiDamage( lInfo, this );
#else
        AddMultiDamage( info, this );
#endif

        int blood = BloodColor();

#ifdef LUA_SDK
        CBaseEntity *pAttacker = lInfo.GetAttacker();
#else
        CBaseEntity *pAttacker = info.GetAttacker();
#endif

        if ( pAttacker )
        {
            if ( ExperimentRules()->IsTeamplay() &&
                 pAttacker->InSameTeam( this ) == true )
                return;
        }

        if ( blood != DONT_BLEED )
        {
            // a little surface blood.
#ifdef LUA_SDK
            SpawnBlood( vecOrigin, lvecDir, blood, flDistance );
            TraceBleed( flDistance, lvecDir, ptr, lInfo.GetDamageType() );
#else
            SpawnBlood( vecOrigin, vecDir, blood, flDistance );
            TraceBleed( flDistance, vecDir, ptr, info.GetDamageType() );
#endif
        }
    }
}

C_Experiment_Player *C_Experiment_Player::GetLocalExperimentPlayer()
{
    return ( C_Experiment_Player * )C_BasePlayer::GetLocalPlayer();
}

void C_Experiment_Player::Initialize( void )
{
    m_headYawPoseParam = LookupPoseParameter( "head_yaw" );
    GetPoseParameterRange( m_headYawPoseParam, m_headYawMin, m_headYawMax );

    m_headPitchPoseParam = LookupPoseParameter( "head_pitch" );
    GetPoseParameterRange( m_headPitchPoseParam, m_headPitchMin, m_headPitchMax );

    CStudioHdr *hdr = GetModelPtr();
    for ( int i = 0; i < hdr->GetNumPoseParameters(); i++ )
    {
        SetPoseParameter( hdr, i, 0.0 );
    }
}

CStudioHdr *C_Experiment_Player::OnNewModel( void )
{
    CStudioHdr *hdr = BaseClass::OnNewModel();

    Initialize();

    // Reset the players animation states, gestures
    if ( m_PlayerAnimState )
    {
        m_PlayerAnimState->OnNewModel();
    }

    return hdr;
}

//-----------------------------------------------------------------------------
/**
 * Orient head and eyes towards m_lookAt.
 */
void C_Experiment_Player::UpdateLookAt( void )
{
    bool bFoundViewTarget = false;

    Vector vForward;
    AngleVectors( GetLocalAngles(), &vForward );

    Vector vMyOrigin = GetAbsOrigin();

    Vector vecLookAtTarget = vec3_origin;

    if ( exp_clientsideeye_lookats.GetBool() )
    {
        for ( int iClient = 1; iClient <= gpGlobals->maxClients; ++iClient )
        {
            CBaseEntity *pEnt = UTIL_PlayerByIndex( iClient );
            if ( !pEnt || !pEnt->IsPlayer() )
                continue;

            if ( !pEnt->IsAlive() )
                continue;

            if ( pEnt == this )
                continue;

            Vector vDir = pEnt->GetAbsOrigin() - vMyOrigin;

            if ( vDir.Length() > 300 )
                continue;

            VectorNormalize( vDir );

            if ( DotProduct( vForward, vDir ) < 0.0f )
                continue;

            vecLookAtTarget = pEnt->EyePosition();
            bFoundViewTarget = true;
            break;
        }
    }

    if ( bFoundViewTarget == false )
    {
        // no target, look forward
        vecLookAtTarget = GetAbsOrigin() + vForward * 512;
    }

    // orient eyes
    m_viewtarget = vecLookAtTarget;
}

void C_Experiment_Player::ClientThink( void )
{
    bool bFoundViewTarget = false;

    Vector vForward;
    AngleVectors( GetLocalAngles(), &vForward );

    for ( int iClient = 1; iClient <= gpGlobals->maxClients; ++iClient )
    {
        CBaseEntity *pEnt = UTIL_PlayerByIndex( iClient );
        if ( !pEnt || !pEnt->IsPlayer() )
            continue;

        if ( pEnt->entindex() == entindex() )
            continue;

        Vector vTargetOrigin = pEnt->GetAbsOrigin();
        Vector vMyOrigin = GetAbsOrigin();

        Vector vDir = vTargetOrigin - vMyOrigin;

        if ( vDir.Length() > 128 )
            continue;

        VectorNormalize( vDir );

        if ( DotProduct( vForward, vDir ) < 0.0f )
            continue;

        m_vLookAtTarget = pEnt->EyePosition();
        bFoundViewTarget = true;
        break;
    }

    if ( bFoundViewTarget == false )
    {
        m_vLookAtTarget = GetAbsOrigin() + vForward * 512;
    }

    UpdateIDTarget();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int C_Experiment_Player::DrawModel( int flags )
{
    if ( !m_bReadyToDraw )
        return 0;

    return BaseClass::DrawModel( flags );
}

//-----------------------------------------------------------------------------
// Should this object receive shadows?
//-----------------------------------------------------------------------------
bool C_Experiment_Player::ShouldReceiveProjectedTextures( int flags )
{
    Assert( flags & SHADOW_FLAGS_PROJECTED_TEXTURE_TYPE_MASK );

    if ( IsEffectActive( EF_NODRAW ) )
        return false;

    if ( flags & SHADOW_FLAGS_FLASHLIGHT )
    {
        return true;
    }

    return BaseClass::ShouldReceiveProjectedTextures( flags );
}

void C_Experiment_Player::DoImpactEffect( trace_t &tr, int nDamageType )
{
    if ( GetActiveWeapon() )
    {
        GetActiveWeapon()->DoImpactEffect( tr, nDamageType );
        return;
    }

    BaseClass::DoImpactEffect( tr, nDamageType );
}

void C_Experiment_Player::PreThink( void )
{
    QAngle vTempAngles = GetLocalAngles();

    if ( GetLocalPlayer() == this )
    {
        vTempAngles[PITCH] = EyeAngles()[PITCH];
    }
    else
    {
        vTempAngles[PITCH] = m_angEyeAngles[PITCH];
    }

    if ( vTempAngles[YAW] < 0.0f )
    {
        vTempAngles[YAW] += 360.0f;
    }

    SetLocalAngles( vTempAngles );

    BaseClass::PreThink();
}

const QAngle &C_Experiment_Player::EyeAngles()
{
    if ( IsLocalPlayer() )
    {
        return BaseClass::EyeAngles();
    }
    else
    {
        return m_angEyeAngles;
    }
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_Experiment_Player::AddEntity( void )
{
    BaseClass::AddEntity();

    // Zero out model pitch, blending takes care of all of it.
    SetLocalAnglesDim( X_INDEX, 0 );

    if ( this != C_BasePlayer::GetLocalPlayer() )
    {
        if ( IsEffectActive( EF_DIMLIGHT ) )
        {
            int iAttachment = LookupAttachment( "anim_attachment_RH" );

            if ( iAttachment < 0 )
                return;

            Vector vecOrigin;
            QAngle eyeAngles = m_angEyeAngles;

            GetAttachment( iAttachment, vecOrigin, eyeAngles );

            Vector vForward;
            AngleVectors( eyeAngles, &vForward );

            trace_t tr;
            UTIL_TraceLine( vecOrigin, vecOrigin + ( vForward * 200 ), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );

            if ( !m_pFlashlightBeam )
            {
                BeamInfo_t beamInfo;
                beamInfo.m_nType = TE_BEAMPOINTS;
                beamInfo.m_vecStart = tr.startpos;
                beamInfo.m_vecEnd = tr.endpos;
                beamInfo.m_pszModelName = "sprites/glow01.vmt";
                beamInfo.m_pszHaloName = "sprites/glow01.vmt";
                beamInfo.m_flHaloScale = 3.0;
                beamInfo.m_flWidth = 8.0f;
                beamInfo.m_flEndWidth = 35.0f;
                beamInfo.m_flFadeLength = 300.0f;
                beamInfo.m_flAmplitude = 0;
                beamInfo.m_flBrightness = 60.0;
                beamInfo.m_flSpeed = 0.0f;
                beamInfo.m_nStartFrame = 0.0;
                beamInfo.m_flFrameRate = 0.0;
                beamInfo.m_flRed = 255.0;
                beamInfo.m_flGreen = 255.0;
                beamInfo.m_flBlue = 255.0;
                beamInfo.m_nSegments = 8;
                beamInfo.m_bRenderable = true;
                beamInfo.m_flLife = 0.5;
                beamInfo.m_nFlags = FBEAM_FOREVER | FBEAM_ONLYNOISEONCE |
                                    FBEAM_NOTILE | FBEAM_HALOBEAM;

                m_pFlashlightBeam = beams->CreateBeamPoints( beamInfo );
            }

            if ( m_pFlashlightBeam )
            {
                BeamInfo_t beamInfo;
                beamInfo.m_vecStart = tr.startpos;
                beamInfo.m_vecEnd = tr.endpos;
                beamInfo.m_flRed = 255.0;
                beamInfo.m_flGreen = 255.0;
                beamInfo.m_flBlue = 255.0;

                beams->UpdateBeamInfo( m_pFlashlightBeam, beamInfo );

                dlight_t *el = effects->CL_AllocDlight( 0 );
                el->origin = tr.endpos;
                el->radius = 50;
                el->color.r = 200;
                el->color.g = 200;
                el->color.b = 200;
                el->die = gpGlobals->curtime + 0.1;
            }
        }
        else if ( m_pFlashlightBeam )
        {
            ReleaseFlashlight();
        }
    }
}

/// <summary>
/// This is a modified version of: src/game/client/tf/c_tf_player.cpp
/// Note that the TF2 implementation also pushes objects away.
/// </summary>
/// <param name="pCmd"></param>
void C_Experiment_Player::AvoidPlayers( CUserCmd *pCmd )
{
    // Turn off the avoid player code.
    if ( !m_bAvoidPlayers )
        return;

    // Don't test if the player doesn't exist or is dead.
    if ( IsAlive() == false )
        return;

    C_Team *pTeam = GetTeam();
    if ( !pTeam )
        return;

    // Up vector.
    static Vector vecUp( 0.0f, 0.0f, 1.0f );

    Vector vecTFPlayerCenter = GetAbsOrigin();
    Vector vecTFPlayerMin = GetPlayerMins();
    Vector vecTFPlayerMax = GetPlayerMaxs();
    float flZHeight = vecTFPlayerMax.z - vecTFPlayerMin.z;
    vecTFPlayerCenter.z += 0.5f * flZHeight;
    VectorAdd( vecTFPlayerMin, vecTFPlayerCenter, vecTFPlayerMin );
    VectorAdd( vecTFPlayerMax, vecTFPlayerCenter, vecTFPlayerMax );

    // Find an intersecting player.
    int nAvoidPlayerCount = 0;
    C_Experiment_Player *pAvoidPlayerList[MAX_PLAYERS_ARRAY_SAFE];

    C_Experiment_Player *pIntersectPlayer = NULL;
    float flAvoidRadius = 0.0f;

    Vector vecAvoidCenter, vecAvoidMin, vecAvoidMax;
    for ( int i = 0; i < pTeam->GetNumPlayers(); ++i )
    {
        C_Experiment_Player *pAvoidPlayer = static_cast< C_Experiment_Player * >( pTeam->GetPlayer( i ) );
        if ( pAvoidPlayer == NULL )
            continue;
        // Is the avoid player me?
        if ( pAvoidPlayer == this )
            continue;

        if ( !IsIndexIntoPlayerArrayValid( nAvoidPlayerCount ) )
            break;

        // Save as list to check against for objects.
        pAvoidPlayerList[nAvoidPlayerCount] = pAvoidPlayer;
        ++nAvoidPlayerCount;

        // Check to see if the avoid player is dormant.
        if ( pAvoidPlayer->IsDormant() )
            continue;

        // Is the avoid player solid?
        if ( pAvoidPlayer->IsSolidFlagSet( FSOLID_NOT_SOLID ) )
            continue;

        Vector t1, t2;

        vecAvoidCenter = pAvoidPlayer->GetAbsOrigin();
        vecAvoidMin = pAvoidPlayer->GetPlayerMins();
        vecAvoidMax = pAvoidPlayer->GetPlayerMaxs();
        flZHeight = vecAvoidMax.z - vecAvoidMin.z;
        vecAvoidCenter.z += 0.5f * flZHeight;
        VectorAdd( vecAvoidMin, vecAvoidCenter, vecAvoidMin );
        VectorAdd( vecAvoidMax, vecAvoidCenter, vecAvoidMax );

        if ( IsBoxIntersectingBox( vecTFPlayerMin, vecTFPlayerMax, vecAvoidMin, vecAvoidMax ) )
        {
            // Need to avoid this player.
            if ( !pIntersectPlayer )
            {
                pIntersectPlayer = pAvoidPlayer;
                break;
            }
        }
    }

    // Anything to avoid?
    if ( !pIntersectPlayer )
    {
        return;
    }

    // Calculate the push strength and direction.
    Vector vecDelta;

    VectorSubtract( pIntersectPlayer->WorldSpaceCenter(), vecTFPlayerCenter, vecDelta );

    Vector vRad = pIntersectPlayer->WorldAlignMaxs() - pIntersectPlayer->WorldAlignMins();
    vRad.z = 0;

    flAvoidRadius = vRad.Length();

    float flPushStrength = RemapValClamped( vecDelta.Length(), flAvoidRadius, 0, 0, exp_max_separation_force.GetInt() );  // flPushScale;

    // Msg( "PushScale = %f\n", flPushStrength );

    // Check to see if we have enough push strength to make a difference.
    if ( flPushStrength < 0.01f )
        return;

    Vector vecPush;
    if ( GetAbsVelocity().Length2DSqr() > 0.1f )
    {
        Vector vecVelocity = GetAbsVelocity();
        vecVelocity.z = 0.0f;
        CrossProduct( vecUp, vecVelocity, vecPush );
        VectorNormalize( vecPush );
    }
    else
    {
        // We are not moving, but we're still intersecting.
        QAngle angView = pCmd->viewangles;
        angView.x = 0.0f;
        AngleVectors( angView, NULL, &vecPush, NULL );
    }

    // Move away from the other player/object.
    Vector vecSeparationVelocity;
    if ( vecDelta.Dot( vecPush ) < 0 )
    {
        vecSeparationVelocity = vecPush * flPushStrength;
    }
    else
    {
        vecSeparationVelocity = vecPush * -flPushStrength;
    }

    // Don't allow the max push speed to be greater than the max player speed.
    float flMaxPlayerSpeed = GetMaxSpeed();
    float flCropFraction = 1.33333333f;

    if ( ( GetFlags() & FL_DUCKING ) && ( GetGroundEntity() != NULL ) )
    {
        flMaxPlayerSpeed *= flCropFraction;
    }

    float flMaxPlayerSpeedSqr = flMaxPlayerSpeed * flMaxPlayerSpeed;

    if ( vecSeparationVelocity.LengthSqr() > flMaxPlayerSpeedSqr )
    {
        vecSeparationVelocity.NormalizeInPlace();
        VectorScale( vecSeparationVelocity, flMaxPlayerSpeed, vecSeparationVelocity );
    }

    QAngle vAngles = pCmd->viewangles;
    vAngles.x = 0;
    Vector currentdir;
    Vector rightdir;

    AngleVectors( vAngles, &currentdir, &rightdir, NULL );

    Vector vDirection = vecSeparationVelocity;

    VectorNormalize( vDirection );

    float fwd = currentdir.Dot( vDirection );
    float rt = rightdir.Dot( vDirection );

    float forward = fwd * flPushStrength;
    float side = rt * flPushStrength;

    // Msg( "fwd: %f - rt: %f - forward: %f - side: %f\n", fwd, rt, forward, side );

    pCmd->forwardmove += forward;
    pCmd->sidemove += side;

    // Clamp the move to within legal limits, preserving direction. This is a little
    // complicated because we have different limits for forward, back, and side

    // Msg( "PRECLAMP: forwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );

    float flForwardScale = 1.0f;
    if ( pCmd->forwardmove > fabs( cl_forwardspeed.GetFloat() ) )
    {
        flForwardScale = fabs( cl_forwardspeed.GetFloat() ) / pCmd->forwardmove;
    }
    else if ( pCmd->forwardmove < -fabs( cl_backspeed.GetFloat() ) )
    {
        flForwardScale = fabs( cl_backspeed.GetFloat() ) / fabs( pCmd->forwardmove );
    }

    float flSideScale = 1.0f;
    if ( fabs( pCmd->sidemove ) > fabs( cl_sidespeed.GetFloat() ) )
    {
        flSideScale = fabs( cl_sidespeed.GetFloat() ) / fabs( pCmd->sidemove );
    }

    float flScale = MIN( flForwardScale, flSideScale );
    pCmd->forwardmove *= flScale;
    pCmd->sidemove *= flScale;

    // Msg( "Pforwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : flInputSampleTime -
//			*pCmd -
//-----------------------------------------------------------------------------
bool C_Experiment_Player::CreateMove( float flInputSampleTime, CUserCmd *pCmd )
{
    bool bResult = BaseClass::CreateMove( flInputSampleTime, pCmd );

    AvoidPlayers( pCmd );

    return bResult;
}

ShadowType_t C_Experiment_Player::ShadowCastType( void )
{
    if ( !IsVisible() )
        return SHADOWS_NONE;

    return SHADOWS_RENDER_TO_TEXTURE_DYNAMIC;
}

const QAngle &C_Experiment_Player::GetRenderAngles()
{
    if ( IsRagdoll() )
    {
        return vec3_angle;
    }
    else
    {
        return m_PlayerAnimState->GetRenderAngles();
    }
}

bool C_Experiment_Player::ShouldDraw( void )
{
    // If we're dead, our ragdoll will be drawn for us instead.
    if ( !IsAlive() )
        return false;

    //	if( GetTeamNumber() == TEAM_SPECTATOR )
    //		return false;

    if ( IsLocalPlayer() && IsRagdoll() )
        return true;

    if ( IsRagdoll() )
        return false;

    return BaseClass::ShouldDraw();
}

void C_Experiment_Player::NotifyShouldTransmit( ShouldTransmitState_t state )
{
    if ( state == SHOULDTRANSMIT_END )
    {
        if ( m_pFlashlightBeam != NULL )
        {
            ReleaseFlashlight();
        }
    }

    BaseClass::NotifyShouldTransmit( state );
}

void C_Experiment_Player::OnDataChanged( DataUpdateType_t type )
{
    BaseClass::OnDataChanged( type );

    if ( type == DATA_UPDATE_CREATED )
    {
        SetNextClientThink( CLIENT_THINK_ALWAYS );
    }

    UpdateVisibility();
}

void C_Experiment_Player::PostDataUpdate( DataUpdateType_t updateType )
{
    if ( m_iSpawnInterpCounter != m_iSpawnInterpCounterCache )
    {
        MoveToLastReceivedPosition( true );
        ResetLatched();
        m_iSpawnInterpCounterCache = m_iSpawnInterpCounter;
    }

    BaseClass::PostDataUpdate( updateType );
}

void C_Experiment_Player::RecvProxy_CycleLatch( const CRecvProxyData *pData,
                                                void *pStruct,
                                                void *pOut )
{
    C_Experiment_Player *pPlayer = static_cast< C_Experiment_Player * >( pStruct );

    float flServerCycle = ( float )pData->m_Value.m_Int / 16.0f;
    float flCurCycle = pPlayer->GetCycle();
    // The cycle is way out of sync.
    if ( fabs( flCurCycle - flServerCycle ) > CYCLELATCH_TOLERANCE )
    {
        pPlayer->SetServerIntendedCycle( flServerCycle );
    }
}

void C_Experiment_Player::ReleaseFlashlight( void )
{
    if ( m_pFlashlightBeam )
    {
        m_pFlashlightBeam->flags = 0;
        m_pFlashlightBeam->die = gpGlobals->curtime - 1;

        m_pFlashlightBeam = NULL;
    }
}

float C_Experiment_Player::GetFOV( void )
{
    // Find our FOV with offset zoom value
    float flFOVOffset = C_BasePlayer::GetFOV() + GetZoom();

    // Clamp FOV in MP
    int min_fov = GetMinFOV();

    // Don't let it go too low
    flFOVOffset = MAX( min_fov, flFOVOffset );

    return flFOVOffset;
}

//=========================================================
// Autoaim
// set crosshair position to point to enemey
//=========================================================
Vector C_Experiment_Player::GetAutoaimVector( float flDelta )
{
    // Never autoaim a predicted weapon (for now)
    Vector forward;
    AngleVectors( EyeAngles() + m_Local.m_vecPunchAngle, &forward );
    return forward;
}

//-----------------------------------------------------------------------------
// Purpose: Returns whether or not we are allowed to sprint now.
//-----------------------------------------------------------------------------
bool C_Experiment_Player::CanSprint( void )
{
    return ( ( !m_Local.m_bDucked && !m_Local.m_bDucking ) &&
             ( GetWaterLevel() != 3 ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_Experiment_Player::StartSprinting( void )
{
    if ( m_HL2Local.m_flSuitPower < 10 )
    {
        // Don't sprint unless there's a reasonable
        // amount of suit power.
        CPASAttenuationFilter filter( this );
        filter.UsePredictionRules();
        EmitSound( filter, entindex(), "HL2Player.SprintNoPower" );
        return;
    }

    CPASAttenuationFilter filter( this );
    filter.UsePredictionRules();
    EmitSound( filter, entindex(), "HL2Player.SprintStart" );

#if defined( EXPERIMENT_SOURCE )
    SetMaxSpeed( GetRunSpeed() );
#elif defined( HL2MP )
    SetMaxSpeed( HL2_SPRINT_SPEED );
#else
    SetMaxSpeed( hl2_sprintspeed.GetFloat() );
#endif
    m_fIsSprinting = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_Experiment_Player::StopSprinting( void )
{
#if defined( EXPERIMENT_SOURCE )
    SetMaxSpeed( GetNormalSpeed() );
#elif defined( HL2MP )
    SetMaxSpeed( HL2_NORM_SPEED );
#else
    SetMaxSpeed( hl2_normspeed.GetFloat() );
#endif
    m_fIsSprinting = false;
}

extern ConVar sv_maxspeed;

void C_Experiment_Player::HandleSpeedChanges( CMoveData *mv )
{
    int nChangedButtons = mv->m_nButtons ^ mv->m_nOldButtons;

    bool bJustPressedSpeed = !!( nChangedButtons & IN_SPEED );

    const bool bWantSprint = ( CanSprint() && IsSuitEquipped() && ( mv->m_nButtons & IN_SPEED ) );
    const bool bWantsToChangeSprinting = ( m_HL2Local.m_bNewSprinting != bWantSprint ) && ( nChangedButtons & IN_SPEED ) != 0;

    bool bSprinting = m_HL2Local.m_bNewSprinting;
    if ( bWantsToChangeSprinting )
    {
        if ( bWantSprint )
        {
            if ( m_HL2Local.m_flSuitPower < 10.0f )
            {
                if ( bJustPressedSpeed )
                {
                    CPASAttenuationFilter filter( this );
                    filter.UsePredictionRules();
                    EmitSound( filter, entindex(), "HL2Player.SprintNoPower" );
                }
            }
            else
            {
                bSprinting = true;
            }
        }
        else
        {
            bSprinting = false;
        }
    }

    if ( m_HL2Local.m_flSuitPower < 0.01 )
    {
        bSprinting = false;
    }

    bool bWantWalking;

    if ( IsSuitEquipped() )
    {
        bWantWalking = ( mv->m_nButtons & IN_WALK ) && !bSprinting && !( mv->m_nButtons & IN_DUCK );
    }
    else
    {
        bWantWalking = true;
    }

    if ( bWantWalking )
    {
        bSprinting = false;
    }

    m_HL2Local.m_bNewSprinting = bSprinting;

    if ( bSprinting )
    {
        if ( bJustPressedSpeed )
        {
            CPASAttenuationFilter filter( this );
            filter.UsePredictionRules();
            EmitSound( filter, entindex(), "HL2Player.SprintStart" );
        }
        mv->m_flClientMaxSpeed = GetRunSpeed();
    }
    else if ( bWantWalking )
    {
        mv->m_flClientMaxSpeed = GetWalkSpeed();
    }
    else
    {
        mv->m_flClientMaxSpeed = GetNormalSpeed();
    }

    mv->m_flMaxSpeed = sv_maxspeed.GetFloat();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_Experiment_Player::StartWalking( void )
{
#if defined( EXPERIMENT_SOURCE )
    SetMaxSpeed( GetWalkSpeed() );
#elif defined( HL2MP )
    SetMaxSpeed( HL2_WALK_SPEED );
#else
    SetMaxSpeed( hl2_walkspeed.GetFloat() );
#endif
    m_fIsWalking = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_Experiment_Player::StopWalking( void )
{
#if defined( EXPERIMENT_SOURCE )
    SetMaxSpeed( GetRunSpeed() );
#elif defined( HL2MP )
    SetMaxSpeed( HL2_SPRINT_SPEED );
#else
    SetMaxSpeed( hl2_sprintspeed.GetFloat() );
#endif
    m_fIsWalking = false;
}

void C_Experiment_Player::ItemPreFrame( void )
{
    if ( GetFlags() & FL_FROZEN )
        return;

    // Disallow shooting while zooming
    if ( m_nButtons & IN_ZOOM )
    {
        // FIXME: Held weapons like the grenade get sad when this happens
        m_nButtons &= ~( IN_ATTACK | IN_ATTACK2 );
    }

    BaseClass::ItemPreFrame();
}

void C_Experiment_Player::ItemPostFrame( void )
{
    if ( GetFlags() & FL_FROZEN )
        return;

    BaseClass::ItemPostFrame();
}

C_BaseAnimating *C_Experiment_Player::BecomeRagdollOnClient()
{
    // Let the C_CSRagdoll entity do this.
    // m_builtRagdoll = true;
    return NULL;
}

void C_Experiment_Player::CalcView( CViewSetup &viewSetup )
{
    BaseClass::CalcView( viewSetup );
}

IRagdoll *C_Experiment_Player::GetRepresentativeRagdoll() const
{
    if ( m_hRagdoll.Get() )
    {
        C_ExperimentRagdoll *pRagdoll = ( C_ExperimentRagdoll * )m_hRagdoll.Get();

        return pRagdoll->GetIRagdoll();
    }
    else
    {
        return NULL;
    }
}

;  // clang-format off

IMPLEMENT_CLIENTCLASS_DT_NOBASE( C_ExperimentRagdoll, DT_ExperimentRagdoll, CExperimentRagdoll )
    RecvPropVector( RECVINFO( m_vecRagdollOrigin ) ),
    RecvPropEHandle( RECVINFO( m_hPlayer ) ),
    RecvPropInt( RECVINFO( m_nModelIndex ) ),
    RecvPropInt( RECVINFO( m_nForceBone ) ),
    RecvPropVector( RECVINFO( m_vecForce ) ),
    RecvPropVector( RECVINFO( m_vecRagdollVelocity ) )
END_RECV_TABLE()

;  // clang-format on

C_ExperimentRagdoll::C_ExperimentRagdoll()
{
}

C_ExperimentRagdoll::~C_ExperimentRagdoll()
{
    PhysCleanupFrictionSounds( this );

    if ( m_hPlayer )
    {
        m_hPlayer->CreateModelInstance();
    }
}

void C_ExperimentRagdoll::Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity )
{
    if ( !pSourceEntity )
        return;

    VarMapping_t *pSrc = pSourceEntity->GetVarMapping();
    VarMapping_t *pDest = GetVarMapping();

    // Find all the VarMapEntry_t's that represent the same variable.
    for ( int i = 0; i < pDest->m_Entries.Count(); i++ )
    {
        VarMapEntry_t *pDestEntry = &pDest->m_Entries[i];
        const char *pszName = pDestEntry->watcher->GetDebugName();
        for ( int j = 0; j < pSrc->m_Entries.Count(); j++ )
        {
            VarMapEntry_t *pSrcEntry = &pSrc->m_Entries[j];
            if ( !Q_strcmp( pSrcEntry->watcher->GetDebugName(), pszName ) )
            {
                pDestEntry->watcher->Copy( pSrcEntry->watcher );
                break;
            }
        }
    }
}

void C_ExperimentRagdoll::ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName )
{
    IPhysicsObject *pPhysicsObject = VPhysicsGetObject();

    if ( !pPhysicsObject )
        return;

    Vector dir = pTrace->endpos - pTrace->startpos;

    if ( iDamageType == DMG_BLAST )
    {
        dir *= 4000;  // adjust impact strenght

        // apply force at object mass center
        pPhysicsObject->ApplyForceCenter( dir );
    }
    else
    {
        Vector hitpos;

        VectorMA( pTrace->startpos, pTrace->fraction, dir, hitpos );
        VectorNormalize( dir );

        dir *= 4000;  // adjust impact strenght

        // apply force where we hit it
        pPhysicsObject->ApplyForceOffset( dir, hitpos );

        // Blood spray!
        //		FX_CS_BloodSpray( hitpos, dir, 10 );
    }

    m_pRagdoll->ResetRagdollSleepAfterTime();
}

void C_ExperimentRagdoll::CreateExperimentRagdoll( void )
{
    // First, initialize all our data. If we have the player's entity on our client,
    // then we can make ourselves start out exactly where the player is.
    C_Experiment_Player *pPlayer = dynamic_cast< C_Experiment_Player * >( m_hPlayer.Get() );

    if ( pPlayer && !pPlayer->IsDormant() )
    {
        // move my current model instance to the ragdoll's so decals are preserved.
        pPlayer->SnatchModelInstance( this );

        VarMapping_t *varMap = GetVarMapping();

        // Copy all the interpolated vars from the player entity.
        // The entity uses the interpolated history to get bone velocity.
        bool bRemotePlayer = ( pPlayer != C_BasePlayer::GetLocalPlayer() );
        if ( bRemotePlayer )
        {
            Interp_Copy( pPlayer );

            SetAbsAngles( pPlayer->GetRenderAngles() );
            GetRotationInterpolator().Reset();

            m_flAnimTime = pPlayer->m_flAnimTime;
            SetSequence( pPlayer->GetSequence() );
            m_flPlaybackRate = pPlayer->GetPlaybackRate();
        }
        else
        {
            // This is the local player, so set them in a default
            // pose and slam their velocity, angles and origin
            SetAbsOrigin( m_vecRagdollOrigin );

            SetAbsAngles( pPlayer->GetRenderAngles() );

            SetAbsVelocity( m_vecRagdollVelocity );

            int iSeq = pPlayer->GetSequence();
            if ( iSeq == -1 )
            {
                Assert( false );  // missing walk_lower?
                iSeq = 0;
            }

            SetSequence( iSeq );  // walk_lower, basic pose
            SetCycle( 0.0 );

            Interp_Reset( varMap );
        }
    }
    else
    {
        // overwrite network origin so later interpolation will
        // use this position
        SetNetworkOrigin( m_vecRagdollOrigin );

        SetAbsOrigin( m_vecRagdollOrigin );
        SetAbsVelocity( m_vecRagdollVelocity );

        Interp_Reset( GetVarMapping() );
    }

    SetModelIndex( m_nModelIndex );

    // Make us a ragdoll..
    m_nRenderFX = kRenderFxRagdoll;

    matrix3x4_t boneDelta0[MAXSTUDIOBONES];
    matrix3x4_t boneDelta1[MAXSTUDIOBONES];
    matrix3x4_t currentBones[MAXSTUDIOBONES];
    const float boneDt = 0.05f;

    if ( pPlayer && !pPlayer->IsDormant() )
    {
        pPlayer->GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
    }
    else
    {
        GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
    }

    InitAsClientRagdoll( boneDelta0, boneDelta1, currentBones, boneDt );
}

void C_ExperimentRagdoll::OnDataChanged( DataUpdateType_t type )
{
    BaseClass::OnDataChanged( type );

    if ( type == DATA_UPDATE_CREATED )
    {
        CreateExperimentRagdoll();
    }
}

IRagdoll *C_ExperimentRagdoll::GetIRagdoll() const
{
    return m_pRagdoll;
}

void C_ExperimentRagdoll::UpdateOnRemove( void )
{
    VPhysicsSetObject( NULL );

    BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: clear out any face/eye values stored in the material system
//-----------------------------------------------------------------------------
void C_ExperimentRagdoll::SetupWeights( const matrix3x4_t *pBoneToWorld,
                                        int nFlexWeightCount,
                                        float *pFlexWeights,
                                        float *pFlexDelayedWeights )
{
    BaseClass::SetupWeights( pBoneToWorld, nFlexWeightCount, pFlexWeights, pFlexDelayedWeights );

    static float destweight[128];
    static bool bIsInited = false;

    CStudioHdr *hdr = GetModelPtr();
    if ( !hdr )
        return;

    int nFlexDescCount = hdr->numflexdesc();
    if ( nFlexDescCount )
    {
        Assert( !pFlexDelayedWeights );
        memset( pFlexWeights, 0, nFlexWeightCount * sizeof( float ) );
    }

    if ( m_iEyeAttachment > 0 )
    {
        matrix3x4_t attToWorld;
        if ( GetAttachment( m_iEyeAttachment, attToWorld ) )
        {
            Vector local, tmp;
            local.Init( 1000.0f, 0.0f, 0.0f );
            VectorTransform( local, attToWorld, tmp );
            modelrender->SetViewTarget( GetModelPtr(), GetBody(), tmp );
        }
    }
}

void C_Experiment_Player::UpdateClientSideAnimation()
{
    m_PlayerAnimState->Update( EyeAngles()[YAW], EyeAngles()[PITCH] );

    BaseClass::UpdateClientSideAnimation();
}

// --------------------------------------------------------------------------------
// Player animation event. Sent to the client when a player fires, jumps,
// reloads, etc..
// --------------------------------------------------------------------------------
class C_TEPlayerAnimEvent : public C_BaseTempEntity
{
   public:
    DECLARE_CLASS( C_TEPlayerAnimEvent, C_BaseTempEntity );
    DECLARE_CLIENTCLASS();

    virtual void PostDataUpdate( DataUpdateType_t updateType )
    {
        // Create the effect.
        C_Experiment_Player *pPlayer =
            dynamic_cast< C_Experiment_Player * >( m_hPlayer.Get() );
        if ( pPlayer && !pPlayer->IsDormant() )
        {
            pPlayer->DoAnimationEvent( ( PlayerAnimEvent_t )m_iEvent.Get(),
                                       m_nData );
        }
    }

   public:
    CNetworkHandle( CBasePlayer, m_hPlayer );
    CNetworkVar( int, m_iEvent );
    CNetworkVar( int, m_nData );
};

;  // clang-format off

IMPLEMENT_CLIENTCLASS_EVENT( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent, CTEPlayerAnimEvent );

BEGIN_RECV_TABLE_NOBASE( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent )
    RecvPropEHandle( RECVINFO( m_hPlayer ) ),
    RecvPropInt( RECVINFO( m_iEvent ) ),
    RecvPropInt( RECVINFO( m_nData ) )
END_RECV_TABLE()

;  // clang-format on

void C_Experiment_Player::DoAnimationEvent( PlayerAnimEvent_t event,
                                            int nData )
{
    if ( IsLocalPlayer() )
    {
        if ( ( prediction->InPrediction() && !prediction->IsFirstTimePredicted() ) )
            return;
    }

    MDLCACHE_CRITICAL_SECTION();
    m_PlayerAnimState->DoAnimationEvent( event, nData );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_Experiment_Player::CalculateIKLocks( float currentTime )
{
    if ( !m_pIk )
        return;

    int targetCount = m_pIk->m_target.Count();
    if ( targetCount == 0 )
        return;

    // In TF, we might be attaching a player's view to a walking model that's
    // using IK. If we are, it can get in here during the view setup code, and
    // it's not normally supposed to be able to access the spatial partition
    // that early in the rendering loop. So we allow access right here for that
    // special case.
    SpatialPartitionListMask_t curSuppressed = partition->GetSuppressedLists();
    partition->SuppressLists( PARTITION_ALL_CLIENT_EDICTS, false );
    CBaseEntity::PushEnableAbsRecomputations( false );

    for ( int i = 0; i < targetCount; i++ )
    {
        trace_t trace;
        CIKTarget *pTarget = &m_pIk->m_target[i];

        if ( !pTarget->IsActive() )
            continue;

        switch ( pTarget->type )
        {
            case IK_GROUND:
            {
                pTarget->SetPos( Vector( pTarget->est.pos.x, pTarget->est.pos.y, GetRenderOrigin().z ) );
                pTarget->SetAngles( GetRenderAngles() );
            }
            break;

            case IK_ATTACHMENT:
            {
                C_BaseEntity *pEntity = NULL;
                float flDist = pTarget->est.radius;

                // FIXME: make entity finding sticky!
                // FIXME: what should the radius check be?
                for ( CEntitySphereQuery sphere( pTarget->est.pos, 64 );
                      ( pEntity = sphere.GetCurrentEntity() ) != NULL;
                      sphere.NextEntity() )
                {
                    C_BaseAnimating *pAnim = pEntity->GetBaseAnimating();
                    if ( !pAnim )
                        continue;

                    int iAttachment = pAnim->LookupAttachment(
                        pTarget->offset.pAttachmentName );
                    if ( iAttachment <= 0 )
                        continue;

                    Vector origin;
                    QAngle angles;
                    pAnim->GetAttachment( iAttachment, origin, angles );

                    // debugoverlay->AddBoxOverlay( origin, Vector( -1, -1, -1
                    // ), Vector( 1, 1, 1 ), QAngle( 0, 0, 0 ), 255, 0, 0, 0, 0
                    // );

                    float d = ( pTarget->est.pos - origin ).Length();

                    if ( d >= flDist )
                        continue;

                    flDist = d;
                    pTarget->SetPos( origin );
                    pTarget->SetAngles( angles );
                    // debugoverlay->AddBoxOverlay( pTarget->est.pos, Vector(
                    // -pTarget->est.radius, -pTarget->est.radius,
                    // -pTarget->est.radius ), Vector( pTarget->est.radius,
                    // pTarget->est.radius, pTarget->est.radius), QAngle( 0, 0,
                    // 0 ), 0, 255, 0, 0, 0 );
                }

                if ( flDist >= pTarget->est.radius )
                {
                    // debugoverlay->AddBoxOverlay( pTarget->est.pos, Vector(
                    // -pTarget->est.radius, -pTarget->est.radius,
                    // -pTarget->est.radius ), Vector( pTarget->est.radius,
                    // pTarget->est.radius, pTarget->est.radius), QAngle( 0, 0,
                    // 0 ), 0, 0, 255, 0, 0 ); no solution, disable ik rule
                    pTarget->IKFailed();
                }
            }
            break;
        }
    }

    CBaseEntity::PopEnableAbsRecomputations();
    partition->SuppressLists( curSuppressed, true );
}

bool C_Experiment_Player::KeyDown( int buttonCode )
{
    return m_nButtons & buttonCode;
}
