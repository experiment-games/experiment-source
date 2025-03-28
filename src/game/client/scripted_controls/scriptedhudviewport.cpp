//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client DLL VGUI2 Viewport
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

// our definition
#include "scriptedhudviewport.h"

// lua hooks
#ifdef LUA_SDK
#include "luamanager.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//================================================================
CScriptedHudViewport::CScriptedHudViewport( lua_State* L )
    : vgui::EditablePanel( NULL, "CScriptedHudViewport" )
{
    m_lua_State = L;

    SetKeyBoardInputEnabled( false );
    SetMouseInputEnabled( false );

    SetProportional( true );
}

//-----------------------------------------------------------------------------
// Purpose: Sets the parent for each panel to use
//-----------------------------------------------------------------------------
void CScriptedHudViewport::SetParent( vgui::VPANEL parent )
{
    EditablePanel::SetParent( parent );
    // force ourselves to be proportional - when we set our parent above, if our new
    // parent happened to be non-proportional (such as the vgui root panel), we got
    // slammed to be nonproportional
    EditablePanel::SetProportional( true );
}

void CScriptedHudViewport::Paint()
{
    LUA_CALL_HOOK_BEGIN( "HudDraw" );
    LUA_CALL_HOOK_END( 0, 0 );
}
