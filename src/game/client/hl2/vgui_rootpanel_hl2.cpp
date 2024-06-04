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
C_ScriptedBaseGameUIPanel *g_pScriptedBaseGameUIPanel = NULL;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void VGUI_CreateGameUIRootPanel(void) {
    g_pScriptedBaseGameUIPanel =
        new C_ScriptedBaseGameUIPanel(enginevgui->GetPanel(PANEL_GAMEUIDLL));
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void VGUI_DestroyGameUIRootPanel(void) {
    delete g_pScriptedBaseGameUIPanel;
    g_pScriptedBaseGameUIPanel = NULL;
}
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
