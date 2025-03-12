#include "cbase.h"
#include "mainmenu.h"

#include "vgui/ILocalize.h"
#include "vgui/IPanel.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"
#include "ienginevgui.h"
#include <engine/IEngineSound.h>
#include "filesystem.h"
#include <icommandline.h>
#include "GameUI/IGameConsole.h"
#include "vgui/IInput.h"
#include <mountsteamcontent.h>
#include <createmultiplayergamedialog.h>
#include <util/jsontokv.h>
#include <time.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>
#include <clientmode_experimentnormal.h>

using namespace vgui;

CBaseMenuPanel *g_BaseMenuPanel = NULL;

bool GameUIUtil::IsInLevel()
{
    return engine->IsInGame() && !engine->IsLevelMainMenuBackground();
}

bool GameUIUtil::IsInBackgroundLevel()
{
    return engine->IsInGame() && engine->IsLevelMainMenuBackground();
}

bool GameUIUtil::IsInMenu()
{
    return !engine->IsInGame();
}

void GameUIUtil::PositionDialog( vgui::PHandle dialog )
{
    if ( !dialog.Get() )
        return;

    int x, y, workspaceWidth, workspaceHeight, width, height;
    vgui::surface()->GetWorkspaceBounds( x, y, workspaceWidth, workspaceHeight );
    dialog->GetSize( width, height );

    dialog->SetPos( x + ( ( workspaceWidth - width ) * .5 ), y + ( ( workspaceHeight - height ) * .5 ) );
}

CBaseMenuPanel::CBaseMenuPanel()
    : BaseClass( NULL, "ExperimentBaseMenuPanel" )
{
    g_BaseMenuPanel = this;

    SetVisible( true );
    SetAutoDelete( false );

    m_eBackgroundState = BACKGROUND_INITIAL;

    // Set our initial size
    int screenWide, screenTall;
    surface()->GetScreenSize( screenWide, screenTall );
    SetBounds( 0, 0, screenWide, screenTall );

    AddActionSignalTarget( this );

    // Add a HTML panel that takes up the whole screen
    // This is where the main menu will be displayed
    m_pHTML = new MainMenuHTML( this, "MainMenuHTML" );
    m_pHTML->AddCustomURLHandler( "mainmenu", this );
    m_pHTML->AddCustomURLHandler( "gamemenucommand", this );
    m_pHTML->OpenURL( "asset://experiment/menus/main.html", NULL );

    vgui::ivgui()->AddTickSignal( GetVPanel(), 50 );

    AttachToGameUI();
}

CBaseMenuPanel::~CBaseMenuPanel()
{
    if ( GetClientModeExperimentNormal()->GameUI() )
    {
        GetClientModeExperimentNormal()->GameUI()->SetMainMenuOverride( NULL );
    }

    vgui::ivgui()->RemoveTickSignal( GetVPanel() );
}

CBaseMenuPanel *CBaseMenuPanel::Init()
{
    if ( g_BaseMenuPanel )
        return g_BaseMenuPanel;

    g_BaseMenuPanel = new CBaseMenuPanel();
    return g_BaseMenuPanel;
}

void CBaseMenuPanel::AttachToGameUI()
{
    if ( GetClientModeExperimentNormal()->GameUI() )
    {
        GetClientModeExperimentNormal()->GameUI()->SetMainMenuOverride( GetVPanel() );
    }

    SetKeyBoardInputEnabled( true );
    SetMouseInputEnabled( true );
    SetCursor( dc_arrow );
}

void CBaseMenuPanel::PerformLayout()
{
    BaseClass::PerformLayout();

    int wide, tall;
    vgui::surface()->GetScreenSize( wide, tall );
    SetSize( wide, tall );

    if ( m_pHTML )
    {
        m_pHTML->SetBounds( 0, 0, wide, tall );
    }
}

bool CBaseMenuPanel::IsVisible( void )
{
    return true;
}

void CBaseMenuPanel::SetBackgroundRenderState( EBackgroundState state )
{
    if ( state == m_eBackgroundState )
    {
        return;
    }

    m_eBackgroundState = state;

    char szScript[256];
    Q_snprintf( szScript, sizeof( szScript ), "SetBackgroundRenderState( %d )", state );

    if ( m_pHTML )
        m_pHTML->RunJavascript( szScript );

    // if ( state == BACKGROUND_DISCONNECTED || state == BACKGROUND_MAINMENU )
    // {
    //     if ( state == BACKGROUND_MAINMENU )
    //     {
    //         if ( m_pHTML )
    //             m_pHTML->RunJavascript( "SetBackgroundRenderState( BACKGROUND_MAINMENU )" );
    //     }
    // }
    // else
    if ( state == BACKGROUND_LOADING )
    {
        SetAlpha( 0 );
    }
    else if ( state == BACKGROUND_LEVEL )
    {
        SetAlpha( 255 );
    }
}

void CBaseMenuPanel::UpdateBackgroundState()
{
    if ( GameUIUtil::IsInLevel() )
    {
        SetBackgroundRenderState( BACKGROUND_LEVEL );
    }
    else if ( GameUIUtil::IsInBackgroundLevel() /*&& !m_bLevelLoading*/ )
    {
        // level loading is truly completed when the progress bar is gone, then transition to main menu
        SetBackgroundRenderState( BACKGROUND_MAINMENU );
    }
    // else if ( m_bLevelLoading )
    //{
    //     SetBackgroundRenderState( BACKGROUND_LOADING );
    // }
    else if ( GameUIUtil::IsInMenu() )
    {
        SetBackgroundRenderState( BACKGROUND_MAINMENU );
    }
    /*else
    {
        SetBackgroundRenderState( BACKGROUND_DISCONNECTED );
    }*/
}

static void GetMaps( CUtlVector< CUtlString > &outMaps )
{
    FileFindHandle_t findHandle = NULL;

    const char *pszFilename = g_pFullFileSystem->FindFirst( "maps/*.bsp", &findHandle );
    while ( pszFilename )
    {
        char mapname[256];

        // FindFirst ignores the pszPathID, so check it here
        // TODO: this doesn't find maps in fallback dirs
        Q_snprintf( mapname, sizeof( mapname ), "maps/%s", pszFilename );
        if ( !g_pFullFileSystem->FileExists( mapname, CONTENT_SEARCH_PATH ) )
        {
            goto nextFile;
        }

        // remove the text 'maps/' and '.bsp' from the file name to get the map name

        const char *str = Q_strstr( pszFilename, "maps" );
        if ( str )
        {
            Q_strncpy( mapname, str + 5, sizeof( mapname ) - 1 );  // maps + \\ = 5
        }
        else
        {
            Q_strncpy( mapname, pszFilename, sizeof( mapname ) - 1 );
        }
        char *ext = Q_strstr( mapname, ".bsp" );
        if ( ext )
        {
            *ext = 0;
        }

        // add to the map list
        outMaps.AddToTail( mapname );

        // get the next file
    nextFile:
        pszFilename = g_pFullFileSystem->FindNext( findHandle );
    }
    g_pFullFileSystem->FindClose( findHandle );
}

static void GetGamemodes( CUtlVector< CUtlString > &outGamemodes )
{
    FileFindHandle_t findHandle = NULL;

    const char *pszFilename = g_pFullFileSystem->FindFirst( "gamemodes/*", &findHandle );
    while ( pszFilename )
    {
        if ( Q_stricmp( pszFilename, ".." ) == 0 || Q_stricmp( pszFilename, "." ) == 0 )
        {
            goto nextFile;
        }

        if ( !g_pFullFileSystem->FindIsDirectory( findHandle ) )
        {
            goto nextFile;
        }

        outGamemodes.AddToTail( pszFilename );

    nextFile:
        pszFilename = g_pFullFileSystem->FindNext( findHandle );
    }
    g_pFullFileSystem->FindClose( findHandle );
}

void MainMenuHTML::OnInstallJavaScriptInterop()
{
    AddJavascriptObject( "GameUI" );
    AddJavascriptObjectCallback( "GameUI", "LoadMountableContentInfo" );
    AddJavascriptObjectCallback( "GameUI", "LoadServerVariables" );
    AddJavascriptObjectCallback( "GameUI", "MountGameContent" );
    AddJavascriptObjectCallback( "GameUI", "UnmountGameContent" );
    AddJavascriptObjectCallback( "GameUI", "HostServer" );
    AddJavascriptObjectCallback( "GameUI", "RequestServerList" );
    AddJavascriptObjectCallback( "GameUI", "CancelServerListRequest" );
    AddJavascriptObjectCallback( "GameUI", "ModifyFavoriteGame" );
    AddJavascriptObjectCallback( "GameUI", "ConnectToServer" );
}

void MainMenuHTML::OnJavaScriptCallback( KeyValues *pData )
{
    const char *pszObject = pData->GetString( "object" );
    const char *pszProperty = pData->GetString( "property" );
    int callbackId = pData->GetInt( "callbackId" );
    // KeyValues *pArguments = pData->FindKey( "arguments" );

    // for ( KeyValues *pArg = pArguments->GetFirstSubKey(); pArg; pArg = pArg->GetNextKey() )
    //{
    //     DevMsg( "Argument: %s (inside %s.%s call)\n", pArg->GetName(), pszObject, pszProperty );
    // }

    // This panel only handles GameUI callbacks
    if ( Q_strcmp( pszObject, "GameUI" ) != 0 )
        return;

    if ( Q_strcmp( pszProperty, "LoadMountableContentInfo" ) == 0 )
    {
        KeyValues *pParameters = new KeyValues( "parameters" );
        CUtlVector< mountableGame_t > &mountableGames = GetMountableGames();

        KeyValues *pMountableGames = new KeyValues( "mountableGames" );
        pParameters->AddSubKey( pMountableGames );

        for ( int i = 0; i < mountableGames.Count(); i++ )
        {
            mountableGame_t &game = mountableGames[i];

            char icon[MAX_PATH];
            Q_snprintf( icon, sizeof( icon ), "images/game-icons/%s.png", game.directoryName );

            KeyValues *pGame = new KeyValues( "" );
            pGame->SetString( "icon", icon );
            pGame->SetString( "name", game.name );
            pGame->SetInt( "id", game.appId );
            pGame->SetBool( "mounted", game.isMounted );
            pMountableGames->AddSubKey( pGame );
        }

        CallJavascriptObjectCallback( callbackId, pParameters );
    }
    else if ( Q_strcmp( pszProperty, "LoadServerVariables" ) == 0 )
    {
        KeyValues *pParameters = new KeyValues( "parameters" );

        CUtlVector< CUtlString > maps;
        GetMaps( maps );

        KeyValues *pMaps = new KeyValues( "maps" );
        pParameters->AddSubKey( pMaps );

        for ( int i = 0; i < maps.Count(); i++ )
        {
            CUtlString map = maps[i];

            KeyValues *pMap = new KeyValues( "" );
            pMap->SetString( "id", map );
            pMaps->AddSubKey( pMap );
        }

        KeyValues *pGamemodes = new KeyValues( "gamemodes" );
        pParameters->AddSubKey( pGamemodes );

        CUtlVector< CUtlString > gamemodes;
        GetGamemodes( gamemodes );

        for ( int i = 0; i < gamemodes.Count(); i++ )
        {
            CUtlString gamemode = gamemodes[i];

            KeyValues *pGamemode = new KeyValues( "" );
            pGamemode->SetString( "id", gamemode );
            pGamemodes->AddSubKey( pGamemode );
        }

        CallJavascriptObjectCallback( callbackId, pParameters );
    }
    else if ( Q_strcmp( pszProperty, "MountGameContent" ) == 0 )
    {
        KeyValues *pArguments = pData->FindKey( "arguments" );
        int nGameAppId = pArguments->GetInt( "1" );

        KeyValues *pResponse = new KeyValues( "parameters" );
        pResponse->SetBool( "wasSuccessful", MountGameContentByAppId( nGameAppId ) );
        CallJavascriptObjectCallback( callbackId, pResponse );
    }
    else if ( Q_strcmp( pszProperty, "UnmountGameContent" ) == 0 )
    {
        KeyValues *pArguments = pData->FindKey( "arguments" );
        int nGameAppId = pArguments->GetInt( "1" );

        KeyValues *pResponse = new KeyValues( "parameters" );
        pResponse->SetBool( "wasSuccessful", UnmountGameContentByAppId( nGameAppId ) );
        CallJavascriptObjectCallback( callbackId, pResponse );
    }
    else if ( Q_strcmp( pszProperty, "HostServer" ) == 0 )
    {
        KeyValues *pArguments = pData->FindKey( "arguments" );
        KeyValues *config = pArguments->FindKey( "1" );

        const char *hostName = config->GetString( "name" );
        const char *gamemode = config->GetString( "gamemode" );
        const char *map = config->GetString( "map" );
        const char *maxPlayers = config->GetString( "maxPlayers" );
        const char *password = config->GetString( "password" );

        // TODO: Save preferences for map and gamemode (copy logic from game\client\experiment\ui\createmultiplayergamedialog.cpp)
        // if ( m_pSavedData )
        //{
        //    if ( m_pServerPage->IsRandomMapSelected() )
        //    {
        //        // it's set to random map, just save an
        //        m_pSavedData->SetString( "map", "" );
        //    }
        //    else
        //    {
        //        m_pSavedData->SetString( "map", map );
        //    }

        //    // save config to a file
        //    m_pSavedData->SaveToFile( g_pFullFileSystem, "ServerConfig.vdf", "GAME" );
        //}

        // reset server enforced cvars
        g_pCVar->RevertFlaggedConVars( FCVAR_REPLICATED );

        // Cheats were disabled; revert all cheat cvars to their default values.
        // This must be done heading into multiplayer games because people can play
        // demos etc and set cheat cvars with sv_cheats 0.
        g_pCVar->RevertFlaggedConVars( FCVAR_CHEAT );

        char startCommand[1024];

        // create the command to execute
        Q_snprintf(
            startCommand,
            sizeof( startCommand ),
            "disconnect\n"
            "wait\n"
            "wait\n"
            "sv_lan 1\n"
            "setmaster enable\n"

            "maxplayers %i\n"
            "sv_password \"%s\"\n"
            "gamemode \"%s\"\n"
            "hostname \"%s\"\n"
            "progress_enable\n"
            "map %s\n",
            maxPlayers,
            password,
            gamemode,
            hostName,
            map );

        engine->ClientCmd_Unrestricted( startCommand );

        KeyValues *pResponse = new KeyValues( "parameters" );
        pResponse->SetBool( "wasSuccessful", true );
        CallJavascriptObjectCallback( callbackId, pResponse );
    }
    else if ( Q_strcmp( pszProperty, "RequestServerList" ) == 0 )
    {
        RequestServerList();

        // Fetch all favorited games
        ISteamMatchmaking *steamMM = SteamMatchmaking();
        int favoriteGameCount = steamMM->GetFavoriteGameCount();

        for ( size_t i = 0; i < favoriteGameCount; i++ )
        {
            AppId_t appId;
            uint32 ip;
            uint16 port;
            uint16 portQuery;
            uint32 flags;
            uint32 timeLastPlayed;

            if ( steamMM->GetFavoriteGame( i, &appId, &ip, &port, &portQuery, &flags, &timeLastPlayed ) )
            {
                KeyValues *pServerInfo = new KeyValues( "" );

                // This is the minimal data that will be displayed, once the server list refresh gathers
                // more data, the HTML will update the row.
                pServerInfo->SetInt( "serverAppId", appId );
                pServerInfo->SetInt( "serverIp", ip );
                pServerInfo->SetInt( "serverPort", port );
                pServerInfo->SetInt( "serverPortQuery", portQuery );

                if ( flags & k_unFavoriteFlagFavorite )
                {
                    pServerInfo->SetString( "serverTab", "favorites" );
                }
                else if ( flags & k_unFavoriteFlagHistory )
                {
                    pServerInfo->SetString( "serverTab", "history" );
                }

                char szJson[8096];
                CJsonToKeyValues::ConvertKeyValuesToJson( pServerInfo, szJson, sizeof( szJson ) );

                char szScript[8096];
                Q_snprintf( szScript, sizeof( szScript ), "ServerListAdd(%s);", szJson );

                RunJavascript( szScript );
            }
        }

        KeyValues *pResponse = new KeyValues( "parameters" );
        pResponse->SetBool( "wasSuccessful", true );
        CallJavascriptObjectCallback( callbackId, pResponse );
    }
    else if ( Q_strcmp( pszProperty, "CancelServerListRequest" ) == 0 )
    {
        ISteamMatchmakingServers *steamMM = steamapicontext->SteamMatchmakingServers();
        steamMM->CancelQuery( m_hSteamRequest );
    }
    else if ( Q_strcmp( pszProperty, "ModifyFavoriteGame" ) == 0 )
    {
        KeyValues *pArguments = pData->FindKey( "arguments" );
        KeyValues *serverToAdd = pArguments->FindKey( "1" );

        int appId = serverToAdd->GetInt( "appId" );
        int ip = serverToAdd->GetInt( "ip" );
        int port = serverToAdd->GetInt( "port" );
        int portQuery = serverToAdd->GetInt( "portQuery" );
        bool isHistory = serverToAdd->GetBool( "isHistory", false );
        bool isRemoving = serverToAdd->GetBool( "isRemoving", false );

        ISteamMatchmaking *steamMM = SteamMatchmaking();
        bool wasSuccessful;

        if ( !isRemoving )
        {
            steamMM->AddFavoriteGame(
                appId,
                ip,
                port,
                portQuery,
                isHistory ? k_unFavoriteFlagHistory : k_unFavoriteFlagFavorite,
                time( NULL ) );
            wasSuccessful = true;
        }
        else
        {
            wasSuccessful = steamMM->RemoveFavoriteGame(
                appId,
                ip,
                port,
                portQuery,
                isHistory ? k_unFavoriteFlagHistory : k_unFavoriteFlagFavorite );
        }

        KeyValues *pResponse = new KeyValues( "parameters" );
        pResponse->SetBool( "wasSuccessful", wasSuccessful );
        CallJavascriptObjectCallback( callbackId, pResponse );
    }
    else if ( Q_strcmp( pszProperty, "ConnectToServer" ) == 0 )
    {
        KeyValues *pArguments = pData->FindKey( "arguments" );
        KeyValues *serverToConnect = pArguments->FindKey( "1" );

        const char *address = serverToConnect->GetString( "address" );

        char command[256];
        Q_snprintf( command, sizeof( command ), "connect %s\n", address );
        engine->ClientCmd_Unrestricted( command );

        KeyValues *pResponse = new KeyValues( "parameters" );
        pResponse->SetBool( "wasSuccessful", true );
        CallJavascriptObjectCallback( callbackId, pResponse );
    }
}

// Source: https://github.com/NeotokyoRebuild/neo/blob/1d2dc708d1d950ba5a4eb20f95280f0bf8e0d4b7/src/game/client/neo/ui/neo_root_serverbrowser.cpp#L92
void MainMenuHTML::RequestServerList( GameServerType iType )
{
    static MatchMakingKeyValuePair_t mmFilters[] = {
        // { "hasplayers", "" },  // Uncomment this and comment the next line for testing with many servers
        { "gamedir", "experiment" },
    };
    MatchMakingKeyValuePair_t *pMMFilters = mmFilters;

    static constexpr HServerListRequest ( ISteamMatchmakingServers::*P_FN_REQ[GS__TOTAL] )(
        AppId_t, MatchMakingKeyValuePair_t **, uint32, ISteamMatchmakingServerListResponse * ) = {
        &ISteamMatchmakingServers::RequestInternetServerList,
        nullptr,
        &ISteamMatchmakingServers::RequestFriendsServerList,
        &ISteamMatchmakingServers::RequestFavoritesServerList,
        &ISteamMatchmakingServers::RequestHistoryServerList,
        &ISteamMatchmakingServers::RequestSpectatorServerList,
    };

    ISteamMatchmakingServers *steamMM = steamapicontext->SteamMatchmakingServers();
    m_hSteamRequest = ( iType == GS_LAN )
                          ? steamMM->RequestLANServerList( engine->GetAppID(), this )
                          : ( steamMM->*P_FN_REQ[iType] )( engine->GetAppID(), &pMMFilters, ARRAYSIZE( mmFilters ), this );
    m_iRequestType = iType;
    m_bIsSearching = true;
}

// Server has responded ok with updated data
void MainMenuHTML::ServerResponded( HServerListRequest hRequest, int iServer )
{
    if ( hRequest != m_hSteamRequest ) return;

    ISteamMatchmakingServers *steamMM = steamapicontext->SteamMatchmakingServers();
    gameserveritem_t *pServerDetails = steamMM->GetServerDetails( hRequest, iServer );
    if ( pServerDetails )
    {
        AddServerToList( m_iRequestType, *pServerDetails );
    }
}

// Server has failed to respond
void MainMenuHTML::ServerFailedToRespond( HServerListRequest hRequest, [[maybe_unused]] int iServer )
{
    if ( hRequest != m_hSteamRequest ) return;

    RunJavascript( "ServerListFailed('ServerFailedToRespond...')" );
}

// A list refresh you had initiated is now 100% completed
void MainMenuHTML::RefreshComplete( HServerListRequest hRequest, EMatchMakingServerResponse response )
{
    if ( hRequest != m_hSteamRequest ) return;

    m_bIsSearching = false;
    if ( response == eNoServersListedOnMasterServer )
    {
        RunJavascript( "ServerListComplete(false, 'No servers listed...')" );
    }
}

void MainMenuHTML::AddServerToList( GameServerType iType, const gameserveritem_t &serverInfo )
{
    KeyValues *pServerInfo = new KeyValues( "" );

    // Experiment; Since KeyValues cache keys we have to be unique or the JSON will have unexpected key capitalization
    pServerInfo->SetString( "serverAddress", serverInfo.m_NetAdr.GetConnectionAddressString() );
    pServerInfo->SetInt( "serverIp", serverInfo.m_NetAdr.GetIP() );
    pServerInfo->SetInt( "serverPort", serverInfo.m_NetAdr.GetConnectionPort() );
    // pServerInfo->SetString( "serverAddressQuery", serverInfo.m_NetAdr.GetQueryAddressString() );
    // pServerInfo->SetInt( "serverIpQuery", serverInfo.m_NetAdr.GetIP() );
    pServerInfo->SetInt( "serverPortQuery", serverInfo.m_NetAdr.GetQueryPort() );
    pServerInfo->SetInt( "serverPing", serverInfo.m_nPing );
    pServerInfo->SetString( "serverHostName", serverInfo.GetName() );
    pServerInfo->SetString( "serverGameDirectory", serverInfo.m_szGameDir );
    pServerInfo->SetString( "serverMap", serverInfo.m_szMap );
    pServerInfo->SetString( "serverGameDescription", serverInfo.m_szGameDescription );
    pServerInfo->SetInt( "serverAppId", serverInfo.m_nAppID );
    pServerInfo->SetInt( "serverPlayers", serverInfo.m_nPlayers );
    pServerInfo->SetInt( "serverMaxPlayers", serverInfo.m_nMaxPlayers );
    pServerInfo->SetInt( "serverBotPlayers", serverInfo.m_nBotPlayers );
    pServerInfo->SetBool( "serverHasPassword", serverInfo.m_bPassword );
    pServerInfo->SetBool( "serverIsSecure", serverInfo.m_bSecure );
    pServerInfo->SetInt( "serverTimeLastPlayed", serverInfo.m_ulTimeLastPlayed );
    pServerInfo->SetInt( "serverVersion", serverInfo.m_nServerVersion );
    pServerInfo->SetString( "serverGameTags", serverInfo.m_szGameTags );
    pServerInfo->SetString( "serverSteamId", serverInfo.m_steamID.Render() );

    if ( iType == GameServerType::GS_FAVORITES )
    {
        pServerInfo->SetString( "serverTab", "favorites" );
    }
    else if ( iType == GameServerType::GS_FRIENDS )
    {
        pServerInfo->SetString( "serverTab", "friends" );
    }
    else if ( iType == GameServerType::GS_HISTORY )
    {
        pServerInfo->SetString( "serverTab", "history" );
    }
    else if ( iType == GameServerType::GS_INTERNET )
    {
        pServerInfo->SetString( "serverTab", "internet" );
    }
    else if ( iType == GameServerType::GS_LAN )
    {
        pServerInfo->SetString( "serverTab", "lan" );
    }
    else if ( iType == GameServerType::GS_SPEC )
    {
        pServerInfo->SetString( "serverTab", "spectator" );
    }

    char szJson[8096];
    CJsonToKeyValues::ConvertKeyValuesToJson( pServerInfo, szJson, sizeof( szJson ) );

    char szScript[8096];
    Q_snprintf( szScript, sizeof( szScript ), "ServerListAdd(%s);", szJson );

    RunJavascript( szScript );
}

void CBaseMenuPanel::OnThink()
{
    BaseClass::OnThink();

    UpdateBackgroundState();

    if ( ipanel() )
    {
        int screenWide, screenTall;
        engine->GetScreenSize( screenWide, screenTall );
        SetBounds( 0, 0, screenWide, screenTall );
    }

    // Experiment; We had this so the options and console were always in front,
    //             but let's try find a less hacky way so we can have inputs
    //             in the HTML that we can focus.
    // surface()->MovePopupToBack( GetVPanel() );
}

void CBaseMenuPanel::OnCustomURLHandler( const char *pszUrl )
{
    const char *pszMainMenu = "mainmenu://";
    const char *pszGameMenuCommand = "gamemenucommand://";

    if ( Q_strncmp( pszUrl, pszGameMenuCommand, Q_strlen( pszGameMenuCommand ) ) == 0 )
    {
        char szCommand[256];
        const char *pszCommand = pszUrl + Q_strlen( pszGameMenuCommand );
        Q_snprintf( szCommand, sizeof( szCommand ), "gamemenucommand %s", pszCommand );

        if ( OnKnownCommand( pszCommand ) )
            return;

        engine->ClientCmd( szCommand );
    }
    else if ( Q_strncmp( pszUrl, pszMainMenu, Q_strlen( pszMainMenu ) ) == 0 )
    {
        DevWarning( "TODO: Main menu action: %s\n", pszUrl );
    }
}

bool CBaseMenuPanel::OnKnownCommand( const char *command )
{
    // if ( !Q_stricmp( command, "OpenCreateMultiplayerGameDialog" ) )
    //{
    //     OnOpenCreateMultiplayerGameDialog();
    //     return true;
    // }

    return false;
}

void CBaseMenuPanel::OnKeyCodeUnhandled( int code )
{
    if ( !m_pHTML )
        return;

    if ( code == KEY_1 )
    {
        // Let's make the menu development a bit easier by refreshing the main menu easily
        m_pHTML->Refresh();
    }
    else if ( code == KEY_2 )
    {
        m_pHTML->OpenDeveloperTools();
    }
}
