#include "cbase.h"
#include "weapon_experimentbasehlmpcombatweapon.h"
#include "experiment_player.h"
#include "globalstate.h"
#include "game.h"
#include "gamerules.h"
#include "experiment_player_shared.h"
#include "predicted_viewmodel.h"
#include "in_buttons.h"
#include "experiment_gamerules.h"
#include "KeyValues.h"
#include "team.h"
#include "weapon_experimentbase.h"
#include "grenade_satchel.h"
#include "eventqueue.h"
#include "gamestats.h"
#include "tier0/vprof.h"
#include "bone_setup.h"

#ifdef LUA_SDK
#include "luamanager.h"
#include "lbaseentity_shared.h"
#include "lexperiment_player_shared.h"
#include "ltakedamageinfo.h"
#endif

#include "engine/IEngineSound.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"

#include "ilagcompensationmanager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

int g_iLastCitizenModel = 0;
int g_iLastCombineModel = 0;

CBaseEntity *g_pLastCombineSpawn = NULL;
CBaseEntity *g_pLastRebelSpawn = NULL;
extern CBaseEntity *g_pLastSpawn;

#define Experiment_COMMAND_MAX_RATE 0.3

#define CYCLELATCH_UPDATE_INTERVAL 0.2f

void DropPrimedFragGrenade( CExperiment_Player *pPlayer, CBaseCombatWeapon *pGrenade );

;  // clang-format off

LINK_ENTITY_TO_CLASS( player, CExperiment_Player );

LINK_ENTITY_TO_CLASS( info_player_combine, CPointEntity );
LINK_ENTITY_TO_CLASS( info_player_rebel, CPointEntity );

extern void SendProxy_Origin( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID );

BEGIN_SEND_TABLE_NOBASE( CExperiment_Player, DT_ExperimentLocalPlayerExclusive )
    // send a hi-res origin to the local player for use in prediction
    SendPropVector( SENDINFO( m_vecOrigin ), -1, SPROP_NOSCALE | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),
    SendPropFloat( SENDINFO_VECTORELEM( m_angEyeAngles, 0 ), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f ),
    //	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 10, SPROP_CHANGES_OFTEN ),
END_SEND_TABLE()

BEGIN_SEND_TABLE_NOBASE( CExperiment_Player, DT_ExperimentNonLocalPlayerExclusive )
    // send a lo-res origin to other players
    SendPropVector( SENDINFO( m_vecOrigin ), -1, SPROP_COORD_MP_LOWPRECISION | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),
    SendPropFloat( SENDINFO_VECTORELEM( m_angEyeAngles, 0 ), 8, SPROP_CHANGES_OFTEN, -90.0f, 90.0f ),
    SendPropAngle( SENDINFO_VECTORELEM( m_angEyeAngles, 1 ), 10, SPROP_CHANGES_OFTEN ),
    // Only need to latch cycle for other players
    // If you increase the number of bits networked, make sure to also modify the code below and in the client class.
    SendPropInt( SENDINFO( m_cycleLatch ), 4, SPROP_UNSIGNED ),
    SendPropBool( SENDINFO( m_bAvoidPlayers ) ),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST( CExperiment_Player, DT_Experiment_Player )
    SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
    SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),
    SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
    SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
    SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),

    SendPropExclude( "DT_BaseEntity", "m_vecOrigin" ),

    // playeranimstate and clientside animation takes care of these on the client
    SendPropExclude( "DT_ServerAnimationData", "m_flCycle" ),
    SendPropExclude( "DT_AnimTimeMustBeFirst", "m_flAnimTime" ),

    SendPropExclude( "DT_BaseFlex", "m_flexWeight" ),
    SendPropExclude( "DT_BaseFlex", "m_blinktoggle" ),
    SendPropExclude( "DT_BaseFlex", "m_viewtarget" ),

    // Data that only gets sent to the local player.
    SendPropDataTable( "experimentlocaldata", 0, &REFERENCE_SEND_TABLE( DT_ExperimentLocalPlayerExclusive ), SendProxy_SendLocalDataTable ),
    // Data that gets sent to all other players
    SendPropDataTable( "experimentnonlocaldata", 0, &REFERENCE_SEND_TABLE( DT_ExperimentNonLocalPlayerExclusive ), SendProxy_SendNonLocalDataTable ),

    SendPropEHandle( SENDINFO( m_hRagdoll ) ),
    SendPropInt( SENDINFO( m_iSpawnInterpCounter ), 4 ),
    SendPropInt( SENDINFO( m_iPlayerSoundType ), 3 ),
    SendPropInt( SENDINFO( m_ArmorValue ) ),
    SendPropInt( SENDINFO( m_MaxArmorValue ) ),
END_SEND_TABLE()

BEGIN_DATADESC( CExperiment_Player )
END_DATADESC()

;  // clang-format on

const char *g_ppszRandomCitizenModels[] = {
    "models/humans/group03/male_01.mdl",
    "models/humans/group03/male_02.mdl",
    "models/humans/group03/female_01.mdl",
    "models/humans/group03/male_03.mdl",
    "models/humans/group03/female_02.mdl",
    "models/humans/group03/male_04.mdl",
    "models/humans/group03/female_03.mdl",
    "models/humans/group03/male_05.mdl",
    "models/humans/group03/female_04.mdl",
    "models/humans/group03/male_06.mdl",
    "models/humans/group03/female_06.mdl",
    "models/humans/group03/male_07.mdl",
    "models/humans/group03/female_07.mdl",
    "models/humans/group03/male_08.mdl",
    "models/humans/group03/male_09.mdl",
};

const char *g_ppszRandomCombineModels[] = {
    "models/stalkertnb/bandit_reager.mdl",
    "models/stalkertnb/bandit2.mdl",
    "models/stalkertnb/exo_free.mdl",
};

const char *g_ppszDefaultModel = "models/stalkertnb/exo_free.mdl";

#define MAX_COMBINE_MODELS 4
#define MODEL_CHANGE_INTERVAL 5.0f
#define TEAM_CHANGE_INTERVAL 5.0f

#define ExperimentPLAYER_PHYSDAMAGE_SCALE 4.0f

#pragma warning( disable : 4355 )

CExperiment_Player::CExperiment_Player()
#ifndef NO_STEAM
    : m_CallbackValidateAuthTicketResponse( this, &CExperiment_Player::OnValidateAuthTicketResponse )
#endif
{
    // Tony; create our player animation state.
    m_PlayerAnimState = CreateExperimentPlayerAnimState( this );
    UseClientSideAnimation();

    m_angEyeAngles.Init();

    m_iLastWeaponFireUsercmd = 0;

    m_flNextModelChangeTime = 0.0f;
    m_flNextTeamChangeTime = 0.0f;

    m_iSpawnInterpCounter = 0;

    m_bEnterObserver = false;
    m_bReady = false;

    m_cycleLatch = 0;
    m_cycleLatchTimer.Invalidate();

    m_bAvoidPlayers = false;

    BaseClass::ChangeTeam( TEAM_UNASSIGNED );
}

CExperiment_Player::~CExperiment_Player( void )
{
    m_PlayerAnimState->Release();
}

#ifndef NO_STEAM
void CExperiment_Player::OnValidateAuthTicketResponse( ValidateAuthTicketResponse_t *pResponse )
{
#ifdef LUA_SDK
    const char *pszSteamID = pResponse->m_SteamID.Render();

    // GetUniqueID() relies on engine->GetClientSteamID getting the correct SteamID
    Assert( Q_strcmp( engine->GetClientSteamID( edict() )->Render(), pszSteamID ) == 0 );

    LUA_CALL_HOOK_BEGIN( "PlayerAuthed" );
    CExperiment_Player::PushLuaInstanceSafe( L, this );
    lua_pushstring( L, pszSteamID );
    lua_pushinteger( L, ( lua_Integer )GetUniqueID() );
    LUA_CALL_HOOK_END( 3, 0 );
#endif
}
#endif

void CExperiment_Player::UpdateOnRemove( void )
{
    if ( m_hRagdoll )
    {
        UTIL_RemoveImmediate( m_hRagdoll );
        m_hRagdoll = NULL;
    }

    BaseClass::UpdateOnRemove();
}

void CExperiment_Player::Precache( void )
{
    BaseClass::Precache();

    PrecacheModel( "sprites/glow01.vmt" );
    PrecacheModel( g_ppszDefaultModel );

    // https://developer.valvesoftware.com/wiki/Hand_Viewmodels
    // TODO: Make dependant on model:
    PrecacheModel( "models/weapons/c_arms_citizen.mdl" );

    // Precache Citizen models
    int nHeads = ARRAYSIZE( g_ppszRandomCitizenModels );
    int i;

    for ( i = 0; i < nHeads; ++i )
        PrecacheModel( g_ppszRandomCitizenModels[i] );

    // Precache Combine Models
    nHeads = ARRAYSIZE( g_ppszRandomCombineModels );

    for ( i = 0; i < nHeads; ++i )
        PrecacheModel( g_ppszRandomCombineModels[i] );

    PrecacheFootStepSounds();

    PrecacheScriptSound( "NPC_MetroPolice.Die" );
    PrecacheScriptSound( "NPC_CombineS.Die" );
    PrecacheScriptSound( "NPC_Citizen.die" );
}

void CExperiment_Player::GiveAllItems( void )
{
    EquipSuit();

    CBasePlayer::GiveAmmo( 255, "Pistol" );
    CBasePlayer::GiveAmmo( 255, "AR2" );
    CBasePlayer::GiveAmmo( 5, "AR2AltFire" );
    CBasePlayer::GiveAmmo( 255, "SMG1" );
    CBasePlayer::GiveAmmo( 1, "smg1_grenade" );
    CBasePlayer::GiveAmmo( 255, "Buckshot" );
    CBasePlayer::GiveAmmo( 32, "357" );
    CBasePlayer::GiveAmmo( 3, "rpg_round" );

    CBasePlayer::GiveAmmo( 1, "grenade" );
    CBasePlayer::GiveAmmo( 2, "slam" );

    GiveNamedItem( "weapon_crowbar" );
    GiveNamedItem( "weapon_stunstick" );
    GiveNamedItem( "weapon_pistol" );
    GiveNamedItem( "weapon_357" );

    GiveNamedItem( "weapon_smg1" );
    GiveNamedItem( "weapon_ar2" );

    GiveNamedItem( "weapon_shotgun" );
    GiveNamedItem( "weapon_frag" );

    GiveNamedItem( "weapon_crossbow" );

    GiveNamedItem( "weapon_rpg" );

    GiveNamedItem( "weapon_slam" );

    GiveNamedItem( "weapon_physcannon" );
}

void CExperiment_Player::GiveDefaultItems( void )
{
#ifdef LUA_SDK
    LUA_CALL_HOOK_BEGIN( "GiveDefaultItems" );
    CExperiment_Player::PushLuaInstanceSafe( L, this );
    LUA_CALL_HOOK_END( 1, 0 );
#else
    EquipSuit();

    CBasePlayer::GiveAmmo( 255, "Pistol" );
    CBasePlayer::GiveAmmo( 45, "SMG1" );
    CBasePlayer::GiveAmmo( 1, "grenade" );
    CBasePlayer::GiveAmmo( 6, "Buckshot" );
    CBasePlayer::GiveAmmo( 6, "357" );

    if ( GetPlayerModelType() == PLAYER_SOUNDS_METROPOLICE ||
         GetPlayerModelType() == PLAYER_SOUNDS_COMBINESOLDIER )
    {
        GiveNamedItem( "weapon_stunstick" );
    }
    else if ( GetPlayerModelType() == PLAYER_SOUNDS_CITIZEN )
    {
        GiveNamedItem( "weapon_crowbar" );
    }

    GiveNamedItem( "weapon_pistol" );
    GiveNamedItem( "weapon_smg1" );
    GiveNamedItem( "weapon_frag" );
    GiveNamedItem( "weapon_physcannon" );

    const char *szDefaultWeaponName = engine->GetClientConVarValue(
        engine->IndexOfEdict( edict() ), "cl_defaultweapon" );

    CBaseCombatWeapon *pDefaultWeapon =
        Weapon_OwnsThisType( szDefaultWeaponName );

    if ( pDefaultWeapon )
    {
        Weapon_Switch( pDefaultWeapon );
    }
    else
    {
        Weapon_Switch( Weapon_OwnsThisType( "weapon_physcannon" ) );
    }
#endif
}

void CExperiment_Player::PickDefaultSpawnTeam( void )
{
    if ( GetTeamNumber() == 0 )
    {
        if ( ExperimentRules()->IsTeamplay() == false )
        {
            if ( GetModelPtr() == NULL )
            {
                const char *szModelName = NULL;
                szModelName = engine->GetClientConVarValue(
                    engine->IndexOfEdict( edict() ), "cl_playermodel" );

                if ( ValidatePlayerModel( szModelName ) == false )
                {
                    char szReturnString[512];

                    Q_snprintf( szReturnString, sizeof( szReturnString ), "cl_playermodel %s \n", g_ppszDefaultModel );
                    engine->ClientCommand( edict(), szReturnString );
                }

                ChangeTeam( TEAM_UNASSIGNED );
            }
        }
        else
        {
            CTeam *pCombine = g_Teams[TEAM_COMBINE];
            CTeam *pRebels = g_Teams[TEAM_REBELS];

            if ( pCombine == NULL || pRebels == NULL )
            {
                ChangeTeam( random->RandomInt( TEAM_COMBINE, TEAM_REBELS ) );
            }
            else
            {
                if ( pCombine->GetNumPlayers() > pRebels->GetNumPlayers() )
                {
                    ChangeTeam( TEAM_REBELS );
                }
                else if ( pCombine->GetNumPlayers() <
                          pRebels->GetNumPlayers() )
                {
                    ChangeTeam( TEAM_COMBINE );
                }
                else
                {
                    ChangeTeam( random->RandomInt( TEAM_COMBINE, TEAM_REBELS ) );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Purpose: Sets HL2 specific defaults.
//-----------------------------------------------------------------------------
void CExperiment_Player::Spawn( void )
{
    m_flNextModelChangeTime = 0.0f;
    m_flNextTeamChangeTime = 0.0f;

    PickDefaultSpawnTeam();

    BaseClass::Spawn();
    // GetHands()->SetModel( "models/weapons/c_arms_citizen.mdl" ); // implemented lua-side

    if ( !IsObserver() )
    {
        pl.deadflag = false;
        RemoveSolidFlags( FSOLID_NOT_SOLID );

        RemoveEffects( EF_NODRAW );

        GiveDefaultItems();
    }

    m_nRenderFX = kRenderNormal;

    m_Local.m_iHideHUD = 0;

    AddFlag( FL_ONGROUND );  // set the player on the ground at the start of the
                             // round.

    m_impactEnergyScale = ExperimentPLAYER_PHYSDAMAGE_SCALE;

    if ( ExperimentRules()->IsIntermission() )
    {
        AddFlag( FL_FROZEN );
    }
    else
    {
        RemoveFlag( FL_FROZEN );
    }

    m_iSpawnInterpCounter = ( m_iSpawnInterpCounter + 1 ) % 8;

    m_Local.m_bDucked = false;

    SetPlayerUnderwater( false );

    m_bReady = false;

    m_cycleLatchTimer.Start( CYCLELATCH_UPDATE_INTERVAL );

    // Tony; do the spawn animevent
    DoAnimationEvent( PLAYERANIMEVENT_SPAWN );
}

void CExperiment_Player::PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize )
{
#ifdef LUA_SDK
    LUA_CALL_HOOK_BEGIN( "AllowPlayerPickup" );
    CExperiment_Player::PushLuaInstanceSafe( L, this );
    CBaseEntity::PushLuaInstanceSafe( L, pObject );
    lua_pushboolean( L, bLimitMassAndSize );
    LUA_CALL_HOOK_END( 3, 1 );

    LUA_RETURN_NONE_IF_FALSE();
#endif

#ifdef EXPERIMENT_SOURCE
    // can't pick up what you're standing on
    if ( GetGroundEntity() == pObject )
        return;

    if ( bLimitMassAndSize == true )
    {
        if ( CBasePlayer::CanPickupObject( pObject, 35, 128 ) == false )
            return;
    }

    // Can't be picked up if NPCs are on me
    if ( pObject->HasNPCsOnIt() )
        return;

    PlayerPickupObject( this, pObject );
#endif
}

bool CExperiment_Player::ValidatePlayerModel( const char *pModel )
{
    int iModels = ARRAYSIZE( g_ppszRandomCitizenModels );
    int i;

    for ( i = 0; i < iModels; ++i )
    {
        if ( !Q_stricmp( g_ppszRandomCitizenModels[i], pModel ) )
        {
            return true;
        }
    }

    iModels = ARRAYSIZE( g_ppszRandomCombineModels );

    for ( i = 0; i < iModels; ++i )
    {
        if ( !Q_stricmp( g_ppszRandomCombineModels[i], pModel ) )
        {
            return true;
        }
    }

    return false;
}

void CExperiment_Player::SetPlayerTeamModel( void )
{
    const char *szModelName = NULL;
    szModelName = engine->GetClientConVarValue( engine->IndexOfEdict( edict() ),
                                                "cl_playermodel" );

    int modelIndex = modelinfo->GetModelIndex( szModelName );

    if ( modelIndex == -1 || ValidatePlayerModel( szModelName ) == false )
    {
        szModelName = g_ppszDefaultModel;
        m_iModelType = TEAM_COMBINE;

        char szReturnString[512];

        Q_snprintf( szReturnString, sizeof( szReturnString ), "cl_playermodel %s\n", szModelName );
        engine->ClientCommand( edict(), szReturnString );
    }

    if ( GetTeamNumber() == TEAM_COMBINE )
    {
        if ( Q_stristr( szModelName, "models/human" ) )
        {
            int nHeads = ARRAYSIZE( g_ppszRandomCombineModels );

            g_iLastCombineModel = ( g_iLastCombineModel + 1 ) % nHeads;
            szModelName = g_ppszRandomCombineModels[g_iLastCombineModel];
        }

        m_iModelType = TEAM_COMBINE;
    }
    else if ( GetTeamNumber() == TEAM_REBELS )
    {
        if ( !Q_stristr( szModelName, "models/human" ) )
        {
            int nHeads = ARRAYSIZE( g_ppszRandomCitizenModels );

            g_iLastCitizenModel = ( g_iLastCitizenModel + 1 ) % nHeads;
            szModelName = g_ppszRandomCitizenModels[g_iLastCitizenModel];
        }

        m_iModelType = TEAM_REBELS;
    }

    SetModel( szModelName );
    SetupPlayerSoundsByModel( szModelName );

    m_flNextModelChangeTime = gpGlobals->curtime + MODEL_CHANGE_INTERVAL;
}

void CExperiment_Player::SetPlayerModel( void )
{
    const char *szModelName = NULL;
    const char *pszCurrentModelName = modelinfo->GetModelName( GetModel() );

    szModelName = engine->GetClientConVarValue( engine->IndexOfEdict( edict() ),
                                                "cl_playermodel" );

    if ( ValidatePlayerModel( szModelName ) == false )
    {
        char szReturnString[512];

        if ( ValidatePlayerModel( pszCurrentModelName ) == false )
        {
            pszCurrentModelName = g_ppszDefaultModel;
        }

        Q_snprintf( szReturnString, sizeof( szReturnString ), "cl_playermodel %s\n", pszCurrentModelName );
        engine->ClientCommand( edict(), szReturnString );

        szModelName = pszCurrentModelName;
    }

    if ( GetTeamNumber() == TEAM_COMBINE )
    {
        int nHeads = ARRAYSIZE( g_ppszRandomCombineModels );

        g_iLastCombineModel = ( g_iLastCombineModel + 1 ) % nHeads;
        szModelName = g_ppszRandomCombineModels[g_iLastCombineModel];

        m_iModelType = TEAM_COMBINE;
    }
    else if ( GetTeamNumber() == TEAM_REBELS )
    {
        int nHeads = ARRAYSIZE( g_ppszRandomCitizenModels );

        g_iLastCitizenModel = ( g_iLastCitizenModel + 1 ) % nHeads;
        szModelName = g_ppszRandomCitizenModels[g_iLastCitizenModel];

        m_iModelType = TEAM_REBELS;
    }
    else
    {
        if ( Q_strlen( szModelName ) == 0 )
        {
            szModelName = g_ppszRandomCitizenModels[0];
        }

        if ( Q_stristr( szModelName, "models/human" ) )
        {
            m_iModelType = TEAM_REBELS;
        }
        else
        {
            m_iModelType = TEAM_COMBINE;
        }
    }

    int modelIndex = modelinfo->GetModelIndex( szModelName );

    if ( modelIndex == -1 )
    {
        szModelName = g_ppszDefaultModel;
        m_iModelType = TEAM_COMBINE;

        char szReturnString[512];

        Q_snprintf( szReturnString, sizeof( szReturnString ), "cl_playermodel %s\n", szModelName );
        engine->ClientCommand( edict(), szReturnString );
    }

    SetModel( szModelName );
    SetupPlayerSoundsByModel( szModelName );

    m_flNextModelChangeTime = gpGlobals->curtime + MODEL_CHANGE_INTERVAL;
}

void CExperiment_Player::SetupPlayerSoundsByModel( const char *pModelName )
{
    if ( Q_stristr( pModelName, "models/human" ) )
    {
        m_iPlayerSoundType = ( int )PLAYER_SOUNDS_CITIZEN;
    }
    else if ( Q_stristr( pModelName, "police" ) )
    {
        m_iPlayerSoundType = ( int )PLAYER_SOUNDS_METROPOLICE;
    }
    else if ( Q_stristr( pModelName, "combine" ) )
    {
        m_iPlayerSoundType = ( int )PLAYER_SOUNDS_COMBINESOLDIER;
    }
}

bool CExperiment_Player::Weapon_Switch( CBaseCombatWeapon *pWeapon,
                                        int viewmodelindex )
{
    bool bRet = BaseClass::Weapon_Switch( pWeapon, viewmodelindex );

    return bRet;
}

void CExperiment_Player::PreThink( void )
{
    BaseClass::PreThink();
    State_PreThink();

    // Reset bullet force accumulator, only lasts one frame
    m_vecTotalBulletForce = vec3_origin;
}

void CExperiment_Player::PostThink( void )
{
    BaseClass::PostThink();

    if ( GetFlags() & FL_DUCKING )
    {
        SetCollisionBounds( VEC_CROUCH_TRACE_MIN, VEC_CROUCH_TRACE_MAX );
    }

    QAngle angles = GetLocalAngles();
    angles[PITCH] = 0;
    SetLocalAngles( angles );

    // Store the eye angles pitch so the client can compute its animation state
    // correctly.
    m_angEyeAngles = EyeAngles();
    m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );

    if ( IsAlive() && m_cycleLatchTimer.IsElapsed() )
    {
        m_cycleLatchTimer.Start( CYCLELATCH_UPDATE_INTERVAL );
        // Compress the cycle into 4 bits. Can represent 0.0625 in steps which is enough.
        m_cycleLatch.GetForModify() = 16 * GetCycle();
    }
}

void CExperiment_Player::PlayerDeathThink()
{
#ifdef LUA_SDK
    LUA_CALL_HOOK_BEGIN( "PlayerDeathThink" );
    CExperiment_Player::PushLuaInstanceSafe( L, this );
    LUA_CALL_HOOK_END( 1, 0 );
#endif

    if ( !IsObserver() )
    {
        BaseClass::PlayerDeathThink();
    }
}

void CExperiment_Player::FireBullets( const FireBulletsInfo_t &info )
{
    // Move other players back to history positions based on local player's lag
    lagcompensation->StartLagCompensation( this, this->GetCurrentCommand() );

    FireBulletsInfo_t modinfo = info;

    CWeaponExperimentBase *pWeapon =
        dynamic_cast< CWeaponExperimentBase * >( GetActiveWeapon() );

    if ( pWeapon )
    {
        modinfo.m_iPlayerDamage = modinfo.m_flDamage =
            pWeapon->GetExperimentWpnData().m_iPlayerDamage;
    }

    NoteWeaponFired();

    BaseClass::FireBullets( modinfo );

    // Move other players back to history positions based on local player's lag
    lagcompensation->FinishLagCompensation( this );
}

void CExperiment_Player::NoteWeaponFired( void )
{
    Assert( m_pCurrentCommand );
    if ( m_pCurrentCommand )
    {
        m_iLastWeaponFireUsercmd = m_pCurrentCommand->command_number;
    }
}

extern ConVar sv_maxunlag;

bool CExperiment_Player::WantsLagCompensationOnEntity(
    const CBasePlayer *pPlayer,
    const CUserCmd *pCmd,
    const CBitVec< MAX_EDICTS > *pEntityTransmitBits ) const
{
    // No need to lag compensate at all if we're not attacking in this command
    // and we haven't attacked recently.
    if ( !( pCmd->buttons & IN_ATTACK ) &&
         ( pCmd->command_number - m_iLastWeaponFireUsercmd > 5 ) )
        return false;

    // If this entity hasn't been transmitted to us and acked, then don't bother
    // lag compensating it.
    if ( pEntityTransmitBits && !pEntityTransmitBits->Get( pPlayer->entindex() ) )
        return false;

    const Vector &vMyOrigin = GetAbsOrigin();
    const Vector &vHisOrigin = pPlayer->GetAbsOrigin();

    // get max distance player could have moved within max lag compensation
    // time, multiply by 1.5 to to avoid "dead zones"  (sqrt(2) would be the
    // exact value)
    float maxDistance = 1.5 * pPlayer->GetMaxSpeed() * sv_maxunlag.GetFloat();

    // If the player is within this distance, lag compensate them in case
    // they're running past us.
    if ( vHisOrigin.DistTo( vMyOrigin ) < maxDistance )
        return true;

    // If their origin is not within a 45 degree cone in front of us, no need to
    // lag compensate.
    Vector vForward;
    AngleVectors( pCmd->viewangles, &vForward );

    Vector vDiff = vHisOrigin - vMyOrigin;
    VectorNormalize( vDiff );

    float flCosAngle = 0.707107f;  // 45 degree angle
    if ( vForward.Dot( vDiff ) < flCosAngle )
        return false;

    return true;
}

Activity CExperiment_Player::TranslateTeamActivity( Activity ActToTranslate )
{
    if ( m_iModelType == TEAM_COMBINE )
        return ActToTranslate;

    if ( ActToTranslate == ACT_RUN )
        return ACT_RUN_AIM_AGITATED;

    if ( ActToTranslate == ACT_IDLE )
        return ACT_IDLE_AIM_AGITATED;

    if ( ActToTranslate == ACT_WALK )
        return ACT_WALK_AIM_AGITATED;

    return ActToTranslate;
}

extern int gEvilImpulse101;
//-----------------------------------------------------------------------------
// Purpose: Player reacts to bumping a weapon.
// Input  : pWeapon - the weapon that the player bumped into.
// Output : Returns true if player picked up the weapon
//-----------------------------------------------------------------------------
bool CExperiment_Player::BumpWeapon( CBaseCombatWeapon *pWeapon )
{
    CBaseCombatCharacter *pOwner = pWeapon->GetOwner();

    // Can I have this weapon type?
    if ( !IsAllowedToPickupWeapons() )
        return false;

    if ( pOwner || !Weapon_CanUse( pWeapon ) ||
         !g_pGameRules->CanHavePlayerItem( this, pWeapon ) )
    {
        if ( gEvilImpulse101 )
        {
            UTIL_Remove( pWeapon );
        }
        return false;
    }

    // Don't let the player fetch weapons through walls (use MASK_SOLID so that
    // you can't pickup through windows)
    if ( !pWeapon->FVisible( this, MASK_SOLID ) && !( GetFlags() & FL_NOTARGET ) )
    {
        return false;
    }

    bool bOwnsWeaponAlready =
        !!Weapon_OwnsThisType( pWeapon->GetClassname(), pWeapon->GetSubType() );

    if ( bOwnsWeaponAlready == true )
    {
        // If we have room for the ammo, then "take" the weapon too.
        if ( Weapon_EquipAmmoOnly( pWeapon ) )
        {
            pWeapon->CheckRespawn();

            UTIL_Remove( pWeapon );
            return true;
        }
        else
        {
            return false;
        }
    }

    pWeapon->CheckRespawn();
    Weapon_Equip( pWeapon );

    return true;
}

void CExperiment_Player::ChangeTeam( int iTeam )
{
    // Experiment: Disabled since we just want this to change the team number
    // bool bKill = false;

    // if ( ExperimentRules()->IsTeamplay() != true && iTeam != TEAM_SPECTATOR )
    //{
    //     // don't let them try to join combine or rebels during deathmatch.
    //     iTeam = TEAM_UNASSIGNED;
    // }

    // if ( ExperimentRules()->IsTeamplay() == true )
    //{
    //     if ( iTeam != GetTeamNumber() && GetTeamNumber() != TEAM_UNASSIGNED )
    //     {
    //         bKill = true;
    //     }
    // }

    BaseClass::ChangeTeam( iTeam );

    // m_flNextTeamChangeTime = gpGlobals->curtime + TEAM_CHANGE_INTERVAL;

    // if ( ExperimentRules()->IsTeamplay() == true )
    //{
    //     SetPlayerTeamModel();
    // }
    // else
    //{
    SetPlayerModel();
    //}

    if ( iTeam == TEAM_SPECTATOR )
    {
        RemoveAllItems( true );

        State_Transition( STATE_OBSERVER_MODE );
    }

    // if ( bKill == true )
    //{
    //     CommitSuicide();
    // }
}

bool CExperiment_Player::HandleCommand_JoinTeam( int team )
{
    if ( !GetGlobalTeam( team ) || team == 0 )
    {
        Warning( "HandleCommand_JoinTeam( %d ) - invalid team index.\n", team );
        return false;
    }

    if ( team == TEAM_SPECTATOR )
    {
        // Prevent this is the cvar is set
        if ( !mp_allowspectators.GetInt() )
        {
            ClientPrint( this, HUD_PRINTCENTER, "#Cannot_Be_Spectator" );
            return false;
        }

        if ( GetTeamNumber() != TEAM_UNASSIGNED && !IsDead() )
        {
            m_fNextSuicideTime =
                gpGlobals->curtime;  // allow the suicide to work

            CommitSuicide();

            // add 1 to frags to balance out the 1 subtracted for killing
            // yourself
            IncrementFragCount( 1 );
        }

        ChangeTeam( TEAM_SPECTATOR );

        return true;
    }
    else
    {
        StopObserverMode();
        State_Transition( STATE_ACTIVE );
    }

    // Switch their actual team...
    ChangeTeam( team );

    return true;
}

bool CExperiment_Player::ClientCommand( const CCommand &args )
{
    if ( FStrEq( args[0], "spectate" ) )
    {
        if ( ShouldRunRateLimitedCommand( args ) )
        {
            // instantly join spectators
            HandleCommand_JoinTeam( TEAM_SPECTATOR );
        }
        return true;
    }
    else if ( FStrEq( args[0], "jointeam" ) )
    {
        if ( args.ArgC() < 2 )
        {
            Warning( "Player sent bad jointeam syntax\n" );
        }

        if ( ShouldRunRateLimitedCommand( args ) )
        {
            int iTeam = atoi( args[1] );
            HandleCommand_JoinTeam( iTeam );
        }
        return true;
    }
    else if ( FStrEq( args[0], "joingame" ) )
    {
        return true;
    }

    return BaseClass::ClientCommand( args );
}

void CExperiment_Player::CheatImpulseCommands( int iImpulse )
{
#ifdef LUA_SDK
    LUA_CALL_HOOK_BEGIN( "CheatImpulseCommands" );
    CExperiment_Player::PushLuaInstanceSafe( L, this );
    lua_pushinteger( L, iImpulse );
    LUA_CALL_HOOK_END( 2, 1 );

    LUA_RETURN_NONE_IF_FALSE();
#endif

    switch ( iImpulse )
    {
        case 101:
        {
            if ( sv_cheats->GetBool() )
            {
                GiveAllItems();
            }
        }
        break;

        default:
            BaseClass::CheatImpulseCommands( iImpulse );
    }
}

bool CExperiment_Player::ShouldRunRateLimitedCommand( const CCommand &args )
{
    int i = m_RateLimitLastCommandTimes.Find( args[0] );
    if ( i == m_RateLimitLastCommandTimes.InvalidIndex() )
    {
        m_RateLimitLastCommandTimes.Insert( args[0], gpGlobals->curtime );
        return true;
    }
    else if ( ( gpGlobals->curtime - m_RateLimitLastCommandTimes[i] ) <
              Experiment_COMMAND_MAX_RATE )
    {
        // Too fast.
        return false;
    }
    else
    {
        m_RateLimitLastCommandTimes[i] = gpGlobals->curtime;
        return true;
    }
}

void CExperiment_Player::CreateViewModel( int index /*=0*/ )
{
    Assert( index >= 0 && index < MAX_VIEWMODELS );

    if ( GetViewModel( index ) )
        return;

    CPredictedViewModel *vm =
        ( CPredictedViewModel * )CreateEntityByName( "predicted_viewmodel" );
    if ( vm )
    {
        vm->SetAbsOrigin( GetAbsOrigin() );
        vm->SetOwner( this );
        vm->SetIndex( index );
        DispatchSpawn( vm );
        vm->FollowEntity( this, false );
        m_hViewModel.Set( index, vm );
    }
}

bool CExperiment_Player::BecomeRagdollOnClient( const Vector &force )
{
    return true;
}

// --------------------------------------------------------------------------------
// Ragdoll entities.
// --------------------------------------------------------------------------------

;  // clang-format off

LINK_ENTITY_TO_CLASS( experiment_ragdoll, CExperimentRagdoll );

IMPLEMENT_SERVERCLASS_ST_NOBASE( CExperimentRagdoll, DT_ExperimentRagdoll )
    SendPropVector( SENDINFO( m_vecRagdollOrigin ), -1, SPROP_COORD ),
    SendPropEHandle( SENDINFO( m_hPlayer ) ),
    SendPropModelIndex( SENDINFO( m_nModelIndex ) ),
    SendPropInt( SENDINFO( m_nForceBone ), 8, 0 ),
    SendPropVector( SENDINFO( m_vecForce ), -1, SPROP_NOSCALE ),
    SendPropVector( SENDINFO( m_vecRagdollVelocity ) )
END_SEND_TABLE()

;  // clang-format on

void CExperiment_Player::CreateRagdollEntity( void )
{
    if ( m_hRagdoll )
    {
        UTIL_RemoveImmediate( m_hRagdoll );
        m_hRagdoll = NULL;
    }

    // If we already have a ragdoll, don't make another one.
    CExperimentRagdoll *pRagdoll = dynamic_cast< CExperimentRagdoll * >( m_hRagdoll.Get() );

    if ( !pRagdoll )
    {
        // create a new one
        pRagdoll =
            dynamic_cast< CExperimentRagdoll * >( CreateEntityByName( "experiment_ragdoll" ) );
    }

    if ( pRagdoll )
    {
        pRagdoll->m_hPlayer = this;
        pRagdoll->m_vecRagdollOrigin = GetAbsOrigin();
        pRagdoll->m_vecRagdollVelocity = GetAbsVelocity();
        pRagdoll->m_nModelIndex = m_nModelIndex;
        pRagdoll->m_nForceBone = m_nForceBone;
        pRagdoll->m_vecForce = m_vecTotalBulletForce;
        pRagdoll->SetAbsOrigin( GetAbsOrigin() );
    }

    // ragdolls will be removed on round restart automatically
    m_hRagdoll = pRagdoll;

    if ( L )
    {
        LUA_CALL_HOOK_BEGIN( "CreateEntityRagdoll" );
        CExperiment_Player::PushLuaInstanceSafe( L, this );
        CBaseEntity::PushLuaInstanceSafe( L, pRagdoll );
        LUA_CALL_HOOK_END( 2, 0 );
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CExperiment_Player::FlashlightIsOn( void )
{
    return IsEffectActive( EF_DIMLIGHT );
}

extern ConVar flashlight;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CExperiment_Player::FlashlightTurnOn( void )
{
    if ( flashlight.GetInt() > 0 && IsAlive() )
    {
        AddEffects( EF_DIMLIGHT );
        EmitSound( "HL2Player.FlashlightOn" );
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CExperiment_Player::FlashlightTurnOff( void )
{
    RemoveEffects( EF_DIMLIGHT );

    if ( IsAlive() )
    {
        EmitSound( "HL2Player.FlashlightOff" );
    }
}

void CExperiment_Player::Weapon_Drop( CBaseCombatWeapon *pWeapon,
                                      const Vector *pvecTarget,
                                      const Vector *pVelocity )
{
    // Drop a grenade if it's primed.
    if ( GetActiveWeapon() )
    {
        CBaseCombatWeapon *pGrenade = Weapon_OwnsThisType( "weapon_frag" );

        if ( GetActiveWeapon() == pGrenade )
        {
            if ( ( m_nButtons & IN_ATTACK ) || ( m_nButtons & IN_ATTACK2 ) )
            {
                DropPrimedFragGrenade( this, pGrenade );
                return;
            }
        }
    }

    BaseClass::Weapon_Drop( pWeapon, pvecTarget, pVelocity );
}

void CExperiment_Player::DetonateTripmines( void )
{
    CBaseEntity *pEntity = NULL;

    while ( ( pEntity = gEntList.FindEntityByClassname( pEntity, "npc_satchel" ) ) !=
            NULL )
    {
        CSatchelCharge *pSatchel = dynamic_cast< CSatchelCharge * >( pEntity );
        if ( pSatchel->m_bIsLive && pSatchel->GetThrower() == this )
        {
            g_EventQueue.AddEvent( pSatchel, "Explode", 0.20, this, this );
        }
    }

    // Play sound for pressing the detonator
    EmitSound( "Weapon_SLAM.SatchelDetonate" );
}

void CExperiment_Player::Event_Killed( const CTakeDamageInfo &info )
{
    // update damage info with our accumulated physics force
    CTakeDamageInfo subinfo = info;
    subinfo.SetDamageForce( m_vecTotalBulletForce );

    // Note: since we're dead, it won't draw us on the client, but we don't set
    // EF_NODRAW because we still want to transmit to the clients in our PVS.
    CreateRagdollEntity();

    DetonateTripmines();

    BaseClass::Event_Killed( subinfo );

    if ( info.GetDamageType() & DMG_DISSOLVE )
    {
        if ( m_hRagdoll )
        {
            m_hRagdoll->GetBaseAnimating()->Dissolve(
                NULL, gpGlobals->curtime, false, ENTITY_DISSOLVE_NORMAL );
        }
    }

    CBaseEntity *pAttacker = info.GetAttacker();

    if ( pAttacker )
    {
        int iScoreToAdd = 1;

        if ( pAttacker == this )
        {
            iScoreToAdd = -1;
        }

        GetGlobalTeam( pAttacker->GetTeamNumber() )->AddScore( iScoreToAdd );
    }

    FlashlightTurnOff();

    m_lifeState = LIFE_DEAD;

    RemoveEffects( EF_NODRAW );  // still draw player body
    StopZooming();
}

int CExperiment_Player::OnTakeDamage( const CTakeDamageInfo &inputInfo )
{
    // return here if the player is in the respawn grace period vs. slams.
    if ( gpGlobals->curtime < m_flSlamProtectTime &&
         ( inputInfo.GetDamageType() == DMG_BLAST ) )
        return 0;

    m_vecTotalBulletForce += inputInfo.GetDamageForce();

    gamestats->Event_PlayerDamage( this, inputInfo );

    return BaseClass::OnTakeDamage( inputInfo );
}

void CExperiment_Player::DeathSound( const CTakeDamageInfo &info )
{
#ifdef LUA_SDK
    CTakeDamageInfo lInfo = info;

    LUA_CALL_HOOK_BEGIN( "PlayerDeathSound" );
    CExperiment_Player::PushLuaInstanceSafe( L, this );
    lua_pushdamageinfo( L, lInfo );
    LUA_CALL_HOOK_END( 2, 1 );

    LUA_RETURN_NONE_IF_TRUE();
#endif

    if ( m_hRagdoll && m_hRagdoll->GetBaseAnimating()->IsDissolving() )
        return;

    char szStepSound[128];

    Q_snprintf( szStepSound, sizeof( szStepSound ), "%s.Die", GetPlayerModelSoundPrefix() );

    const char *pModelName = STRING( GetModelName() );

    CSoundParameters params;
    if ( GetParametersForSound( szStepSound, params, pModelName ) == false )
        return;

    Vector vecOrigin = GetAbsOrigin();

    CRecipientFilter filter;
    filter.AddRecipientsByPAS( vecOrigin );

    EmitSound_t ep;
    ep.m_nChannel = params.channel;
    ep.m_pSoundName = params.soundname;
    ep.m_flVolume = params.volume;
    ep.m_SoundLevel = params.soundlevel;
    ep.m_nFlags = 0;
    ep.m_nPitch = params.pitch;
    ep.m_pOrigin = &vecOrigin;

    EmitSound( filter, entindex(), ep );
}

CBaseEntity *CExperiment_Player::EntSelectSpawnPoint( void )
{
#ifdef LUA_SDK
    LUA_CALL_HOOK_BEGIN( "PlayerEntSelectSpawnPoint" );
    CExperiment_Player::PushLuaInstanceSafe( L, this );
    LUA_CALL_HOOK_END( 1, 1 );

    LUA_RETURN_ENTITY();

    // Experiment; Note that HL2SB has a bunch more code below that we
    // didn't copy over. It handled choosing a spawn point based on the
    // spawn point entities from CS:S, DoD:S, etc.
#endif

    CBaseEntity *pSpot = NULL;
    CBaseEntity *pLastSpawnPoint = g_pLastSpawn;
    edict_t *player = edict();
    const char *pSpawnpointName = "info_player_deathmatch";

    if ( ExperimentRules()->IsTeamplay() == true )
    {
        if ( GetTeamNumber() == TEAM_COMBINE )
        {
            pSpawnpointName = "info_player_combine";
            pLastSpawnPoint = g_pLastCombineSpawn;
        }
        else if ( GetTeamNumber() == TEAM_REBELS )
        {
            pSpawnpointName = "info_player_rebel";
            pLastSpawnPoint = g_pLastRebelSpawn;
        }

        if ( gEntList.FindEntityByClassname( NULL, pSpawnpointName ) == NULL )
        {
            pSpawnpointName = "info_player_deathmatch";
            pLastSpawnPoint = g_pLastSpawn;
        }
    }

    pSpot = pLastSpawnPoint;
    // Randomize the start spot
    for ( int i = random->RandomInt( 1, 5 ); i > 0; i-- )
        pSpot = gEntList.FindEntityByClassname( pSpot, pSpawnpointName );
    if ( !pSpot )  // skip over the null point
        pSpot = gEntList.FindEntityByClassname( pSpot, pSpawnpointName );

    CBaseEntity *pFirstSpot = pSpot;

    do
    {
        if ( pSpot )
        {
            // check if pSpot is valid
            if ( g_pGameRules->IsSpawnPointValid( pSpot, this ) )
            {
                if ( pSpot->GetLocalOrigin() == vec3_origin )
                {
                    pSpot =
                        gEntList.FindEntityByClassname( pSpot, pSpawnpointName );
                    continue;
                }

                // if so, go to pSpot
                goto ReturnSpot;
            }
        }
        // increment pSpot
        pSpot = gEntList.FindEntityByClassname( pSpot, pSpawnpointName );
    } while ( pSpot != pFirstSpot );  // loop if we're not back to the start

    // we haven't found a place to spawn yet,  so kill any guy at the first
    // spawn point and spawn there
    if ( pSpot )
    {
        CBaseEntity *ent = NULL;
        for ( CEntitySphereQuery sphere( pSpot->GetAbsOrigin(), 128 );
              ( ent = sphere.GetCurrentEntity() ) != NULL;
              sphere.NextEntity() )
        {
            // if ent is a client, kill em (unless they are ourselves)
            if ( ent->IsPlayer() && !( ent->edict() == player ) )
                ent->TakeDamage( CTakeDamageInfo(
                    GetContainingEntity( INDEXENT( 0 ) ),
                    GetContainingEntity( INDEXENT( 0 ) ),
                    300,
                    DMG_GENERIC ) );
        }
        goto ReturnSpot;
    }

    if ( !pSpot )
    {
        pSpot = gEntList.FindEntityByClassname( pSpot, "info_player_start" );

        if ( pSpot )
            goto ReturnSpot;
    }

ReturnSpot:

    if ( ExperimentRules()->IsTeamplay() == true )
    {
        if ( GetTeamNumber() == TEAM_COMBINE )
        {
            g_pLastCombineSpawn = pSpot;
        }
        else if ( GetTeamNumber() == TEAM_REBELS )
        {
            g_pLastRebelSpawn = pSpot;
        }
    }

    g_pLastSpawn = pSpot;

    m_flSlamProtectTime = gpGlobals->curtime + 0.5;

    return pSpot;
}

CON_COMMAND( timeleft, "prints the time remaining in the match" )
{
    CExperiment_Player *pPlayer = ToExperimentPlayer( UTIL_GetCommandClient() );

    int iTimeRemaining = ( int )ExperimentRules()->GetMapRemainingTime();

    if ( iTimeRemaining == 0 )
    {
        if ( pPlayer )
        {
            ClientPrint( pPlayer, HUD_PRINTTALK, "This game has no timelimit." );
        }
        else
        {
            Msg( "* No Time Limit *\n" );
        }
    }
    else
    {
        int iMinutes, iSeconds;
        iMinutes = iTimeRemaining / 60;
        iSeconds = iTimeRemaining % 60;

        char minutes[8];
        char seconds[8];

        Q_snprintf( minutes, sizeof( minutes ), "%d", iMinutes );
        Q_snprintf( seconds, sizeof( seconds ), "%2.2d", iSeconds );

        if ( pPlayer )
        {
            ClientPrint( pPlayer, HUD_PRINTTALK, "Time left in map: %s1:%s2", minutes, seconds );
        }
        else
        {
            Msg( "Time Remaining:  %s:%s\n", minutes, seconds );
        }
    }
}

void CExperiment_Player::Reset()
{
    ResetDeathCount();
    ResetFragCount();
}

bool CExperiment_Player::IsReady()
{
    return m_bReady;
}

void CExperiment_Player::SetReady( bool bReady )
{
    m_bReady = bReady;
}

void CExperiment_Player::CheckChatText( char *p, int bufsize )
{
    // Look for escape sequences and replace

    char *buf = new char[bufsize];
    int pos = 0;

    // Parse say text for escape sequences
    for ( char *pSrc = p; pSrc != NULL && *pSrc != 0 && pos < bufsize - 1;
          pSrc++ )
    {
        // copy each char across
        buf[pos] = *pSrc;
        pos++;
    }

    buf[pos] = '\0';

    // copy buf back into p
    Q_strncpy( p, buf, bufsize );

    delete[] buf;

    const char *pReadyCheck = p;

    ExperimentRules()->CheckChatForReadySignal( this, pReadyCheck );
}

void CExperiment_Player::State_Transition( ExperimentPlayerState newState )
{
    State_Leave();
    State_Enter( newState );
}

void CExperiment_Player::State_Enter( ExperimentPlayerState newState )
{
    m_iPlayerState = newState;
    m_pCurStateInfo = State_LookupInfo( newState );

    // Initialize the new state.
    if ( m_pCurStateInfo && m_pCurStateInfo->pfnEnterState )
        ( this->*m_pCurStateInfo->pfnEnterState )();
}

void CExperiment_Player::State_Leave()
{
    if ( m_pCurStateInfo && m_pCurStateInfo->pfnLeaveState )
    {
        ( this->*m_pCurStateInfo->pfnLeaveState )();
    }
}

void CExperiment_Player::State_PreThink()
{
    if ( m_pCurStateInfo && m_pCurStateInfo->pfnPreThink )
    {
        ( this->*m_pCurStateInfo->pfnPreThink )();
    }
}

CExperimentPlayerStateInfo *CExperiment_Player::State_LookupInfo( ExperimentPlayerState state )
{
    // This table MUST match the
    static CExperimentPlayerStateInfo playerStateInfos[] = {
        { STATE_ACTIVE, "STATE_ACTIVE", &CExperiment_Player::State_Enter_ACTIVE, NULL, &CExperiment_Player::State_PreThink_ACTIVE },
        { STATE_OBSERVER_MODE, "STATE_OBSERVER_MODE", &CExperiment_Player::State_Enter_OBSERVER_MODE, NULL, &CExperiment_Player::State_PreThink_OBSERVER_MODE } };

    for ( int i = 0; i < ARRAYSIZE( playerStateInfos ); i++ )
    {
        if ( playerStateInfos[i].m_iPlayerState == state )
            return &playerStateInfos[i];
    }

    return NULL;
}

bool CExperiment_Player::StartObserverMode( int mode )
{
    // we only want to go into observer mode if the player asked to, not on a
    // death timeout
    if ( m_bEnterObserver == true )
    {
        VPhysicsDestroyObject();
        return BaseClass::StartObserverMode( mode );
    }
    return false;
}

void CExperiment_Player::StopObserverMode()
{
    m_bEnterObserver = false;
    BaseClass::StopObserverMode();
}

void CExperiment_Player::State_Enter_OBSERVER_MODE()
{
    int observerMode = m_iObserverLastMode;
    if ( IsNetClient() )
    {
        const char *pIdealMode = engine->GetClientConVarValue(
            engine->IndexOfEdict( edict() ), "cl_spec_mode" );
        if ( pIdealMode )
        {
            observerMode = atoi( pIdealMode );
            if ( observerMode <= OBS_MODE_FIXED ||
                 observerMode > OBS_MODE_ROAMING )
            {
                observerMode = m_iObserverLastMode;
            }
        }
    }
    m_bEnterObserver = true;
    StartObserverMode( observerMode );
}

void CExperiment_Player::State_PreThink_OBSERVER_MODE()
{
    // Make sure nobody has changed any of our state.
    //	Assert( GetMoveType() == MOVETYPE_FLY );
    Assert( m_takedamage == DAMAGE_NO );
    Assert( IsSolidFlagSet( FSOLID_NOT_SOLID ) );
    //	Assert( IsEffectActive( EF_NODRAW ) );

    // Must be dead.
    Assert( m_lifeState == LIFE_DEAD );
    Assert( pl.deadflag );
}

void CExperiment_Player::State_Enter_ACTIVE()
{
    SetMoveType( MOVETYPE_WALK );

    // md 8/15/07 - They'll get set back to solid when they actually respawn. If
    // we set them solid now and mp_forcerespawn is false, then they'll be
    // spectating but blocking live players from moving. RemoveSolidFlags(
    // FSOLID_NOT_SOLID );

    m_Local.m_iHideHUD = 0;
}

void CExperiment_Player::State_PreThink_ACTIVE()
{
    // we don't really need to do anything here.
    // This state_prethink structure came over from CS:S and was doing an assert
    // check that fails the way hl2dm handles death
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CExperiment_Player::CanHearAndReadChatFrom( CBasePlayer *pPlayer )
{
    // can always hear the console unless we're ignoring all chat
    if ( !pPlayer )
        return false;

    return true;
}

//-----------------------------------------------------------------------------
// Purpose: multiplayer does not do autoaiming.
//-----------------------------------------------------------------------------
Vector CExperiment_Player::GetAutoaimVector( float flScale )
{
    // No Autoaim
    Vector forward;
    AngleVectors( EyeAngles() + m_Local.m_vecPunchAngle, &forward );
    return forward;
}

//-----------------------------------------------------------------------------
// Purpose: Do nothing multiplayer_animstate takes care of animation.
// Input  : playerAnim -
//-----------------------------------------------------------------------------
void CExperiment_Player::SetAnimation( PLAYER_ANIM playerAnim )
{
    // Experiment; copied from NEO: https://github.com/AdamTadeusz/neo/blob/master/mp/src/game/server/neo/neo_player.cpp#L1273-L1284
    PlayerAnimEvent_t animEvent;

    if ( !PlayerAnimToPlayerAnimEvent( playerAnim, animEvent ) )
    {
        // DevWarning("SRV Tried to get unknown PLAYER_ANIM %d\n", playerAnim);
    }
    else
    {
        m_PlayerAnimState->DoAnimationEvent( animEvent );
    }
}

// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //
class CTEPlayerAnimEvent : public CBaseTempEntity
{
   public:
    DECLARE_CLASS( CTEPlayerAnimEvent, CBaseTempEntity );
    DECLARE_SERVERCLASS();

    CTEPlayerAnimEvent( const char *name )
        : CBaseTempEntity( name )
    {
    }

    CNetworkHandle( CBasePlayer, m_hPlayer );
    CNetworkVar( int, m_iEvent );
    CNetworkVar( int, m_nData );
};

IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEPlayerAnimEvent, DT_TEPlayerAnimEvent )
SendPropEHandle( SENDINFO( m_hPlayer ) ),
    SendPropInt( SENDINFO( m_iEvent ), Q_log2( PLAYERANIMEVENT_COUNT ) + 1, SPROP_UNSIGNED ),
    SendPropInt( SENDINFO( m_nData ), 32 )
        END_SEND_TABLE()

            static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

void TE_PlayerAnimEvent( CBasePlayer *pPlayer, PlayerAnimEvent_t event, int nData )
{
    CPVSFilter filter( ( const Vector & )pPlayer->EyePosition() );

    // Tony; use prediction rules.
    filter.UsePredictionRules();

    g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
    g_TEPlayerAnimEvent.m_iEvent = event;
    g_TEPlayerAnimEvent.m_nData = nData;
    g_TEPlayerAnimEvent.Create( filter, 0 );
}

void CExperiment_Player::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
    m_PlayerAnimState->DoAnimationEvent( event, nData );
    TE_PlayerAnimEvent( this, event, nData );  // Send to any clients who can see this guy.
}

bool CExperiment_Player::KeyDown( int buttonCode )
{
    return m_nButtons & buttonCode;
}

//-----------------------------------------------------------------------------
// Purpose: Override setup bones so that is uses the render angles from
//			the Experiment animation state to setup the hitboxes.
//-----------------------------------------------------------------------------
void CExperiment_Player::SetupBones( matrix3x4_t *pBoneToWorld, int boneMask )
{
    VPROF_BUDGET( "CExperiment_Player::SetupBones", VPROF_BUDGETGROUP_SERVER_ANIM );

    // Get the studio header.
    Assert( GetModelPtr() );
    CStudioHdr *pStudioHdr = GetModelPtr();
    if ( !pStudioHdr )
        return;

    Vector pos[MAXSTUDIOBONES];
    Quaternion q[MAXSTUDIOBONES];

    // Adjust hit boxes based on IK driven offset.
    Vector adjOrigin = GetAbsOrigin() + Vector( 0, 0, m_flEstIkOffset );

    // FIXME: pass this into Studio_BuildMatrices to skip transforms
    CBoneBitList boneComputed;
    if ( m_pIk )
    {
        m_iIKCounter++;
        m_pIk->Init( pStudioHdr, GetAbsAngles(), adjOrigin, gpGlobals->curtime, m_iIKCounter, boneMask );
        GetSkeleton( pStudioHdr, pos, q, boneMask );

        m_pIk->UpdateTargets( pos, q, pBoneToWorld, boneComputed );
        CalculateIKLocks( gpGlobals->curtime );
        m_pIk->SolveDependencies( pos, q, pBoneToWorld, boneComputed );
    }
    else
    {
        GetSkeleton( pStudioHdr, pos, q, boneMask );
    }

    CBaseAnimating *pParent = dynamic_cast< CBaseAnimating * >( GetMoveParent() );
    if ( pParent )
    {
        // We're doing bone merging, so do special stuff here.
        CBoneCache *pParentCache = pParent->GetBoneCache();
        if ( pParentCache )
        {
            BuildMatricesWithBoneMerge(
                pStudioHdr,
                m_PlayerAnimState->GetRenderAngles(),
                adjOrigin,
                pos,
                q,
                pBoneToWorld,
                pParent,
                pParentCache );

            return;
        }
    }

    Studio_BuildMatrices(
        pStudioHdr,
        m_PlayerAnimState->GetRenderAngles(),
        adjOrigin,
        pos,
        q,
        -1,
        GetModelScale(),  // Scaling
        pBoneToWorld,
        boneMask );
}
