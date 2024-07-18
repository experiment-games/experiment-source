//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef LEDITABLEPANEL_H
#define LEDITABLEPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "scripted_controls/lPanel.h"

using namespace vgui;

class LEditablePanel : public EditablePanel
{
    DECLARE_CLASS_SIMPLE( LEditablePanel, EditablePanel );

   public:
    LEditablePanel( Panel *parent, const char *panelName, lua_State *L );
    ~LEditablePanel();
    virtual void PushPanelToLua( lua_State *L );
    void SetFocusTopLevel( bool state );

    virtual void Activate();
    virtual void DoModal();
    virtual bool IsModal();
    virtual void OnClose();

    // closes the dialog
    MESSAGE_FUNC( Close, "Close" );
    MESSAGE_FUNC( CloseModal, "CloseModal" );

   private:
    void FinishClose();
    VPANEL m_hPreviousModal;

   protected:
    virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
    {
        BEGIN_LUA_CALL_PANEL_METHOD( "ApplySchemeSettings" );
        END_LUA_CALL_PANEL_METHOD( 0, 0 );

        BaseClass::ApplySchemeSettings( pScheme );
    }
};

/* type for EditablePanel functions */
typedef LEditablePanel lua_EditablePanel;

/*
** access functions (stack -> C)
*/

LUA_API lua_EditablePanel *( lua_toeditablepanel )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pusheditablepanel )( lua_State *L, lua_EditablePanel *pPanel );

LUALIB_API lua_EditablePanel *( luaL_checkeditablepanel )( lua_State *L, int narg );

#endif  // LEDITABLEPANEL_H
