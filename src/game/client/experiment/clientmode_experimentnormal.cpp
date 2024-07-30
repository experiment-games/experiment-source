//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Draws the normal TF2 or HL2 HUD.
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "clientmode_experimentnormal.h"
#include "vgui_int.h"
#include "hud.h"
#include <vgui/IInput.h>
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui_controls/AnimationController.h>
#include "iinput.h"
#include "experimentclientscoreboard.h"
#include "experimenttextwindow.h"
#include "ienginevgui.h"
#ifdef LUA_SDK
#include "scriptedhudviewport.h"
#include "scriptedclientluapanel.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
vgui::HScheme g_hVGuiCombineScheme = 0;

// Instance the singleton and expose the interface to it.
IClientMode* GetClientModeNormal()
{
    static ClientModeExperimentNormal g_ClientModeNormal;
    return &g_ClientModeNormal;
}

ClientModeExperimentNormal* GetClientModeExperimentNormal()
{
    Assert( dynamic_cast< ClientModeExperimentNormal* >( GetClientModeNormal() ) );

    return static_cast< ClientModeExperimentNormal* >( GetClientModeNormal() );
}

//-----------------------------------------------------------------------------
// Purpose: this is the viewport that contains all the hud elements
//-----------------------------------------------------------------------------
class CHudViewport : public CBaseViewport
{
   private:
    DECLARE_CLASS_SIMPLE( CHudViewport, CBaseViewport );

   protected:
    virtual void ApplySchemeSettings( vgui::IScheme* pScheme )
    {
        BaseClass::ApplySchemeSettings( pScheme );

        gHUD.InitColors( pScheme );

        SetPaintBackgroundEnabled( false );
    }

    virtual IViewPortPanel* CreatePanelByName( const char* szPanelName );
};

int ClientModeExperimentNormal::GetDeathMessageStartHeight( void )
{
    return m_pViewport->GetDeathMessageStartHeight();
}

IViewPortPanel* CHudViewport::CreatePanelByName( const char* szPanelName )
{
    IViewPortPanel* newpanel = NULL;

    if ( Q_strcmp( PANEL_SCOREBOARD, szPanelName ) == 0 )
    {
        newpanel = new CExperimentClientScoreBoardDialog( this );
        return newpanel;
    }
    else if ( Q_strcmp( PANEL_INFO, szPanelName ) == 0 )
    {
        newpanel = new CExperimentTextWindow( this );
        return newpanel;
    }
    else if ( Q_strcmp( PANEL_SPECGUI, szPanelName ) == 0 )
    {
        newpanel = new CExperimentSpectatorGUI( this );
        return newpanel;
    }

    return BaseClass::CreatePanelByName( szPanelName );
}

//-----------------------------------------------------------------------------
// ClientModeHLNormal implementation
//-----------------------------------------------------------------------------
ClientModeExperimentNormal::ClientModeExperimentNormal()
{
#ifdef LUA_SDK
    m_pScriptedViewport = new CScriptedHudViewport( LGameUI );
#endif
    m_pViewport = new CHudViewport();
    m_pViewport->Start( gameuifuncs, gameeventmanager );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
ClientModeExperimentNormal::~ClientModeExperimentNormal()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ClientModeExperimentNormal::Init()
{
    BaseClass::Init();

    // Load up the combine control panel scheme
    g_hVGuiCombineScheme = vgui::scheme()->LoadSchemeFromFileEx( enginevgui->GetPanel( PANEL_CLIENTDLL ), "resource/CombinePanelScheme.res", "CombineScheme" );
    if ( !g_hVGuiCombineScheme )
    {
        Warning( "Couldn't load combine panel scheme!\n" );
    }
}
