#include "cbase.h"
#include "mapload_background.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

// Source: https://developer.valvesoftware.com/wiki/Custom_loading_screen
CMapLoadBG::CMapLoadBG( char const* panelName )
    : EditablePanel( NULL, panelName )
{
    VPANEL toolParent = enginevgui->GetPanel( PANEL_GAMEUIDLL );
    SetParent( toolParent );

    // Fenix: We load a RES file rather than create the element here for taking advantage of the "F" parameter for wide and tall
    // Is the sole thing that makes fill the background to the entire screen regardless of the texture size
    // Congratulations to Valve for once again give options to only one side and not both
    LoadControlSettings( "resource/loadingdialogbackground.res" );

    m_pBackground = FindControl< ImagePanel >( "LoadingImage", true );
}

CMapLoadBG::~CMapLoadBG()
{
}

void CMapLoadBG::ApplySchemeSettings( IScheme* pScheme )
{
    BaseClass::ApplySchemeSettings( pScheme );

    int iWide, iTall;
    surface()->GetScreenSize( iWide, iTall );
    SetSize( iWide, iTall );
}

void CMapLoadBG::SetNewBackgroundImage( char const* imageName )
{
    m_pBackground->SetImage( imageName );
}

void CMapLoadBG::OnFileReceived( const char* fileName, unsigned int transferID )
{
    Warning( "File received: %s (%d)\n", fileName, transferID );
}

// URAKOLOUY5: Recursively find all elements with this name and unparent them
void SetVisibilityRecursive( vgui::VPANEL parentPanel, const char* targetPanelName, int depth = 0 )
{
    int numChildren = vgui::ipanel()->GetChildCount( parentPanel );

    for ( int i = 0; i < numChildren; i++ )
    {
        vgui::VPANEL childPanel = vgui::ipanel()->GetChild( parentPanel, i );
        const char* childPanelName = vgui::ipanel()->GetName( childPanel );

        if ( V_strcmp( childPanelName, targetPanelName ) == 0 )
            vgui::ipanel()->SetParent( childPanel, NULL );

        SetVisibilityRecursive( childPanel, targetPanelName, depth + 1 );
    }
}

// URAKOLOUY5: Override OnMessage to detect "Activate" message and call SetVisibilityRecursive
void CMapLoadBG::OnMessage( const KeyValues* params, vgui::VPANEL fromPanel )
{
    if ( V_strcmp( params->GetName(), "Activate" ) == 0 )
    {
        //vgui::VPANEL rootPanel = enginevgui->GetPanel( PANEL_GAMEUIDLL );
        //SetVisibilityRecursive( rootPanel, "LoadingDialog" );
    }

    // Call the base class OnMessage to handle messages we didn't process
    BaseClass::OnMessage( params, fromPanel );
}
