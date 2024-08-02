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

struct mountableGame_t
{
    const char *name;
    int appId;
    CUtlVector< const char * > vpks;

    mountableGame_t( const char *gameName, int gameId )
        : name( gameName ), appId( gameId ) {}

    mountableGame_t( const mountableGame_t &other )
        : name( other.name ), appId( other.appId )
    {
        for ( int i = 0; i < other.vpks.Count(); i++ )
        {
            vpks.AddToTail( other.vpks[i] );
        }
    }

    void AddVPK( const char *vpkPath )
    {
        vpks.AddToTail( vpkPath );
    }
};

static CUtlVector< mountableGame_t > g_GamePaths;

#define REGISTER_GAME( game_name, game_id ) \
    g_GamePaths.AddToTail( mountableGame_t( game_name, game_id ) );

#define REGISTER_GAME_WITH_VPK( game_name, game_id, ... ) \
    {                                                     \
        mountableGame_t game( game_name, game_id );       \
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
        //if ( g_GamePaths[i].appId == HL1MP_APPID )
        //{
        //    // Andrew; Half-Life Deathmatch: Source requires Half-Life: Source's path added!!
        //    Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s", appInstallPath, g_GamePaths[2].name );
        //    filesystem->AddSearchPath( fullPath, "GAME", PATH_ADD_TO_TAIL );
        //}

        if ( g_GamePaths[i].appId != nAppId )
            continue;

        Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s", appInstallPath, g_GamePaths[i].name );
        filesystem->AddSearchPath( fullPath, "GAME", PATH_ADD_TO_TAIL );

        auto &vpkPaths = g_GamePaths[i].vpks;

        for ( int j = 0; j < vpkPaths.Count(); j++ )
        {
            Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s", appInstallPath, vpkPaths[j] );
            filesystem->AddSearchPath( fullPath, "GAME", PATH_ADD_TO_TAIL );
        }
    }
}

void MountUserContent()
{
    REGISTER_GAME( "hl2", 220 );
    REGISTER_GAME( "cstrike", 240 );
    REGISTER_GAME( "hl1", 280 );
    REGISTER_GAME( "dod", 300 );
    REGISTER_GAME( "lostcoast", 340 );
    REGISTER_GAME( "hl1mp", HL1MP_APPID );
    REGISTER_GAME( "episodic", 380 );
    REGISTER_GAME( "portal", 400 );
    REGISTER_GAME( "ep2", 420 );
    REGISTER_GAME( "tf", 440 );

    REGISTER_GAME_WITH_VPK( "garrysmod", 4000, "garrysmod/fallback_dir.vpk", "garrysmod/garrysmod_dir.vpk" );

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
            if ( strcmp( pKey->GetName(), "AppId" ) != 0 )
                continue;

            int nExtraContentId = pKey->GetInt();

            if ( nExtraContentId )
            {
                AddSearchPathByAppId( nExtraContentId );
            }
        }
    }

    pMainFile->deleteThis();
}
