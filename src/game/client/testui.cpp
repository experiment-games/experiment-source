#include "cbase.h"
#include "convar.h"

using namespace vgui;
#include <ienginevgui.h>
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>

class CTestPanel : public vgui::Frame
{
    DECLARE_CLASS_SIMPLE( CTestPanel, vgui::Frame );

    CTestPanel( vgui::VPANEL parent );
    ~CTestPanel(){};

   protected:
    virtual void OnTick();
    virtual void OnCommand( const char* pcCommand );

   private:
};

CTestPanel::CTestPanel( vgui::VPANEL parent )
    : BaseClass( NULL, "TestUI" )
{
    SetParent( parent );

    SetKeyBoardInputEnabled( true );
    SetMouseInputEnabled( true );

    SetProportional( false );
    SetTitleBarVisible( true );
    SetMinimizeButtonVisible( false );
    SetMaximizeButtonVisible( false );
    SetCloseButtonVisible( false );
    SetSizeable( true );
    SetMoveable( true );
    SetVisible( true );

    SetScheme( vgui::scheme()->LoadSchemeFromFile( "resource/SourceScheme.res", "SourceScheme" ) );

    LoadControlSettings( "resource/UI/testui.res" );

    vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );

    DevMsg( "TestUI has been constructed\n" );
}

class CTestPanelInterface
{
   private:
    CTestPanel* MyPanel;

   public:
    CTestPanelInterface()
    {
        MyPanel = NULL;
    }
    void Create( vgui::VPANEL parent )
    {
        MyPanel = new CTestPanel( parent );
    }
    void Destroy()
    {
        if ( MyPanel )
        {
            MyPanel->SetParent( ( vgui::Panel* )NULL );
            delete MyPanel;
        }
    }
};

static CTestPanelInterface g_MyPanel;
CTestPanelInterface* myPanel = ( CTestPanelInterface* )&g_MyPanel;

void CTestPanel::OnTick()
{
    BaseClass::OnTick();
}

void CTestPanel::OnCommand( const char* pcCommand )
{
    BaseClass::OnCommand( pcCommand );
}

CON_COMMAND( testui, "Open the testui" )
{
    if ( myPanel )
    {
        myPanel->Destroy();
    }

    myPanel->Create( enginevgui->GetPanel( PANEL_GAMEUIDLL ) );
}

CON_COMMAND( testui_close, "Close the testui" )
{
    myPanel->Destroy();
}
