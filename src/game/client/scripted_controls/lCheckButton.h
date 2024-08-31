#ifndef VGUI_LCHECKBUTTON_H
#define VGUI_LCHECKBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/CheckButton.h>

namespace vgui
{

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class LCheckButton : public CheckButton
{
    DECLARE_CLASS_SIMPLE( LCheckButton, CheckButton );

    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( LCheckButton, "CheckButton" );

   public:
    LCheckButton( Panel *parent, const char *panelName, const char *text, lua_State *L );
    virtual ~LCheckButton();

   protected:
    MESSAGE_FUNC( OnCheckButtonChecked, "CheckButtonChecked" );

    virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
    {
        LUA_CALL_PANEL_METHOD_BEGIN( "ApplySchemeSettings" );
        LUA_CALL_PANEL_METHOD_END( 0, 0 );

        BaseClass::ApplySchemeSettings( pScheme );
    }
};

}  // namespace vgui

/* type for CheckButton functions */
typedef LCheckButton lua_CheckButton;

/*
** access functions (stack -> C)
*/

LUA_API lua_CheckButton *( lua_tocheckbutton )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CheckButton *( luaL_checkcheckbutton )( lua_State *L, int narg );

#endif  // VGUI_LCHECKBUTTON_H
