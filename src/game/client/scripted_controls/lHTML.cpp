#include <cbase.h>

#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <lColor.h>

#include <scripted_controls/lHTML.h>
#include "scripted_controls/lPanel.h"

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
        luaL_argerror( L, narg, "HTML expected, got INVALID_PANEL" );
    return d;
}

static int HTML_OpenURL( lua_State *L )
{
    luaL_checkhtml( L, 1 )->OpenURL( luaL_checkstring( L, 2 ), luaL_optstring( L, 3, "" ), luaL_optboolean( L, 4, false ) );
    return 0;
}

static int HTML_StopLoading( lua_State *L )
{
    luaL_checkhtml( L, 1 )->StopLoading();
    return 0;
}

static int HTML_Refresh( lua_State *L )
{
    luaL_checkhtml( L, 1 )->Refresh();
    return 0;
}

static int HTML_OnMove( lua_State *L )
{
    luaL_checkhtml( L, 1 )->OnMove();
    return 0;
}

static int HTML_RunJavascript( lua_State *L )
{
    luaL_checkhtml( L, 1 )->RunJavascript( luaL_checkstring( L, 2 ) );
    return 0;
}

static int HTML_GoBack( lua_State *L )
{
    luaL_checkhtml( L, 1 )->GoBack();
    return 0;
}

static int HTML_GoForward( lua_State *L )
{
    luaL_checkhtml( L, 1 )->GoForward();
    return 0;
}

static int HTML_BCanGoBack( lua_State *L )
{
    lua_pushboolean( L, luaL_checkhtml( L, 1 )->BCanGoBack() );
    return 1;
}

static int HTML_BCanGoFoward( lua_State *L )
{
    lua_pushboolean( L, luaL_checkhtml( L, 1 )->BCanGoFoward() );
    return 1;
}

static int HTML_SetScrollbarsEnabled( lua_State *L )
{
    luaL_checkhtml( L, 1 )->SetScrollbarsEnabled( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int HTML_SetContextMenuEnabled( lua_State *L )
{
    luaL_checkhtml( L, 1 )->SetContextMenuEnabled( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int HTML_SetViewSourceEnabled( lua_State *L )
{
    luaL_checkhtml( L, 1 )->SetViewSourceEnabled( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int HTML_NewWindowsOnly( lua_State *L )
{
    luaL_checkhtml( L, 1 )->NewWindowsOnly( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int HTML_IsLoading( lua_State *L )
{
    LHTML *html = dynamic_cast< LHTML * >( luaL_checkhtml( L, 1 ) );

    if ( html == NULL )
    {
        // TODO: Will this happen?
        luaL_argerror( L, 1, "Lua HTML expected, got a different HTML?" );
        return 0;
    }

    lua_pushboolean( L, html->IsLoading() );
    return 1;
}

static int HTML_IsScrolledToBottom( lua_State *L )
{
    lua_pushboolean( L, luaL_checkhtml( L, 1 )->IsScrolledToBottom() );
    return 1;
}

static int HTML_IsScrollbarVisible( lua_State *L )
{
    lua_pushboolean( L, luaL_checkhtml( L, 1 )->IsScrollbarVisible() );
    return 1;
}

static int HTML_AddCustomURLHandler( lua_State *L )
{
    luaL_checkhtml( L, 1 )->AddCustomURLHandler( luaL_checkstring( L, 2 ), luaL_checkpanel( L, 3 ) );
    return 0;
}

static int HTML_Find( lua_State *L )
{
    luaL_checkhtml( L, 1 )->Find( luaL_checkstring( L, 2 ) );
    return 0;
}

static int HTML_StopFind( lua_State *L )
{
    luaL_checkhtml( L, 1 )->StopFind();
    return 0;
}

static int HTML_FindNext( lua_State *L )
{
    luaL_checkhtml( L, 1 )->FindNext();
    return 0;
}

static int HTML_FindPrevious( lua_State *L )
{
    luaL_checkhtml( L, 1 )->FindPrevious();
    return 0;
}

static int HTML_ShowFindDialog( lua_State *L )
{
    luaL_checkhtml( L, 1 )->ShowFindDialog();
    return 0;
}

static int HTML_HideFindDialog( lua_State *L )
{
    luaL_checkhtml( L, 1 )->HideFindDialog();
    return 0;
}

static int HTML_FindDialogVisible( lua_State *L )
{
    lua_pushboolean( L, luaL_checkhtml( L, 1 )->FindDialogVisible() );
    return 1;
}

static int HTML_HorizontalScrollMax( lua_State *L )
{
    lua_pushinteger( L, luaL_checkhtml( L, 1 )->HorizontalScrollMax() );
    return 1;
}

static int HTML_VerticalScrollMax( lua_State *L )
{
    lua_pushinteger( L, luaL_checkhtml( L, 1 )->VerticalScrollMax() );
    return 1;
}

static int HTML_GetLinkAtPosition( lua_State *L )
{
    luaL_checkhtml( L, 1 )->GetLinkAtPosition( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
    return 0;
}

static int HTML___index( lua_State *L )
{
    HTML *pHTML = lua_tohtml( L, 1 );
    LUA_METATABLE_INDEX_CHECK_VALID( L, Panel_IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pHTML );

    LHTML *plHTML = dynamic_cast< LHTML * >( pHTML );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plHTML );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "HTML" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "Panel" );

    lua_pushnil( L );
    return 1;
}

static int HTML___newindex( lua_State *L )
{
    HTML *pHTML = lua_tohtml( L, 1 );
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
    lua_setfield( L, -2, luaL_checkstring( L, 2 ) );
    lua_pop( L, 1 );

    return 0;
}

static int HTML___eq( lua_State *L )
{
    lua_pushboolean( L, lua_tohtml( L, 1 ) == lua_tohtml( L, 2 ) );
    return 1;
}

static int HTML___tostring( lua_State *L )
{
    HTML *pHTML = lua_tohtml( L, 1 );
    if ( pHTML == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pHTML->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "HTML: \"%s\"", pName );
    }
    return 1;
}

static const luaL_Reg HTMLmeta[] = {
    { "OpenURL", HTML_OpenURL },
    { "StopLoading", HTML_StopLoading },
    { "Refresh", HTML_Refresh },
    { "OnMove", HTML_OnMove },
    { "RunJavascript", HTML_RunJavascript },
    { "GoBack", HTML_GoBack },
    { "GoForward", HTML_GoForward },
    { "BCanGoBack", HTML_BCanGoBack },
    { "BCanGoFoward", HTML_BCanGoFoward },
    { "SetScrollbarsEnabled", HTML_SetScrollbarsEnabled },
    { "SetContextMenuEnabled", HTML_SetContextMenuEnabled },
    { "SetViewSourceEnabled", HTML_SetViewSourceEnabled },
    { "NewWindowsOnly", HTML_NewWindowsOnly },
    { "IsLoading", HTML_IsLoading },
    { "IsScrolledToBottom", HTML_IsScrolledToBottom },
    { "IsScrollbarVisible", HTML_IsScrollbarVisible },
    { "AddCustomURLHandler", HTML_AddCustomURLHandler },
    { "Find", HTML_Find },
    { "StopFind", HTML_StopFind },
    { "FindNext", HTML_FindNext },
    { "FindPrevious", HTML_FindPrevious },
    { "ShowFindDialog", HTML_ShowFindDialog },
    { "HideFindDialog", HTML_HideFindDialog },
    { "FindDialogVisible", HTML_FindDialogVisible },
    { "HorizontalScrollMax", HTML_HorizontalScrollMax },
    { "VerticalScrollMax", HTML_VerticalScrollMax },
    { "GetLinkAtPosition", HTML_GetLinkAtPosition },
    { "__index", HTML___index },
    { "__newindex", HTML___newindex },
    { "__eq", HTML___eq },
    { "__tostring", HTML___tostring },
    { "__gc", Panel___gc },

    { NULL, NULL } };

static int luasrc_HTML( lua_State *L )
{
    lua_HTML *pPanel =
        new lua_HTML( luaL_optpanel( L, 1, VGui_GetClientLuaRootPanel() ),
                   luaL_optstring( L, 2, "HTML" ),
                   luaL_optboolean( L, 3, false ),
                   luaL_optboolean( L, 4, false ),
                   L );
    pPanel->PushLuaInstance( L );
    return 1;
}

static const luaL_Reg HTML_funcs[] = { { "HTML", luasrc_HTML },
                                       { NULL, NULL } };

/*
** Open HTML object
*/
LUALIB_API int luaopen_vgui_HTML( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "HTML" );
    luaL_register( L, NULL, HTMLmeta );
    lua_pushstring( L, LUA_PANELLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */
    luaL_register( L, LUA_VGUILIBNAME, HTML_funcs );

    // Pop the panel library off the stack
    lua_pop( L, 1 );
    lua_pop( L, 1 );  // Pop the HTML metatable

    return 0;
}
