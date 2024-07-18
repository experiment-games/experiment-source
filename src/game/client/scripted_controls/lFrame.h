//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

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

   public:
    LFrame( Panel *parent, const char *panelName, bool showTaskbarIcon = true, lua_State *L = NULL );
    virtual ~LFrame();

    virtual void PushPanelToLua( lua_State *L );

   protected:
    virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
    {
        BaseClass::ApplySchemeSettings( pScheme );
        BEGIN_LUA_CALL_PANEL_METHOD( "ApplySchemeSettings" );
        END_LUA_CALL_PANEL_METHOD( 0, 0 );
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
LUA_API void( lua_pushframe )( lua_State *L, lua_Frame *pFrame );

LUALIB_API lua_Frame *( luaL_checkframe )( lua_State *L, int narg );

#endif  // VGUI_LFRAME_H
