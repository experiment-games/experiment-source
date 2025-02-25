#include <cbase.h>
#include <vgui/IVGui.h>
#include <vgui_int.h>
#include <ienginevgui.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <LKeyValues.h>

#include <scripted_controls/lPanel.h>
#include <scripted_controls/lFrame.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LFrame::LFrame( Panel *parent, const char *panelName, bool showTaskbarIcon, lua_State *L /* = nullptr */ )
    : Frame( parent, panelName, showTaskbarIcon, L )
{
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
LFrame::~LFrame()
{
}

/*
** access functions (stack -> C)
*/

LUA_API lua_Frame *lua_toframe( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_Frame * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_Frame *luaL_checkframe( lua_State *L, int narg )
{
    lua_Frame *d = lua_toframe( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "Frame expected, got INVALID_PANEL" );
    return d;
}

LUA_REGISTRATION_INIT( Frame )

LUA_BINDING_BEGIN( Frame, Activate, "class", "Activates the frame" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    frame->Activate();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, ActivateMinimized, "class", "Activates the frame minimized" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    frame->ActivateMinimized();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, CanChainKeysToParent, "class", "Returns whether keys can be chained to the parent" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushboolean( L, frame->CanChainKeysToParent() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether keys can be chained to the parent" )

// LUA_BINDING_BEGIN( Frame, ChainToAnimationMap, "class|static", "Chains the frame to an animation map" )
//{
//     lua_Frame::ChainToAnimationMap();
//     return 0;
// }
// LUA_BINDING_END()

// LUA_BINDING_BEGIN( Frame, ChainToMap, "class|static", "Chains the frame to a keyboard map" )
//{
//     lua_Frame::ChainToMap();
//     return 0;
// }

LUA_BINDING_BEGIN( Frame, Close, "class", "Closes the frame" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    frame->Close();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, CloseModal, "class", "Closes the frame modally" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    frame->CloseModal();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, DisableFadeEffect, "class", "Disables the fade effect" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    frame->DisableFadeEffect();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, FlashWindow, "class", "Flashes the frame" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    frame->FlashWindow();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, FlashWindowStop, "class", "Stops flashing the frame" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    frame->FlashWindowStop();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, GetBottomRightSize, "class", "Gets the bottom right size" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushinteger( L, frame->GetBottomRightSize() );
    return 1;
}
LUA_BINDING_END( "integer", "The bottom right size" )

LUA_BINDING_BEGIN( Frame, GetCaptionHeight, "class", "Gets the caption height" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushinteger( L, frame->GetCaptionHeight() );
    return 1;
}
LUA_BINDING_END( "integer", "The caption height" )

LUA_BINDING_BEGIN( Frame, GetClipToParent, "class", "Returns whether the frame clips to the parent" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushboolean( L, frame->GetClipToParent() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the frame clips to the parent" )

LUA_BINDING_BEGIN( Frame, GetCornerSize, "class", "Gets the corner size" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushinteger( L, frame->GetCornerSize() );
    return 1;
}
LUA_BINDING_END( "integer", "The corner size" )

LUA_BINDING_BEGIN( Frame, GetDraggerSize, "class", "Gets the dragger size" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushinteger( L, frame->GetDraggerSize() );
    return 1;
}
LUA_BINDING_END( "integer", "The dragger size" )

LUA_BINDING_BEGIN( Frame, IsActive, "class", "Returns whether the frame is currently focussed" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushboolean( L, frame->FrameHasFocus() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the frame is focussed" )

LUA_BINDING_BEGIN( Frame, IsMinimized, "class", "Returns whether the frame is minimized" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushboolean( L, frame->IsMinimized() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the frame is minimized" )

LUA_BINDING_BEGIN( Frame, IsMoveable, "class", "Returns whether the frame is moveable" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushboolean( L, frame->IsMoveable() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the frame is moveable" )

LUA_BINDING_BEGIN( Frame, IsSizeable, "class", "Returns whether the frame is sizeable" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushboolean( L, frame->IsSizeable() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the frame is sizeable" )

LUA_BINDING_BEGIN( Frame, IsSmallCaption, "class", "Returns whether the frame has a small caption" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    lua_pushboolean( L, frame->IsSmallCaption() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the frame has a small caption" )

// LUA_BINDING_BEGIN( Frame, KB_AddBoundKey, "class|static", "Adds a bound key" )
//{
//     const char *keyName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "keyName" );
//     int keyCode = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "keyCode" );
//     int modifiers = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "modifiers" );
//     lua_Frame::KB_AddBoundKey( keyName, keyCode, modifiers );
//     return 0;
// }
// LUA_BINDING_END()

// LUA_BINDING_BEGIN( Frame, KB_ChainToMap, "class|static", "Chains the frame to a keyboard map" )
//{
//     lua_Frame::KB_ChainToMap();
//     return 0;
// }

LUA_BINDING_BEGIN( Frame, MoveToCenterOfScreen, "class", "Moves the frame to the center of the screen" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    frame->MoveToCenterOfScreen();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, PlaceUnderCursor, "class", "Places the frame under the cursor" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    frame->PlaceUnderCursor();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetChainKeysToParent, "class", "Sets whether keys can be chained to the parent" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetChainKeysToParent( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetClipToParent, "class", "Sets whether the frame clips to the parent" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetClipToParent( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetCloseButtonVisible, "class", "Sets whether the close button is visible" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetCloseButtonVisible( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetDeleteSelfOnClose, "class", "Sets whether the frame deletes itself on close" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetDeleteSelfOnClose( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetImages, "class", "Sets the images" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    const char *imageName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "imageName" );
    const char *imageNameArmed = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, NULL, "imageNameArmed" );
    frame->SetImages( imageName, imageNameArmed );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetMaximizeButtonVisible, "class", "Sets whether the maximize button is visible" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetMaximizeButtonVisible( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetMenuButtonResponsive, "class", "Sets whether the menu button is responsive" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetMenuButtonResponsive( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetMenuButtonVisible, "class", "Sets whether the menu button is visible" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetMenuButtonVisible( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetMinimizeButtonVisible, "class", "Sets whether the minimize button is visible" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetMinimizeButtonVisible( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetMinimizeToSysTrayButtonVisible, "class", "Sets whether the minimize to system tray button is visible" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetMinimizeToSysTrayButtonVisible( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetMoveable, "class", "Sets whether the frame is moveable" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetMoveable( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetSizeable, "class", "Sets whether the frame is sizeable" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetSizeable( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetSmallCaption, "class", "Sets whether the frame has a small caption" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetSmallCaption( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetTitle, "class", "Sets the title" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    const char *title = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "title" );
    bool showTaskbarIcon = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, true, "showTaskbarIcon" );
    frame->SetTitle( title, showTaskbarIcon );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, SetTitleBarVisible, "class", "Sets whether the title bar is visible" )
{
    lua_Frame *frame = LUA_BINDING_ARGUMENT( luaL_checkframe, 1, "frame" );
    bool state = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "state" );
    frame->SetTitleBarVisible( state );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, __index, "class", "Metamethod called when a non-existent field is indexed" )
{
    Frame *pFrame = LUA_BINDING_ARGUMENT( lua_toframe, 1, "frame" );
    LUA_METATABLE_INDEX_CHECK_VALID( L, PanelIsValid );
    LUA_METATABLE_INDEX_CHECK( L, pFrame );

    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LFrame *plFrame = dynamic_cast< LFrame * >( pFrame );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plFrame );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "Frame" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "EditablePanel" );

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( Frame, __newindex, "class", "Metamethod called when a new field is added" )
{
    Frame *pFrame = LUA_BINDING_ARGUMENT( lua_toframe, 1, "frame" );

    if ( pFrame == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );

        return lua_error( L );
    }

    LFrame *plFrame = dynamic_cast< LFrame * >( pFrame );

    LUA_GET_REF_TABLE( L, plFrame );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" ) );
    lua_pop( L, 1 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Frame, __eq, "class", "Metamethod called when two Frame objects are compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_toframe, 1, "frame" ) == LUA_BINDING_ARGUMENT( lua_toframe, 2, "other" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the Frame objects are equal" )

LUA_BINDING_BEGIN( Frame, __tostring, "class", "Metamethod called when the Frame object is converted to a string" )
{
    Frame *pFrame = LUA_BINDING_ARGUMENT( lua_toframe, 1, "frame" );

    if ( pFrame == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pFrame->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "Frame: \"%s\"", pName );
    }
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the Frame object" )

LUA_REGISTRATION_INIT( Panels )

LUA_BINDING_BEGIN( Panels, Frame, "library", "Creates a new Frame panel" )
{
    Panel *parent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, VGui_GetClientLuaRootPanel(), "parent" );
    const char *name = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "Frame", "name" );

    LFrame *pPanel = new LFrame( parent, name, L );
    LFrame::PushLuaInstanceSafe( L, pPanel );
    return 1;
}
LUA_BINDING_END( "Frame", "The new Frame Panel" )

/*
** Open Frame object
*/
LUALIB_API int luaopen_vgui_Frame( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "Frame" );

    LUA_REGISTRATION_COMMIT( Frame );

    lua_pushstring( L, LUA_PANELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Panels );

    lua_pop( L, 2 );  // remove metatable and Frame_funcs
    return 0;
}
