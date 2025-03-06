#include "cbase.h"
#include <vgui_controls/Panel.h>
#include "c_vguiscreen.h"
#include "iclientmode.h"
#include "ienginevgui.h"
#include <vgui/IVGui.h>
#include "panelmetaclassmgr.h"
#include <vgui_controls/PHandle.h>
#include "luamanager.h"
#include "luasrclib.h"
#include "vgui_int.h"
#include "lPanel.h"
#include <scripted_controls/lPanel.h>
#include "lColor.h"
#include "vgui/LVGUI.h"
#include <vgui/LIScheme.h>
#include <LKeyValues.h>

using namespace vgui;

/*
** access functions (stack -> C)
*/

LUA_API lua_Panel *lua_topanel( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return ( lua_Panel * )phPanel->Get();
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_Panel *luaL_checkpanel( lua_State *L, int narg )
{
    lua_Panel *d = lua_topanel( L, narg );

    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "Panel expected, got INVALID_PANEL" );

    return d;
}

LUALIB_API lua_Panel *luaL_optpanel( lua_State *L, int narg, lua_Panel *def )
{
    return luaL_opt( L, luaL_checkpanel, narg, def );
}

int PanelIsValid( lua_State *L )
{
    lua_Panel *d = lua_topanel( L, 1 );
    lua_pushboolean( L, d != NULL );
    return 1;
}

// Experiment; Disabled since we want to work with only vgui::Panel* objects in Lua.
// LUALIB_API VPANEL luaL_checkvpanel( lua_State *L, int narg )
//{
//    lua_Panel *d = lua_topanel( L, narg );
//    if ( d == NULL ) /* avoid extra test when d is not 0 */
//        luaL_argerror( L, narg, "VPanel expected, got INVALID_PANEL" );
//    PHandle hPanel;
//    hPanel.Set( d );
//    return ivgui()->HandleToPanel( hPanel.m_iPanelID );
//}
//
// LUALIB_API VPANEL luaL_optvpanel( lua_State *L, int narg, VPANEL def )
//{
//    return luaL_opt( L, luaL_checkvpanel, narg, def );
//}

LUA_REGISTRATION_INIT( Panel )

// LUA_BINDING_BEGIN( Panel, KB_AddBoundKey, "class|static", "Adds a bound key" )
//{
//     const char *keyName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "keyName" );
//     int keyCode = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "keyCode" );
//     int modifiers = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "modifiers" );
//     lua_Panel::KB_AddBoundKey( keyName, keyCode, modifiers );
//     return 0;
// }
// LUA_BINDING_END()

// LUA_BINDING_BEGIN( Panel, KB_ChainToMap, "class|static", "Chains the Panel to a keyboard map" )
//{
//     lua_Panel::KB_ChainToMap();
//     return 0;
// }
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, AddKeyBinding, "class", "Adds a key binding to the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    const char *key = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" );
    int keyCode = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 3, "keyCode" );
    int modifiers = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "modifiers" );
    panel->AddKeyBinding( key, keyCode, modifiers );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, AddActionSignalTarget, "class", "Adds an action signal target to the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_Panel *target = LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "target" );
    panel->AddActionSignalTarget( target );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, CanStartDragging, "class", "Checks if dragging can start from the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int startX = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "startX" );
    int startY = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "startY" );
    int cursorX = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "cursorX" );
    int cursorY = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "cursorY" );
    lua_pushboolean( L, panel->CanStartDragging( startX, startY, cursorX, cursorY ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether dragging can start" )

// LUA_BINDING_BEGIN( Panel, ChainToAnimationMap, "class|static", "Chains the panel to the animation map" )
//{
//     lua_Panel::ChainToAnimationMap();
//     return 0;
// }
// LUA_BINDING_END()

// LUA_BINDING_BEGIN( Panel, ChainToMap, "class", "Chains the panel to the map" )
//{
//     lua_Panel::ChainToMap();
//     return 0;
// }
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, DeletePanel, "class", "Deletes the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->DeletePanel();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, DisableMouseInputForThisPanel, "class", "Disables mouse input for the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool disable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "disable" );
    panel->DisableMouseInputForThisPanel( disable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, DrawBox, "class", "Draws a box on the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );
    int wide = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "wide" );
    int tall = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "tall" );
    lua_Color &col = LUA_BINDING_ARGUMENT( luaL_checkcolor, 6, "color" );
    int normalizedAlpha = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "normalizedAlpha" );
    bool isHollow = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 8, false, "isHollow" );
    panel->DrawBox( x, y, wide, tall, col, normalizedAlpha, isHollow );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, DrawBoxFade, "class", "Draws a fading box on the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );
    int wide = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "wide" );
    int tall = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "tall" );
    lua_Color &col = LUA_BINDING_ARGUMENT( luaL_checkcolor, 6, "color" );
    int normalizedAlpha = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "normalizedAlpha" );
    int alpha0 = LUA_BINDING_ARGUMENT( luaL_checknumber, 8, "alpha0" );
    int alpha1 = LUA_BINDING_ARGUMENT( luaL_checknumber, 9, "alpha1" );
    bool isHorizontal = LUA_BINDING_ARGUMENT( luaL_checkboolean, 10, "isHorizontal" );
    bool isHollow = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 11, false, "isHollow" );
    panel->DrawBoxFade( x, y, wide, tall, col, normalizedAlpha, alpha0, alpha1, isHorizontal, isHollow );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, DrawHollowBox, "class", "Draws a hollow box on the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );
    int wide = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "wide" );
    int tall = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "tall" );
    lua_Color &col = LUA_BINDING_ARGUMENT( luaL_checkcolor, 6, "color" );
    int normalizedAlpha = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "normalizedAlpha" );
    panel->DrawHollowBox( x, y, wide, tall, col, normalizedAlpha );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, DrawTexturedBox, "class", "Draws a textured box on the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );
    int wide = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "wide" );
    int tall = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "tall" );
    lua_Color &col = LUA_BINDING_ARGUMENT( luaL_checkcolor, 6, "color" );
    int normalizedAlpha = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "normalizedAlpha" );
    panel->DrawTexturedBox( x, y, wide, tall, col, normalizedAlpha );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, EditKeyBindings, "class", "Opens the key bindings editor for the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->EditKeyBindings();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, FillRectangleSkippingPanel, "class", "Fills a rectangle on the panel, skipping a panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_Color &col = LUA_BINDING_ARGUMENT( luaL_checkcolor, 2, "color" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "y" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "width" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "height" );
    lua_Panel *skip = LUA_BINDING_ARGUMENT( luaL_checkpanel, 7, "panelToSkip" );
    panel->FillRectSkippingPanel( col, x, y, width, height, skip );
    return 0;
}
LUA_BINDING_END()

// Experiment; Disabled since this might return a non-Lua panel
// LUA_BINDING_BEGIN( Panel, FindChildByName, "class", "Finds a child panel by its name" )
//{
//    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
//    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" );
//    bool recursive = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "shouldSearchRecursive" );
//    vgui::Panel *pPanel = panel->FindChildByName( name, recursive );
//
//    LPanel::PushLuaInstance( L, pPanel );
//
//    return 1;
//}
// LUA_BINDING_END( "Panel", "The child panel" )

LUA_BINDING_BEGIN( Panel, FindChildIndexByName, "class", "Finds the index of a child panel by its name" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" );
    lua_pushinteger( L, panel->FindChildIndexByName( name ) );
    return 1;
}
LUA_BINDING_END( "integer", "The index of the child panel" )

// Experiment; Disabled since this might return a non-Lua panel
// LUA_BINDING_BEGIN( Panel, FindSiblingByName, "class", "Finds a sibling panel by its name" )
//{
//    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
//    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" );
//    panel->FindSiblingByName( name );
//    LPanel::PushLuaInst
//    return 1;
//}
// LUA_BINDING_END( "Panel", "The sibling panel" )

LUA_BINDING_BEGIN( Panel, GetAlpha, "class", "Gets the alpha value of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetAlpha() );
    return 1;
}
LUA_BINDING_END( "integer", "The alpha value of the panel" )

LUA_BINDING_BEGIN( Panel, GetBackgroundColor, "class", "Gets the background color of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushcolor( L, panel->GetBgColor() );
    return 1;
}
LUA_BINDING_END( "Color", "The background color of the panel" )

LUA_BINDING_BEGIN( Panel, GetBounds, "class", "Gets the bounds of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x, y, wide, tall;
    panel->GetBounds( x, y, wide, tall );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 4;
}
LUA_BINDING_END( "integer", "The x position of the panel", "integer", "The y position of the panel", "integer", "The width of the panel", "integer", "The height of the panel" )

LUA_BINDING_BEGIN( Panel, GetChild, "class", "Gets a child panel by its index (starting at 0)" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int index = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "zeroBasedIndex" );

    // ! Note: We are using 0-based indexing here
    Panel::PushLuaInstanceSafe( L, panel->GetChild( index ) );
    return 1;
}
LUA_BINDING_END( "Panel", "The child panel" )

LUA_BINDING_BEGIN( Panel, GetChildCount, "class", "Gets the number of child panels" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetChildCount() );
    return 1;
}
LUA_BINDING_END( "integer", "The number of child panels" )

LUA_BINDING_BEGIN( Panel, GetChildren, "class", "Gets the child panels" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_newtable( L );
    CUtlVector< VPANEL > &children = panel->GetChildren();

    for ( int i = 0; i < children.Count(); i++ )
    {
        Panel::PushVPanelLuaInstance( L, children[i] );
        lua_rawseti( L, -2, i + 1 );
    }

    lua_pushinteger( L, children.Count() );

    return 2;
}
LUA_BINDING_END( "table", "The child panels", "integer", "The number of child panels" )

LUA_BINDING_BEGIN( Panel, GetChildrenSize, "class", "Gets the size of the child panels" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int wide, tall;
    panel->GetChildrenSize( wide, tall );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 2;
}
LUA_BINDING_END( "integer", "The width of the child panels", "integer", "The height of the child panels" )

LUA_BINDING_BEGIN( Panel, GetClassName, "class", "Gets the class name of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushstring( L, panel->GetClassName() );
    return 1;
}
LUA_BINDING_END( "string", "The class name of the panel" )

LUA_BINDING_BEGIN( Panel, GetClipRectangle, "class", "Gets the clipping rectangle of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x0, y0, x1, y1;
    panel->GetClipRect( x0, y0, x1, y1 );
    lua_pushinteger( L, x0 );
    lua_pushinteger( L, y0 );
    lua_pushinteger( L, x1 );
    lua_pushinteger( L, y1 );
    return 4;
}
LUA_BINDING_END( "integer", "The x position of the clipping rectangle", "integer", "The y position of the clipping rectangle", "integer", "The width of the clipping rectangle", "integer", "The height of the clipping rectangle" )

LUA_BINDING_BEGIN( Panel, GetCornerTextureSize, "class", "Gets the size of the corner texture of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int w, h;
    panel->GetCornerTextureSize( w, h );
    lua_pushinteger( L, w );
    lua_pushinteger( L, h );
    return 2;
}
LUA_BINDING_END( "integer", "The width of the corner texture", "integer", "The height of the corner texture" )

LUA_BINDING_BEGIN( Panel, GetDescription, "class", "Gets the description of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushstring( L, panel->GetDescription() );
    return 1;
}
LUA_BINDING_END( "string", "The description of the panel" )

LUA_BINDING_BEGIN( Panel, GetDragFrameColor, "class", "Gets the color of the drag frame of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushcolor( L, panel->GetDragFrameColor() );
    return 1;
}
LUA_BINDING_END( "Color", "The color of the drag frame of the panel" )

LUA_BINDING_BEGIN( Panel, GetDragPanel, "class", "Gets the drag panel of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    Panel::PushLuaInstanceSafe( L, panel->GetDragPanel() );
    return 1;
}
LUA_BINDING_END( "Panel", "The drag panel of the panel" )

LUA_BINDING_BEGIN( Panel, GetDragStartTolerance, "class", "Gets the drag start tolerance of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetDragStartTolerance() );
    return 1;
}
LUA_BINDING_END( "integer", "The drag start tolerance of the panel" )

LUA_BINDING_BEGIN( Panel, GetDropFrameColor, "class", "Gets the color of the drop frame of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushcolor( L, panel->GetDropFrameColor() );
    return 1;
}
LUA_BINDING_END( "Color", "The color of the drop frame of the panel" )

LUA_BINDING_BEGIN( Panel, GetDock, "class", "Gets the dock of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetDock() );
    return 1;
}
LUA_BINDING_END( "integer", "The dock of the panel" )

LUA_BINDING_BEGIN( Panel, GetDockMargin, "class", "Gets the dock margin of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    Thickness margin = panel->GetDockMargin();
    lua_pushinteger( L, margin.left );
    lua_pushinteger( L, margin.top );
    lua_pushinteger( L, margin.right );
    lua_pushinteger( L, margin.bottom );
    return 4;
}
LUA_BINDING_END( "integer", "The left margin of the dock", "integer", "The top margin of the dock", "integer", "The right margin of the dock", "integer", "The bottom margin of the dock" )

LUA_BINDING_BEGIN( Panel, GetDockPadding, "class", "Gets the dock padding of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    Thickness padding = panel->GetDockPadding();
    lua_pushinteger( L, padding.left );
    lua_pushinteger( L, padding.top );
    lua_pushinteger( L, padding.right );
    lua_pushinteger( L, padding.bottom );
    return 4;
}
LUA_BINDING_END( "integer", "The left padding of the dock", "integer", "The top padding of the dock", "integer", "The right padding of the dock", "integer", "The bottom padding of the dock" )

LUA_BINDING_BEGIN( Panel, GetForegroundColor, "class", "Gets the foreground color of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushcolor( L, panel->GetFgColor() );
    return 1;
}
LUA_BINDING_END( "Color", "The foreground color of the panel" )

LUA_BINDING_BEGIN( Panel, GetInset, "class", "Gets the inset of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int left, top, right, bottom;
    panel->GetInset( left, top, right, bottom );
    lua_pushinteger( L, left );
    lua_pushinteger( L, top );
    lua_pushinteger( L, right );
    lua_pushinteger( L, bottom );
    return 4;
}
LUA_BINDING_END( "integer", "The left inset of the panel", "integer", "The top inset of the panel", "integer", "The right inset of the panel", "integer", "The bottom inset of the panel" )

LUA_BINDING_BEGIN( Panel, GetKeyBindingsFile, "class", "Gets the key bindings file of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushstring( L, panel->GetKeyBindingsFile() );
    return 1;
}
LUA_BINDING_END( "string", "The key bindings file of the panel" )

LUA_BINDING_BEGIN( Panel, GetKeyBindingsFilePathId, "class", "Gets the key bindings file path ID of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushstring( L, panel->GetKeyBindingsFilePathID() );
    return 1;
}
LUA_BINDING_END( "string", "The key bindings file path ID of the panel" )

LUA_BINDING_BEGIN( Panel, GetKeyMappingCount, "class", "Gets the key mapping count of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetKeyMappingCount() );
    return 1;
}
LUA_BINDING_END( "integer", "The key mapping count of the panel" )

LUA_BINDING_BEGIN( Panel, GetLocalCursorPosition, "class", "Gets the local cursor position of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x, y;
    panel->GetLocalCursorPosition( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}
LUA_BINDING_END( "integer", "The x position of the local cursor", "integer", "The y position of the local cursor" )

LUA_BINDING_BEGIN( Panel, GetMinimumSize, "class", "Gets the minimum size of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int wide, tall;
    panel->GetMinimumSize( wide, tall );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 2;
}
LUA_BINDING_END( "integer", "The minimum width of the panel", "integer", "The minimum height of the panel" )

LUA_BINDING_BEGIN( Panel, GetModuleName, "class", "Gets the module name of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushstring( L, panel->GetModuleName() );
    return 1;
}
LUA_BINDING_END( "string", "The module name of the panel" )

LUA_BINDING_BEGIN( Panel, GetName, "class", "Gets the name of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushstring( L, panel->GetName() );
    return 1;
}
LUA_BINDING_END( "string", "The name of the panel" )

LUA_BINDING_BEGIN( Panel, GetPaintBackgroundType, "class", "Gets the paint background type of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetPaintBackgroundType() );
    return 1;
}
LUA_BINDING_END( "integer", "The paint background type of the panel" )

LUA_BINDING_BEGIN( Panel, GetPaintSize, "class", "Gets the paint size of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int wide, tall;
    panel->GetPaintSize( wide, tall );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 2;
}
LUA_BINDING_END( "integer", "The width of the panel", "integer", "The height of the panel" )

LUA_BINDING_BEGIN( Panel, GetParent, "class", "Gets the parent of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_Panel *parent = panel->GetParent();
    Panel::PushLuaInstanceSafe( L, parent );
    return 1;
}
LUA_BINDING_END( "Panel", "The parent of the panel" )

LUA_BINDING_BEGIN( Panel, GetPinCorner, "class", "Gets the pin corner of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetPinCorner() );
    return 1;
}
LUA_BINDING_END( "enumeration/PIN_CORNER", "The pin corner of the panel" )

LUA_BINDING_BEGIN( Panel, GetPinOffset, "class", "Gets the pin offset of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int dx, dy;
    panel->GetPinOffset( dx, dy );
    lua_pushinteger( L, dx );
    lua_pushinteger( L, dy );
    return 2;
}
LUA_BINDING_END( "integer", "The x offset of the pin", "integer", "The y offset of the pin" )

LUA_BINDING_BEGIN( Panel, GetPosition, "class", "Gets the position of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x, y;
    panel->GetPos( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}
LUA_BINDING_END( "integer", "The x position of the panel", "integer", "The y position of the panel" )

LUA_BINDING_BEGIN( Panel, GetRefTable, "class", "Gets the reference table of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    LUA_GET_REF_TABLE( L, panel );
    return 1;
}
LUA_BINDING_END( "table", "The reference table of the panel" )

LUA_BINDING_BEGIN( Panel, GetResizeOffset, "class", "Gets the resize offset of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int dx, dy;
    panel->GetResizeOffset( dx, dy );
    lua_pushinteger( L, dx );
    lua_pushinteger( L, dy );
    return 2;
}
LUA_BINDING_END( "integer", "The x offset of the resize", "integer", "The y offset of the resize" )

LUA_BINDING_BEGIN( Panel, GetScheme, "class", "Gets the scheme of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushischeme( L, scheme()->GetIScheme( panel->GetScheme() ) );
    return 1;
}
LUA_BINDING_END( "Scheme", "The scheme of the panel" )

LUA_BINDING_BEGIN( Panel, GetSize, "class", "Gets the size of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int wide, tall;
    panel->GetSize( wide, tall );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 2;
}
LUA_BINDING_END( "integer", "The width of the panel", "integer", "The height of the panel" )

LUA_BINDING_BEGIN( Panel, GetTabPosition, "class", "Gets the tab position of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetTabPosition() );
    return 1;
}
LUA_BINDING_END( "integer", "The tab position of the panel" )

LUA_BINDING_BEGIN( Panel, GetTall, "class", "Gets the height of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetTall() );
    return 1;
}
LUA_BINDING_END( "integer", "The height of the panel" )

// Experiment; Disabled since we don't want to bother Lua users with VPanel pointers
// LUA_BINDING_BEGIN( Panel, GetVPanel, "class", "Gets the VPanel of the panel" )
//{
//    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
//    Panel::PushVPanelLuaInstance( L, panel->GetVPanel() );
//    return 1;
//}
// LUA_BINDING_END( "VPanel", "The VPanel of the panel" )
//
// LUA_BINDING_BEGIN( Panel, GetVPanelAsInteger, "class", "Gets the VPanel of the panel as an integer" )
//{
//    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
//    lua_pushinteger( L, panel->GetVPanel() );
//    return 1;
//}
// LUA_BINDING_END( "integer", "The VPanel of the panel" )
//
// LUA_BINDING_BEGIN( Panel, GetVParent, "class", "Gets the VParent of the panel" )
//{
//    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
//    Panel::PushVPanelLuaInstance( L, panel->GetVParent() );
//    return 1;
//}
// LUA_BINDING_END( "VPanel", "The VParent of the panel" )

LUA_BINDING_BEGIN( Panel, GetWide, "class", "Gets the width of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetWide() );
    return 1;
}
LUA_BINDING_END( "integer", "The width of the panel" )

LUA_BINDING_BEGIN( Panel, GetZIndex, "class", "Gets the Z position of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushinteger( L, panel->GetZPos() );
    return 1;
}
LUA_BINDING_END( "integer", "The Z position of the panel" )

LUA_BINDING_BEGIN( Panel, HasChildren, "class", "Checks if the panel has children" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->GetChildCount() > 0 );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel has children, false otherwise" )

LUA_BINDING_BEGIN( Panel, HasFocus, "class", "Checks if the panel has focus" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->HasFocus() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel has focus, false otherwise" )

static bool ChildHasFocus( Panel *pPanel )
{
    if ( pPanel->HasFocus() )
        return true;

    for ( int i = 0; i < pPanel->GetChildCount(); i++ )
    {
        Panel *pChild = pPanel->GetChild( i );
        if ( ChildHasFocus( pChild ) )
            return true;
    }

    return false;
}

LUA_BINDING_BEGIN( Panel, HasHierarchicalFocus, "class", "Checks if the panel has hierarchical focus" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, ChildHasFocus( panel ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel has hierarchical focus, false otherwise" )

LUA_BINDING_BEGIN( Panel, HasParent, "class", "Checks if the panel is a descendant of another panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_Panel *potentialParent = LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "potentialParent" );
    lua_pushboolean( L, panel->HasParent( potentialParent->GetVParent() ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is a descendant of the parent, false otherwise" )

LUA_BINDING_BEGIN( Panel, HasUserConfigSettings, "class", "Checks if the panel has user config settings" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->HasUserConfigSettings() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel has user config settings, false otherwise" )

LUA_BINDING_BEGIN( Panel, InitPropertyConverters, "class|static", "Initializes the property converters" )
{
    lua_Panel::InitPropertyConverters();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, InvalidateLayout, "class", "Invalidates the layout of the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool shouldLayoutNow = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "shouldLayoutNow" );
    bool shouldReloadScheme = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "shouldReloadScheme" );

    panel->InvalidateLayout( shouldLayoutNow, shouldReloadScheme );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, IsAutoDeleteSet, "class", "Checks if the panel is set to auto delete" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsAutoDeleteSet() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is set to auto delete, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsBeingDragged, "class", "Checks if the panel is being dragged" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsBeingDragged() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is being dragged, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsBlockingDragChaining, "class", "Checks if the panel is blocking drag chaining" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsBlockingDragChaining() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is blocking drag chaining, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsBottomAligned, "class", "Checks if the panel is bottom aligned" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsBottomAligned() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is bottom aligned, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsBuildGroupEnabled, "class", "Checks if the panel is build group enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsBuildGroupEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is build group enabled, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsBuildModeActive, "class", "Checks if the panel is in build mode" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsBuildModeActive() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is in build mode, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsBuildModeDeletable, "class", "Checks if the panel is build mode deletable" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsBuildModeDeletable() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is build mode deletable, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsBuildModeEditable, "class", "Checks if the panel is build mode editable" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsBuildModeEditable() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is build mode editable, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsChildOfModalSubTree, "class", "Checks if the panel is a child of the modal sub tree" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsChildOfModalSubTree() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is a child of the modal sub tree, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsChildOfSurfaceModalPanel, "class", "Checks if the panel is a child of the surface modal panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsChildOfSurfaceModalPanel() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is a child of the surface modal panel, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsCursorNone, "class", "Checks if the panel has no cursor" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsCursorNone() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel has no cursor, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsCursorOver, "class", "Checks if the cursor is over the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsCursorOver() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the cursor is over the panel, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsDragEnabled, "class", "Checks if the panel is drag enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsDragEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is drag enabled, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsDropEnabled, "class", "Checks if the panel is drop enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsDropEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is drop enabled, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsEnabled, "class", "Checks if the panel is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is enabled, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsKeyBindingChainToParentAllowed, "class", "Checks if the key binding chain to parent is allowed" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsKeyBindingChainToParentAllowed() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the key binding chain to parent is allowed, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsKeyBoardInputEnabled, "class", "Checks if the keyboard input is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsKeyBoardInputEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the keyboard input is enabled, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsKeyOverridden, "class", "Checks if the key is overridden" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    KeyCode keyCode = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 2, "keyCode" );
    int modifiers = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "modifiers" );

    lua_pushboolean( L, panel->IsKeyOverridden( keyCode, modifiers ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the key is overridden, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsKeyRebound, "class", "Checks if the key is rebound" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    KeyCode keyCode = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 2, "keyCode" );
    int modifiers = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "modifiers" );

    lua_pushboolean( L, panel->IsKeyRebound( keyCode, modifiers ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the key is rebound, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsLayoutInvalid, "class", "Checks if the layout is invalid" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsLayoutInvalid() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the layout is invalid, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsMarkedForDeletion, "class", "Checks if the panel is marked for deletion" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsMarkedForDeletion() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is marked for deletion, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsMouseInputDisabledForThisPanel, "class", "Checks if the mouse input is disabled for this panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsMouseInputDisabledForThisPanel() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the mouse input is disabled for this panel, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsMouseInputEnabled, "class", "Checks if the mouse input is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsMouseInputEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the mouse input is enabled, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsOpaque, "class", "Checks if the panel is opaque" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsOpaque() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is opaque, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsPopup, "class", "Checks if the panel is a popup" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsPopup() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is a popup, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsProportional, "class", "Checks if the panel is proportional" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsProportional() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is proportional, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsRightAligned, "class", "Checks if the panel is right aligned" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsRightAligned() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is right aligned, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsStartDragWhenMouseExitsPanel, "class", "Checks if the panel starts dragging when the mouse exits the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsStartDragWhenMouseExitsPanel() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel starts dragging when the mouse exits the panel, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsTriplePressAllowed, "class", "Checks if the panel allows triple press" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsTriplePressAllowed() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel allows triple press, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsValidKeyBindingsContext, "class", "Checks if the panel is a valid key bindings context" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsValidKeyBindingsContext() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is a valid key bindings context, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsValid, "class", "Checks if the panel is valid" )
{
    return PanelIsValid( L );
}
LUA_BINDING_END( "boolean", "True if the panel is valid, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsVisible, "class", "Checks if the panel is visible" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsVisible() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is visible, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsWithin, "class", "Checks if the panel is within the specified position" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );

    lua_pushboolean( L, panel->IsWithin( x, y ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is within the specified position, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsWithinTraverse, "class", "Checks if the panel is within the specified position" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );
    bool traversePopups = LUA_BINDING_ARGUMENT( luaL_checkboolean, 4, "traversePopups" );

    lua_pushboolean( L, panel->IsWithinTraverse( x, y, traversePopups ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is within the specified position, false otherwise" )

LUA_BINDING_BEGIN( Panel, IsWorldClicker, "class", "Checks if the panel is a world clicker" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->IsWorldClicker() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is a world clicker, false otherwise" )

LUA_BINDING_BEGIN( Panel, LocalToScreen, "class", "Converts a local position to a screen position" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );

    panel->LocalToScreen( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}
LUA_BINDING_END( "integer", "The x position of the screen", "integer", "The y position of the screen" )

LUA_BINDING_BEGIN( Panel, MakePopup, "class", "Makes the panel a popup" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->MakePopup( false, false );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, MakeReadyForUse, "class", "Makes the panel ready for use" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->MakeReadyForUse();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, MarkForDeletion, "class", "Marks the panel for deletion" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->MarkForDeletion();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, MoveToBack, "class", "Moves the panel to the back" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->MoveToBack();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, MoveToFront, "class", "Moves the panel to the front" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->MoveToFront();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnCommand, "class", "Handles a command" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    const char *command = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "command" );
    panel->OnCommand( command );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnCursorEntered, "class", "Handles the cursor entering the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnCursorEntered();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnCursorExited, "class", "Handles the cursor exiting the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnCursorExited();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnCursorMoved, "class", "Handles the cursor moving" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );

    panel->OnCursorMoved( x, y );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnDelete, "class", "Handles the panel being deleted" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnDelete();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnDraggablePanelPaint, "class", "Handles the draggable panel being painted" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnDraggablePanelPaint();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnKeyCodePressed, "class", "Handles a key code being pressed" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    KeyCode keyCode = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 2, "keyCode" );
    panel->OnKeyCodePressed( keyCode );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, RequestInfo, "class", "OutputData - keyName is the name of the attribute requested. for Labels 'text' is an option that returns the default text image text returns true on success in finding the requested value. false on failure." )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->RequestInfo( LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 2, "outputData" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the info was requested" )

LUA_BINDING_BEGIN( Panel, OnKeyCodeTyped, "class", "Handles a key code being typed" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    KeyCode keyCode = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 2, "keyCode" );
    panel->OnKeyCodeTyped( keyCode );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnKeyFocusTicked, "class", "Handles the key focus being ticked" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnKeyFocusTicked();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnKillFocus, "class", "Handles the focus being killed" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnKillFocus();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnMouseCaptureLost, "class", "Handles the mouse capture being lost" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnMouseCaptureLost();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnMouseDoublePressed, "class", "Handles the mouse being double pressed" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    MouseCode code = LUA_BINDING_ARGUMENT_ENUM( MouseCode, 2, "code" );
    panel->OnMouseDoublePressed( code );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnMouseFocusTicked, "class", "Handles the mouse focus being ticked" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnMouseFocusTicked();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnMousePressed, "class", "Handles the mouse being pressed" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    MouseCode code = LUA_BINDING_ARGUMENT_ENUM( MouseCode, 2, "code" );
    panel->OnMousePressed( code );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnMouseReleased, "class", "Handles the mouse being released" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    MouseCode code = LUA_BINDING_ARGUMENT_ENUM( MouseCode, 2, "code" );
    panel->OnMouseReleased( code );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnMouseTriplePressed, "class", "Handles the mouse being triple pressed" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    MouseCode code = LUA_BINDING_ARGUMENT_ENUM( MouseCode, 2, "code" );
    panel->OnMouseTriplePressed( code );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnMouseWheeled, "class", "Handles the mouse being wheeled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int delta = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "delta" );
    panel->OnMouseWheeled( delta );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnMove, "class", "Handles the panel being moved" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnMove();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnSetFocus, "class", "Handles the focus being set" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnSetFocus();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnSizeChanged, "class", "Handles the size being changed" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "width" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "height" );

    panel->OnSizeChanged( width, height );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnThink, "class", "Handles the panel being thought" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnThink();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, OnTick, "class", "Handles the panel being ticked" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->OnTick();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, PaintManual, "class", "Paints the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool shouldUnclamp = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "shouldUnclamp" );

    if ( shouldUnclamp )
    {
        luaL_argerror( L, 2, "Unclamping is not supported" );
        return 0;
    }

    panel->PaintManual();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetPaintedManually, "class", "Sets whether the panel is painted manually" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool isPaintedManually = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "isPaintedManually" );
    panel->SetPaintEnabled( !isPaintedManually );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, PaintBackground, "class", "Paints the panel background" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->PaintBackground();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, PaintBorder, "class", "Paints the panel border" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->PaintBorder();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, PaintBuildOverlay, "class", "Paints the panel build overlay" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->PaintBuildOverlay();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, ParentLocalToScreen, "class", "Converts a parent local position to a screen position" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x, y;
    panel->ParentLocalToScreen( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}
LUA_BINDING_END( "integer", "The x position of the screen", "integer", "The y position of the screen" )

LUA_BINDING_BEGIN( Panel, ParentToHud, "class", "Sets the parent to the HUD" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->SetParent( VGui_GetClientLuaRootPanelHUD() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, PerformLayout, "class", "Performs the panel layout" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->PerformLayout();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, Prepare, "class", "Prepares the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->Prepare();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, PostChildPaint, "class", "Handles the post child paint" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->PostChildPaint();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, ReloadKeyBindings, "class", "Reloads the key bindings" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->ReloadKeyBindings();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, RemoveActionSignalTarget, "class", "Removes an action signal target" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_Panel *target = LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "target" );
    panel->RemoveActionSignalTarget( target );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, RemoveAllKeyBindings, "class", "Removes all key bindings" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->RemoveAllKeyBindings();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, Repaint, "class", "Repaints the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->Repaint();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, RequestFocus, "class", "Requests the focus" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int direction = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 2, 0, "direction" );
    panel->RequestFocus( direction );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, RevertKeyBindingsToDefault, "class", "Reverts the key bindings to default" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    panel->RevertKeyBindingsToDefault();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, ScreenToLocal, "class", "Converts a screen position to a local position" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x, y;
    panel->ScreenToLocal( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}
LUA_BINDING_END( "integer", "The x position of the local", "integer", "The y position of the local" )

LUA_BINDING_BEGIN( Panel, SetAllowKeyBindingChainToParent, "class", "Sets whether the key binding chain is allowed to go to the parent" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool allow = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "allow" );
    panel->SetAllowKeyBindingChainToParent( allow );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetAlpha, "class", "Sets the panel alpha" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    float alpha = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "alpha" );
    panel->SetAlpha( alpha );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetAutoDelete, "class", "Sets whether the panel is auto deleted" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool autoDelete = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "autoDelete" );
    panel->SetAutoDelete( autoDelete );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetAutoResize, "class", "Sets the auto resize" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    Panel::PinCorner_e pinCorner = LUA_BINDING_ARGUMENT_ENUM( Panel::PinCorner_e, 2, "pinCorner" );
    Panel::AutoResize_e autoResize = LUA_BINDING_ARGUMENT_ENUM( Panel::AutoResize_e, 3, "autoResize" );
    int pinOffsetX = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "pinOffsetX" );
    int pinOffsetY = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "pinOffsetY" );
    int unpinnedCornerOffsetX = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "unpinnedCornerOffsetX" );
    int unpinnedCornerOffsetY = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "unpinnedCornerOffsetY" );

    panel->SetAutoResize( pinCorner, autoResize, pinOffsetX, pinOffsetY, unpinnedCornerOffsetX, unpinnedCornerOffsetY );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetBackgroundColor, "class", "Sets the background color" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_Color &color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 2, "color" );
    panel->SetBgColor( color );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetBlockDragChaining, "class", "Sets whether the drag chaining is blocked" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool block = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "block" );
    panel->SetBlockDragChaining( block );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetBounds, "class", "Sets the bounds" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "width" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "height" );

    panel->SetBounds( x, y, width, height );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetBuildModeDeletable, "class", "Sets whether the build mode is deletable" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool deletable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "deletable" );
    panel->SetBuildModeDeletable( deletable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetBuildModeEditable, "class", "Sets whether the build mode is editable" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool editable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "editable" );
    panel->SetBuildModeEditable( editable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetCursor, "class", "Sets the cursor" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    HCursor cursor = LUA_BINDING_ARGUMENT_ENUM( CursorCode, 2, "cursorCode" );
    panel->SetCursor( cursor );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetDock, "class", "Sets the dock" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    Dock::Type dock = LUA_BINDING_ARGUMENT_ENUM( Dock::Type, 2, "dock" );
    panel->SetDock( dock );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetDockMargin, "class", "Sets the dock margin" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int left = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "left" );
    int top = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "top" );
    int right = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "right" );
    int bottom = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "bottom" );

    panel->SetDockMargin( Thickness( left, top, right, bottom ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetDockPadding, "class", "Sets the dock padding" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int left = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "left" );
    int top = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "top" );
    int right = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "right" );
    int bottom = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "bottom" );

    panel->SetDockPadding( Thickness( left, top, right, bottom ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetDragEnabled, "class", "Sets whether the drag is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    panel->SetDragEnabled( enabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetDragStartTolerance, "class", "Sets the drag start tolerance" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int tolerance = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "tolerance" );
    panel->SetDragSTartTolerance( tolerance );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetDropEnabled, "class", "Sets whether the drop is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    float radius = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0.0f, "radius" );

    panel->SetDropEnabled( enabled, radius );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetEnabled, "class", "Sets whether the panel is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    panel->SetEnabled( enabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetForegroundColor, "class", "Sets the foreground color" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_Color &color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 2, "color" );
    panel->SetFgColor( color );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetKeyboardInputEnabled, "class", "Sets whether the keyboard input is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    panel->SetKeyBoardInputEnabled( enabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetMinimumSize, "class", "Sets the minimum size" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "width" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "height" );

    panel->SetMinimumSize( width, height );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetMouseInputEnabled, "class", "Sets whether the mouse input is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    panel->SetMouseInputEnabled( enabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetName, "class", "Sets the name" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" );
    panel->SetName( name );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetPaintBackgroundEnabled, "class", "Sets whether the background is painted" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    panel->SetPaintBackgroundEnabled( enabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetPaintBackgroundType, "class", "Sets the background type" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int type = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "type" );
    panel->SetPaintBackgroundType( type );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetPaintBorderEnabled, "class", "Sets whether the border is painted" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    panel->SetPaintBorderEnabled( enabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetPaintClippingEnabled, "class", "Sets whether the clipping is painted" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    panel->SetPaintClippingEnabled( enabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetPaintEnabled, "class", "Sets whether the panel is painted" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    panel->SetPaintEnabled( enabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetParent, "class", "Sets the parent" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_Panel *parent = LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "parent" );
    panel->SetParent( parent );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetPinCorner, "class", "Sets the pin corner" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    Panel::PinCorner_e corner = LUA_BINDING_ARGUMENT_ENUM( Panel::PinCorner_e, 2, "corner" );
    int offsetX = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "offsetX" );
    int offsetY = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "offsetY" );

    panel->SetPinCorner( corner, offsetX, offsetY );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetPosition, "class", "Sets the position" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );

    panel->SetPos( x, y );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetPostChildPaintEnabled, "class", "Sets whether the post child paint is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" );
    panel->SetPostChildPaintEnabled( enabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetProportional, "class", "Sets whether the panel is proportional" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool proportional = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "proportional" );
    panel->SetProportional( proportional );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetScheme, "class", "Sets the scheme" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    const char *scheme = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "scheme" );
    panel->SetScheme( scheme );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetSilentMode, "class", "Sets whether the silent mode is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool silent = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "silent" );
    panel->SetSilentMode( silent );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetSize, "class", "Sets the size" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "width" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "height" );

    panel->SetSize( width, height );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetSkipChildDuringPainting, "class", "Sets the child to skip during painting" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_Panel *skip = LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "skip" );
    panel->SetSkipChildDuringPainting( skip );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetStartDragWhenMouseExitsPanel, "class", "Sets whether the drag starts when the mouse exits the panel" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool start = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "start" );
    panel->SetStartDragWhenMouseExitsPanel( start );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetTabPosition, "class", "Sets the tab position" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int position = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "position" );
    panel->SetTabPosition( position );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetTall, "class", "Sets the tall" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int tall = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "tall" );
    panel->SetTall( tall );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetTriplePressAllowed, "class", "Sets whether the triple press is allowed" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool allowed = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "allowed" );
    panel->SetTriplePressAllowed( allowed );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetVisible, "class", "Sets whether the panel is visible" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool visible = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "visible" );
    panel->SetVisible( visible );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetWide, "class", "Sets the wide" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int wide = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "wide" );
    panel->SetWide( wide );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetWorldClicker, "class", "Sets whether the world clicker is enabled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool clicker = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "clicker" );
    panel->SetWorldClicker( clicker );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SetZIndex, "class", "Sets the z position" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    int zPos = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "zPos" );
    panel->SetZPos( zPos );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, SizeToChildren, "class", "Sizes to children" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    bool width = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "width" );
    bool height = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "height" );

    panel->SizeToChildren( width, height );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, ShouldHandleInputMessage, "class", "Checks if the input message should be handled" )
{
    lua_Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, panel->ShouldHandleInputMessage() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the input message should be handled" )

LUA_BINDING_BEGIN( Panel, StringToKeyCode, "class", "Converts the string to key code" )
{
    const char *str = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "str" );
    lua_pushinteger( L, lua_Panel::StringToKeyCode( str ) );
    return 1;
}
LUA_BINDING_END( "enumeration/BUTTON", "The key code" )

LUA_BINDING_BEGIN( Panel, __index, "class", "Metamethod called when a non-existent field is indexed" )
{
    Panel *plPanel = LUA_BINDING_ARGUMENT( lua_topanel, 1, "panel" );

    LUA_METATABLE_INDEX_CHECK_VALID( L, PanelIsValid );
    LUA_METATABLE_INDEX_CHECK( L, plPanel );

    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( Q_strcmp( field, "x" ) == 0 )
        lua_pushnumber( L, plPanel->GetXPos() );
    else if ( Q_strcmp( field, "y" ) == 0 )
        lua_pushnumber( L, plPanel->GetYPos() );
    else if ( Q_strcmp( field, "wide" ) == 0 )
        lua_pushnumber( L, plPanel->GetWide() );
    else if ( Q_strcmp( field, "tall" ) == 0 )
        lua_pushnumber( L, plPanel->GetTall() );
    else
    {
        LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plPanel );

        if ( lua_getmetatable( L, 1 ) )
        {
            LUA_METATABLE_INDEX_CHECK_TABLE( L );
        }

        luaL_getmetatable( L, "Panel" );
        LUA_METATABLE_INDEX_CHECK_TABLE( L );

        lua_pushnil( L );
    }

    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( Panel, __newindex, "class", "Metamethod called when a new field is added" )
{
    Panel *plPanel = LUA_BINDING_ARGUMENT( lua_topanel, 1, "panel" );

    if ( plPanel == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( Q_strcmp( field, "x" ) == 0 )
    {
        int x, y;
        plPanel->GetPos( x, y );
        plPanel->SetPos( luaL_checknumber( L, 3 ), y );
    }
    else if ( Q_strcmp( field, "y" ) == 0 )
    {
        int x, y;
        plPanel->GetPos( x, y );
        plPanel->SetPos( x, luaL_checknumber( L, 3 ) );
    }
    else if ( Q_strcmp( field, "wide" ) == 0 )
    {
        plPanel->SetWide( luaL_checknumber( L, 3 ) );
    }
    else if ( Q_strcmp( field, "tall" ) == 0 )
    {
        plPanel->SetTall( luaL_checknumber( L, 3 ) );
    }
    else
    {
        LUA_GET_REF_TABLE( L, plPanel );
        lua_pushvalue( L, 3 );
        lua_setfield( L, -2, field );
        lua_pop( L, 1 );
    }

    return 0;
}
LUA_BINDING_END()

int PanelCollectGarbage( lua_State *L )
{
    // Experiment; We've disabled this for now, or Lua would have to keep a reference to every panel
    // TODO: Perhaps we should modify Panels.Create to keep references for the user.
    // TODO: We should clean up the panels when their accompanying Lua state is destroyed.
    /*Panel *plPanel = dynamic_cast< Panel * >( lua_topanel( L, 1 ) );

    if ( plPanel )
    {
        --plPanel->m_nRefCount;
        if ( plPanel->m_nRefCount <= 0 )
        {
            delete plPanel;
        }
    }*/

    return 0;
}

LUA_BINDING_BEGIN( Panel, __gc, "class", "Metamethod called when the panel is garbage collected" )
{
    return PanelCollectGarbage( L );
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panel, __eq, "class", "Metamethod called when two Panel objects are compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_topanel, 1, "panel" ) == LUA_BINDING_ARGUMENT( lua_topanel, 2, "other" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the Panel objects are equal" )

LUA_BINDING_BEGIN( Panel, __tostring, "class", "Metamethod called when the Panel object is converted to a string" )
{
    Panel *pPanel = LUA_BINDING_ARGUMENT( lua_topanel, 1, "panel" );

    if ( pPanel == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pPanel->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "Panel: \"%s\"", pName );
    }
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the Panel object" )

LUA_REGISTRATION_INIT( Panels )

LUA_BINDING_BEGIN( Panels, Panel, "library", "Creates a new Panel object" )
{
    Panel *parent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, VGui_GetClientLuaRootPanel(), "parent" );
    const char *name = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "Panel", "name" );

    lua_Panel *pPanel = new lua_Panel(
        parent,
        name,
        L );
    Panel::PushLuaInstanceSafe( L, pPanel );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panels, GetGameUiPanel, "library", "Gets the game UI panel" )
{
    Panel::PushLuaInstanceSafe( L, VGui_GetGameUIPanel() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panels, GetClientLuaRootPanel, "library", "Gets the client Lua root panel" )
{
    Panel::PushLuaInstanceSafe( L, VGui_GetClientLuaRootPanel() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Panels, GetClientLuaRootHudPanel, "library", "Gets the client Lua root panel for the HUD" )
{
    Panel::PushLuaInstanceSafe( L, VGui_GetClientLuaRootPanelHUD() );
    return 1;
}
LUA_BINDING_END()

/*
** Open Panel object
*/
LUALIB_API int luaopen_vgui_Panel( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "Panel" );

    LUA_REGISTRATION_COMMIT( Panel );

    lua_pushstring( L, LUA_PANELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */
    lua_pop( L, 1 );                 // pop metatable

    LUA_REGISTRATION_COMMIT_LIBRARY( Panels );

    // Andrew; Don't be mislead, INVALID_PANEL is not NULL internally, but we
    // need a name other than NULL, because NULL has already been assigned as an
    // entity.
    Panel::PushVPanelLuaInstance( L, NULL );
    lua_setglobal( L, "INVALID_PANEL" ); /* set global INVALID_PANEL */

    LUA_SET_ENUM_LIB_BEGIN( L, "PIN_CORNER" );
    lua_pushenum( L, Panel::PinCorner_e::PIN_TOPLEFT, "TOP_LEFT" );
    lua_pushenum( L, Panel::PinCorner_e::PIN_TOPRIGHT, "TOP_RIGHT" );
    lua_pushenum( L, Panel::PinCorner_e::PIN_BOTTOMLEFT, "BOTTOM_LEFT" );
    lua_pushenum( L, Panel::PinCorner_e::PIN_BOTTOMRIGHT, "BOTTOM_RIGHT" );
    lua_pushenum( L, Panel::PinCorner_e::PIN_CENTER_TOP, "CENTER_TOP" );
    lua_pushenum( L, Panel::PinCorner_e::PIN_CENTER_RIGHT, "CENTER_RIGHT" );
    lua_pushenum( L, Panel::PinCorner_e::PIN_CENTER_BOTTOM, "CENTER_BOTTOM" );
    lua_pushenum( L, Panel::PinCorner_e::PIN_CENTER_LEFT, "CENTER_LEFT" );
    LUA_SET_ENUM_LIB_END( L );

    LUA_SET_ENUM_LIB_BEGIN( L, "AUTO_RESIZE" );
    lua_pushenum( L, Panel::AutoResize_e::AUTORESIZE_NO, "NO" );
    lua_pushenum( L, Panel::AutoResize_e::AUTORESIZE_RIGHT, "RIGHT" );
    lua_pushenum( L, Panel::AutoResize_e::AUTORESIZE_DOWN, "DOWN" );
    lua_pushenum( L, Panel::AutoResize_e::AUTORESIZE_DOWNANDRIGHT, "DOWN_AND_RIGHT" );
    LUA_SET_ENUM_LIB_END( L );

    LUA_SET_ENUM_LIB_BEGIN( L, "DOCK_TYPE" );
    lua_pushenum( L, Dock::None, "NONE" );
    lua_pushenum( L, Dock::Fill, "FILL" );
    lua_pushenum( L, Dock::Left, "LEFT" );
    lua_pushenum( L, Dock::Right, "RIGHT" );
    lua_pushenum( L, Dock::Top, "TOP" );
    lua_pushenum( L, Dock::Bottom, "BOTTOM" );
    LUA_SET_ENUM_LIB_END( L );

    LUA_SET_ENUM_LIB_BEGIN( L, "PANEL_ALIGNMENT" );
    lua_pushenum( L, Alignment::None, "NONE" );
    lua_pushenum( L, Alignment::Left, "LEFT" );
    lua_pushenum( L, Alignment::Right, "RIGHT" );
    lua_pushenum( L, Alignment::Top, "TOP" );
    lua_pushenum( L, Alignment::Bottom, "BOTTOM" );
    lua_pushenum( L, Alignment::CenterVertical, "CENTER_VERTICAL" );
    lua_pushenum( L, Alignment::CenterHorizontal, "CENTER_HORIZONTAL" );
    lua_pushenum( L, Alignment::Center, "CENTER" );
    LUA_SET_ENUM_LIB_END( L );

    LUA_SET_ENUM_LIB_BEGIN( L, "CURSOR" );
    lua_pushenum( L, CursorCode::dc_none, "NONE" );
    lua_pushenum( L, CursorCode::dc_user, "USER" );
    lua_pushenum( L, CursorCode::dc_arrow, "ARROW" );
    lua_pushenum( L, CursorCode::dc_ibeam, "IBEAM" );
    lua_pushenum( L, CursorCode::dc_hourglass, "HOURGLASS" );
    lua_pushenum( L, CursorCode::dc_waitarrow, "WAIT_ARROW" );
    lua_pushenum( L, CursorCode::dc_crosshair, "CROSSHAIR" );
    lua_pushenum( L, CursorCode::dc_up, "UP" );
    lua_pushenum( L, CursorCode::dc_sizenwse, "SIZE_NORTHWEST_SOUTHEAST" );
    lua_pushenum( L, CursorCode::dc_sizenesw, "SIZE_NORTHEAST_SOUTHWEST" );
    lua_pushenum( L, CursorCode::dc_sizewe, "SIZE_WEST_EAST" );
    lua_pushenum( L, CursorCode::dc_sizens, "SIZE_NORTH_SOUTH" );
    lua_pushenum( L, CursorCode::dc_sizeall, "SIZE_ALL" );
    lua_pushenum( L, CursorCode::dc_no, "NO" );
    lua_pushenum( L, CursorCode::dc_hand, "HAND" );
    lua_pushenum( L, CursorCode::dc_blank, "BLANK" );
    lua_pushenum( L, CursorCode::dc_last, "LAST" );
    lua_pushenum( L, CursorCode::dc_alwaysvisible_push, "ALWAYS_VISIBLE_PUSH" );
    lua_pushenum( L, CursorCode::dc_alwaysvisible_pop, "ALWAYS_VISIBLE_POP" );
    LUA_SET_ENUM_LIB_END( L );

    return 1;
}
