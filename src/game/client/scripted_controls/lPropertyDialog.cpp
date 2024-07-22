//========= Copyright © 1996-2005, Valve Corporation, All rights reserved.
//============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include <cbase.h>
#include <vgui/KeyCode.h>
#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"

#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/Button.h>

#include <scripted_controls/lPanel.h>
#include <scripted_controls/lPropertyDialog.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LPropertyDialog::LPropertyDialog( Panel *parent, const char *panelName, lua_State *L )
    : PropertyDialog( parent, panelName )
{
    m_lua_State = L;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
LPropertyDialog::~LPropertyDialog()
{
}

void LPropertyDialog::PushPanelToLua( lua_State *L )
{
    lua_pushpropertydialog( L, this );
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the sheet
//-----------------------------------------------------------------------------
void LPropertyDialog::PerformLayout()
{
    BaseClass::PerformLayout();

#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYDIALOG_METHOD( "PerformLayout" );
    END_LUA_CALL_PANEL_METHOD( 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Handles command text from the buttons
//-----------------------------------------------------------------------------
void LPropertyDialog::OnCommand( const char *command )
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYDIALOG_METHOD( "OnCommand" );
    lua_pushstring( m_lua_State, command );
    END_LUA_CALL_PANEL_METHOD( 1, 1 );

    RETURN_LUA_PANEL_NONE();
#endif

    BaseClass::OnCommand( command );
}

//-----------------------------------------------------------------------------
// Purpose: called when the Cancel button is pressed
//-----------------------------------------------------------------------------
void LPropertyDialog::OnCancel()
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYDIALOG_METHOD( "OnCancel" );
    END_LUA_CALL_PANEL_METHOD( 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : code -
//-----------------------------------------------------------------------------
void LPropertyDialog::OnKeyCodeTyped( KeyCode code )
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYDIALOG_METHOD( "OnKeyCodeTyped" );
    lua_pushinteger( m_lua_State, code );
    END_LUA_CALL_PANEL_METHOD( 1, 1 );

    RETURN_LUA_PANEL_NONE();
#endif

    BaseClass::OnKeyCodeTyped( code );
}

//-----------------------------------------------------------------------------
// Purpose: Command handler
//-----------------------------------------------------------------------------
bool LPropertyDialog::OnOK( bool applyOnly )
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_PROPERTYDIALOG_METHOD( "OnOK" );
    lua_pushboolean( m_lua_State, applyOnly );
    END_LUA_CALL_PANEL_METHOD( 1, 1 );

    RETURN_LUA_PANEL_BOOLEAN();
#endif

    // the sheet should have the pages apply changes before we tell the world
    _propertySheet->ApplyChanges();

    // this should tell anybody who's watching us that we're done
    PostActionSignal( new KeyValues( "ApplyChanges" ) );

    // default to closing
    return true;
}

//-----------------------------------------------------------------------------
// Purpose: enable/disable the apply button
//-----------------------------------------------------------------------------
void LPropertyDialog::EnableApplyButton( bool bEnable )
{
    _applyButton->SetEnabled( bEnable );
    InvalidateLayout();
}

/*
** access functions (stack -> C)
*/

LUA_API lua_PropertyDialog *lua_topropertydialog( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_PropertyDialog * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushpropertydialog( lua_State *L, lua_PropertyDialog *pDialog )
{
    LUA_PUSH_PANEL_USERDATA( L, pDialog, lua_PropertyDialog, "PropertyDialog" );
}

LUALIB_API lua_PropertyDialog *luaL_checkpropertydialog( lua_State *L,
                                                         int narg )
{
    lua_PropertyDialog *d = lua_topropertydialog( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "PropertyDialog expected, got INVALID_PANEL" );
    return d;
}

static int PropertyDialog_AddPage( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->AddPage( luaL_checkpanel( L, 2 ),
                                               luaL_checkstring( L, 3 ) );
    return 0;
}

static int PropertyDialog_ApplyChanges( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->ApplyChanges();
    return 0;
}

static int PropertyDialog_ChainToAnimationMap( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->ChainToAnimationMap();
    return 0;
}

static int PropertyDialog_ChainToMap( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->ChainToMap();
    return 0;
}

static int PropertyDialog_EnableApplyButton( lua_State *L )
{
    LPropertyDialog *plDialog =
        dynamic_cast< LPropertyDialog * >( luaL_checkpropertydialog( L, 1 ) );
    if ( plDialog )
        plDialog->EnableApplyButton( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int PropertyDialog_GetActivePage( lua_State *L )
{
    lua_pushpanel( L, luaL_checkpropertydialog( L, 1 )->GetActivePage() );
    return 1;
}

static int PropertyDialog_GetPanelBaseClassName( lua_State *L )
{
    lua_pushstring( L, luaL_checkpropertydialog( L, 1 )->GetPanelBaseClassName() );
    return 1;
}

static int PropertyDialog_GetPanelClassName( lua_State *L )
{
    lua_pushstring( L, luaL_checkpropertydialog( L, 1 )->GetPanelClassName() );
    return 1;
}

static int PropertyDialog_KB_AddBoundKey( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->KB_AddBoundKey(
        luaL_checkstring( L, 2 ), luaL_checknumber( L, 3 ), luaL_checknumber( L, 4 ) );
    return 0;
}

static int PropertyDialog_KB_ChainToMap( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->KB_ChainToMap();
    return 0;
}

static int PropertyDialog_ResetAllData( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->ResetAllData();
    return 0;
}

static int PropertyDialog_SetApplyButtonText( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->SetApplyButtonText( luaL_checkstring( L, 2 ) );
    return 0;
}

static int PropertyDialog_SetApplyButtonVisible( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->SetApplyButtonVisible(
        luaL_checkboolean( L, 2 ) );
    return 0;
}

static int PropertyDialog_SetCancelButtonText( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->SetCancelButtonText( luaL_checkstring( L, 2 ) );
    return 0;
}

static int PropertyDialog_SetCancelButtonVisible( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->SetCancelButtonVisible(
        luaL_checkboolean( L, 2 ) );
    return 0;
}

static int PropertyDialog_SetOKButtonText( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->SetOKButtonText( luaL_checkstring( L, 2 ) );
    return 0;
}

static int PropertyDialog_SetOKButtonVisible( lua_State *L )
{
    luaL_checkpropertydialog( L, 1 )->SetOKButtonVisible( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int PropertyDialog___index( lua_State *L )
{
    PropertyDialog *pDialog = lua_topropertydialog( L, 1 );
    LUA_METATABLE_INDEX_CHECK_VALID( L, Panel_IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pDialog );

    LPropertyDialog *plDialog = dynamic_cast< LPropertyDialog * >( pDialog );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plDialog );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "Frame" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    luaL_getmetatable( L, "EditablePanel" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    luaL_getmetatable( L, "Panel" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    lua_pushnil( L );
    return 1;
}

static int PropertyDialog___newindex( lua_State *L )
{
    PropertyDialog *pDialog = lua_topropertydialog( L, 1 );

    if ( pDialog == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LPropertyDialog *plDialog = dynamic_cast< LPropertyDialog * >( pDialog );

    LUA_GET_REF_TABLE( L, plDialog );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, luaL_checkstring( L, 2 ) );
    lua_pop( L, 1 );
    return 0;
}

static int PropertyDialog___eq( lua_State *L )
{
    lua_pushboolean( L,
                     lua_topropertydialog( L, 1 ) == lua_topropertydialog( L, 2 ) );
    return 1;
}

static int PropertyDialog___tostring( lua_State *L )
{
    PropertyDialog *pDialog = lua_topropertydialog( L, 1 );
    if ( pDialog == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pDialog->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "PropertyDialog: \"%s\"", pName );
    }
    return 1;
}

static const luaL_Reg PropertyDialogmeta[] = {
    { "AddPage", PropertyDialog_AddPage },
    { "ApplyChanges", PropertyDialog_ApplyChanges },
    { "ChainToAnimationMap", PropertyDialog_ChainToAnimationMap },
    { "ChainToMap", PropertyDialog_ChainToMap },
    { "EnableApplyButton", PropertyDialog_EnableApplyButton },
    { "GetActivePage", PropertyDialog_GetActivePage },
    { "GetPanelBaseClassName", PropertyDialog_GetPanelBaseClassName },
    { "GetPanelClassName", PropertyDialog_GetPanelClassName },
    { "KB_AddBoundKey", PropertyDialog_KB_AddBoundKey },
    { "KB_ChainToMap", PropertyDialog_KB_ChainToMap },
    { "ResetAllData", PropertyDialog_ResetAllData },
    { "SetApplyButtonText", PropertyDialog_SetApplyButtonText },
    { "SetApplyButtonVisible", PropertyDialog_SetApplyButtonVisible },
    { "SetCancelButtonText", PropertyDialog_SetCancelButtonText },
    { "SetCancelButtonVisible", PropertyDialog_SetCancelButtonVisible },
    { "SetOKButtonText", PropertyDialog_SetOKButtonText },
    { "SetOKButtonVisible", PropertyDialog_SetOKButtonVisible },
    { "__index", PropertyDialog___index },
    { "__newindex", PropertyDialog___newindex },
    { "__eq", PropertyDialog___eq },
    { "__tostring", PropertyDialog___tostring },
    { "__gc", Panel___gc },
    { NULL, NULL } };

static int luasrc_PropertyDialog( lua_State *L )
{
    lua_PropertyDialog *pPanel =
        new lua_PropertyDialog( luaL_optpanel( L, 1, VGui_GetClientLuaRootPanel() ),
                             luaL_checkstring( L, 2 ),
                             L );
    lua_pushpropertydialog( L, pPanel );
    return 1;
}

static const luaL_Reg PropertyDialog_funcs[] = {
    { "PropertyDialog", luasrc_PropertyDialog },
    { NULL, NULL } };

/*
** Open PropertyDialog object
*/
LUALIB_API int luaopen_vgui_PropertyDialog( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "PropertyDialog" );
    luaL_register( L, NULL, PropertyDialogmeta );
    lua_pushstring( L, LUA_PANELLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */
    luaL_register( L, LUA_VGUILIBNAME, PropertyDialog_funcs );
    lua_pop( L, 2 );
    return 0;
}
