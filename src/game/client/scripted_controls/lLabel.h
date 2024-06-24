#ifndef LLABEL_H
#define LLABEL_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Label.h>
#include "scripted_controls/lPanel.h"

namespace vgui
{

class LLabel : public Label
{
    DECLARE_CLASS_SIMPLE( LLabel, Label );

   public:
    LLabel( Panel *parent, const char *panelName, const char *text, lua_State *L = NULL );

   public:
    virtual void PushPanelToLua( lua_State *L );
};

}  // namespace vgui

/* type for Label functions */
typedef vgui::Label lua_Label;

/*
** access functions (stack -> C)
*/

LUA_API lua_Label *( lua_tolabel )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushlabel )( lua_State *L, lua_Label *pLabel );

LUALIB_API lua_Label *( luaL_checklabel )( lua_State *L, int narg );

#endif  // LLABEL_H
