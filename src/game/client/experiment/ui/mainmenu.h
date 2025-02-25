#include "vgui_controls/Panel.h"
#include "vgui_controls/EditablePanel.h"
#include <game/client/iviewport.h>
#include "vgui_controls/HTML.h"
#include "GameUI/IGameUI.h"
#include "GameEventListener.h"

using namespace vgui;

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

class CBaseMenuPanel : public EditablePanel
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

   protected:
    void OnThink() OVERRIDE;

   private:
    void SetBackgroundRenderState( EBackgroundState state );
    bool LoadGameUI();
    void UpdateBackgroundState();
    EBackgroundState m_eBackgroundState;

    IGameUI* m_pGameUI;

    CMainMenu* m_pMainMenu;
};

class CMainMenu : public Panel
{
    DECLARE_CLASS_SIMPLE( CMainMenu, Panel );

   public:
    CMainMenu( Panel* pParent );
    virtual ~CMainMenu();

    void SetBackgroundRenderState( EBackgroundState state );
    virtual bool OnKnownCommand( const char* command );

    MESSAGE_FUNC_CHARPTR( OnCustomURLHandler, "CustomURL", url );
    MESSAGE_FUNC_INT( OnKeyCodeUnhandled, "KeyCodeUnhandled", code );

   protected:
    void OnThink() OVERRIDE;

    virtual void PerformLayout() OVERRIDE;

    void OnOpenCreateMultiplayerGameDialog();
    vgui::DHANDLE< vgui::Frame > m_hCreateMultiplayerGameDialog;

   private:
    HTML* m_pHTML;
};

extern CBaseMenuPanel* g_BaseMenuPanel;
