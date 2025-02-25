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
#include "scriptedclientluapanel.h"

// lua hooks
#ifdef LUA_SDK
#include "luamanager.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//================================================================
CScriptedClientLuaPanel::CScriptedClientLuaPanel( lua_State *L )
    : vgui::EditablePanel( NULL, "CScriptedClientLuaPanel" )
{
    m_lua_State = L;
    // Experiment; We always want a reference to the panel, so it isn't
    // removed by the garbage collector.
    m_nRefCount = 1;

    SetKeyBoardInputEnabled( false );
    SetMouseInputEnabled( false );

    SetProportional( true );
}

CScriptedClientLuaPanel::~CScriptedClientLuaPanel()
{
}

void CScriptedClientLuaPanel::CreateDefaultPanels( void )
{
    // Was a nice idea, but is called on game init and not level init
#if 0
    LUA_CALL_HOOK_BEGIN( "CreateDefaultPanels" );
    LUA_CALL_HOOK_END( 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: called when the VGUI subsystem starts up
//			Creates the sub panels and initialises them
//-----------------------------------------------------------------------------
void CScriptedClientLuaPanel::Start( IGameUIFuncs *pGameUIFuncs, IGameEventManager2 *pGameEventManager )
{
    CreateDefaultPanels();
}

//-----------------------------------------------------------------------------
// Purpose: Sets the parent for each panel to use
//-----------------------------------------------------------------------------
void CScriptedClientLuaPanel::SetParent( vgui::VPANEL parent )
{
    EditablePanel::SetParent( parent );
    // force ourselves to be proportional - when we set our parent above, if our new
    // parent happened to be non-proportional (such as the vgui root panel), we got
    // slammed to be nonproportional
    EditablePanel::SetProportional( true );
}
