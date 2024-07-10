//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef SCRIPTEDCLIENTLUAPANEL_H
#define SCRIPTEDCLIENTLUAPANEL_H

#include <vgui_controls/Frame.h>

//==============================================================================
class CScriptedClientLuaPanel : public vgui::EditablePanel
{
    DECLARE_CLASS_SIMPLE( CScriptedClientLuaPanel, vgui::EditablePanel );

   public:
    CScriptedClientLuaPanel( lua_State *L );
    virtual ~CScriptedClientLuaPanel();

    virtual void CreateDefaultPanels( void );

    virtual void Start( IGameUIFuncs *pGameUIFuncs, IGameEventManager2 *pGameEventManager );
    virtual void SetParent( vgui::VPANEL parent );

    void DeleteChildren()
    {
        // Stop our children from pointing at us, and delete them if possible
        while ( vgui::ipanel()->GetChildCount( GetVPanel() ) )
        {
            vgui::VPANEL child = vgui::ipanel()->GetChild( GetVPanel(), 0 );
            if ( vgui::ipanel()->IsAutoDeleteSet( child ) )
            {
                vgui::ipanel()->DeletePanel( child );
            }
            else
            {
                vgui::ipanel()->SetParent( child, NULL );
            }
        }
    }

   protected:
    virtual void Paint();
};

#endif
