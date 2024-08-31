#include <cbase.h>

#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <lColor.h>

#include <scripted_controls/lButton.h>
#include "scripted_controls/lPanel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LButton::LButton( Panel *parent, const char *panelName, const char *text, Panel *pActionSignalTarget, const char *pCmd, lua_State *L /* = nullptr */ )
    : Button( parent, panelName, text, pActionSignalTarget, pCmd, L )
{
}

void LButton::DoClick()
{
    Button::DoClick();

#ifdef LUA_SDK
    LUA_CALL_PANEL_METHOD_BEGIN( "OnClick" );
    LUA_CALL_PANEL_METHOD_END( 0, 0 );
#endif
}

/*
** access functions (stack -> C)
*/

LUA_API lua_Button *lua_tobutton( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_Button * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_Button *luaL_checkbutton( lua_State *L, int narg )
{
    lua_Button *d = lua_tobutton( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "Button expected, got INVALID_PANEL" );
    return d;
}

LUA_REGISTRATION_INIT( Button )

LUA_BINDING_BEGIN( Button, GetPanelBaseClassName, "class|static", "Gets the base class name of the panel" )
{
    lua_pushstring( L, lua_Button::GetPanelBaseClassName() );
    return 1;
}
LUA_BINDING_END( "string", "The base class name of the panel" )

LUA_BINDING_BEGIN( Button, GetPanelClassName, "class|static", "Gets the class name of the panel" )
{
    lua_pushstring( L, lua_Button::GetPanelClassName() );
    return 1;
}
LUA_BINDING_END( "string", "The class name of the panel" )

LUA_BINDING_BEGIN( Button, CanBeDefaultButton, "class", "Returns whether the button can be the default button" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushboolean( L, button->CanBeDefaultButton() );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns whether the button can be the default button" )

LUA_BINDING_BEGIN( Button, DoClick, "class", "Simulates a click on the button" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->DoClick();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, DrawFocusBox, "class", "Draws the focus box" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->DrawFocusBox( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldDraw" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, FireActionSignal, "class", "Fires the action signal" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->FireActionSignal();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, ForceDepressed, "class", "Forces the button to be depressed" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->ForceDepressed( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "depressed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, GetButtonBackgroundColor, "class", "Returns the button's background color" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushcolor( L, button->GetButtonBgColor() );
    return 1;
}
LUA_BINDING_END( "Color", "Returns the button's background color" )

LUA_BINDING_BEGIN( Button, GetButtonForegroundColor, "class", "Returns the button's foreground color" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushcolor( L, button->GetButtonFgColor() );
    return 1;
}
LUA_BINDING_END( "Color", "Returns the button's foreground color" )

LUA_BINDING_BEGIN( Button, IsArmed, "class", "Returns whether the button is armed" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushboolean( L, button->IsArmed() );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns whether the button is armed" )

LUA_BINDING_BEGIN( Button, IsBlinking, "class", "Returns whether the button is blinking" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushboolean( L, button->IsBlinking() );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns whether the button is blinking" )

LUA_BINDING_BEGIN( Button, IsDepressed, "class", "Returns whether the button is depressed" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushboolean( L, button->IsDepressed() );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns whether the button is depressed" )

LUA_BINDING_BEGIN( Button, IsDrawingFocusBox, "class", "Returns whether the button is drawing the focus box" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushboolean( L, button->IsDrawingFocusBox() );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns whether the button is drawing the focus box" )

LUA_BINDING_BEGIN( Button, IsMouseClickEnabled, "class", "Returns whether mouse clicks are enabled" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushboolean( L,
                     button->IsMouseClickEnabled(
                         LUA_BINDING_ARGUMENT_ENUM( MouseCode, 2, "mouseCode" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns whether mouse clicks are enabled" )

LUA_BINDING_BEGIN( Button, IsSelected, "class", "Returns whether the button is selected" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushboolean( L, button->IsSelected() );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns whether the button is selected" )

LUA_BINDING_BEGIN( Button, IsUseCaptureMouseEnabled, "class", "Returns whether the button uses capture mouse" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushboolean( L, button->IsUseCaptureMouseEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns whether the button uses capture mouse" )

LUA_BINDING_BEGIN( Button, OnHotkey, "class", "Called when a hotkey is pressed" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->OnHotkey();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, RecalculateDepressedState, "class", "Recalculates the depressed state" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->RecalculateDepressedState();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetArmed, "class", "Sets whether the button is armed" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetArmed( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "isArmed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetArmedColor, "class", "Sets the armed color" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetArmedColor( LUA_BINDING_ARGUMENT( luaL_checkcolor, 2, "armedForegroundColor" ),
                           LUA_BINDING_ARGUMENT( luaL_checkcolor, 3, "armedBackgroundColor" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetArmedSound, "class", "Sets the armed sound" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetArmedSound( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "sound" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetAsCurrentDefaultButton, "class", "Sets the button as the current default button" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetAsCurrentDefaultButton( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "state" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetAsDefaultButton, "class", "Sets the button as the default button" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetAsDefaultButton( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "state" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetBlink, "class", "Sets whether the button is blinking" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetBlink( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldBlink" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetBlinkColor, "class", "Sets the blink color" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetBlinkColor( LUA_BINDING_ARGUMENT( luaL_checkcolor, 2, "color" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetButtonActivationType, "class", "Sets the button activation type" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetButtonActivationType(
        LUA_BINDING_ARGUMENT_ENUM( Button::ActivationType_t, 2, "activationType" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetButtonBorderEnabled, "class", "Sets whether the button border is enabled" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetButtonBorderEnabled( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enabled" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetDefaultColor, "class", "Sets the default color" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetDefaultColor( LUA_BINDING_ARGUMENT( luaL_checkcolor, 2, "defaultForegroundColor" ),
                             LUA_BINDING_ARGUMENT( luaL_checkcolor, 3, "defaultBackgroundColor" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetDepressedColor, "class", "Sets the depressed color" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetDepressedColor( LUA_BINDING_ARGUMENT( luaL_checkcolor, 2, "depressedForegroundColor" ),
                               LUA_BINDING_ARGUMENT( luaL_checkcolor, 3, "depressedBackgroundColor" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetDepressedSound, "class", "Sets the depressed sound" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetDepressedSound( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "sound" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetMouseClickEnabled, "class", "Sets whether mouse clicks are enabled" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetMouseClickEnabled(
        LUA_BINDING_ARGUMENT_ENUM( MouseCode, 2, "mouseCode" ),
        LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "enabled" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetReleasedSound, "class", "Sets the released sound" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetReleasedSound( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "sound" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetSelected, "class", "Sets whether the button is selected" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetSelected( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "selected" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetShouldPaint, "class", "Sets whether the button should paint" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetShouldPaint( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldPaint" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, SetUseCaptureMouse, "class", "Sets whether the button uses capture mouse" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    button->SetUseCaptureMouse( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "useCaptureMouse" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, ShouldPaint, "class", "Returns whether the button should paint" )
{
    lua_Button *button = LUA_BINDING_ARGUMENT( luaL_checkbutton, 1, "button" );
    lua_pushboolean( L, button->ShouldPaint() );
    return 1;
}
LUA_BINDING_END( "boolean", "Returns whether the button should paint" )

LUA_BINDING_BEGIN( Button, __index, "class", "Metamethod that is called when a non-existant field is indexed" )
{
    lua_Button *pButton = LUA_BINDING_ARGUMENT( lua_tobutton, 1, "button" );
    LUA_METATABLE_INDEX_CHECK_VALID( L, PanelIsValid );
    LUA_METATABLE_INDEX_CHECK( L, pButton );

    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LButton *plButton = dynamic_cast< LButton * >( pButton );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plButton );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "Button" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "Label" );

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( Button, __newindex, "class", "Metamethod that is called when a new field is added to the panel" )
{
    lua_Button *pButton = LUA_BINDING_ARGUMENT( lua_tobutton, 1, "button" );

    if ( pButton == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LButton *plButton = dynamic_cast< LButton * >( pButton );

    LUA_GET_REF_TABLE( L, plButton );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" ) );
    lua_pop( L, 1 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, __eq, "class", "Metamethod that is called when two buttons are compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_tobutton, 1, "label" ) == LUA_BINDING_ARGUMENT( lua_tobutton, 2, "other" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the labels are equal" )

LUA_BINDING_BEGIN( Button, __tostring, "class", "Metamethod that is called when the button is to be converted to a string" )
{
    lua_Button *pButton = LUA_BINDING_ARGUMENT( lua_tobutton, 1, "button" );
    if ( pButton == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pButton->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "Button: \"%s\"", pName );
    }
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Button, __gc, "class", "Metamethod that is called when the panel is garbage collected" )
{
    return PanelCollectGarbage( L );
}
LUA_BINDING_END()

LUA_REGISTRATION_INIT( Panels )

LUA_BINDING_BEGIN( Panels, Button, "library", "Creates a new button" )
{
    Panel *parent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, VGui_GetClientLuaRootPanel(), "parent" );
    const char *name = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "Button", "name" );
    const char *text = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, "", "text" );
    Panel *pActionSignalTarget = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 4, 0, "actionSignalTarget" );
    const char *pCmd = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 5, 0, "cmd" );
    lua_Button *pPanel = new lua_Button( parent, name, text, pActionSignalTarget, pCmd, L );
    LButton::PushLuaInstanceSafe( L, pPanel );
    return 1;
}
LUA_BINDING_END( "Button", "The new button" )

/*
** Open Button object
*/
LUALIB_API int luaopen_vgui_Button( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "Button" );

    LUA_REGISTRATION_COMMIT( Button );

    lua_pushstring( L, LUA_PANELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Panels );

    // Pop the panel library off the stack
    lua_pop( L, 1 );
    lua_pop( L, 1 );  // Pop the Button metatable

    LUA_SET_ENUM_LIB_BEGIN( L, "ACTIVATION_TYPE" );
    lua_pushenum( L, Button::ACTIVATE_ONPRESSEDANDRELEASED, "ON_PRESSED_AND_RELEASED" );
    lua_pushenum( L, Button::ACTIVATE_ONPRESSED, "ON_PRESSED" );
    lua_pushenum( L, Button::ACTIVATE_ONRELEASED, "ON_RELEASED" );
    LUA_SET_ENUM_LIB_END( L );

    return 0;
}
