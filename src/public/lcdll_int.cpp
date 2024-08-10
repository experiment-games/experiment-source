//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Interfaces between the client.dll and engine
//
//===========================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "mathlib/lvector.h"
#include <lbaseplayer_shared.h>
#include "gameinfostore.h"
#include <lconvar.h>
#include <lconcommand.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int engine_ActivateOccluder( lua_State *L )
{
    engine->ActivateOccluder( luaL_checknumber( L, 1 ), luaL_checkboolean( L, 2 ) );
    return 0;
}

static int engine_CheckKeyTrapping( lua_State *L )
{
    ButtonCode_t code;
    engine->CheckDoneKeyTrapping( code );
    lua_pushinteger( L, code );
    return 1;
}
static int engine_IsKeyTrapping( lua_State *L )
{
    ButtonCode_t code;
    lua_pushboolean( L, engine->CheckDoneKeyTrapping( code ) );
    return 1;
}

static int engine_StartKeyTrapMode( lua_State *L )
{
    engine->StartKeyTrapMode();
    return 0;
}

static int engine_CheckPoint( lua_State *L )
{
    engine->CheckPoint( luaL_checkstring( L, 1 ) );
    return 0;
}

static int engine_ClientCmd( lua_State *L )
{
    const char *pszCommandString = luaL_checkstring( L, 1 );

    if ( TryRunConsoleCommand( pszCommandString ) )
        return 0;

    engine->ClientCmd( luaL_checkstring( L, 1 ) );
    return 0;
}

static int engine_ClientCmd_Unrestricted( lua_State *L )
{
    engine->ClientCmd_Unrestricted( luaL_checkstring( L, 1 ) );
    return 0;
}

static int engine_Con_IsVisible( lua_State *L )
{
    lua_pushboolean( L, engine->Con_IsVisible() );
    return 1;
}

static int engine_CopyFrameBufferToMaterial( lua_State *L )
{
    lua_pushboolean( L, engine->CopyFrameBufferToMaterial( luaL_checkstring( L, 1 ) ) );
    return 1;
}

static int engine_CullBox( lua_State *L )
{
    lua_pushboolean( L, engine->CullBox( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int engine_DoesBoxTouchAreaFrustum( lua_State *L )
{
    lua_pushboolean( L, engine->DoesBoxTouchAreaFrustum( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ), luaL_checknumber( L, 3 ) ) );
    return 1;
}

static int engine_DrawPortals( lua_State *L )
{
    engine->DrawPortals();
    return 0;
}

static int engine_EngineStats_BeginFrame( lua_State *L )
{
    engine->EngineStats_BeginFrame();
    return 0;
}

static int engine_EngineStats_EndFrame( lua_State *L )
{
    engine->EngineStats_EndFrame();
    return 0;
}

static int engine_ExecuteClientCmd( lua_State *L )
{
    engine->ExecuteClientCmd( luaL_checkstring( L, 1 ) );
    return 0;
}

static int engine_FireEvents( lua_State *L )
{
    engine->FireEvents();
    return 0;
}

static int engine_GameLumpSize( lua_State *L )
{
    engine->GameLumpSize( luaL_checknumber( L, 1 ) );
    return 1;
}

static int engine_GameLumpVersion( lua_State *L )
{
    engine->GameLumpVersion( luaL_checknumber( L, 1 ) );
    return 1;
}

static int engine_GetClientConVarValue( lua_State *L )
{
    int iPlayer;

    if ( lua_toplayer( L, 1 ) )
        iPlayer = lua_toplayer( L, 1 )->entindex();
    else
        iPlayer = luaL_checkinteger( L, 1 );

    if ( iPlayer != ( *C_BasePlayer::GetLocalPlayer() ).entindex() )
        Warning( "engine.GetClientConVarValue: Cannot get convars for other players. Returning own convar value.\n" );

    ConVarRef var( luaL_checkstring( L, 2 ) );

    if ( var.IsValid() )
    {
        lua_pushstring( L, var.GetString() );
    }
    else
    {
        lua_pushstring( L, "" );
    }

    return 1;
}

static int engine_GetClientConVarValueAsNumber( lua_State *L )
{
    int iPlayer;

    if ( lua_toplayer( L, 1 ) )
        iPlayer = lua_toplayer( L, 1 )->entindex();
    else
        iPlayer = luaL_checkinteger( L, 1 );

    if ( iPlayer != ( *C_BasePlayer::GetLocalPlayer() ).entindex() )
        Warning( "engine.GetClientConVarValueAsNumber: Cannot get convars for other players. Returning own convar value.\n" );

    ConVarRef var( luaL_checkstring( L, 2 ) );

    if ( var.IsValid() )
    {
        lua_pushnumber( L, var.GetFloat() );
    }
    else
    {
        lua_pushnumber( L, 0 );
    }

    return 1;
}
static int engine_GetDXSupportLevel( lua_State *L )
{
    lua_pushinteger( L, engine->GetDXSupportLevel() );
    return 1;
}

static int engine_GetEngineBuildNumber( lua_State *L )
{
    lua_pushinteger( L, engine->GetEngineBuildNumber() );
    return 1;
}

static int engine_GetGameDirectory( lua_State *L )
{
    lua_pushstring( L, engine->GetGameDirectory() );
    return 1;
}

static int engine_GetLastTimeStamp( lua_State *L )
{
    lua_pushnumber( L, engine->GetLastTimeStamp() );
    return 1;
}

static int engine_GetLightForPoint( lua_State *L )
{
    Vector v = engine->GetLightForPoint( luaL_checkvector( L, 1 ), luaL_checkboolean( L, 2 ) );
    lua_pushvector( L, v );
    return 1;
}

static int engine_GetLightForPointFast( lua_State *L )
{
    Vector v = engine->GetLightForPointFast( luaL_checkvector( L, 1 ), luaL_checkboolean( L, 2 ) );
    lua_pushvector( L, v );
    return 1;
}

static int engine_GetLocalPlayer( lua_State *L )
{
    lua_pushinteger( L, engine->GetLocalPlayer() );
    return 1;
}

static int engine_GetMainMenuBackgroundName( lua_State *L )
{
    char *dest = "";
    engine->GetMainMenuBackgroundName( dest, luaL_checknumber( L, 2 ) );
    lua_pushstring( L, dest );
    return 1;
}

static int engine_GetMaxClients( lua_State *L )
{
    lua_pushinteger( L, engine->GetMaxClients() );
    return 1;
}

static int engine_GetMostRecentSaveGame( lua_State *L )
{
    lua_pushstring( L, engine->GetMostRecentSaveGame() );
    return 1;
}

static int engine_GetPlayerForUserID( lua_State *L )
{
    lua_pushinteger( L, engine->GetPlayerForUserID( luaL_checknumber( L, 1 ) ) );
    return 1;
}

static int engine_GetPlayerInfo( lua_State *L )
{
    player_info_t pinfo;
    lua_pushboolean( L, engine->GetPlayerInfo( luaL_checknumber( L, 1 ), &pinfo ) );
    lua_newtable( L );
    lua_pushstring( L, "name" );
    lua_pushstring( L, pinfo.name );
    lua_settable( L, -3 );
    lua_pushstring( L, "userID" );
    lua_pushinteger( L, pinfo.userID );
    lua_settable( L, -3 );
    lua_pushstring( L, "guid" );
    lua_pushstring( L, pinfo.guid );
    lua_settable( L, -3 );
    lua_pushstring( L, "friendsID" );
    lua_pushinteger( L, pinfo.friendsID );
    lua_settable( L, -3 );
    lua_pushstring( L, "friendsName" );
    lua_pushstring( L, pinfo.friendsName );
    lua_settable( L, -3 );
    lua_pushstring( L, "fakeplayer" );
    lua_pushboolean( L, pinfo.fakeplayer );
    lua_settable( L, -3 );
    lua_pushstring( L, "ishltv" );
    lua_pushboolean( L, pinfo.ishltv );
    lua_settable( L, -3 );
    return 2;
}

static int engine_GetProductVersionString( lua_State *L )
{
    lua_pushstring( L, engine->GetProductVersionString() );
    return 1;
}

static int engine_GetScreenAspectRatio( lua_State *L )
{
    lua_pushnumber( L, engine->GetScreenAspectRatio() );
    return 1;
}

static int engine_GetScreenSize( lua_State *L )
{
    int width, height;
    engine->GetScreenSize( width, height );
    lua_pushinteger( L, width );
    lua_pushinteger( L, height );
    return 2;
}

static int engine_GetUILanguage( lua_State *L )
{
    char *dest = "";
    engine->GetUILanguage( dest, luaL_checknumber( L, 1 ) );
    lua_pushstring( L, dest );
    return 1;
}

static int engine_GrabPreColorCorrectedFrame( lua_State *L )
{
    engine->GrabPreColorCorrectedFrame( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ), luaL_checknumber( L, 4 ) );
    return 0;
}

static int engine_IsBoxInViewCluster( lua_State *L )
{
    luaL_checknumber( L, engine->IsBoxInViewCluster( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int engine_IsBoxVisible( lua_State *L )
{
    luaL_checknumber( L, engine->IsBoxVisible( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int engine_IsConnected( lua_State *L )
{
    lua_pushboolean( L, engine->IsConnected() );
    return 1;
}

static int engine_IsDrawingLoadingImage( lua_State *L )
{
    lua_pushboolean( L, engine->IsDrawingLoadingImage() );
    return 1;
}

static int engine_IsHammerRunning( lua_State *L )
{
    lua_pushboolean( L, engine->IsHammerRunning() );
    return 1;
}

static int engine_IsHLTV( lua_State *L )
{
    lua_pushboolean( L, engine->IsHLTV() );
    return 1;
}

static int engine_IsInGame( lua_State *L )
{
    lua_pushboolean( L, engine->IsInGame() );
    return 1;
}

static int engine_IsLevelMainMenuBackground( lua_State *L )
{
    lua_pushboolean( L, engine->IsLevelMainMenuBackground() );
    return 1;
}

static int engine_IsOccluded( lua_State *L )
{
    luaL_checkboolean( L, engine->IsOccluded( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int engine_IsPlayingDemo( lua_State *L )
{
    lua_pushboolean( L, engine->IsPlayingDemo() );
    return 1;
}

static int engine_IsPlayingTimeDemo( lua_State *L )
{
    lua_pushboolean( L, engine->IsPlayingTimeDemo() );
    return 1;
}

static int engine_IsRecordingDemo( lua_State *L )
{
    lua_pushboolean( L, engine->IsRecordingDemo() );
    return 1;
}

static int engine_IsSaveInProgress( lua_State *L )
{
    lua_pushboolean( L, engine->IsSaveInProgress() );
    return 1;
}

static int engine_IsSkyboxVisibleFromPoint( lua_State *L )
{
    lua_pushinteger( L, engine->IsSkyboxVisibleFromPoint( luaL_checkvector( L, 1 ) ) );
    return 1;
}

static int engine_IsTakingScreenshot( lua_State *L )
{
    lua_pushboolean( L, engine->IsTakingScreenshot() );
    return 1;
}

static int engine_LevelLeafCount( lua_State *L )
{
    lua_pushinteger( L, engine->LevelLeafCount() );
    return 1;
}

static int engine_LightStyleValue( lua_State *L )
{
    lua_pushnumber( L, engine->LightStyleValue( luaL_checknumber( L, 1 ) ) );
    return 1;
}

static int engine_LinearToGamma( lua_State *L )
{
    float linear = luaL_checknumber( L, 1 );
    float gamma;
    engine->LinearToGamma( &linear, &gamma );
    lua_pushnumber( L, gamma );
    return 1;
}

static int engine_MapHasHDRLighting( lua_State *L )
{
    lua_pushboolean( L, engine->MapHasHDRLighting() );
    return 1;
}

static int engine_MapLoadFailed( lua_State *L )
{
    lua_pushboolean( L, engine->MapLoadFailed() );
    return 1;
}

static int engine_OnStorageDeviceDetached( lua_State *L )
{
    engine->OnStorageDeviceDetached();
    return 0;
}

static int engine_REMOVED_SteamProcessCall( lua_State *L )
{
    bool finished = luaL_checkboolean( L, 1 );
    lua_pushboolean( L, engine->REMOVED_SteamProcessCall( finished ) );
    return 1;
}

static int engine_ResetDemoInterpolation( lua_State *L )
{
    engine->ResetDemoInterpolation();
    return 0;
}

static int engine_ServerCmd( lua_State *L )
{
    engine->ServerCmd( luaL_checkstring( L, 1 ), luaL_checkboolean( L, 2 ) );
    return 0;
}

static int engine_SetMapLoadFailed( lua_State *L )
{
    engine->SetMapLoadFailed( luaL_checkboolean( L, 1 ) );
    return 0;
}

static int engine_SetRestrictClientCommands( lua_State *L )
{
    engine->SetRestrictClientCommands( luaL_checkboolean( L, 1 ) );
    return 0;
}

static int engine_SetRestrictServerCommands( lua_State *L )
{
    engine->SetRestrictServerCommands( luaL_checkboolean( L, 1 ) );
    return 0;
}

static int engine_Sound_ExtraUpdate( lua_State *L )
{
    engine->Sound_ExtraUpdate();
    return 0;
}

static int engine_StartXboxExitingProcess( lua_State *L )
{
    engine->StartXboxExitingProcess();
    return 0;
}

static int engine_SupportsHDR( lua_State *L )
{
    lua_pushboolean( L, engine->SupportsHDR() );
    return 1;
}

static const luaL_Reg enginelib[] = {
    { "ActivateOccluder", engine_ActivateOccluder },

    { "CheckKeyTrapping", engine_CheckKeyTrapping },
    { "IsKeyTrapping", engine_IsKeyTrapping },
    { "StartKeyTrapMode", engine_StartKeyTrapMode },

    { "CheckPoint", engine_CheckPoint },
    { "ClientCommand", engine_ClientCmd },
    { "ClientCommand_Unrestricted", engine_ClientCmd_Unrestricted },
    { "Con_IsVisible", engine_Con_IsVisible },
    { "CopyFrameBufferToMaterial", engine_CopyFrameBufferToMaterial },
    { "CullBox", engine_CullBox },
    { "DoesBoxTouchAreaFrustum", engine_DoesBoxTouchAreaFrustum },
    { "DrawPortals", engine_DrawPortals },
    { "EngineStats_BeginFrame", engine_EngineStats_BeginFrame },
    { "EngineStats_EndFrame", engine_EngineStats_EndFrame },
    { "GameLumpSize", engine_GameLumpSize },
    { "GetClientConVarValue", engine_GetClientConVarValue },
    { "GetClientConVarValueAsNumber", engine_GetClientConVarValueAsNumber },
    { "GetDXSupportLevel", engine_GetDXSupportLevel },
    { "GetEngineBuildNumber", engine_GetEngineBuildNumber },
    { "GetGameDirectory", engine_GetGameDirectory },
    { "GetLastTimeStamp", engine_GetLastTimeStamp },
    { "GetLightForPoint", engine_GetLightForPoint },
    { "GetLightForPointFast", engine_GetLightForPointFast },
    { "GetLocalPlayer", engine_GetLocalPlayer },
    { "GetMainMenuBackgroundName", engine_GetMainMenuBackgroundName },
    { "GetMaxClients", engine_GetMaxClients },
    { "GetMostRecentSaveGame", engine_GetMostRecentSaveGame },
    { "GetPlayerForUserID", engine_GetPlayerForUserID },
    { "GetPlayerInfo", engine_GetPlayerInfo },
    { "GetProductVersionString", engine_GetProductVersionString },
    { "GetScreenAspectRatio", engine_GetScreenAspectRatio },
    { "GetScreenSize", engine_GetScreenSize },
    { "GetUILanguage", engine_GetUILanguage },
    { "GrabPreColorCorrectedFrame", engine_GrabPreColorCorrectedFrame },
    { "IsBoxInViewCluster", engine_IsBoxInViewCluster },
    { "IsBoxVisible", engine_IsBoxVisible },
    { "IsConnected", engine_IsConnected },
    { "IsDrawingLoadingImage", engine_IsDrawingLoadingImage },
    { "IsHammerRunning", engine_IsHammerRunning },
    { "IsHLTV", engine_IsHLTV },
    { "IsInGame", engine_IsInGame },
    { "IsLevelMainMenuBackground", engine_IsLevelMainMenuBackground },
    { "IsOccluded", engine_IsOccluded },
    { "IsPlayingDemo", engine_IsPlayingDemo },
    { "IsPlayingTimeDemo", engine_IsPlayingTimeDemo },
    { "IsRecordingDemo", engine_IsRecordingDemo },
    { "IsSaveInProgress", engine_IsSaveInProgress },
    { "IsSkyboxVisibleFromPoint", engine_IsSkyboxVisibleFromPoint },
    { "IsTakingScreenshot", engine_IsTakingScreenshot },
    { "LevelLeafCount", engine_LevelLeafCount },
    { "LightStyleValue", engine_LightStyleValue },
    { "LinearToGamma", engine_LinearToGamma },
    { "MapHasHDRLighting", engine_MapHasHDRLighting },
    { "MapLoadFailed", engine_MapLoadFailed },
    { "OnStorageDeviceDetached", engine_OnStorageDeviceDetached },
    { "REMOVED_SteamProcessCall", engine_REMOVED_SteamProcessCall },
    { "ResetDemoInterpolation", engine_ResetDemoInterpolation },
    { "ServerCmd", engine_ServerCmd },
    { "SetMapLoadFailed", engine_SetMapLoadFailed },
    { "SetRestrictClientCommands", engine_SetRestrictClientCommands },
    { "SetRestrictServerCommands", engine_SetRestrictServerCommands },
    { "Sound_ExtraUpdate", engine_Sound_ExtraUpdate },
    { "StartXboxExitingProcess", engine_StartXboxExitingProcess },
    { "SupportsHDR", engine_SupportsHDR },
    { NULL, NULL } };

/*
** Open engine library
*/
LUALIB_API int luaopen_engine( lua_State *L )
{
    luaL_register( L, LUA_ENGINELIBNAME, enginelib );
    return 1;
}
