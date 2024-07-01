#ifndef LTEXTENTRY_H
#define LTEXTENTRY_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/TextEntry.h>
#include "scripted_controls/lPanel.h"

namespace vgui
{

class LTextEntry : public TextEntry
{
    DECLARE_CLASS_SIMPLE( LTextEntry, TextEntry );

   public:
    LTextEntry(Panel *parent, const char *panelName, lua_State *L = NULL );

   public:
    virtual void PushPanelToLua( lua_State *L );
};

}  // namespace vgui

/* type for TextEntry functions */
typedef vgui::TextEntry lua_TextEntry;

/*
** access functions (stack -> C)
*/

LUA_API lua_TextEntry *( lua_totextentry )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushtextentry )( lua_State *L, lua_TextEntry *pTextEntry );

LUALIB_API lua_TextEntry *( luaL_checktextentry )( lua_State *L, int narg );

#endif  // LTEXTENTRY_H
