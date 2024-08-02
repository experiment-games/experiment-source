#include "vgui_controls/Panel.h"
#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/HTML.h"
#include "GameUI/IGameUI.h"

using namespace vgui;

class CMainMenu : public EditablePanel
{
    DECLARE_CLASS_SIMPLE( CMainMenu, EditablePanel );

   public:
    CMainMenu( VPANEL parent );
    virtual ~CMainMenu();

   protected:
    virtual void ApplySchemeSettings( IScheme* pScheme );

    MESSAGE_FUNC_CHARPTR( OnCustomURLHandler, "CustomURL", url );
    MESSAGE_FUNC_ENUM_ENUM( OnRequestFocus, "OnRequestFocus", VPANEL, subFocus, VPANEL, defaultPanel );
    MESSAGE_FUNC_INT( OnKeyCodeReleased, "KeyCodeReleased", code );

   private:
    bool LoadGameUI();

    int m_ExitingFrameCount;
    bool m_bCopyFrameBuffer;

    IGameUI* m_pGameUI;

    HTML* m_pHTML;
};

void InitMainMenu( VPANEL parent = NULL );
void DestroyMainMenu();

extern CMainMenu* g_MainMenuPanel;
