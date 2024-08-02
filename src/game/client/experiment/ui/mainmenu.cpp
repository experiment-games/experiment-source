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

using namespace vgui;

// Based on: https://developer.valvesoftware.com/wiki/Override_GameUI
// See interface.h/.cpp for specifics: basically this ensures that we actually Sys_UnloadModule the dll and that we don't call Sys_LoadModule
//  over and over again.
static CDllDemandLoader g_GameUIDLL( "GameUI" );

CMainMenu *g_MainMenuPanel = NULL;

void InitMainMenu( vgui::VPANEL parent )
{
    g_MainMenuPanel = new CMainMenu( parent );
}

void DestroyMainMenu()
{
    if ( !g_MainMenuPanel )
        return;

    g_MainMenuPanel->SetParent( ( vgui::Panel * )NULL );
    delete g_MainMenuPanel;
    g_MainMenuPanel = NULL;
}

/// <summary>
/// Prevents the Console from dissapearing behind the main menu panel on
/// bootup. Or when the user clicks on the main menu panel.
/// </summary>
static void BringConsoleToFrontIfNeeded()
{
    auto gameUIFactory = g_GameUIDLL.GetFactory();
    auto console = ( IGameConsole * )gameUIFactory( GAMECONSOLE_INTERFACE_VERSION, NULL );

    if ( !console )
        return;

    if ( console->IsConsoleVisible() )
        console->Activate();
}

CMainMenu::CMainMenu( VPANEL parent )
    : BaseClass( NULL, "MainMenuUI" )
{
    SetAutoDelete( false );
    SetVisible( true );
    SetParent( parent );
    SetKeyBoardInputEnabled( true );

    m_bCopyFrameBuffer = false;
    m_pGameUI = NULL;
    m_ExitingFrameCount = 0;

    // Add a HTML panel that takes up the whole screen
    // This is where the main menu will be displayed
    m_pHTML = new HTML( this, "HTML" );
    m_pHTML->AddCustomURLHandler( "mainmenu", this );
    m_pHTML->AddCustomURLHandler( "gamemenucommand", this );
    m_pHTML->OpenURL( "asset://experiment/menus/main.html", NULL );

    if ( LoadGameUI() )
    {
        m_pGameUI->SetMainMenuOverride( this->GetVPanel() );
        MakePopup( false );  // Without this, closing the console crashes the game :/
    }

    BringConsoleToFrontIfNeeded();
}

CMainMenu::~CMainMenu()
{
    m_pGameUI->SetMainMenuOverride( NULL );
    m_pGameUI = NULL;
    g_GameUIDLL.Unload();
}

bool CMainMenu::LoadGameUI()
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

void CMainMenu::ApplySchemeSettings( IScheme *pScheme )
{
    BaseClass::ApplySchemeSettings( pScheme );

    int wide, tall;
    vgui::surface()->GetScreenSize( wide, tall );
    SetSize( wide, tall );

    m_pHTML->SetBounds( 0, 0, wide, tall );
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

void CMainMenu::OnRequestFocus( VPANEL subFocus, VPANEL defaultPanel )
{
    BaseClass::OnRequestFocus( GetVPanel(), defaultPanel );
    BringConsoleToFrontIfNeeded();
}

void CMainMenu::OnKeyCodeReleased( int code )
{
    if ( code == KEY_1 )
    {
        // Let's make the menu development a bit easier by refreshing the main menu easily
        m_pHTML->Refresh();
    }
}
