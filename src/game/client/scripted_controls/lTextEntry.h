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

    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( LTextEntry, "TextEntry" );

   public:
    LTextEntry(Panel *parent, const char *panelName, lua_State *L = NULL );

   protected:
    virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
    {
        // Not sure why this is the order that causes text to be drawn in the Lua specified colour :/
        LUA_CALL_PANEL_METHOD_BEGIN( "ApplySchemeSettings" );
        LUA_CALL_PANEL_METHOD_END( 0, 0 );

        BaseClass::ApplySchemeSettings( pScheme );
    }
};

}  // namespace vgui

/* type for TextEntry functions */
typedef LTextEntry lua_TextEntry;

/*
** access functions (stack -> C)
*/

LUA_API lua_TextEntry *( lua_totextentry )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_TextEntry *( luaL_checktextentry )( lua_State *L, int narg );

#endif  // LTEXTENTRY_H
