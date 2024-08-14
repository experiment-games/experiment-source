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

LUA_REGISTRATION_INIT( Engines );

LUA_BINDING_BEGIN( Engines, ActivateOccluder, "library", "Allow immediate edict reuse." )
{
    int occluderIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "occluderIndex" );
    bool shouldActivate = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldActivate" );
    engine->ActivateOccluder( occluderIndex, shouldActivate );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, IsKeyTrapping, "library", "Check if a key is being trapped." )
{
    ButtonCode_t code;
    lua_pushboolean( L, engine->CheckDoneKeyTrapping( code ) );
    lua_pushinteger( L, code );
    return 2;
}
LUA_BINDING_END( "boolean", "True if a key is being trapped, false otherwise.", "integer", "The key code being trapped." )

LUA_BINDING_BEGIN( Engines, CheckKeyTrapping, "library", "Check which key was last trapped." )
{
    ButtonCode_t code;
    engine->CheckDoneKeyTrapping( code );
    lua_pushinteger( L, code );
    return 1;
}
LUA_BINDING_END( "integer", "The key code being trapped." )

LUA_BINDING_BEGIN( Engines, StartKeyTrapMode, "library", "Start trapping a key." )
{
    engine->StartKeyTrapMode();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, CheckPoint, "library", "Check a point." )
{
    const char *pointName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "pointName" );
    engine->CheckPoint( pointName );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, ClientCommand, "library", "Send a command to the client." )
{
    const char *command = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "command" );
    engine->ClientCmd( command );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, ClientCommandUnrestricted, "library", "Send a command to the client without any restrictions." )
{
    const char *command = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "command" );
    engine->ClientCmd_Unrestricted( command );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, ConIsVisible, "library", "Check if the console is visible." )
{
    lua_pushboolean( L, engine->Con_IsVisible() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the console is visible, false otherwise." )

LUA_BINDING_BEGIN( Engines, CopyFrameBufferToMaterial, "library", "Copy the frame buffer to a material." )
{
    const char *materialName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "materialName" );
    lua_pushboolean( L, engine->CopyFrameBufferToMaterial( materialName ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the frame buffer was copied to the material, false otherwise." )

LUA_BINDING_BEGIN( Engines, CullBox, "library", "Cull a box." )
{
    Vector boxMin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "boxMin" );
    Vector boxMax = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "boxMax" );
    lua_pushboolean( L, engine->CullBox( boxMin, boxMax ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the box was culled, false otherwise." )

LUA_BINDING_BEGIN( Engines, DoesBoxTouchAreaFrustum, "library", "Check if a box touches an area frustum." )
{
    Vector boxMin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "boxMin" );
    Vector boxMax = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "boxMax" );
    int area = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "area" );
    lua_pushboolean( L, engine->DoesBoxTouchAreaFrustum( boxMin, boxMax, area ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the box touches the area frustum, false otherwise." )

LUA_BINDING_BEGIN( Engines, DrawPortals, "library", "Draw portals." )
{
    engine->DrawPortals();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, EngineStatsBeginFrame, "library", "Begin engine stats frame." )
{
    engine->EngineStats_BeginFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, EngineStatsEndFrame, "library", "End engine stats frame." )
{
    engine->EngineStats_EndFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, ExecuteClientCommand, "library", "Execute a client command." )
{
    const char *command = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "command" );
    engine->ExecuteClientCmd( command );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, FireEvents, "library", "Fire events." )
{
    engine->FireEvents();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, GameLumpSize, "library", "Get the size of a game lump." )
{
    int lumpId = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "lumpId" );
    lua_pushnumber( L, engine->GameLumpSize( lumpId ) );
    return 1;
}
LUA_BINDING_END( "number", "The size of the game lump." )

LUA_BINDING_BEGIN( Engines, GameLumpVersion, "library", "Get the version of a game lump." )
{
    int lumpId = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "lumpId" );
    lua_pushnumber( L, engine->GameLumpVersion( lumpId ) );
    return 1;
}
LUA_BINDING_END( "number", "The version of the game lump." )

LUA_BINDING_BEGIN( Engines, GetClientConsoleVariableValue, "library", "Get the value of a client console variable." )
{
    int playerIndex;

    if ( lua_toplayer( L, 1 ) )
        playerIndex = LUA_BINDING_ARGUMENT( lua_toplayer, 1, "playerOrIndex" )->entindex();
    else
        playerIndex = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "playerOrIndex" );

    if ( playerIndex != ( *C_BasePlayer::GetLocalPlayer() ).entindex() )
        Warning( "Engines.GetClientConsoleVariableValue: Cannot get convars for other players. Returning own convar value.\n" );

    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    ConVarRef var( varName );
    lua_pushstring( L, var.IsValid() ? var.GetString() : "" );
    return 1;
}
LUA_BINDING_END( "string", "The value of the client console variable." )

LUA_BINDING_BEGIN( Engines, GetClientConsoleVariableValueAsNumber, "library", "Get the value of a client console variable as a number." )
{
    int playerIndex;

    if ( lua_toplayer( L, 1 ) )
        playerIndex = LUA_BINDING_ARGUMENT( lua_toplayer, 1, "playerOrIndex" )->entindex();
    else
        playerIndex = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "playerOrIndex" );

    if ( playerIndex != ( *C_BasePlayer::GetLocalPlayer() ).entindex() )
        Warning( "Engines.GetClientConsoleVariableValueAsNumber: Cannot get convars for other players. Returning own convar value.\n" );

    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    ConVarRef var( varName );
    lua_pushnumber( L, var.IsValid() ? var.GetFloat() : 0 );
    return 1;
}
LUA_BINDING_END( "number", "The value of the client console variable as a number." )

LUA_BINDING_BEGIN( Engines, GetDxSupportLevel, "library", "Get the DirectX support level." )
{
    lua_pushnumber( L, engine->GetDXSupportLevel() );
    return 1;
}
LUA_BINDING_END( "number", "The DirectX support level." )

LUA_BINDING_BEGIN( Engines, GetEngineBuildNumber, "library", "Get the engine build number." )
{
    lua_pushnumber( L, engine->GetEngineBuildNumber() );
    return 1;
}
LUA_BINDING_END( "number", "The engine build number." )

LUA_BINDING_BEGIN( Engines, GetGameDirectory, "library", "Get the game directory." )
{
    lua_pushstring( L, engine->GetGameDirectory() );
    return 1;
}
LUA_BINDING_END( "string", "The game directory." )

LUA_BINDING_BEGIN( Engines, GetLastTimeStamp, "library", "Get the last time stamp." )
{
    lua_pushnumber( L, engine->GetLastTimeStamp() );
    return 1;
}
LUA_BINDING_END( "number", "The last time stamp." )

LUA_BINDING_BEGIN( Engines, GetLightForPoint, "library", "Get the light for a point." )
{
    Vector point = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "point" );
    bool clamp = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "clamp" );
    Vector light = engine->GetLightForPoint( point, clamp );
    lua_pushvector( L, light );
    return 1;
}
LUA_BINDING_END( "Vector", "The light for the point." )

LUA_BINDING_BEGIN( Engines, GetLightForPointFast, "library", "Get the light for a point quickly." )
{
    Vector point = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "point" );
    bool clamp = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "clamp" );
    Vector light = engine->GetLightForPointFast( point, clamp );
    lua_pushvector( L, light );
    return 1;
}
LUA_BINDING_END( "Vector", "The light for the point." )

LUA_BINDING_BEGIN( Engines, GetLocalPlayer, "library", "Get the local player." )
{
    lua_pushnumber( L, engine->GetLocalPlayer() );
    return 1;
}
LUA_BINDING_END( "number", "The local player." )

LUA_BINDING_BEGIN( Engines, GetMainMenuBackgroundName, "library", "Get the main menu background name." )
{
    char name[256];
    engine->GetMainMenuBackgroundName( name, sizeof( name ) );
    lua_pushstring( L, name );
    return 1;
}
LUA_BINDING_END( "string", "The main menu background name." )

LUA_BINDING_BEGIN( Engines, GetMostRecentSaveGame, "library", "Get the most recent save game." )
{
    lua_pushstring( L, engine->GetMostRecentSaveGame() );
    return 1;
}
LUA_BINDING_END( "string", "The most recent save game." )

LUA_BINDING_BEGIN( Engines, GetPlayerEntityIndexForUserId, "library", "Get the player entity number for a user ID." )
{
    int userId = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "userId" );
    lua_pushnumber( L, engine->GetPlayerForUserID( userId ) );
    return 1;
}
LUA_BINDING_END( "number", "The player entity number for the user ID." )

LUA_BINDING_BEGIN( Engines, GetPlayerInfo, "library", "Get the player information." )
{
    int playerIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "playerIndex" );
    player_info_t pinfo;
    lua_pushboolean( L, engine->GetPlayerInfo( playerIndex, &pinfo ) );
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
LUA_BINDING_END( "table", "The player information." )

LUA_BINDING_BEGIN( Engines, GetProductVersionString, "library", "Get the product version string." )
{
    lua_pushstring( L, engine->GetProductVersionString() );
    return 1;
}
LUA_BINDING_END( "string", "The product version string." )

LUA_BINDING_BEGIN( Engines, GetScreenAspectRatio, "library", "Get the screen aspect ratio." )
{
    lua_pushnumber( L, engine->GetScreenAspectRatio() );
    return 1;
}
LUA_BINDING_END( "number", "The screen aspect ratio." )

LUA_BINDING_BEGIN( Engines, GetScreenSize, "library", "Get the screen size." )
{
    int width, height;
    engine->GetScreenSize( width, height );
    lua_pushinteger( L, width );
    lua_pushinteger( L, height );
    return 2;
}
LUA_BINDING_END( "number", "The screen width.", "number", "The screen height." )

LUA_BINDING_BEGIN( Engines, GetUiLanguage, "library", "Get the UI language." )
{
    char dest[64];
    engine->GetUILanguage( dest, sizeof( dest ) );
    lua_pushstring( L, dest );
    return 1;
}
LUA_BINDING_END( "string", "The UI language." )

LUA_BINDING_BEGIN( Engines, GrabPreColorCorrectedFrame, "library", "Grab a pre-color corrected frame." )
{
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "width" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "height" );
    engine->GrabPreColorCorrectedFrame( x, y, width, height );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, IsBoxInViewCluster, "library", "Check if a box is in a view cluster." )
{
    Vector boxMin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "boxMin" );
    Vector boxMax = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "boxMax" );
    lua_pushboolean( L, engine->IsBoxInViewCluster( boxMin, boxMax ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the box is in the view cluster, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsBoxVisible, "library", "Check if a box is visible." )
{
    Vector boxMin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "boxMin" );
    Vector boxMax = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "boxMax" );
    lua_pushboolean( L, engine->IsBoxVisible( boxMin, boxMax ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the box is visible, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsConnected, "library", "Check if the client is connected." )
{
    lua_pushboolean( L, engine->IsConnected() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the client is connected, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsDrawingLoadingImage, "library", "Check if the client is drawing a loading image." )
{
    lua_pushboolean( L, engine->IsDrawingLoadingImage() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the client is drawing a loading image, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsHammerRunning, "library", "Check if Hammer is running." )
{
    lua_pushboolean( L, engine->IsHammerRunning() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if Hammer is running, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsHLTV, "library", "Check if the client is an HLTV." )
{
    lua_pushboolean( L, engine->IsHLTV() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the client is an HLTV, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsInGame, "library", "Check if the client is in a game." )
{
    lua_pushboolean( L, engine->IsInGame() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the client is in a game, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsLevelMainMenuBackground, "library", "Check if the level is a main menu background." )
{
    lua_pushboolean( L, engine->IsLevelMainMenuBackground() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the level is a main menu background, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsOccluded, "library", "Check if a box is occluded." )
{
    Vector boxMin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "boxMin" );
    Vector boxMax = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "boxMax" );
    lua_pushboolean( L, engine->IsOccluded( boxMin, boxMax ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the box is occluded, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsPlayingDemo, "library", "Check if the client is playing a demo." )
{
    lua_pushboolean( L, engine->IsPlayingDemo() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the client is playing a demo, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsPlayingTimeDemo, "library", "Check if the client is playing a time demo." )
{
    lua_pushboolean( L, engine->IsPlayingTimeDemo() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the client is playing a time demo, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsRecordingDemo, "library", "Check if the client is recording a demo." )
{
    lua_pushboolean( L, engine->IsRecordingDemo() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the client is recording a demo, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsSaveInProgress, "library", "Check if a save is in progress." )
{
    lua_pushboolean( L, engine->IsSaveInProgress() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if a save is in progress, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsSkyboxVisibleFromPoint, "library", "Check if the skybox is visible from a point." )
{
    Vector point = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "point" );
    lua_pushinteger( L, engine->IsSkyboxVisibleFromPoint( point ) );
    return 1;
}
LUA_BINDING_END( "number", "The skybox visibility." )

LUA_BINDING_BEGIN( Engines, IsTakingScreenshot, "library", "Check if a screenshot is being taken." )
{
    lua_pushboolean( L, engine->IsTakingScreenshot() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if a screenshot is being taken, false otherwise." )

LUA_BINDING_BEGIN( Engines, LevelLeafCount, "library", "Get the level leaf count." )
{
    lua_pushinteger( L, engine->LevelLeafCount() );
    return 1;
}
LUA_BINDING_END( "number", "The level leaf count." )

LUA_BINDING_BEGIN( Engines, LightStyleValue, "library", "Get the light style value." )
{
    lua_pushnumber( L, engine->LightStyleValue( luaL_checknumber( L, 1 ) ) );
    return 1;
}
LUA_BINDING_END( "number", "The light style value." )

LUA_BINDING_BEGIN( Engines, LinearToGamma, "library", "Convert a linear value to a gamma value." )
{
    float linear = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "linear" );
    float gamma;
    engine->LinearToGamma( &linear, &gamma );
    lua_pushnumber( L, gamma );
    return 1;
}
LUA_BINDING_END( "number", "The gamma value." )

LUA_BINDING_BEGIN( Engines, MapHasHdrLighting, "library", "Check if the map has HDR lighting." )
{
    lua_pushboolean( L, engine->MapHasHDRLighting() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the map has HDR lighting, false otherwise." )

LUA_BINDING_BEGIN( Engines, MapLoadFailed, "library", "Check if the map load failed." )
{
    lua_pushboolean( L, engine->MapLoadFailed() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the map load failed, false otherwise." )

LUA_BINDING_BEGIN( Engines, OnStorageDeviceDetached, "library", "Handle a storage device detachment." )
{
    engine->OnStorageDeviceDetached();
    return 0;
}
LUA_BINDING_END()

//LUA_BINDING_BEGIN( Engines, REMOVED_SteamProcessCall, "library", "Process a Steam call." )
//{
//    bool finished = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "finished" );
//    lua_pushboolean( L, engine->REMOVED_SteamProcessCall( finished ) );
//    return 1;
//}
//LUA_BINDING_END( "boolean", "True if the Steam call was processed, false otherwise." )

LUA_BINDING_BEGIN( Engines, ResetDemoInterpolation, "library", "Reset demo interpolation." )
{
    engine->ResetDemoInterpolation();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, ServerCommand, "library", "Send a command to the server." )
{
    const char *command = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "command" );
    engine->ServerCmd( command, LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, true, "reliable" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, SetMapLoadFailed, "library", "Set the map load failed state." )
{
    engine->SetMapLoadFailed( LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "failed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, SetRestrictClientCommands, "library", "Set the restrict client commands state." )
{
    engine->SetRestrictClientCommands( LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "restrict" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, SetRestrictServerCommands, "library", "Set the restrict server commands state." )
{
    engine->SetRestrictServerCommands( LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "restrict" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, SoundExtraUpdate, "library", "Update sound." )
{
    engine->Sound_ExtraUpdate();
    return 0;
}
LUA_BINDING_END()

//LUA_BINDING_BEGIN( Engines, StartXboxExitingProcess, "library", "Start the Xbox exiting process." )
//{
//    engine->StartXboxExitingProcess();
//    return 0;
//}
//LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, SupportsHdr, "library", "Check if HDR is supported." )
{
    lua_pushboolean( L, engine->SupportsHDR() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if HDR is supported, false otherwise." )

/*
** Open engine library
*/
LUALIB_API int luaopen_engine( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Engines );
    return 1;
}
