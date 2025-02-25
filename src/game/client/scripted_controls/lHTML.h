#ifndef LHTML_H
#define LHTML_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/HTML.h>
#include "scripted_controls/lPanel.h"

namespace vgui
{

class LHTML : public HTML
{
    DECLARE_CLASS_SIMPLE( LHTML, HTML );

    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( LHTML, "Html" );

   public:
    // You can optionally pass in the panel to send the click message to and the name of the command to send to that panel.
    LHTML( Panel *parent, const char *name, bool allowJavaScript = false, bool bPopupWindow = false, lua_State *L = NULL );

    virtual bool OnStartRequest( const char *url, const char *target, const char *pchPostData, bool bIsRedirect );
    virtual void OnFinishRequest( const char *url, const char *pageTitle, const CUtlMap< CUtlString, CUtlString > &headers );
    virtual void OnJavaScriptCallback( KeyValues *pData ) OVERRIDE;

    bool IsLoading()
    {
        return m_bIsLoading;
    }

   protected:
    virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
    {
        LUA_CALL_PANEL_METHOD_BEGIN( "ApplySchemeSettings" );
        LUA_CALL_PANEL_METHOD_END( 0, 0 );

        BaseClass::ApplySchemeSettings( pScheme );
    }

   private:
    bool m_bIsLoading;
};

}  // namespace vgui

/* type for HTML functions */
typedef LHTML lua_HTML;

/*
** access functions (stack -> C)
*/

LUA_API lua_HTML *( lua_tohtml )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_HTML *( luaL_checkhtml )( lua_State *L, int narg );

#endif  // LHTML_H
