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
LLabel::LLabel( Panel *parent, const char *panelName, const char *text, lua_State *L /* = nullptr */ )
    : Label( parent, panelName, text, L )
{
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

LUALIB_API lua_Label *luaL_checklabel( lua_State *L, int narg )
{
    lua_Label *d = lua_tolabel( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "Label expected, got INVALID_PANEL" );
    return d;
}

LUA_REGISTRATION_INIT( Label )

// LUA_BINDING_BEGIN( Label, ChainToAnimationMap, "class|static", "Chains the panel to an animation map" )
//{
//     lua_Label::ChainToAnimationMap();
//     return 0;
// }
// LUA_BINDING_END()

// LUA_BINDING_BEGIN( Label, ChainToMap, "class|static", "Chains the panel to a map" )
//{
//     lua_Label::ChainToMap();
//     return 0;
// }
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, GetContentSize, "class", "Gets the size of the content" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    int wide, tall;
    label->GetContentSize( wide, tall );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 2;
}
LUA_BINDING_END( "integer", "The width of the content", "integer", "The height of the content" )

LUA_BINDING_BEGIN( Label, GetValue, "class", "Gets the value of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    char buffer[8092];
    label->GetText( buffer, sizeof( buffer ) );
    lua_pushstring( L, buffer );
    return 1;
}
LUA_BINDING_END( "string", "The value of the label" )

// LUA_BINDING_BEGIN( Label, AddBoundKey, "class|static", "Adds a bound key to the label" )
//{
//     const char *bindingName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "bindingName" );
//     KeyCode keyCode = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 2, "keyCode" );
//     int modifiers = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "modifiers" );
//     lua_Label::KB_AddBoundKey( bindingName, keyCode, modifiers );
//     return 0;
// }
// LUA_BINDING_END()

// LUA_BINDING_BEGIN( Label, ChainToKeyBindingMap, "class|static", "Chains the label to a map" )
//{
//     lua_Label::KB_ChainToMap();
//     return 0;
// }
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, SizeToContents, "class", "Sizes the label to its contents" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SizeToContents();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, SetContentAlignment, "class", "Sets the content alignment of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    vgui::Label::Alignment alignment = LUA_BINDING_ARGUMENT_ENUM( vgui::Label::Alignment, 2, "alignment" );
    label->SetContentAlignment( alignment );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, SetFont, "class", "Sets the font of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SetFont( LUA_BINDING_ARGUMENT( luaL_checkfont, 2, "font" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, GetFont, "class", "Gets the font of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    lua_pushfont( L, label->GetFont() );
    return 1;
}
LUA_BINDING_END( "font", "The font of the label" )

LUA_BINDING_BEGIN( Label, GetText, "class", "Gets the text of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    char buffer[1024];
    label->GetText( buffer, sizeof( buffer ) );
    lua_pushstring( L, buffer );
    return 1;
}
LUA_BINDING_END( "string", "The text of the label" )

LUA_BINDING_BEGIN( Label, SetText, "class", "Sets the text of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SetText( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "text" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, GetTextInset, "class", "Gets the text inset of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    int xInset, yInset;
    label->GetTextInset( &xInset, &yInset );
    lua_pushinteger( L, xInset );
    lua_pushinteger( L, yInset );
    return 2;
}
LUA_BINDING_END( "integer", "The x inset of the label", "integer", "The y inset of the label" )

LUA_BINDING_BEGIN( Label, SetTextInset, "class", "Sets the text inset of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SetTextInset(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "xInset" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "yInset" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, GetHotKey, "class", "Gets the hotkey of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    lua_pushinteger( L, label->GetHotKey() );
    return 1;
}
LUA_BINDING_END( "integer", "The hotkey of the label" )

// Experiment; Disabled since this might return a non-Lua panel
// LUA_BINDING_BEGIN( Label, HasHotkey, "class", "Checks if the label has a hotkey" )
//{
//    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
//    label->HasHotkey( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" ) )->PushLuaInstance( L );
//    return 1;
//}
// LUA_BINDING_END( "label", "The label with the hotkey" )

LUA_BINDING_BEGIN( Label, SetHotkey, "class", "Sets the hotkey of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SetHotkey( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, SetAssociatedControl, "class", "Sets the associated control of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SetAssociatedControl( LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "control" ) );
    return 0;
}
LUA_BINDING_END()

// TODO: Implement luaL_checkimage etc. (low priority, used nowhere)
// static int Label_AddImage( lua_State *L )
//{
//    luaL_checklabel( L, 1 )->AddImage( luaL_checkimage( L, 2 ), luaL_checknumber( L, 3 ) );
//    return 0;
//}
//
// static int Label_SetImageAtIndex( lua_State *L )
//{
//    luaL_checklabel( L, 1 )->SetImageAtIndex( luaL_checknumber( L, 2 ), luaL_checkimage( L, 3 ), luaL_checknumber( L, 4 ) );
//    return 0;
//}
// static int Label_GetImageAtIndex( lua_State *L )
//{
//     lua_pushimage( L, luaL_checklabel( L, 1 )->GetImageAtIndex( luaL_checknumber( L, 2 ) ) );
//     return 1;
// }
// static int Label_GetTextImage( lua_State *L )
//{
//     lua_pushimage( L, luaL_checklabel( L, 1 )->GetTextImage() );
//     return 1;
// }

LUA_BINDING_BEGIN( Label, SetImagePreOffset, "class", "Sets the image pre-offset of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SetImagePreOffset(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, GetImageCount, "class", "Gets the image count of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    lua_pushinteger( L, label->GetImageCount() );
    return 1;
}
LUA_BINDING_END( "integer", "The image count of the label" )

LUA_BINDING_BEGIN( Label, ClearImages, "class", "Clears the images of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->ClearImages();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, ResetToSimpleTextImage, "class", "Resets the label to a simple text image" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->ResetToSimpleTextImage();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, SetImageBounds, "class", "Sets the image bounds of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SetImageBounds(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "x" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "width" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, SetTextImageIndex, "class", "Sets the text image index of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    lua_pushinteger( L, label->SetTextImageIndex( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The text image index of the label" )

LUA_BINDING_BEGIN( Label, SetWrap, "class", "Sets the wrap of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SetWrap( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldWrap" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, SetCenterWrap, "class", "Sets the center wrap of the label" )
{
    lua_Label *label = LUA_BINDING_ARGUMENT( luaL_checklabel, 1, "label" );
    label->SetCenterWrap( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldCenterWrap" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, __index, "class", "Metamethod that is called when a non-existant field is indexed" )
{
    lua_Label *pLabel = LUA_BINDING_ARGUMENT( lua_tolabel, 1, "label" );
    LUA_METATABLE_INDEX_CHECK_VALID( L, PanelIsValid );
    LUA_METATABLE_INDEX_CHECK( L, pLabel );

    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LLabel *plLabel = dynamic_cast< LLabel * >( pLabel );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plLabel );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "Label" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "Panel" );

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( Label, __newindex, "class", "Metamethod that is called when a new field is added to the panel" )
{
    lua_Label *pLabel = LUA_BINDING_ARGUMENT( lua_tolabel, 1, "label" );

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

    // Experiment; TODO: Didn't we already fix this? I think we did. Test this.
    // Special case for LButtons
    // TODO: Somehow inherit instead, but multiple inheritance is tricky with our current setup
    // (LButton inherits from Button, which inherits from Panel, like LLabel -> Label -> Panel)
    // This is a quick fix for now
    LButton *plButton = dynamic_cast< LButton * >( pLabel );

    LUA_GET_REF_TABLE( L, plButton );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" ) );
    lua_pop( L, 1 );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Label, __eq, "class", "Metamethod that is called when two labels are compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_tolabel, 1, "label" ) == LUA_BINDING_ARGUMENT( lua_tolabel, 2, "other" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the labels are equal" )

LUA_BINDING_BEGIN( Label, __tostring, "class", "Metamethod that is called when the label is to be treated as a string" )
{
    LLabel *pLabel = LUA_BINDING_ARGUMENT( lua_tolabel, 1, "label" );
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
LUA_BINDING_END( "string", "The string representation of the label" )

LUA_BINDING_BEGIN( Label, __gc, "class", "Metamethod that is called when the panel is garbage collected" )
{
    return PanelCollectGarbage( L );
}
LUA_BINDING_END()

LUA_REGISTRATION_INIT( Panels )

LUA_BINDING_BEGIN( Panels, Label, "library", "Creates a new label" )
{
    Panel *parent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, VGui_GetClientLuaRootPanel(), "parent" );
    const char *text = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "", "text" );
    const char *name = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, "Label", "name" );
    lua_Label *pPanel = new lua_Label( parent, name, text, L );
    LLabel::PushLuaInstanceSafe( L, pPanel );
    return 1;
}
LUA_BINDING_END( "Label", "The new label" )

/*
** Open Label object
*/
LUALIB_API int luaopen_vgui_Label( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "Label" );
    LUA_REGISTRATION_COMMIT( Label );

    lua_pushstring( L, LUA_PANELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Panels );

    lua_pop( L, 2 );

    LUA_SET_ENUM_LIB_BEGIN( L, "LABEL_ALIGNMENT" );
    lua_pushenum( L, vgui::Label::Alignment::a_northwest, "NORTH_WEST" );
    lua_pushenum( L, vgui::Label::Alignment::a_north, "NORTH" );
    lua_pushenum( L, vgui::Label::Alignment::a_northeast, "NORTH_EAST" );
    lua_pushenum( L, vgui::Label::Alignment::a_west, "WEST" );
    lua_pushenum( L, vgui::Label::Alignment::a_center, "CENTER" );
    lua_pushenum( L, vgui::Label::Alignment::a_east, "EAST" );
    lua_pushenum( L, vgui::Label::Alignment::a_southwest, "SOUTH_WEST" );
    lua_pushenum( L, vgui::Label::Alignment::a_south, "SOUTH" );
    lua_pushenum( L, vgui::Label::Alignment::a_southeast, "SOUTH_EAST" );
    LUA_SET_ENUM_LIB_END( L );

    return 0;
}
