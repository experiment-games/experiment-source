//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include <cbase.h>
#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <vgui_controls/lPanel.h>

#include <scripted_controls/lPanel.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

static int luasrc_Panel( lua_State *L )
{
    Panel *pPanel = new Panel(
        luaL_optpanel( L, 1, VGui_GetClientLuaRootPanel() ),
        luaL_optstring( L, 2, NULL ),
        L
    );
    lua_pushpanel( L, pPanel );
    return 1;
}

static const luaL_Reg Panel_funcs[] = {
    { "Panel", luasrc_Panel },
    { NULL, NULL } };

/*
** Open Panel object
*/
LUALIB_API int luaopen_vgui_Panel( lua_State *L )
{
    luaL_register( L, LUA_VGUILIBNAME, Panel_funcs );
    lua_pop( L, 1 );
    return 0;
}
