#ifndef LPANEL_H
#define LPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>

using namespace vgui;

/* type for Panel functions */
typedef Panel lua_Panel;

/*
** access functions (stack -> C)
*/

LUA_API lua_Panel *( lua_topanel )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_Panel *( luaL_checkpanel )( lua_State *L, int narg );
LUALIB_API lua_Panel *( luaL_optpanel )( lua_State *L, int narg, lua_Panel *def );

int PanelIsValid( lua_State *L );
int PanelCollectGarbage( lua_State *L );

#endif  // LPANEL_H
