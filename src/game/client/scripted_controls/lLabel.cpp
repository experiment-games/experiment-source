#include <cbase.h>

#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <lColor.h>

#include <scripted_controls/lLabel.h>
#include "scripted_controls/lPanel.h"
#include <scripted_controls/lButton.h>
#include <vgui/LVGUI.h>
#include <LKeyValues.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LLabel::LLabel( Panel *parent, const char *panelName, const char *text, lua_State *L )
    : Label( parent, panelName, text )
{
    m_lua_State = L;
}

void LLabel::PushPanelToLua( lua_State *L )
{
    lua_pushlabel( L, this );
}

/*
** access functions (stack -> C)
*/

LUA_API lua_Label *lua_tolabel( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_Label * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushlabel( lua_State *L, Label *pLabel )
{
    LUA_PUSH_PANEL_USERDATA( L, pLabel, LLabel, "Label" );
}

LUALIB_API lua_Label *luaL_checklabel( lua_State *L, int narg )
{
    lua_Label *d = lua_tolabel( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "Label expected, got INVALID_PANEL" );
    return d;
}

static int Label_ChainToAnimationMap( lua_State *L )
{
    luaL_checklabel( L, 1 )->ChainToAnimationMap();
    return 0;
}

static int Label_ChainToMap( lua_State *L )
{
    luaL_checklabel( L, 1 )->ChainToMap();
    return 0;
}

static int Label_GetContentSize( lua_State *L )
{
    int wide, tall;
    luaL_checklabel( L, 1 )->GetContentSize( wide, tall );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 2;
}

static int Label_GetPanelBaseClassName( lua_State *L )
{
    lua_pushstring( L, luaL_checklabel( L, 1 )->GetPanelBaseClassName() );
    return 1;
}

static int Label_GetPanelClassName( lua_State *L )
{
    lua_pushstring( L, luaL_checklabel( L, 1 )->GetPanelClassName() );
    return 1;
}

static int Label_GetValue( lua_State *L )
{
    char buf[8092];
    luaL_checklabel( L, 1 )->GetText( buf, sizeof( buf ) );
    lua_pushstring( L, buf );
    return 1;
}

static int Label_KB_AddBoundKey( lua_State *L )
{
    luaL_checklabel( L, 1 )->KB_AddBoundKey(
        luaL_checkstring( L, 2 ), luaL_checkint( L, 3 ), luaL_checkint( L, 4 ) );
    return 0;
}

static int Label_KB_ChainToMap( lua_State *L )
{
    luaL_checklabel( L, 1 )->KB_ChainToMap();
    return 0;
}

static int Label_OnCursorEntered( lua_State *L )
{
    luaL_checklabel( L, 1 )->OnCursorEntered();
    return 0;
}

static int Label_OnCursorExited( lua_State *L )
{
    luaL_checklabel( L, 1 )->OnCursorExited();
    return 0;
}

static int Label_OnKillFocus( lua_State *L )
{
    luaL_checklabel( L, 1 )->OnKillFocus();
    return 0;
}

static int Label_OnSetFocus( lua_State *L )
{
    luaL_checklabel( L, 1 )->OnSetFocus();
    return 0;
}

static int Label_PerformLayout( lua_State *L )
{
    luaL_checklabel( L, 1 )->PerformLayout();
    return 0;
}

static int Label_SizeToContents( lua_State *L )
{
    luaL_checklabel( L, 1 )->SizeToContents();
    return 0;
}

static int Label_SetContentAlignment( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetContentAlignment( ( vgui::Label::Alignment )luaL_checkint( L, 2 ) );
    return 0;
}

static int Label_SetFont( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetFont( luaL_checkfont( L, 2 ) );
    return 0;
}

static int Label_GetFont( lua_State *L )
{
    lua_pushfont( L, luaL_checklabel( L, 1 )->GetFont() );
    return 1;
}

static int Label_SetFgColor( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetFgColor( luaL_checkcolor( L, 2 ) );
    return 0;
}

static int Label_GetFgColor( lua_State *L )
{
    lua_pushcolor( L, luaL_checklabel( L, 1 )->GetFgColor() );
    return 1;
}

static int Label_GetText( lua_State *L )
{
    char textBuffer[1024];
    luaL_checklabel( L, 1 )->GetText( textBuffer, sizeof( textBuffer ) );
    lua_pushstring( L, textBuffer );
    return 1;
}

static int Label_SetText( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetText( luaL_checkstring( L, 2 ) );
    return 0;
}

static int Label_GetTextInset( lua_State *L )
{
    int xInset, yInset;
    luaL_checklabel( L, 1 )->GetTextInset( &xInset, &yInset );
    lua_pushinteger( L, xInset );
    lua_pushinteger( L, yInset );
    return 2;
}

static int Label_SetTextInset( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetTextInset( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
    return 0;
}

static int Label_GetHotKey( lua_State *L )
{
    lua_pushinteger( L, luaL_checklabel( L, 1 )->GetHotKey() );
    return 1;
}

static int Label_HasHotkey( lua_State *L )
{
    lua_pushpanel( L, luaL_checklabel( L, 1 )->HasHotkey( luaL_checkint( L, 2 ) ) );
    return 1;
}

static int Label_SetHotkey( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetHotkey( luaL_checkint( L, 2 ) );
    return 0;
}

static int Label_SetAssociatedControl( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetAssociatedControl( luaL_checkpanel( L, 2 ) );
    return 0;
}

// TODO: Implement luaL_checkimage etc. (low priority, used nowhere)
// static int Label_AddImage( lua_State *L )
//{
//    luaL_checklabel( L, 1 )->AddImage( luaL_checkimage( L, 2 ), luaL_checkint( L, 3 ) );
//    return 0;
//}
//
// static int Label_SetImageAtIndex( lua_State *L )
//{
//    luaL_checklabel( L, 1 )->SetImageAtIndex( luaL_checkint( L, 2 ), luaL_checkimage( L, 3 ), luaL_checkint( L, 4 ) );
//    return 0;
//}

static int Label_SetImagePreOffset( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetImagePreOffset( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
    return 0;
}

// static int Label_GetImageAtIndex( lua_State *L )
//{
//     lua_pushimage( L, luaL_checklabel( L, 1 )->GetImageAtIndex( luaL_checkint( L, 2 ) ) );
//     return 1;
// }

static int Label_GetImageCount( lua_State *L )
{
    lua_pushinteger( L, luaL_checklabel( L, 1 )->GetImageCount() );
    return 1;
}

static int Label_ClearImages( lua_State *L )
{
    luaL_checklabel( L, 1 )->ClearImages();
    return 0;
}

static int Label_ResetToSimpleTextImage( lua_State *L )
{
    luaL_checklabel( L, 1 )->ResetToSimpleTextImage();
    return 0;
}

static int Label_SetImageBounds( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetImageBounds( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ), luaL_checknumber( L, 4 ) );
    return 0;
}

// static int Label_GetTextImage( lua_State *L )
//{
//     lua_pushimage( L, luaL_checklabel( L, 1 )->GetTextImage() );
//     return 1;
// }

static int Label_SetTextImageIndex( lua_State *L )
{
    lua_pushinteger( L, luaL_checklabel( L, 1 )->SetTextImageIndex( luaL_checkint( L, 2 ) ) );
    return 1;
}

static int Label_SetWrap( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetWrap( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int Label_SetCenterWrap( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetCenterWrap( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int Label_RequestInfo( lua_State *L )
{
    lua_pushboolean( L, luaL_checklabel( L, 1 )->RequestInfo( luaL_checkkeyvalues( L, 2 ) ) );
    return 1;
}

static int Label___index( lua_State *L )
{
    Label *pLabel = lua_tolabel( L, 1 );
    LUA_METATABLE_INDEX_CHECK_VALID( L, Panel_IsValid );
    LUA_METATABLE_INDEX_CHECK_NULL( L, pLabel );

    LLabel *plLabel = dynamic_cast< LLabel * >( pLabel );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plLabel );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "Panel" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    lua_pushnil( L );
    return 1;
}

static int Label___newindex( lua_State *L )
{
    Label *pLabel = lua_tolabel( L, 1 );

    if ( pLabel == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LLabel *plLabel = dynamic_cast< LLabel * >( pLabel );

    if ( plLabel )
    {
        LUA_GET_REF_TABLE( L, plLabel );
        lua_pushvalue( L, 3 );
        lua_setfield( L, -2, luaL_checkstring( L, 2 ) );
        lua_pop( L, 1 );

        return 0;
    }

    // Special case for LButtons
    // TODO: Somehow inherit instead, but multiple inheritance is tricky with our current setup
    // (LButton inherits from Button, which inherits from Panel, like LLabel -> Label -> Panel)
    // This is a quick fix for now
    LButton *plButton = dynamic_cast< LButton * >( pLabel );

    if ( plButton )
    {
        LUA_GET_REF_TABLE( L, plButton );
        lua_pushvalue( L, 3 );
        lua_setfield( L, -2, luaL_checkstring( L, 2 ) );
        lua_pop( L, 1 );
        return 0;
    }

    lua_Debug ar1;
    lua_getstack( L, 1, &ar1 );
    lua_getinfo( L, "fl", &ar1 );
    lua_Debug ar2;
    lua_getinfo( L, ">S", &ar2 );
    lua_pushfstring( L, "%s:%d: attempt to index a non-scripted panel", ar2.short_src, ar1.currentline );
    return lua_error( L );
}

static int Label___eq( lua_State *L )
{
    lua_pushboolean( L, lua_tolabel( L, 1 ) == lua_tolabel( L, 2 ) );
    return 1;
}

static int Label___tostring( lua_State *L )
{
    Label *pLabel = lua_tolabel( L, 1 );
    if ( pLabel == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pLabel->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "Label: \"%s\"", pName );
    }
    return 1;
}

static const luaL_Reg Labelmeta[] = {
    { "ChainToAnimationMap", Label_ChainToAnimationMap },
    { "ChainToMap", Label_ChainToMap },
    { "GetContentSize", Label_GetContentSize },
    { "GetPanelBaseClassName", Label_GetPanelBaseClassName },
    { "GetPanelClassName", Label_GetPanelClassName },
    { "GetValue", Label_GetValue },
    { "KB_AddBoundKey", Label_KB_AddBoundKey },
    { "KB_ChainToMap", Label_KB_ChainToMap },
    { "OnCursorEntered", Label_OnCursorEntered },
    { "OnCursorExited", Label_OnCursorExited },
    { "OnKillFocus", Label_OnKillFocus },
    { "OnSetFocus", Label_OnSetFocus },
    { "PerformLayout", Label_PerformLayout },
    { "SizeToContents", Label_SizeToContents },
    { "SetContentAlignment", Label_SetContentAlignment },
    { "GetText", Label_GetText },
    { "SetText", Label_SetText },
    { "SetFont", Label_SetFont },
    { "GetFont", Label_GetFont },
    { "SetFgColor", Label_SetFgColor },
    { "GetFgColor", Label_GetFgColor },
    { "GetTextInset", Label_GetTextInset },
    { "SetTextInset", Label_SetTextInset },
    { "GetHotKey", Label_GetHotKey },
    { "HasHotkey", Label_HasHotkey },
    { "SetHotkey", Label_SetHotkey },
    { "SetAssociatedControl", Label_SetAssociatedControl },
    //{ "AddImage", Label_AddImage },
    //{ "SetImageAtIndex", Label_SetImageAtIndex },
    { "SetImagePreOffset", Label_SetImagePreOffset },
    //{ "GetImageAtIndex", Label_GetImageAtIndex },
    { "GetImageCount", Label_GetImageCount },
    { "ClearImages", Label_ClearImages },
    { "ResetToSimpleTextImage", Label_ResetToSimpleTextImage },
    { "SetImageBounds", Label_SetImageBounds },
    //{ "GetTextImage", Label_GetTextImage },
    { "SetTextImageIndex", Label_SetTextImageIndex },
    { "SetWrap", Label_SetWrap },
    { "SetCenterWrap", Label_SetCenterWrap },
    { "RequestInfo", Label_RequestInfo },
    { "__index", Label___index },
    { "__newindex", Label___newindex },
    { "__eq", Label___eq },
    { "__tostring", Label___tostring },
    { "__gc", Panel___gc },

    { NULL, NULL } };

static int luasrc_Label( lua_State *L )
{
    Label *pLabel =
        new LLabel( luaL_optpanel( L, 1, VGui_GetClientLuaRootPanel() ),
                    luaL_checkstring( L, 2 ),
                    luaL_optstring( L, 3, "Label" ),
                    L );
    lua_pushlabel( L, pLabel );
    return 1;
}

static const luaL_Reg Label_funcs[] = { { "Label", luasrc_Label },
                                        { NULL, NULL } };

/*
** Open Label object
*/
LUALIB_API int luaopen_vgui_Label( lua_State *L )
{
    luaL_newmetatable( L, "Label" );
    luaL_register( L, NULL, Labelmeta );
    lua_pushstring( L, LUA_PANELLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */
    luaL_register( L, LUA_VGUILIBNAME, Label_funcs );
    lua_pop( L, 2 );
    return 0;
}
