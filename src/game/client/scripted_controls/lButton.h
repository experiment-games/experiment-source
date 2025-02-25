#ifndef LBUTTON_H
#define LBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Button.h>
#include "scripted_controls/lPanel.h"

namespace vgui
{

class LButton : public Button
{
    DECLARE_CLASS_SIMPLE( LButton, Button );

    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( LButton, "Button" );

    public:
    // You can optionally pass in the panel to send the click message to and the name of the command to send to that panel.
    LButton( Panel *parent, const char *panelName, const char *text, Panel *pActionSignalTarget = NULL, const char *pCmd = NULL, lua_State *L = NULL );

    public:
    virtual void DoClick( void );

    protected:
    virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
    {
        LUA_CALL_PANEL_METHOD_BEGIN( "ApplySchemeSettings" );
        LUA_CALL_PANEL_METHOD_END( 0, 0 );

        BaseClass::ApplySchemeSettings( pScheme );
    }
};

}  // namespace vgui

/* type for Button functions */
typedef LButton lua_Button;

/*
** access functions (stack -> C)
*/

LUA_API lua_Button *( lua_tobutton )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_Button *( luaL_checkbutton )( lua_State *L, int narg );

#endif  // LBUTTON_H
