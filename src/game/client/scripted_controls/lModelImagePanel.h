#ifndef LMODELIMAGEPANEL_H
#define LMODELIMAGEPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include <tier2/camerautils.h>

using namespace vgui;

class LModelImagePanel : public EditablePanel
{
    DECLARE_CLASS_SIMPLE( LModelImagePanel, EditablePanel );

    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( "ModelImagePanel" );

   public:
    LModelImagePanel( Panel *parent, const char *panelName, lua_State *L );
    virtual ~LModelImagePanel();

    virtual void RebuildSpawnIcon( Camera_t *camera = NULL, const char *pszSavePath = NULL );
    virtual void SetModelImage( const char *pngImagePath );

    virtual void Paint();

   protected:
    const char *m_pszModelImagePath;
    const char *m_pszModelPath;
    int m_iSkin;
    const char *m_pszBodyGroups;
    int m_nTextureID;

    virtual void LoadIfExistsOrRebuild();

    virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
    {
        BEGIN_LUA_CALL_PANEL_METHOD( "ApplySchemeSettings" );
        END_LUA_CALL_PANEL_METHOD( 0, 0 );

        BaseClass::ApplySchemeSettings( pScheme );
    }

   public:
    virtual void SetModel( const char *modelPath, int skin = 0, const char *bodyGroups = NULL )
    {
        m_pszModelPath = modelPath;
        m_iSkin = skin;
        m_pszBodyGroups = bodyGroups;

        LoadIfExistsOrRebuild();
    }

    virtual const char *GetModel()
    {
        return m_pszModelPath;
    }

    virtual void SetSkin( int skin )
    {
        m_iSkin = skin;
    }

    virtual int GetSkin()
    {
        return m_iSkin;
    }

    virtual void SetBodyGroups( const char *bodyGroups )
    {
        m_pszBodyGroups = bodyGroups;
    }

    virtual const char *GetBodyGroups()
    {
        return m_pszBodyGroups;
    }
};

/* type for lModelImagePanel functions */
typedef LModelImagePanel lua_ModelImagePanel;

/*
** access functions (stack -> C)
*/

LUA_API lua_ModelImagePanel *( lua_tomodelimagepanel )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_ModelImagePanel *( luaL_checkmodelimagepanel )( lua_State *L, int narg );
LUALIB_API lua_ModelImagePanel *( luaL_optmodelimagepanel )( lua_State *L, int narg, lua_ModelImagePanel *def );

#endif  // LMODELIMAGEPANEL_H
