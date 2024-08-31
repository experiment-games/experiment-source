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

    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( LLabel, "Label" );

   public:
    LLabel( Panel *parent, const char *panelName, const char *text, lua_State *L = nullptr );

   protected:
    virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
    {
        LUA_CALL_PANEL_METHOD_BEGIN( "ApplySchemeSettings" );
        LUA_CALL_PANEL_METHOD_END( 0, 0 );

        BaseClass::ApplySchemeSettings( pScheme );
    }
};

}  // namespace vgui

/* type for Label functions */
typedef LLabel lua_Label;

/*
** access functions (stack -> C)
*/

LUA_API lua_Label *( lua_tolabel )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_Label *( luaL_checklabel )( lua_State *L, int narg );

#endif  // LLABEL_H
