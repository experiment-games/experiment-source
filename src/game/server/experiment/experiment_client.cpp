//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//
/*

===== tf_client.cpp ========================================================

  HL2 client/server game specific stuff

*/

#include "cbase.h"
#include "experiment_player.h"
#include "experiment_gamerules.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "entitylist.h"
#include "physics.h"
#include "game.h"
#include "player_resource.h"
#include "engine/IEngineSound.h"
#include "team.h"
#include "viewport_panel_names.h"
#include "util/networkmanager.h"

#include "tier0/vprof.h"
#include <experiment_bot_temp.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void Host_Say( edict_t *pEdict, bool teamonly );

ConVar sv_motd_unload_on_dismissal( "sv_motd_unload_on_dismissal", "0", 0, "If enabled, the MOTD contents will be unloaded when the player closes the MOTD." );

extern CBaseEntity *FindPickerEntityClass( CBasePlayer *pPlayer, char *classname );
extern bool g_fGameOver;

void FinishClientPutInServer( CExperiment_Player *pPlayer )
{
#ifdef EXPERIMENT_SOURCE
    // We don't call this until BotPutInServer has set the flag that it's a fake client
    if ( !IsPuttingBotInServer() )
    {
#endif
        pPlayer->InitialSpawn();
        pPlayer->Spawn();
#ifdef EXPERIMENT_SOURCE
    }
#endif

    char sName[128];
    Q_strncpy( sName, pPlayer->GetPlayerName(), sizeof( sName ) );

    // First parse the name and remove any %'s
    for ( char *pApersand = sName; pApersand != NULL && *pApersand != 0; pApersand++ )
    {
        // Replace it with a space
        if ( *pApersand == '%' )
            *pApersand = ' ';
    }

    // notify other clients of player joining the game
    UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Game_connected", sName[0] != 0 ? sName : "<unconnected>" );

    if ( ExperimentRules()->IsTeamplay() == true )
    {
        ClientPrint( pPlayer, HUD_PRINTTALK, "You are on team %s1\n", pPlayer->GetTeam()->GetName() );
    }

    // Experiment; We have no interest in the motd. Leaving it as a handy anchor to search for in the future.
#ifdef WITH_MOTD
    const ConVar *hostname = cvar->FindVar( "hostname" );
    const char *title = ( hostname ) ? hostname->GetString() : "MESSAGE OF THE DAY";

    KeyValues *data = new KeyValues( "data" );
    data->SetString( "title", title );  // info panel title
    data->SetString( "type", "1" );     // show userdata from stringtable entry
    data->SetString( "msg", "motd" );   // use this stringtable entry
    data->SetBool( "unload", sv_motd_unload_on_dismissal.GetBool() );

    pPlayer->ShowViewPortPanel( PANEL_INFO, true, data );

    data->deleteThis();
#endif
}

/*
===========
ClientPutInServer

called each time a player is spawned into the game
============
*/
void ClientPutInServer( edict_t *pEdict, const char *playername )
{
    CExperiment_Player *pPlayer = CExperiment_Player::CreatePlayer( "player", pEdict );
    pPlayer->SetPlayerName( playername );
}

// Note that this is also called for bots
void ClientActive( edict_t *pEdict, bool bLoadGame )
{
    // Can't load games in CS!
    Assert( !bLoadGame );

    CExperiment_Player *pPlayer = ToExperimentPlayer( CBaseEntity::Instance( pEdict ) );

    FinishClientPutInServer( pPlayer );
}

/*
===============
const char *GetGameDescription()

Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
===============
*/
const char *GetGameDescription()
{
    if ( g_pGameRules )  // this function may be called before the world has spawned, and the game rules initialized
        return g_pGameRules->GetGameDescription();
    else
        return "Experiment";
}

//-----------------------------------------------------------------------------
// Purpose: Given a player and optional name returns the entity of that
//			classname that the player is nearest facing
//
// Input  :
// Output :
//-----------------------------------------------------------------------------
CBaseEntity *FindEntity( edict_t *pEdict, char *classname )
{
    // If no name was given set bits based on the picked
    if ( FStrEq( classname, "" ) )
    {
        return ( FindPickerEntityClass( static_cast< CBasePlayer * >( GetContainingEntity( pEdict ) ), classname ) );
    }
    return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Precache game-specific models & sounds
//-----------------------------------------------------------------------------
void ClientGamePrecache( void )
{
    CBaseEntity::PrecacheModel( "models/player.mdl" );
    CBaseEntity::PrecacheModel( "models/gibs/agibs.mdl" );
    CBaseEntity::PrecacheModel( "models/weapons/v_hands.mdl" );

    CBaseEntity::PrecacheScriptSound( "HUDQuickInfo.LowAmmo" );
    CBaseEntity::PrecacheScriptSound( "HUDQuickInfo.LowHealth" );

    CBaseEntity::PrecacheScriptSound( "FX_AntlionImpact.ShellImpact" );
    CBaseEntity::PrecacheScriptSound( "Missile.ShotDown" );
    CBaseEntity::PrecacheScriptSound( "Bullets.DefaultNearmiss" );
    CBaseEntity::PrecacheScriptSound( "Bullets.GunshipNearmiss" );
    CBaseEntity::PrecacheScriptSound( "Bullets.StriderNearmiss" );

    CBaseEntity::PrecacheScriptSound( "Geiger.BeepHigh" );
    CBaseEntity::PrecacheScriptSound( "Geiger.BeepLow" );
}

// called by ClientKill and DeadThink
void respawn( CBaseEntity *pEdict, bool fCopyCorpse )
{
    CExperiment_Player *pPlayer = ToExperimentPlayer( pEdict );

    if ( pPlayer )
    {
        if ( gpGlobals->curtime > pPlayer->GetDeathTime() + DEATH_ANIMATION_TIME )
        {
            // respawn player
            pPlayer->Spawn();
        }
        else
        {
            pPlayer->SetNextThink( gpGlobals->curtime + 0.1f );
        }
    }
}

void GameStartFrame( void )
{
    VPROF( "GameStartFrame()" );
    
#ifdef LUA_SDK
    if ( L )
    {
        LUA_CALL_HOOK_BEGIN( "Think" );
        LUA_CALL_HOOK_END( 0, 0 );
    }
#endif

    if ( g_fGameOver )
        return;

    gpGlobals->teamplay = ( teamplay.GetInt() != 0 );

#if defined( DEBUG ) || defined( LUA_SDK )
    extern void Bot_RunAll();
    Bot_RunAll();
#endif
}

//=========================================================
// instantiate the proper game rules object
//=========================================================
void InstallGameRules()
{
    // vanilla deathmatch
    CreateGameRulesObject( "CExperimentRules" );
}
