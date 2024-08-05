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
    return IsInBackgroundLevel() || g_BaseMenuPanel->GetMenuBackgroundState() == BACKGROUND_DISCONNECTED;
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
    // else if ( m_bEverActivated )
    //{
    //     SetBackgroundRenderState( BACKGROUND_DISCONNECTED );
    // }
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
}

CMainMenu::CMainMenu( Panel *pParent )
    : BaseClass( pParent, "MainMenuPanel" )
{
    MakePopup( false );
    SetProportional( true );
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
    m_pHTML = new HTML( this, "MainMenuHTML", true );
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
    if ( state == BACKGROUND_DISCONNECTED || state == BACKGROUND_MAINMENU )
    {
        if ( state == BACKGROUND_MAINMENU )
        {
            if ( m_pHTML )
                m_pHTML->RunJavascript( "SetBackgroundRenderState( BACKGROUND_MAINMENU )" );
        }
    }
    else if ( state == BACKGROUND_LOADING )
    {
        if ( m_pHTML )
            m_pHTML->RunJavascript( "SetBackgroundRenderState( BACKGROUND_LOADING )" );

        SetAlpha( 0 );
    }
    else if ( state == BACKGROUND_LEVEL )
    {
        if ( m_pHTML )
            m_pHTML->RunJavascript( "SetBackgroundRenderState( BACKGROUND_LEVEL )" );

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
        Q_snprintf( szCommand, sizeof( szCommand ), "gamemenucommand %s", pszUrl + Q_strlen( pszGameMenuCommand ) );

        engine->ClientCmd( szCommand );
    }
    else if ( Q_strncmp( pszUrl, pszMainMenu, Q_strlen( pszMainMenu ) ) == 0 )
    {
        DevWarning( "TODO: Main menu action: %s\n", pszUrl );
    }
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
