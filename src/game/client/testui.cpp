#include "cbase.h"
#include "convar.h"

using namespace vgui;
#include <ienginevgui.h>
#include <vgui/IVGui.h>
#include <vgui_controls/HTML.h>
#include <vgui_controls/Frame.h>

class CTestPanel : public vgui::Frame
{
    DECLARE_CLASS_SIMPLE( CTestPanel, vgui::Frame );

    CTestPanel( vgui::VPANEL parent );
    ~CTestPanel();

    MESSAGE_FUNC( OnTextChanged, "TextChanged" );
    MESSAGE_FUNC_PARAMS( OnURLChanged, "OnURLChanged", data );

   protected:
    virtual void OnTick();
    virtual void OnCommand( const char* pcCommand );

   private:
    vgui::HTML* m_pHTML;
    vgui::TextEntry* m_pUrlTextEntry;
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

    m_pHTML = new vgui::HTML( this, "TestHTML" );
    m_pUrlTextEntry = new vgui::TextEntry( this, "UrlTextEntry" );

    LoadControlSettings( "resource/UI/testui.res" );

    DevMsg( "TestUI has been constructed\n" );

    m_pUrlTextEntry->SetText( "http://neverssl.com" );
    m_pHTML->OpenURL( "http://neverssl.com", NULL );

    m_pUrlTextEntry->AddActionSignalTarget( this );

    vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );
}

CTestPanel::~CTestPanel()
{
    ivgui()->RemoveTickSignal( GetVPanel() );
}

void CTestPanel::OnTextChanged()
{
    char url[MAX_PATH];
    m_pUrlTextEntry->GetText( url, sizeof(url));

    m_pHTML->OpenURL( url, NULL );
}

void CTestPanel::OnURLChanged( KeyValues* data )
{
    if ( m_pUrlTextEntry->HasFocus() )
        return;
    /*
    pMessage->SetString( "url", url );
    pMessage->SetString( "postdata", pchPostData );
    pMessage->SetInt( "isredirect", bIsRedirect ? 1 : 0 );
    */
    m_pUrlTextEntry->SetText( data->GetString( "url" ) );
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
