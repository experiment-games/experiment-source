#include <cbase.h>
#include "vgui/iinput.h"
#include "vgui_controls/Controls.h"
#include <luamanager.h>
#include "luasrclib.h"
#include <mathlib/lvector.h>

#include "scripted_controls/lPanel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

LUA_REGISTRATION_INIT( Inputs )

LUA_BINDING_BEGIN( Inputs, IsCandidateListStartingAtOne, "library", "Check if the candidate list starts at index one." )
{
    lua_pushboolean( L, input()->CandidateListStartsAtOne() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the candidate list starts at index one." )

LUA_BINDING_BEGIN( Inputs, GetAppModalSurface, "library", "Get the application modal surface panel." )
{
    Panel::PushVPanelLuaInstance( L, input()->GetAppModalSurface() );
    return 1;
}
LUA_BINDING_END( "PanelHandle", "The application modal surface panel." )

LUA_BINDING_BEGIN( Inputs, GetCandidateListCount, "library", "Get the count of candidates in the candidate list." )
{
    lua_pushinteger( L, input()->GetCandidateListCount() );
    return 1;
}
LUA_BINDING_END( "integer", "Count of candidates." )

LUA_BINDING_BEGIN( Inputs, GetCandidateListPageSize, "library", "Get the page size of the candidate list." )
{
    lua_pushinteger( L, input()->GetCandidateListPageSize() );
    return 1;
}
LUA_BINDING_END( "integer", "Page size of the candidate list." )

LUA_BINDING_BEGIN( Inputs, GetCandidateListPageStart, "library", "Get the starting index of the current page in the candidate list." )
{
    lua_pushinteger( L, input()->GetCandidateListPageStart() );
    return 1;
}
LUA_BINDING_END( "integer", "Starting index of the current page." )

LUA_BINDING_BEGIN( Inputs, GetCandidateListSelectedItem, "library", "Get the selected item's index in the candidate list." )
{
    lua_pushinteger( L, input()->GetCandidateListSelectedItem() );
    return 1;
}
LUA_BINDING_END( "integer", "Index of the selected item." )

LUA_BINDING_BEGIN( Inputs, GetCurrentImeHandle, "library", "Get the handle of the current Input Method Editor (IME)." )
{
    lua_pushinteger( L, input()->GetCurrentIMEHandle() );
    return 1;
}
LUA_BINDING_END( "integer", "IME handle." )

LUA_BINDING_BEGIN( Inputs, GetCursorPositionVcrMode, "library", "Get the cursor position using VCR mode." )
{
    int x, y;
    input()->GetCursorPos__USE_VCR_MODE( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}
LUA_BINDING_END( "integer, integer", "Cursor X and Y positions." )

LUA_BINDING_BEGIN( Inputs, GetCursorPosition, "library", "Get the current cursor position." )
{
    int x, y;
    input()->GetCursorPosition( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}
LUA_BINDING_END( "integer, integer", "Cursor X and Y positions." )

LUA_BINDING_BEGIN( Inputs, GetEnglishImeHandle, "library", "Get the handle of the English Input Method Editor (IME)." )
{
    lua_pushinteger( L, input()->GetEnglishIMEHandle() );
    return 1;
}
LUA_BINDING_END( "integer", "IME handle." )

LUA_BINDING_BEGIN( Inputs, GetFocus, "library", "Get the panel that currently has focus." )
{
    Panel::PushVPanelLuaInstance( L, input()->GetFocus() );
    return 1;
}
LUA_BINDING_END( "PanelHandle", "Focused panel." )

LUA_BINDING_BEGIN( Inputs, GetModalSubTree, "library", "Get the topmost modal sub-tree panel." )
{
    Panel::PushVPanelLuaInstance( L, input()->GetModalSubTree() );
    return 1;
}
LUA_BINDING_END( "PanelHandle", "Topmost modal sub-tree panel." )

LUA_BINDING_BEGIN( Inputs, GetMouseCapture, "library", "Get the panel that currently has mouse capture." )
{
    Panel::PushVPanelLuaInstance( L, input()->GetMouseCapture() );
    return 1;
}
LUA_BINDING_END( "PanelHandle", "Mouse capture panel." )

LUA_BINDING_BEGIN( Inputs, GetMouseOver, "library", "Get the panel that currently is under the mouse cursor." )
{
    Panel::PushVPanelLuaInstance( L, input()->GetMouseOver() );
    return 1;
}
LUA_BINDING_END( "PanelHandle", "Mouse over panel." )

LUA_BINDING_BEGIN( Inputs, GetShouldInvertCompositionString, "library", "Check if the composition string should be inverted." )
{
    lua_pushboolean( L, input()->GetShouldInvertCompositionString() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the composition string should be inverted." )

LUA_BINDING_BEGIN( Inputs, IsKeyDown, "library", "Check if a specific key is down." )
{
    KeyCode code = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 1, "keyCode" );
    lua_pushboolean( L, input()->IsKeyDown( code ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the key is down." )

LUA_BINDING_BEGIN( Inputs, IsMouseDown, "library", "Check if a specific mouse button is down." )
{
    MouseCode code = LUA_BINDING_ARGUMENT_ENUM( MouseCode, 1, "mouseCode" );
    lua_pushboolean( L, input()->IsMouseDown( code ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the mouse button is down." )

LUA_BINDING_BEGIN( Inputs, LookupBinding, "library", "Look up a key binding." )
{
    const char *keyName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "keyName" );
    bool exact = LUA_BINDING_ARGUMENT( lua_toboolean, 2, "exactMatch" );
    if ( exact )
        lua_pushstring( L, engine->Key_LookupBindingExact( keyName ) );
    else
        lua_pushstring( L, engine->Key_LookupBinding( keyName ) );
    return 1;
}
LUA_BINDING_END( "string", "The key binding." )

LUA_BINDING_BEGIN( Inputs, LookupKeyBinding, "library", "Get the binding for a specific key code." )
{
    ButtonCode_t code = LUA_BINDING_ARGUMENT_ENUM( ButtonCode_t, 1, "keyCode" );
    const char *binding = engine->Key_BindingForKey( code );
    if ( binding && ( ( uintptr_t )binding != 0x10000 ) )  // Checking for special case as noted
        lua_pushstring( L, binding );
    else
        lua_pushstring( L, "" );
    return 1;
}
LUA_BINDING_END( "string", "The key binding." )

LUA_BINDING_BEGIN( Inputs, KeyCodeToString, "library", "Converts a key code to its string representation, e.g: KEY_W -> 'KEY_W'" )
{
    KeyCode keyCode = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 1, "keyCode" );
    lua_pushstring( L, Panel::KeyCodeToString( keyCode ) );
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the key code" )

LUA_BINDING_BEGIN( Inputs, KeyCodeToDisplayString, "library", "Converts a key code to a user-friendly string, e.g: KEY_W -> 'W'" )
{
    KeyCode keyCode = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 1, "keyCode" );
    wchar_t const *displayString = Panel::KeyCodeToDisplayString( keyCode );

    int len = Q_wcslen( displayString ) + 1;  // Include null terminator
    char buffer[256];
    V_wcstostr( displayString, len, buffer, len );

    lua_pushstring( L, buffer );

    return 1;
}
LUA_BINDING_END( "string", "The user-friendly string representation of the key code" )

LUA_BINDING_BEGIN( Inputs, OnChangeIme, "library", "Trigger a change in the IME state." )
{
    bool activate = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "activate" );
    input()->OnChangeIME( activate );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnChangeImeByHandle, "library", "Change the IME by handle." )
{
    unsigned int handle = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "handle" );
    input()->OnChangeIMEByHandle( handle );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnChangeImeConversionModeByHandle, "library", "Change the IME conversion mode by handle." )
{
    unsigned int handle = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "handle" );
    input()->OnChangeIMEConversionModeByHandle( handle );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnChangeImeSentenceModeByHandle, "library", "Change the IME sentence mode by handle." )
{
    unsigned int handle = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "handle" );
    input()->OnChangeIMESentenceModeByHandle( handle );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnImeChangeCandidates, "library", "Notify a change in IME candidates." )
{
    input()->OnIMEChangeCandidates();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnImeCloseCandidates, "library", "Close the IME candidates." )
{
    input()->OnIMECloseCandidates();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnImeComposition, "library", "Handle IME composition." )
{
    unsigned int composition = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "composition" );
    input()->OnIMEComposition( composition );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnImeEndComposition, "library", "End the IME composition." )
{
    input()->OnIMEEndComposition();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnImeRecomputeModes, "library", "Recompute IME modes." )
{
    input()->OnIMERecomputeModes();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnImeShowCandidates, "library", "Show IME candidates." )
{
    input()->OnIMEShowCandidates();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnImeStartComposition, "library", "Start the IME composition." )
{
    input()->OnIMEStartComposition();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnInputLanguageChanged, "library", "Handle input language change." )
{
    input()->OnInputLanguageChanged();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, OnKeyCodeUnhandled, "library", "Handle unhandled key codes." )
{
    KeyCode code = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 1, "keyCode" );
    input()->OnKeyCodeUnhandled( code );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, RegisterKeyCodeUnhandledListener, "library", "Register a listener for unhandled key codes." )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    input()->RegisterKeyCodeUnhandledListener( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, ReleaseAppModalSurface, "library", "Release the application modal surface." )
{
    input()->ReleaseAppModalSurface();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, ReleaseModalSubTree, "library", "Release the modal sub-tree." )
{
    input()->ReleaseModalSubTree();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, ScreenToWorld, "library", "Convert screen coordinates to world coordinates." )
{
    float x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    float y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    Vector vecPickingRay = ScreenToWorld( x, y );
    lua_pushvector( L, vecPickingRay );
    return 1;
}
LUA_BINDING_END( "Vector", "World coordinates vector." )

LUA_BINDING_BEGIN( Inputs, AimToVector, "library", "Convert aiming angles and screen coordinates to a vector." )
{
    QAngle vecAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angles" );
    float fov = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "fov" );
    float x = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "x" );
    float y = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "y" );
    float nScreenWidth = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "screenWidth" );
    float nScreenHeight = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "screenHeight" );
    Vector vecPickingRay;
    ScreenToWorld( x, y, fov, vecAngles, vecPickingRay, nScreenWidth, nScreenHeight );
    lua_pushvector( L, vecPickingRay );
    return 1;
}
LUA_BINDING_END( "Vector", "Aim vector in world coordinates." )

LUA_BINDING_BEGIN( Inputs, SetAppModalSurface, "library", "Set the application modal surface." )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    input()->SetAppModalSurface( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, SetCandidateListPageStart, "library", "Set the starting index of the current page in the candidate list." )
{
    int index = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "index" );
    input()->SetCandidateListPageStart( index );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, SetCandidateWindowPos, "library", "Set the position of the candidate window." )
{
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    input()->SetCandidateWindowPos( x, y );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, SetCursorPosition, "library", "Set the cursor position." )
{
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    input()->SetCursorPos( x, y );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, SetModalSubTree, "library", "Set the modal sub-tree." )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "modalSubTree" );
    Panel *scope = LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "scopePanel" );
    bool receiveMessages = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, true, "receiveMessages" );
    input()->SetModalSubTree( panel->GetVPanel(), scope->GetVPanel(), receiveMessages );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, SetModalSubTreeReceiveMessages, "library", "Set the modal sub-tree to receive messages." )
{
    bool receive = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "receive" );
    input()->SetModalSubTreeReceiveMessages( receive );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, SetMouseCapture, "library", "Set the panel that captures the mouse input." )
{
    Panel *panel = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, NULL, "panel" );
    input()->SetMouseCapture( panel ? panel->GetVPanel() : NULL );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, SetMouseCaptureExtended, "library", "Set the mouse capture with specific mouse code." )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    MouseCode code = LUA_BINDING_ARGUMENT_ENUM( MouseCode, 2, "mouseCode" );
    input()->SetMouseCaptureEx( panel->GetVPanel(), code );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, SetMouseFocus, "library", "Set the mouse focus to a panel." )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    input()->SetMouseFocus( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, ShouldModalSubTreeReceiveMessages, "library", "Check if the modal sub-tree should receive messages." )
{
    lua_pushboolean( L, input()->ShouldModalSubTreeReceiveMessages() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the modal sub-tree should receive messages." )

LUA_BINDING_BEGIN( Inputs, UnregisterKeyCodeUnhandledListener, "library", "Unregister a listener for unhandled key codes." )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    input()->UnregisterKeyCodeUnhandledListener( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Inputs, WasKeyPressed, "library", "Check if a specific key was pressed." )
{
    KeyCode code = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 1, "keyCode" );
    lua_pushboolean( L, input()->WasKeyPressed( code ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the key was pressed." )

LUA_BINDING_BEGIN( Inputs, WasKeyReleased, "library", "Check if a specific key was released." )
{
    KeyCode code = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 1, "keyCode" );
    lua_pushboolean( L, input()->WasKeyReleased( code ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the key was released." )

LUA_BINDING_BEGIN( Inputs, WasKeyTyped, "library", "Check if a specific key was typed." )
{
    KeyCode code = LUA_BINDING_ARGUMENT_ENUM( KeyCode, 1, "keyCode" );
    lua_pushboolean( L, input()->WasKeyTyped( code ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the key was typed." )

LUA_BINDING_BEGIN( Inputs, WasMouseDoublePressed, "library", "Check if a specific mouse button was double pressed." )
{
    MouseCode code = LUA_BINDING_ARGUMENT_ENUM( MouseCode, 1, "mouseCode" );
    lua_pushboolean( L, input()->WasMouseDoublePressed( code ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the mouse button was double pressed." )

LUA_BINDING_BEGIN( Inputs, WasMousePressed, "library", "Check if a specific mouse button was pressed." )
{
    MouseCode code = LUA_BINDING_ARGUMENT_ENUM( MouseCode, 1, "mouseCode" );
    lua_pushboolean( L, input()->WasMousePressed( code ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the mouse button was pressed." )

LUA_BINDING_BEGIN( Inputs, WasMouseReleased, "library", "Check if a specific mouse button was released." )
{
    MouseCode code = LUA_BINDING_ARGUMENT_ENUM( MouseCode, 1, "mouseCode" );
    lua_pushboolean( L, input()->WasMouseReleased( code ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the mouse button was released." )

/*
** Open input library
*/
LUALIB_API int luaopen_input( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Inputs );

    return 1;
}
