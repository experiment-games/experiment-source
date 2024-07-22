#include <cbase.h>

#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <lColor.h>
#include <LKeyValues.h>
#include <vgui/LVGUI.h>

#include <scripted_controls/lTextEntry.h>
#include "scripted_controls/lPanel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LTextEntry::LTextEntry( Panel *parent, const char *panelName, lua_State *L )
    : TextEntry( parent, panelName )
{
    m_lua_State = L;
}

/*
** access functions (stack -> C)
*/

LUA_API lua_TextEntry *lua_totextentry( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_TextEntry * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_TextEntry *luaL_checktextentry( lua_State *L, int narg )
{
    lua_TextEntry *d = lua_totextentry( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "TextEntry expected, got INVALID_PANEL" );
    return d;
}

static int TextEntry_SetText( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetText( luaL_checkstring( L, 2 ) );
    return 0;
}

static int TextEntry_GetCursorPos( lua_State *L )
{
    int position = luaL_checktextentry( L, 1 )->GetCursorPos();
    lua_pushinteger( L, position );
    return 1;
}

static int TextEntry_SetCursorPos( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetCursorPos( luaL_checkinteger( L, 2 ) );
    return 0;
}

static int TextEntry_GetText( lua_State *L )
{
    char buf[1023];
    luaL_checktextentry( L, 1 )->GetText( buf, sizeof( buf ) );
    lua_pushstring( L, buf );
    return 1;
}

static int TextEntry_GetTextLength( lua_State *L )
{
    lua_pushinteger( L, luaL_checktextentry( L, 1 )->GetTextLength() );
    return 1;
}

static int TextEntry_GetValue( lua_State *L )
{
    char buf[8092];
    luaL_checktextentry( L, 1 )->GetText( buf, sizeof( buf ) );
    lua_pushstring( L, buf );
    return 1;
}

static int TextEntry_GetValueAsFloat( lua_State *L )
{
    float value = luaL_checktextentry( L, 1 )->GetValueAsFloat();
    lua_pushnumber( L, value );
    return 1;
}

static int TextEntry_GetValueAsInt( lua_State *L )
{
    int value = luaL_checktextentry( L, 1 )->GetValueAsInt();
    lua_pushinteger( L, value );
    return 1;
}

static int TextEntry_IsTextFullySelected( lua_State *L )
{
    lua_pushboolean( L, luaL_checktextentry( L, 1 )->IsTextFullySelected() );
    return 1;
}

static int TextEntry_GotoLeft( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoLeft();
    return 0;
}

static int TextEntry_GotoRight( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoRight();
    return 0;
}

static int TextEntry_GotoUp( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoUp();
    return 0;
}

static int TextEntry_GotoDown( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoDown();
    return 0;
}

static int TextEntry_GotoWordRight( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoWordRight();
    return 0;
}

static int TextEntry_GotoWordLeft( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoWordLeft();
    return 0;
}

static int TextEntry_GotoFirstOfLine( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoFirstOfLine();
    return 0;
}

static int TextEntry_GotoEndOfLine( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoEndOfLine();
    return 0;
}

static int TextEntry_GotoTextStart( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoTextStart();
    return 0;
}

static int TextEntry_GotoTextEnd( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GotoTextEnd();
    return 0;
}

static int TextEntry_InsertString( lua_State *L )
{
    luaL_checktextentry( L, 1 )->InsertString( luaL_checkstring( L, 2 ) );
    return 0;
}

static int TextEntry_Backspace( lua_State *L )
{
    luaL_checktextentry( L, 1 )->Backspace();
    return 0;
}

static int TextEntry_Delete( lua_State *L )
{
    luaL_checktextentry( L, 1 )->Delete();
    return 0;
}

static int TextEntry_SelectNone( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SelectNone();
    return 0;
}

static int TextEntry_OpenEditMenu( lua_State *L )
{
    luaL_checktextentry( L, 1 )->OpenEditMenu();
    return 0;
}

static int TextEntry_DeleteSelected( lua_State *L )
{
    luaL_checktextentry( L, 1 )->DeleteSelected();
    return 0;
}

static int TextEntry_Undo( lua_State *L )
{
    luaL_checktextentry( L, 1 )->Undo();
    return 0;
}

static int TextEntry_SaveUndoState( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SaveUndoState();
    return 0;
}

static int TextEntry_SetFont( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetFont( luaL_checkfont( L, 2 ) );
    return 0;
}

static int TextEntry_SetTextHidden( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetTextHidden( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SetEditable( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetEditable( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_IsEditable( lua_State *L )
{
    lua_pushboolean( L, luaL_checktextentry( L, 1 )->IsEditable() );
    return 1;
}

static int TextEntry_SetEnabled( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetEnabled( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_MoveCursor( lua_State *L )
{
    luaL_checktextentry( L, 1 )->MoveCursor( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
    return 0;
}

static int TextEntry_PaintText( lua_State *L )
{
    luaL_checktextentry( L, 1 )->PaintText( luaL_checkcolor( L, 2 ), luaL_checkcolor( L, 3 ), luaL_checkcolor( L, 4 ) );
    return 0;
}

static int TextEntry_SetDisabledBgColor( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetDisabledBgColor( luaL_checkcolor( L, 2 ) );
    return 0;
}

static int TextEntry_SetMultiline( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetMultiline( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_IsMultiline( lua_State *L )
{
    lua_pushboolean( L, luaL_checktextentry( L, 1 )->IsMultiline() );
    return 1;
}

static int TextEntry_SetVerticalScrollbar( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetVerticalScrollbar( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SetCatchEnterKey( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetCatchEnterKey( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SendNewLine( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SendNewLine( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SetMaximumCharCount( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetMaximumCharCount( luaL_checkinteger( L, 2 ) );
    return 0;
}

static int TextEntry_GetMaximumCharCount( lua_State *L )
{
    lua_pushinteger( L, luaL_checktextentry( L, 1 )->GetMaximumCharCount() );
    return 1;
}

static int TextEntry_SetAutoProgressOnHittingCharLimit( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetAutoProgressOnHittingCharLimit( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SetWrap( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetWrap( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_RecalculateLineBreaks( lua_State *L )
{
    luaL_checktextentry( L, 1 )->RecalculateLineBreaks();
    return 0;
}

static int TextEntry_LayoutVerticalScrollBarSlider( lua_State *L )
{
    luaL_checktextentry( L, 1 )->LayoutVerticalScrollBarSlider();
    return 0;
}

static int TextEntry_RequestInfo( lua_State *L )
{
    KeyValues *outputData = luaL_checkkeyvalues( L, 2 );
    lua_pushboolean( L, luaL_checktextentry( L, 1 )->RequestInfo( outputData ) );
    return 1;
}

static int TextEntry_SetToFullHeight( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetToFullHeight();
    return 0;
}

static int TextEntry_SetToFullWidth( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetToFullWidth();
    return 0;
}

static int TextEntry_GetNumLines( lua_State *L )
{
    lua_pushinteger( L, luaL_checktextentry( L, 1 )->GetNumLines() );
    return 1;
}

static int TextEntry_SelectAllText( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SelectAllText( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SetDrawWidth( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetDrawWidth( luaL_checknumber( L, 2 ) );
    return 0;
}

static int TextEntry_GetDrawWidth( lua_State *L )
{
    lua_pushinteger( L, luaL_checktextentry( L, 1 )->GetDrawWidth() );
    return 1;
}

static int TextEntry_SetHorizontalScrolling( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetHorizontalScrolling( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SetAllowNonAsciiCharacters( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetAllowNonAsciiCharacters( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SetAllowNumericInputOnly( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetAllowNumericInputOnly( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SetDrawLanguageIDAtLeft( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetDrawLanguageIDAtLeft( lua_toboolean( L, 2 ) );
    return 0;
}

// static int TextEntry_GetDropContextMenu( lua_State *L )
//{
//     Menu *menu = luaL_checkmenu( L, 2 ); // TODO: Low priority, used nowhere
//     CUtlVector< KeyValues * > data;
//     luaL_checktextentry( L, 1 )->GetDropContextMenu( menu, data );
//     lua_pushboolean( L, data.Count() > 0 );
//     return 1;
// }

static int TextEntry_IsDroppable( lua_State *L )
{
    CUtlVector< KeyValues * > data;
    luaL_checktextentry( L, 1 )->IsDroppable( data );
    lua_pushboolean( L, data.Count() > 0 );
    return 1;
}

static int TextEntry_OnPanelDropped( lua_State *L )
{
    CUtlVector< KeyValues * > data;
    luaL_checktextentry( L, 1 )->OnPanelDropped( data );
    return 0;
}

static int TextEntry_GetDragPanel( lua_State *L )
{
    luaL_checktextentry( L, 1 )->GetDragPanel()->PushLuaInstance( L );
    return 1;
}

static int TextEntry_OnCreateDragData( lua_State *L )
{
    luaL_checktextentry( L, 1 )->OnCreateDragData( luaL_checkkeyvalues( L, 2 ) );
    return 0;
}

static int TextEntry_SelectAllOnFocusAlways( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SelectAllOnFocusAlways( lua_toboolean( L, 2 ) );
    return 0;
}

static int TextEntry_SetSelectionTextColor( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetSelectionTextColor( luaL_checkcolor( L, 2 ) );
    return 0;
}

static int TextEntry_SetSelectionBgColor( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetSelectionBgColor( luaL_checkcolor( L, 2 ) );
    return 0;
}

static int TextEntry_SetSelectionUnfocusedBgColor( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetSelectionUnfocusedBgColor( luaL_checkcolor( L, 2 ) );
    return 0;
}

static int TextEntry_SetUseFallbackFont( lua_State *L )
{
    luaL_checktextentry( L, 1 )->SetUseFallbackFont( lua_toboolean( L, 2 ), luaL_checkfont( L, 3 ) );
    return 0;
}

static int TextEntry___index( lua_State *L )
{
    TextEntry *pTextEntry = lua_totextentry( L, 1 );
    LUA_METATABLE_INDEX_CHECK_VALID( L, Panel_IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pTextEntry );

    LTextEntry *plTextEntry = dynamic_cast< LTextEntry * >( pTextEntry );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plTextEntry );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "Panel" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    lua_pushnil( L );
    return 1;
}

static int TextEntry___newindex( lua_State *L )
{
    TextEntry *pTextEntry = lua_totextentry( L, 1 );

    if ( pTextEntry == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LTextEntry *plTextEntry = dynamic_cast< LTextEntry * >( pTextEntry );

    LUA_GET_REF_TABLE( L, plTextEntry );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, luaL_checkstring( L, 2 ) );
    lua_pop( L, 1 );
    return 0;
}

static int TextEntry___eq( lua_State *L )
{
    lua_pushboolean( L, lua_totextentry( L, 1 ) == lua_totextentry( L, 2 ) );
    return 1;
}

static int TextEntry___tostring( lua_State *L )
{
    TextEntry *pTextEntry = lua_totextentry( L, 1 );
    if ( pTextEntry == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pTextEntry->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "TextEntry: \"%s\"", pName );
    }
    return 1;
}

static const luaL_Reg TextEntrymeta[] = {
    { "SetText", TextEntry_SetText },
    { "GetText", TextEntry_GetText },
    { "GetCaretPos", TextEntry_GetCursorPos },
    { "SetCaretPos", TextEntry_SetCursorPos },
    { "GetTextLength", TextEntry_GetTextLength },
    { "GetValue", TextEntry_GetValue },
    { "GetValueAsFloat", TextEntry_GetValueAsFloat },
    { "GetValueAsInt", TextEntry_GetValueAsInt },
    { "IsTextFullySelected", TextEntry_IsTextFullySelected },
    { "GotoLeft", TextEntry_GotoLeft },
    { "GotoRight", TextEntry_GotoRight },
    { "GotoUp", TextEntry_GotoUp },
    { "GotoDown", TextEntry_GotoDown },
    { "GotoWordRight", TextEntry_GotoWordRight },
    { "GotoWordLeft", TextEntry_GotoWordLeft },
    { "GotoFirstOfLine", TextEntry_GotoFirstOfLine },
    { "GotoEndOfLine", TextEntry_GotoEndOfLine },
    { "GotoTextStart", TextEntry_GotoTextStart },
    { "GotoTextEnd", TextEntry_GotoTextEnd },
    { "InsertString", TextEntry_InsertString },
    { "Backspace", TextEntry_Backspace },
    { "Delete", TextEntry_Delete },
    { "SelectNone", TextEntry_SelectNone },
    { "OpenEditMenu", TextEntry_OpenEditMenu },
    { "DeleteSelected", TextEntry_DeleteSelected },
    { "Undo", TextEntry_Undo },
    { "SaveUndoState", TextEntry_SaveUndoState },
    { "SetFont", TextEntry_SetFont },
    { "SetTextHidden", TextEntry_SetTextHidden },
    { "SetEditable", TextEntry_SetEditable },
    { "IsEditable", TextEntry_IsEditable },
    { "SetEnabled", TextEntry_SetEnabled },
    { "MoveCursor", TextEntry_MoveCursor },
    { "PaintText", TextEntry_PaintText },
    { "SetDisabledBgColor", TextEntry_SetDisabledBgColor },
    { "SetMultiline", TextEntry_SetMultiline },
    { "IsMultiline", TextEntry_IsMultiline },
    { "SetVerticalScrollbar", TextEntry_SetVerticalScrollbar },
    { "SetCatchEnterKey", TextEntry_SetCatchEnterKey },
    { "SendNewLine", TextEntry_SendNewLine },
    { "SetMaximumCharCount", TextEntry_SetMaximumCharCount },
    { "GetMaximumCharCount", TextEntry_GetMaximumCharCount },
    { "SetAutoProgressOnHittingCharLimit", TextEntry_SetAutoProgressOnHittingCharLimit },
    { "SetWrap", TextEntry_SetWrap },
    { "RecalculateLineBreaks", TextEntry_RecalculateLineBreaks },
    { "LayoutVerticalScrollBarSlider", TextEntry_LayoutVerticalScrollBarSlider },
    { "RequestInfo", TextEntry_RequestInfo },
    { "SetToFullHeight", TextEntry_SetToFullHeight },
    { "SetToFullWidth", TextEntry_SetToFullWidth },
    { "GetNumLines", TextEntry_GetNumLines },
    { "SelectAllText", TextEntry_SelectAllText },
    { "SetDrawWidth", TextEntry_SetDrawWidth },
    { "GetDrawWidth", TextEntry_GetDrawWidth },
    { "SetHorizontalScrolling", TextEntry_SetHorizontalScrolling },
    { "SetAllowNonAsciiCharacters", TextEntry_SetAllowNonAsciiCharacters },
    { "SetAllowNumericInputOnly", TextEntry_SetAllowNumericInputOnly },
    { "SetDrawLanguageIDAtLeft", TextEntry_SetDrawLanguageIDAtLeft },
    // { "GetDropContextMenu", TextEntry_GetDropContextMenu },
    { "IsDroppable", TextEntry_IsDroppable },
    { "OnPanelDropped", TextEntry_OnPanelDropped },
    { "GetDragPanel", TextEntry_GetDragPanel },
    { "OnCreateDragData", TextEntry_OnCreateDragData },
    { "SelectAllOnFocusAlways", TextEntry_SelectAllOnFocusAlways },
    { "SetSelectionTextColor", TextEntry_SetSelectionTextColor },
    { "SetSelectionBgColor", TextEntry_SetSelectionBgColor },
    { "SetSelectionUnfocusedBgColor", TextEntry_SetSelectionUnfocusedBgColor },
    { "SetUseFallbackFont", TextEntry_SetUseFallbackFont },
    { "__index", TextEntry___index },
    { "__newindex", TextEntry___newindex },
    { "__eq", TextEntry___eq },
    { "__tostring", TextEntry___tostring },
    { "__gc", Panel___gc },
    { NULL, NULL } };

static int luasrc_TextEntry( lua_State *L )
{
    lua_TextEntry *pPanel =
        new lua_TextEntry( luaL_optpanel( L, 1, VGui_GetClientLuaRootPanel() ),
                        luaL_optstring( L, 2, "TextEntry" ),
                        L );
    pPanel->PushLuaInstance( L );
    return 1;
}

static const luaL_Reg TextEntry_funcs[] = { { "TextEntry", luasrc_TextEntry },
                                            { NULL, NULL } };

/*
** Open TextEntry object
*/
LUALIB_API int luaopen_vgui_TextEntry( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "TextEntry" );
    luaL_register( L, NULL, TextEntrymeta );
    lua_pushstring( L, LUA_PANELLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */
    luaL_register( L, LUA_VGUILIBNAME, TextEntry_funcs );

    lua_pop( L, 1 );  // Pop the panel library off the stack
    lua_pop( L, 1 );  // Pop the TextEntry metatable

    return 0;
}
