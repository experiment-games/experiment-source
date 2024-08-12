#ifndef VGUI_LFRAME_H
#define VGUI_LFRAME_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>

namespace vgui
{

//-----------------------------------------------------------------------------
// Purpose: Lua wrapper for a windowed frame
//-----------------------------------------------------------------------------
class LFrame : public Frame
{
    DECLARE_CLASS_SIMPLE( LFrame, Frame );

    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( "Frame" );

   public:
    LFrame( Panel *parent, const char *panelName, bool showTaskbarIcon = true, lua_State *L = NULL );
    virtual ~LFrame();

   protected:
    virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
    {
        LUA_CALL_PANEL_METHOD_BEGIN( "ApplySchemeSettings" );
        LUA_CALL_PANEL_METHOD_END( 0, 0 );

        BaseClass::ApplySchemeSettings( pScheme );
    }
};

}  // namespace vgui

/* type for Frame functions */
typedef LFrame lua_Frame;

/*
** access functions (stack -> C)
*/

LUA_API lua_Frame *( lua_toframe )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_Frame *( luaL_checkframe )( lua_State *L, int narg );

#endif  // VGUI_LFRAME_H
