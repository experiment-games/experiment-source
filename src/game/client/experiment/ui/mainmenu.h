#include "vgui_controls/Panel.h"
#include "vgui_controls/EditablePanel.h"
#include <game/client/iviewport.h>
#include "vgui_controls/HTML.h"
#include "GameUI/IGameUI.h"
#include "GameEventListener.h"

using namespace vgui;

// From NeotokyoRebuild
enum GameServerType
{
    GS_INTERNET = 0,
    GS_LAN,
    GS_FRIENDS,
    GS_FAVORITES,
    GS_HISTORY,
    GS_SPEC,

    GS__TOTAL,
};

enum EBackgroundState
{
    BACKGROUND_INITIAL,
    BACKGROUND_LOADING,
    BACKGROUND_MAINMENU,
    BACKGROUND_LEVEL,
    BACKGROUND_DISCONNECTED
};

namespace GameUIUtil
{
bool IsInLevel();
bool IsInBackgroundLevel();
bool IsInMenu();
void PositionDialog( vgui::PHandle dialog );
}  // namespace GameUIUtil

class CMainMenu;  // Forward declaration

class CBaseMenuPanel : public EditablePanel, public IViewPortPanel
{
    DECLARE_CLASS_SIMPLE( CBaseMenuPanel, EditablePanel );

   public:
    CBaseMenuPanel();
    virtual ~CBaseMenuPanel();

    static CBaseMenuPanel* Init();
    void AttachToGameUI();

    EBackgroundState GetMenuBackgroundState() const
    {
        return m_eBackgroundState;
    }

    // Never called if we don't use SetMainMenuOverride (which causes crash on closing console)
    // MESSAGE_FUNC( OnGameUIActivated, "OnGameUIActivated" );
    // MESSAGE_FUNC( OnGameUIHidden, "GameUIHidden" );

    // both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
    vgui::VPANEL GetVPanel( void )
    {
        return BaseClass::GetVPanel();
    }
    virtual bool IsVisible();
    virtual void SetParent( vgui::VPANEL parent )
    {
        BaseClass::SetParent( parent );
    }
    virtual const char* GetName( void )
    {
        return "MainMenuOverride";
    }
    virtual void SetData( KeyValues* data ) {}
    virtual void Reset()
    {
        Update();
        SetVisible( true );
    }
    virtual void Update()
    {
        return;
    }
    virtual bool NeedsUpdate( void )
    {
        return false;
    }
    virtual bool HasInputElements( void )
    {
        return true;
    }
    virtual void ShowPanel( bool bShow )
    {
        SetVisible( true );
    }  // Refuses to hide

    virtual GameActionSet_t GetPreferredActionSet()
    {
        return GAME_ACTION_SET_NONE;
    }  // Seems like this should be GAME_ACTION_SET_MENU, but it's not because it's apparently visible *all* *the* *time*

    void SetBackgroundRenderState( EBackgroundState state );
    virtual bool OnKnownCommand( const char* command );

    MESSAGE_FUNC_CHARPTR( OnCustomURLHandler, "CustomURL", url );
    MESSAGE_FUNC_INT( OnKeyCodeUnhandled, "KeyCodeUnhandled", code );

   protected:
    void OnThink() OVERRIDE;

    virtual void PerformLayout() OVERRIDE;

   private:
    void UpdateBackgroundState();
    EBackgroundState m_eBackgroundState;

    HTML* m_pHTML;
};

class MainMenuHTML : public HTML, public ISteamMatchmakingServerListResponse
{
    DECLARE_CLASS_SIMPLE( MainMenuHTML, HTML );

   public:
    MainMenuHTML( Panel *parent, const char *name, bool allowJavaScript = true )
        : HTML( parent, name, allowJavaScript )
    {
    }

    void RequestServerList( GameServerType iType = GameServerType::GS_INTERNET );

    // ISteamMatchmakingServerListResponse
    virtual void ServerResponded( HServerListRequest hRequest, int iServer );
    virtual void ServerFailedToRespond( HServerListRequest hRequest, int iServer );
    virtual void RefreshComplete( HServerListRequest hRequest, EMatchMakingServerResponse response );

   private:
    void AddServerToList( GameServerType iType, const gameserveritem_t &serverInfo );

    HServerListRequest m_hSteamRequest;
    GameServerType m_iRequestType;
    bool m_bIsSearching;

   protected:
    virtual void OnInstallJavaScriptInterop() OVERRIDE;
    virtual void OnJavaScriptCallback( KeyValues* pData ) OVERRIDE;
};

extern CBaseMenuPanel* g_BaseMenuPanel;
