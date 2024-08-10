//========== Copyleft � 2011, Team Sandbox, Some rights reserved. ===========//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "filesystem.h"
#include "KeyValues.h"
#include "mountsteamcontent.h"
#include "steam/steam_api.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CUtlVector< mountableGame_t > g_GamePaths;

#define REGISTER_GAME( pSteamApps, GameName, GameDirectory, GameAppId ) \
    g_GamePaths.AddToTail( mountableGame_t( GameName, GameDirectory, GameAppId, pSteamApps->BIsAppInstalled( GameAppId ), pSteamApps->BIsSubscribedApp( GameAppId ) ) );

#define REGISTER_GAME_WITH_VPK( pSteamApps, GameName, GameDirectory, GameAppId, ... ) \
    {                                                     \
        mountableGame_t game( GameName, GameDirectory, GameAppId, pSteamApps->BIsAppInstalled( GameAppId ), pSteamApps->BIsSubscribedApp( GameAppId ) );       \
        const char *vpkPaths[] = { __VA_ARGS__ };         \
        for ( int i = 0; i < ARRAYSIZE( vpkPaths ); i++ ) \
        {                                                 \
            game.AddVPK( vpkPaths[i] );                   \
        }                                                 \
        g_GamePaths.AddToTail( game );                    \
    }

#define HL1MP_APPID 360

void AddSearchPathByAppId( int nAppId )
{
    ISteamApps *pSteamApps = steamapicontext->SteamApps();

    if ( !pSteamApps->BIsAppInstalled( nAppId ) )
    {
        Warning( "Failed to mount content for App ID %i (not installed)\n", nAppId );
        return;
    }

    char appInstallPath[MAX_PATH * 2];
    pSteamApps->GetAppInstallDir( nAppId, appInstallPath, sizeof( appInstallPath ) );

    for ( int i = 0; i < g_GamePaths.Count(); i++ )
    {
        char fullPath[MAX_PATH];

        // Commented because it's wrong anyway. g_GamePaths[2] always pointed to cstrike :/
        // if ( g_GamePaths[i].appId == HL1MP_APPID )
        //{
        //    // Andrew; Half-Life Deathmatch: Source requires Half-Life: Source's path added!!
        //    Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s", appInstallPath, g_GamePaths[2].directoryName );
        //    filesystem->AddSearchPath( fullPath, "GAME", PATH_ADD_TO_TAIL );
        //}

        if ( g_GamePaths[i].appId != nAppId )
            continue;

        Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s", appInstallPath, g_GamePaths[i].directoryName );
        filesystem->AddSearchPath( fullPath, "GAME", PATH_ADD_TO_TAIL );

        auto &vpkPaths = g_GamePaths[i].vpks;

        for ( int j = 0; j < vpkPaths.Count(); j++ )
        {
            Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s", appInstallPath, vpkPaths[j] );
            filesystem->AddSearchPath( fullPath, "GAME", PATH_ADD_TO_TAIL );
        }

        g_GamePaths[i].isMounted = true;

#ifdef CLIENT_DLL  // Prevent the message from printing twice in clients
        ConColorMsg( Color( 200, 255, 0, 255 ), "Mounting game content \"%s\"...\n", g_GamePaths[i].name );
#endif

        // We've found the game, no need to continue searching.
        break;
    }
}

void RemoveSearchPathByAppId( int nAppId )
{
    ISteamApps *pSteamApps = steamapicontext->SteamApps();

    if ( !pSteamApps->BIsAppInstalled( nAppId ) )
    {
        Warning( "Failed to unmount content for App ID %i (not installed)\n", nAppId );
        return;
    }

    char appInstallPath[MAX_PATH * 2];
    pSteamApps->GetAppInstallDir( nAppId, appInstallPath, sizeof( appInstallPath ) );

    for ( int i = 0; i < g_GamePaths.Count(); i++ )
    {
        char fullPath[MAX_PATH];

        if ( g_GamePaths[i].appId != nAppId )
            continue;

        Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s", appInstallPath, g_GamePaths[i].directoryName );
        filesystem->RemoveSearchPath( fullPath );

        auto &vpkPaths = g_GamePaths[i].vpks;

        for ( int j = 0; j < vpkPaths.Count(); j++ )
        {
            Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s", appInstallPath, vpkPaths[j] );
            filesystem->RemoveSearchPath( fullPath );
        }

        g_GamePaths[i].isMounted = false;

#ifdef CLIENT_DLL  // Prevent the message from printing twice in clients
        ConColorMsg( Color( 200, 255, 0, 255 ), "Unmounting game content \"%s\"...\n", g_GamePaths[i].name );
#endif

        // We've found the game, no need to continue searching.
        break;
    }

    // Andrew; Half-Life Deathmatch: Source requires Half-Life: Source's path added!!
    // if ( nAppId == HL1MP_APPID )
    //{
    //    Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s", appInstallPath, g_GamePaths[2].directoryName );
    //    filesystem->RemoveSearchPath( fullPath );
    //}
}

CUtlVector< mountableGame_t > &GetMountableGames()
{
    return g_GamePaths;
}

void InitializeGameContentMounting()
{
    ISteamApps *pSteamApps = steamapicontext->SteamApps();

    REGISTER_GAME( pSteamApps, "Half-Life 2", "hl2", 220 );
    REGISTER_GAME( pSteamApps, "Half-Life", "hl1", 280 );
    
    REGISTER_GAME( pSteamApps, "Day of Defeat: Source", "dod", 300 );
    REGISTER_GAME( pSteamApps, "Half-Life 2: Lost Coast", "lostcoast", 340 );
    REGISTER_GAME( pSteamApps, "Half-Life Deathmatch: Source", "hl1mp", HL1MP_APPID );
    REGISTER_GAME( pSteamApps, "Half-Life 2: Episode One", "episodic", 380 );
    REGISTER_GAME( pSteamApps, "Portal", "portal", 400 );
    REGISTER_GAME( pSteamApps, "Half-Life 2: Episode Two", "ep2", 420 );
    REGISTER_GAME( pSteamApps, "Team Fortress 2", "tf", 440 );

    REGISTER_GAME_WITH_VPK( pSteamApps, "Counter-Strike: Source", "cstrike", 240, "cstrike/cstrike_pak_dir.vpk" );
    REGISTER_GAME_WITH_VPK( pSteamApps, "Garry's Mod", "garrysmod", 4000, "garrysmod/fallback_dir.vpk", "garrysmod/garrysmod_dir.vpk" );

    KeyValues *pMainFile, *pFileSystemInfo;
#ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
#else
    char gamePath[256];
    engine->GetGameDir( gamePath, 256 );
    Q_StripTrailingSlash( gamePath );
#endif

    pMainFile = new KeyValues( "gamecontent.txt" );

#ifdef CLIENT_DLL
#define UTIL_VarArgs VarArgs
#endif

    // On linux because of case sensitiviy we need to check for both.
    const char *paths[] = {
#ifdef _LINUX
        "%s/GameContent.txt",
#endif
        "%s/gamecontent.txt" };

    for ( int i = 0; i < ARRAYSIZE( paths ); i++ )
    {
        if ( !pMainFile->LoadFromFile( filesystem, UTIL_VarArgs( paths[i], gamePath ), CONTENT_SEARCH_PATH ) )
            continue;

        pFileSystemInfo = pMainFile->FindKey( "FileSystem" );

        if ( !pFileSystemInfo )
            continue;

        for ( KeyValues *pKey = pFileSystemInfo->GetFirstSubKey(); pKey; pKey = pKey->GetNextKey() )
        {
            // pKey->GetName() is garrysmod/cstrike/etc
            int nAppId = pKey->GetInt();

            if ( nAppId )
            {
                AddSearchPathByAppId( nAppId );
            }
        }
    }

    pMainFile->deleteThis();
}

/// <summary>
/// Adds the specified game to the gamecontent.txt file and mounts the content for the specified game.
/// </summary>
/// <param name="nAppId">The AppID of the game to mount the content for.</param>
/// <returns>Whether the game was successfully mounted.</returns>
bool MountGameContentByAppId( int nAppId )
{
    mountableGame_t *pGameToMount = nullptr;

    for ( int i = 0; i < g_GamePaths.Count(); i++ )
    {
        if ( g_GamePaths[i].appId == nAppId )
        {
            pGameToMount = &g_GamePaths[i];
            break;
        }
    }

    if ( !pGameToMount )
    {
        Warning( "Failed to mount content for game with AppID: %i (game not found)\n", nAppId );
        return false;
    }

    KeyValues *pMainFile, *pFileSystemInfo;
    pMainFile = new KeyValues( "gamecontent.txt" );

#ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
#else
    char gamePath[256];
    engine->GetGameDir( gamePath, 256 );
    Q_StripTrailingSlash( gamePath );
#endif

    // On linux because of case sensitiviy we need to check for both.
    const char *paths[] = {
#ifdef _LINUX
        "%s/GameContent.txt",
#endif
        "%s/gamecontent.txt" };

    bool bFound = false;

    for ( int i = 0; i < ARRAYSIZE( paths ); i++ )
    {
        if ( !pMainFile->LoadFromFile( filesystem, UTIL_VarArgs( paths[i], gamePath ), CONTENT_SEARCH_PATH ) )
            continue;

        pFileSystemInfo = pMainFile->FindKey( "FileSystem" );

        if ( !pFileSystemInfo )
            continue;

        pFileSystemInfo->SetInt( pGameToMount->directoryName, pGameToMount->appId );

        pMainFile->SaveToFile( filesystem, UTIL_VarArgs( paths[i], gamePath ), CONTENT_SEARCH_PATH );
        bFound = true;

        // We've found the game, no need to continue searching.
        break;
    }

    pMainFile->deleteThis();

    if ( !bFound )
    {
        Warning( "Failed to mount content for game %s (GameContent.txt not found)\n", pGameToMount->name );
        return false;
    }

    AddSearchPathByAppId( pGameToMount->appId );
    return true;
}

bool UnmountGameContentByAppId( int nAppId )
{
    mountableGame_t *pGameToUnmount = nullptr;

    for ( int i = 0; i < g_GamePaths.Count(); i++ )
    {
        if ( g_GamePaths[i].appId == nAppId )
        {
            pGameToUnmount = &g_GamePaths[i];
            break;
        }
    }

    if ( !pGameToUnmount )
    {
        Warning( "Failed to unmount content for game with AppID: %i (game not found)\n", nAppId );
        return false;
    }

    KeyValues *pMainFile, *pFileSystemInfo;
    pMainFile = new KeyValues( "gamecontent.txt" );

    #ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
    #else
    char gamePath[256];
    engine->GetGameDir( gamePath, 256 );
    Q_StripTrailingSlash( gamePath );
    #endif

    // On linux because of case sensitiviy we need to check for both.
    const char *paths[] = {
        #ifdef _LINUX
        "%s/GameContent.txt",
        #endif
        "%s/gamecontent.txt" };

    bool bFound = false;

    for ( int i = 0; i < ARRAYSIZE( paths ); i++ )
    {
        if ( !pMainFile->LoadFromFile( filesystem, UTIL_VarArgs( paths[i], gamePath ), CONTENT_SEARCH_PATH ) )
            continue;

        pFileSystemInfo = pMainFile->FindKey( "FileSystem" );

        if ( !pFileSystemInfo )
            continue;

        KeyValues *pSubKey = pFileSystemInfo->FindKey( pGameToUnmount->directoryName );
        pFileSystemInfo->RemoveSubKey( pSubKey );

        pMainFile->SaveToFile( filesystem, UTIL_VarArgs( paths[i], gamePath ), CONTENT_SEARCH_PATH );
        bFound = true;

        // We've found the game, no need to continue searching.
        break;
    }

    pMainFile->deleteThis();

    if ( !bFound )
    {
        Warning( "Failed to unmount content for game %s (GameContent.txt not found)\n", pGameToUnmount->name );
        return false;
    }

    RemoveSearchPathByAppId( pGameToUnmount->appId );

    return true;
}
