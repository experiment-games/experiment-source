//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "vgui_int.h"
#include "ienginevgui.h"

#ifdef LUA_SDK
#include "vgui/IVgui.h"
#include "vgui_rootpanel_hl2.h"
#include "clientmode_shared.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef LUA_SDK
// Experiment;  This is commented since it got in the way of our MainMenuUI and
//              we didn't seem to use it anywhere anyway.
//C_ScriptedBaseGameUIPanel *g_pScriptedBaseGameUIPanel = NULL;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
//void VGUI_CreateGameUIRootPanel( void )
//{
//    g_pScriptedBaseGameUIPanel =
//        new C_ScriptedBaseGameUIPanel( enginevgui->GetPanel( PANEL_GAMEUIDLL ) );
//}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
//void VGUI_DestroyGameUIRootPanel( void )
//{
//    delete g_pScriptedBaseGameUIPanel;
//    g_pScriptedBaseGameUIPanel = NULL;
//}
#endif

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void VGUI_CreateClientDLLRootPanel( void )
{
    // Just using PANEL_ROOT in HL2 right now
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void VGUI_DestroyClientDLLRootPanel( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Game specific root panel
// Output : vgui::Panel
//-----------------------------------------------------------------------------
vgui::VPANEL VGui_GetClientDLLRootPanel( void )
{
    vgui::VPANEL root = enginevgui->GetPanel( PANEL_CLIENTDLL );
    return root;
}

#ifdef LUA_SDK
//vgui::Panel *VGui_GetGameUIPanel( void )
//{
//    return g_pScriptedBaseGameUIPanel;
//}

//-----------------------------------------------------------------------------
// Purpose: Game specific root panel
// Output : vgui::Panel
//-----------------------------------------------------------------------------
CScriptedClientLuaPanel *VGui_GetClientLuaRootPanel( void )
{
    return g_pClientLuaPanel;
}

CScriptedClientLuaPanel *VGui_GetClientLuaRootPanelHUD( void )
{
    return g_pClientLuaPanelHUD;
}
#endif

//-----------------------------------------------------------------------------
// C_ScriptedBaseGameUIPanel implementation.
//-----------------------------------------------------------------------------
C_ScriptedBaseGameUIPanel::C_ScriptedBaseGameUIPanel( vgui::VPANEL parent )
    : BaseClass( NULL, "ScriptedBaseGameUIPanel" )
{
    // Experiment; We always want a reference to the panel, so it isn't
    // removed by the garbage collector.
    m_nRefCount = 1;

    SetParent( parent );
    SetPaintEnabled( false );
    SetPaintBorderEnabled( false );
    SetPaintBackgroundEnabled( false );

    // This panel does post child painting
    SetPostChildPaintEnabled( true );

    // Make it screen sized
    SetBounds( 0, 0, ScreenWidth(), ScreenHeight() );

    // Ask for OnTick messages
    vgui::ivgui()->AddTickSignal( GetVPanel() );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
C_ScriptedBaseGameUIPanel::~C_ScriptedBaseGameUIPanel( void )
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_ScriptedBaseGameUIPanel::PostChildPaint()
{
    BaseClass::PostChildPaint();

    // Draw all panel effects
    RenderPanelEffects();
}

//-----------------------------------------------------------------------------
// Purpose: For each panel effect, check if it wants to draw and draw it on
//  this panel/surface if so
//-----------------------------------------------------------------------------
void C_ScriptedBaseGameUIPanel::RenderPanelEffects( void )
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_ScriptedBaseGameUIPanel::OnTick( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Reset effects on level load/shutdown
//-----------------------------------------------------------------------------
void C_ScriptedBaseGameUIPanel::LevelInit( void )
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_ScriptedBaseGameUIPanel::LevelShutdown( void )
{
}
