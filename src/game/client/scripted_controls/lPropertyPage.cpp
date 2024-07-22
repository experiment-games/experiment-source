//========= Copyright © 1996-2005, Valve Corporation, All rights reserved.
//============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include <cbase.h>
#include "vgui/KeyCode.h"
#include <vgui_int.h>
#include "luamanager.h"
#include "luasrclib.h"

#include "scripted_controls/lPanel.h"
#include "scripted_controls/LPropertyPage.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LPropertyPage::LPropertyPage( Panel *parent, const char *panelName, lua_State *L )
    : PropertyPage( parent, panelName )
{
    m_lua_State = L;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
LPropertyPage::~LPropertyPage()
{
}

//-----------------------------------------------------------------------------
// Purpose: Called when page is loaded.  Data should be reloaded from document
// into controls.
//-----------------------------------------------------------------------------
void LPropertyPage::OnResetData()
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYPAGE_METHOD( "OnResetData" );
    END_LUA_CALL_PANEL_METHOD( 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Called when the OK / Apply button is pressed.  Changed data should
// be written into document.
//-----------------------------------------------------------------------------
void LPropertyPage::OnApplyChanges()
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYPAGE_METHOD( "OnApplyChanges" );
    END_LUA_CALL_PANEL_METHOD( 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Designed to be overriden
//-----------------------------------------------------------------------------
void LPropertyPage::OnPageShow()
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYPAGE_METHOD( "OnPageShow" );
    END_LUA_CALL_PANEL_METHOD( 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Designed to be overriden
//-----------------------------------------------------------------------------
void LPropertyPage::OnPageHide()
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYPAGE_METHOD( "OnPageHide" );
    END_LUA_CALL_PANEL_METHOD( 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pageTab -
//-----------------------------------------------------------------------------
void LPropertyPage::OnPageTabActivated( Panel *pageTab )
{
    _pageTab = pageTab;

#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYPAGE_METHOD( "OnPageTabActivated" );
    pageTab->PushLuaInstance( m_lua_State );
    END_LUA_CALL_PANEL_METHOD( 1, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void LPropertyPage::OnKeyCodeTyped( KeyCode code )
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYPAGE_METHOD( "OnKeyCodeTyped" );
    lua_pushinteger( m_lua_State, code );
    END_LUA_CALL_PANEL_METHOD( 1, 1 );

    RETURN_LUA_PANEL_NONE();
#endif

    BaseClass::OnKeyCodeTyped( code );
}

/*
** access functions (stack -> C)
*/

LUA_API lua_PropertyPage *lua_topropertypage( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_PropertyPage * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_PropertyPage *luaL_checkpropertypage( lua_State *L, int narg )
{
    lua_PropertyPage *d = lua_topropertypage( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "PropertyPage expected, got INVALID_PANEL" );
    return d;
}

static int PropertyPage_ChainToAnimationMap( lua_State *L )
{
    luaL_checkpropertypage( L, 1 )->ChainToAnimationMap();
    return 0;
}

static int PropertyPage_ChainToMap( lua_State *L )
{
    luaL_checkpropertypage( L, 1 )->ChainToMap();
    return 0;
}

static int PropertyPage_GetPanelBaseClassName( lua_State *L )
{
    lua_pushstring( L, luaL_checkpropertypage( L, 1 )->GetPanelBaseClassName() );
    return 1;
}

static int PropertyPage_GetPanelClassName( lua_State *L )
{
    lua_pushstring( L, luaL_checkpropertypage( L, 1 )->GetPanelClassName() );
    return 1;
}

static int PropertyPage_HasUserConfigSettings( lua_State *L )
{
    lua_pushboolean( L, luaL_checkpropertypage( L, 1 )->HasUserConfigSettings() );
    return 1;
}

static int PropertyPage_KB_AddBoundKey( lua_State *L )
{
    luaL_checkpropertypage( L, 1 )->KB_AddBoundKey(
        luaL_checkstring( L, 2 ), luaL_checknumber( L, 3 ), luaL_checknumber( L, 4 ) );
    return 0;
}

static int PropertyPage_KB_ChainToMap( lua_State *L )
{
    luaL_checkpropertypage( L, 1 )->KB_ChainToMap();
    return 0;
}

static int PropertyPage_OnApplyChanges( lua_State *L )
{
    luaL_checkpropertypage( L, 1 )->OnApplyChanges();
    return 0;
}

static int PropertyPage_OnKeyCodeTyped( lua_State *L )
{
    luaL_checkpropertypage( L, 1 )->OnKeyCodeTyped( ( KeyCode )( int )luaL_checknumber( L, 2 ) );
    return 0;
}

static int PropertyPage_OnPageHide( lua_State *L )
{
    luaL_checkpropertypage( L, 1 )->OnPageHide();
    return 0;
}

static int PropertyPage_OnPageShow( lua_State *L )
{
    luaL_checkpropertypage( L, 1 )->OnPageShow();
    return 0;
}

static int PropertyPage_OnResetData( lua_State *L )
{
    luaL_checkpropertypage( L, 1 )->OnResetData();
    return 0;
}

static int PropertyPage___index( lua_State *L )
{
    PropertyPage *pPage = lua_topropertypage( L, 1 );
    LUA_METATABLE_INDEX_CHECK_VALID( L, Panel_IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pPage );

    LPropertyPage *plPage = dynamic_cast< LPropertyPage * >( pPage );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plPage );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "PropertyPage" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "EditablePanel" );

    lua_pushnil( L );
    return 1;
}

static int PropertyPage___newindex( lua_State *L )
{
    PropertyPage *pPage = lua_topropertypage( L, 1 );

    if ( pPage == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LPropertyPage *plPage = dynamic_cast< LPropertyPage * >( pPage );

    LUA_GET_REF_TABLE( L, plPage );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, luaL_checkstring( L, 2 ) );
    lua_pop( L, 1 );
    return 0;
}

static int PropertyPage___eq( lua_State *L )
{
    lua_pushboolean( L, lua_topropertypage( L, 1 ) == lua_topropertypage( L, 2 ) );
    return 1;
}

static int PropertyPage___tostring( lua_State *L )
{
    PropertyPage *pPage = lua_topropertypage( L, 1 );
    if ( pPage == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pPage->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "PropertyPage: \"%s\"", pName );
    }
    return 1;
}

static const luaL_Reg PropertyPagemeta[] = {
    { "ChainToAnimationMap", PropertyPage_ChainToAnimationMap },
    { "ChainToMap", PropertyPage_ChainToMap },
    { "GetPanelBaseClassName", PropertyPage_GetPanelBaseClassName },
    { "GetPanelClassName", PropertyPage_GetPanelClassName },
    { "HasUserConfigSettings", PropertyPage_HasUserConfigSettings },
    { "KB_AddBoundKey", PropertyPage_KB_AddBoundKey },
    { "KB_ChainToMap", PropertyPage_KB_ChainToMap },
    { "OnApplyChanges", PropertyPage_OnApplyChanges },
    { "OnKeyCodeTyped", PropertyPage_OnKeyCodeTyped },
    { "OnPageHide", PropertyPage_OnPageHide },
    { "OnPageShow", PropertyPage_OnPageShow },
    { "OnResetData", PropertyPage_OnResetData },
    { "__index", PropertyPage___index },
    { "__newindex", PropertyPage___newindex },
    { "__eq", PropertyPage___eq },
    { "__tostring", PropertyPage___tostring },
    { "__gc", Panel___gc },
    { NULL, NULL } };

static int luasrc_PropertyPage( lua_State *L )
{
    lua_PropertyPage *pPanel =
        new lua_PropertyPage( luaL_optpanel( L, 1, VGui_GetClientLuaRootPanel() ),
                           luaL_checkstring( L, 2 ),
                           L );
    pPanel->PushLuaInstance( L );
    return 1;
}

static const luaL_Reg PropertyPage_funcs[] = {
    { "PropertyPage", luasrc_PropertyPage },
    { NULL, NULL } };

/*
** Open PropertyPage object
*/
LUALIB_API int luaopen_vgui_PropertyPage( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "PropertyPage" );
    luaL_register( L, NULL, PropertyPagemeta );
    lua_pushstring( L, LUA_PANELLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */
    luaL_register( L, LUA_VGUILIBNAME, PropertyPage_funcs );
    lua_pop( L, 2 );
    return 0;
}
