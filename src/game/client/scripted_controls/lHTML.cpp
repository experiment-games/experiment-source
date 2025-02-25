#include <cbase.h>

#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <lColor.h>

#include <scripted_controls/lHTML.h>
#include "scripted_controls/lPanel.h"

#include "base64.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LHTML::LHTML( Panel *parent, const char *name, bool allowJavaScript, bool bPopupWindow, lua_State *L /* = nullptr */ )
    : HTML( parent, name, allowJavaScript, bPopupWindow, L )
{
}

bool LHTML::OnStartRequest( const char *url, const char *target, const char *pchPostData, bool bIsRedirect )
{
    m_bIsLoading = true;

    return BaseClass::OnStartRequest( url, target, pchPostData, bIsRedirect );
}

void LHTML::OnFinishRequest( const char *url, const char *pageTitle, const CUtlMap< CUtlString, CUtlString > &headers )
{
    BaseClass::OnFinishRequest( url, pageTitle, headers );
    m_bIsLoading = false;
}

void LHTML::OnJavaScriptCallback( KeyValues *pData )
{
    // Call OnCallback with the data
    LUA_CALL_PANEL_METHOD_BEGIN( "OnCallback" );
    lua_pushstring( L, pData->GetString( "object" ) );
    lua_pushstring( L, pData->GetString( "field" ) );

    // Push all arguments in 'arguments' as a table
    lua_newtable( L );

    KeyValues *pArguments = pData->FindKey( "arguments" );

    for ( KeyValues *pArg = pArguments->GetFirstSubKey(); pArg; pArg = pArg->GetNextKey() )
    {
        lua_pushstring( L, pArg->GetName() );
        lua_pushstring( L, pArg->GetString() );
        lua_settable( L, -3 );
    }

    LUA_CALL_PANEL_METHOD_END( 3, 1 );

    LUA_RETURN_PANEL_NONE();

    BaseClass::OnJavaScriptCallback( pData );
}

/*
** access functions (stack -> C)
*/

LUA_API lua_HTML *lua_tohtml( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_HTML * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_HTML *luaL_checkhtml( lua_State *L, int narg )
{
    lua_HTML *d = lua_tohtml( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "Html Panel expected, got INVALID_PANEL" );
    return d;
}

LUA_REGISTRATION_INIT( Html )

LUA_BINDING_BEGIN( Html, OpenUrl, "class", "Opens a URL in the HTML object" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    const char *url = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "url" );
    const char *postData = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, "", "postData" );
    bool bAllowJS = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "allowJS" );
    html->OpenURL( url, postData, bAllowJS );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, StopLoading, "class", "Stops loading the current page" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    html->StopLoading();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, Refresh, "class", "Refreshes the current page" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    html->Refresh();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, RunJavascript, "class", "Executes JavaScript in the HTML object" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    const char *jsCode = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "jsCode" );
    html->RunJavascript( jsCode );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, SetHtml, "class", "Sets the HTML content of the object" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    const char *htmlContent = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "htmlContent" );
    CUtlString script;

    // Encode it, so we don't have to worry about special characters
    std::string encodedString = base64_encode( htmlContent, false );

    script.Format( "document.write( atob( '%s' ) );", encodedString.c_str() );

    html->RunJavascript( script.Get() );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, AddJavascriptObject, "class", "Adds a JavaScript object" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    const char *objectName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "objectName" );
    html->AddJavascriptObject( objectName );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, AddJavascriptObjectCallback, "class", "Adds a callback to a JavaScript object" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    const char *objectName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "objectName" );
    const char *propertyName = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "propertyName" );
    html->AddJavascriptObjectCallback( objectName, propertyName );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, GoBack, "class", "Navigates back in the browser history" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    html->GoBack();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, GoForward, "class", "Navigates forward in the browser history" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    html->GoForward();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, CanGoBack, "class", "Checks if the browser can go back" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    lua_pushboolean( L, html->BCanGoBack() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the browser can go back" )

LUA_BINDING_BEGIN( Html, CanGoForward, "class", "Checks if the browser can go forward" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    lua_pushboolean( L, html->BCanGoFoward() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the browser can go forward" )

LUA_BINDING_BEGIN( Html, SetScrollbarsEnabled, "class", "Enables or disables scrollbars" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    bool enable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" );
    html->SetScrollbarsEnabled( enable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, SetContextMenuEnabled, "class", "Enables or disables the context menu" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    bool enable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" );
    html->SetContextMenuEnabled( enable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, SetViewSourceEnabled, "class", "Enables or disables view source option" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    bool enable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" );
    html->SetViewSourceEnabled( enable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, NewWindowsOnly, "class", "Allows opening new windows only" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    bool enable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" );
    html->NewWindowsOnly( enable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, IsLoading, "class", "Checks if the HTML is currently loading" )
{
    LHTML *html = dynamic_cast< LHTML * >( LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" ) );
    if ( html == NULL )
    {
        // Experiment; TODO: Will this happen?
        luaL_argerror( L, 1, "Lua Html Panel expected, got a different HTML?" );
        return 0;
    }
    lua_pushboolean( L, html->IsLoading() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the HTML is loading" )

LUA_BINDING_BEGIN( Html, IsScrolledToBottom, "class", "Checks if the HTML is scrolled to the bottom" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    lua_pushboolean( L, html->IsScrolledToBottom() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the HTML is scrolled to the bottom" )

LUA_BINDING_BEGIN( Html, IsScrollbarVisible, "class", "Checks if the scrollbar is visible" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    lua_pushboolean( L, html->IsScrollbarVisible() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the scrollbar is visible" )

LUA_BINDING_BEGIN( Html, AddCustomURLHandler, "class", "Adds a custom URL handler" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    const char *url = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "url" );
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 3, "panel" );
    html->AddCustomURLHandler( url, panel );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, Find, "class", "Finds text in the HTML object" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    const char *text = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "text" );
    html->Find( text );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, StopFind, "class", "Stops finding text" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    html->StopFind();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, FindNext, "class", "Finds the next occurrence of text" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    html->FindNext();
    return 0;
}
LUA_BINDING_END()
LUA_BINDING_BEGIN( Html, FindPrevious, "class", "Finds the previous occurrence of text" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    html->FindPrevious();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, ShowFindDialog, "class", "Shows the find dialog" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    html->ShowFindDialog();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, HideFindDialog, "class", "Hides the find dialog" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    html->HideFindDialog();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, FindDialogVisible, "class", "Checks if the find dialog is visible" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    lua_pushboolean( L, html->FindDialogVisible() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the find dialog is visible" )

LUA_BINDING_BEGIN( Html, HorizontalScrollMax, "class", "Gets the maximum horizontal scroll value" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    lua_pushinteger( L, html->HorizontalScrollMax() );
    return 1;
}
LUA_BINDING_END( "integer", "The maximum horizontal scroll value" )

LUA_BINDING_BEGIN( Html, VerticalScrollMax, "class", "Gets the maximum vertical scroll value" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    lua_pushinteger( L, html->VerticalScrollMax() );
    return 1;
}
LUA_BINDING_END( "integer", "The maximum vertical scroll value" )

LUA_BINDING_BEGIN( Html, GetLinkAtPosition, "class", "Gets the link at a specific position" )
{
    lua_HTML *html = LUA_BINDING_ARGUMENT( luaL_checkhtml, 1, "html" );
    double x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    double y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );
    html->GetLinkAtPosition( x, y );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, __index, "class", "Metamethod called when a non-existent field is indexed" )
{
    HTML *pHTML = LUA_BINDING_ARGUMENT( lua_tohtml, 1, "html" );
    LUA_METATABLE_INDEX_CHECK_VALID( L, PanelIsValid );
    LUA_METATABLE_INDEX_CHECK( L, pHTML );

    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LHTML *plHTML = dynamic_cast< LHTML * >( pHTML );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plHTML );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "Html" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "Panel" );

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( Html, __newindex, "class", "Metamethod called when a new field is added" )
{
    HTML *pHTML = LUA_BINDING_ARGUMENT( lua_tohtml, 1, "html" );

    if ( pHTML == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );

        return lua_error( L );
    }

    LHTML *plHTML = dynamic_cast< LHTML * >( pHTML );

    LUA_GET_REF_TABLE( L, plHTML );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" ) );
    lua_pop( L, 1 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Html, __eq, "class", "Metamethod called when two HTML objects are compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_tohtml, 1, "html" ) == LUA_BINDING_ARGUMENT( lua_tohtml, 2, "other" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the HTML objects are equal" )

LUA_BINDING_BEGIN( Html, __tostring, "class", "Metamethod called when the HTML object is converted to a string" )
{
    HTML *pHTML = LUA_BINDING_ARGUMENT( lua_tohtml, 1, "html" );
    if ( pHTML == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pHTML->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "Html: \"%s\"", pName );
    }
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the HTML object" )

LUA_REGISTRATION_INIT( Panels )

LUA_BINDING_BEGIN( Panels, Html, "library", "Creates a new HTML panel" )
{
    Panel *parent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, VGui_GetClientLuaRootPanel(), "parent" );
    const char *name = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "Html", "name" );
    bool shouldAllowJavaScript = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "shouldAllowJavaScript" );
    bool popupWindow = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "popupWindow" );

    lua_HTML *pPanel =
        new lua_HTML( parent,
                      name,
                      shouldAllowJavaScript,
                      popupWindow,
                      L );
    LHTML::PushLuaInstanceSafe( L, pPanel );
    return 1;
}
LUA_BINDING_END( "Html", "The new HTML Panel" )

/*
** Open HTML object
*/
LUALIB_API int luaopen_vgui_HTML( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "Html" );

    LUA_REGISTRATION_COMMIT( Html );

    lua_pushstring( L, LUA_PANELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Panels );

    // Pop the panel library off the stack
    lua_pop( L, 1 );
    lua_pop( L, 1 );  // Pop the HTML metatable

    return 0;
}
