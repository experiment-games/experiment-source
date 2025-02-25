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

using namespace vgui;

// Based on: https://developer.valvesoftware.com/wiki/Override_GameUI
// See interface.h/.cpp for specifics: basically this ensures that we actually Sys_UnloadModule the dll and that we don't call Sys_LoadModule
//  over and over again.
static CDllDemandLoader g_GameUIDLL( "GameUI" );

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

    SetParent( enginevgui->GetPanel( PANEL_GAMEUIDLL ) );

    SetBounds( 0, 0, 640, 480 );
    SetPaintBorderEnabled( false );
    SetPaintBackgroundEnabled( true );
    SetPaintEnabled( true );

    SetMouseInputEnabled( true );
    SetKeyBoardInputEnabled( true );

    SetVisible( true );
    SetAutoDelete( false );
    SetProportional( true );
    SetCursor( dc_arrow );

    m_pGameUI = NULL;
    m_eBackgroundState = BACKGROUND_INITIAL;

    // Add a HTML panel that takes up the whole screen
    // This is where the main menu will be displayed
    m_pMainMenu = new CMainMenu( this );

    // OnGameUIActivated();
}

CBaseMenuPanel::~CBaseMenuPanel()
{
    // m_pGameUI->SetMainMenuOverride( NULL );
    // m_pGameUI = NULL;
    g_GameUIDLL.Unload();
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
    // if ( LoadGameUI() )
    //{
    //     m_pGameUI->SetMainMenuOverride( GetVPanel() );
    // }
}

bool CBaseMenuPanel::LoadGameUI()
{
    if ( !m_pGameUI )
    {
        CreateInterfaceFn gameUIFactory = g_GameUIDLL.GetFactory();

        if ( gameUIFactory )
        {
            m_pGameUI = ( IGameUI * )gameUIFactory( GAMEUI_INTERFACE_VERSION, NULL );

            if ( !m_pGameUI )
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

void CBaseMenuPanel::SetBackgroundRenderState( EBackgroundState state )
{
    if ( state == m_eBackgroundState )
    {
        return;
    }

    m_eBackgroundState = state;

    if ( m_pMainMenu )
    {
        m_pMainMenu->SetBackgroundRenderState( state );
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

class MainMenuHTML : public HTML
{
    DECLARE_CLASS_SIMPLE( MainMenuHTML, HTML );

   public:
    MainMenuHTML( Panel *parent, const char *name, bool allowJavaScript = true )
        : HTML( parent, name, allowJavaScript )
    {
    }

   protected:
    virtual void OnInstallJavaScriptInterop() OVERRIDE
    {
        AddJavascriptObject( "GameUI" );
        AddJavascriptObjectCallback( "GameUI", "LoadMountableContentInfo" );
        AddJavascriptObjectCallback( "GameUI", "MountGameContent" );
        AddJavascriptObjectCallback( "GameUI", "UnmountGameContent" );
    }

    virtual void OnJavaScriptCallback( KeyValues *pData ) OVERRIDE
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
    }
};

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
}

CMainMenu::CMainMenu( Panel *pParent )
    : BaseClass( pParent, "MainMenuPanel" )
{
    MakePopup( false );
    SetProportional( false );
    SetMouseInputEnabled( true );
    SetKeyBoardInputEnabled( true );
    SetPaintBorderEnabled( false );
    SetPaintBackgroundEnabled( false );
    AddActionSignalTarget( this );
    SetZPos( 0 );

    // Set our initial size
    int screenWide, screenTall;
    surface()->GetScreenSize( screenWide, screenTall );
    SetBounds( 0, 0, screenWide, screenTall );

    // Add a HTML panel that takes up the whole screen
    // This is where the main menu will be displayed
    m_pHTML = new MainMenuHTML( this, "MainMenuHTML" );
    m_pHTML->AddCustomURLHandler( "mainmenu", this );
    m_pHTML->AddCustomURLHandler( "gamemenucommand", this );
    m_pHTML->OpenURL( "asset://experiment/menus/main.html", NULL );

    MakeReadyForUse();
    InvalidateLayout( true );
    RequestFocus();
}

CMainMenu::~CMainMenu()
{
}

void CMainMenu::PerformLayout()
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

void CMainMenu::SetBackgroundRenderState( EBackgroundState state )
{
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

void CMainMenu::OnThink()
{
    BaseClass::OnThink();

    surface()->MovePopupToBack( GetVPanel() );
}

void CMainMenu::OnCustomURLHandler( const char *pszUrl )
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

bool CMainMenu::OnKnownCommand( const char *command )
{
    if ( !Q_stricmp( command, "OpenCreateMultiplayerGameDialog" ) )
    {
        OnOpenCreateMultiplayerGameDialog();
        return true;
    }

    return false;
}

void CMainMenu::OnOpenCreateMultiplayerGameDialog()
{
    if ( !m_hCreateMultiplayerGameDialog.Get() )
    {
        m_hCreateMultiplayerGameDialog = new CCreateMultiplayerGameDialog( this );
        GameUIUtil::PositionDialog( m_hCreateMultiplayerGameDialog );
    }

    m_hCreateMultiplayerGameDialog->Activate();
}

void CMainMenu::OnKeyCodeUnhandled( int code )
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
