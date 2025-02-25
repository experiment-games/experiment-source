#include "cbase.h"
#include <vgui_controls/PHandle.h>
#include "luamanager.h"
#include "luasrclib.h"
#include <LKeyValues.h>
#include <vgui_int.h>
#include <vgui/IInput.h>
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Panel.h>

#include "scripted_controls/lPanel.h"
#include "scripted_controls/lEditablePanel.h"

using namespace vgui;

LEditablePanel::LEditablePanel( Panel *parent, const char *panelName, lua_State *L /* = nullptr */ )
    : EditablePanel( parent, panelName, L )
{
}

LEditablePanel::~LEditablePanel()
{
    if ( input()->GetAppModalSurface() == GetVPanel() )
    {
        vgui::input()->ReleaseAppModalSurface();
        if ( m_hPreviousModal != 0 )
        {
            vgui::input()->SetAppModalSurface( m_hPreviousModal );
            m_hPreviousModal = 0;
        }
    }
}

void LEditablePanel::SetFocusTopLevel( bool state )
{
    GetFocusNavGroup().SetFocusTopLevel( state );
}

/*
** The following Modal functions are taken from the Frame vgui
*/

// Purpose: Bring the frame to the front and requests focus, ensures it's not minimized
void LEditablePanel::Activate()
{
    MoveToFront();

    if ( IsKeyBoardInputEnabled() )
    {
        RequestFocus();
    }
    SetVisible( true );
    SetEnabled( true );

    surface()->SetMinimized( GetVPanel(), false );
}

// Sets up, cleans up modal dialogs
void LEditablePanel::DoModal()
{
    InvalidateLayout();
    Activate();
    m_hPreviousModal = vgui::input()->GetAppModalSurface();  // TODO: Crashes?
    vgui::input()->SetAppModalSurface( GetVPanel() );
}

bool LEditablePanel::IsModal()
{
    return vgui::input()->GetAppModalSurface() == GetVPanel();
}

void LEditablePanel::Close()
{
    OnClose();
}

void LEditablePanel::CloseModal()
{
    vgui::input()->ReleaseAppModalSurface();
    if ( m_hPreviousModal != 0 )
    {
        vgui::input()->SetAppModalSurface( m_hPreviousModal );
        m_hPreviousModal = 0;
    }
    PostMessage( this, new KeyValues( "Close" ) );
}

// Go invisible when a close message is recieved.
void LEditablePanel::OnClose()
{
    // if we're modal, release that before we hide the window else the wrong window will get focus
    if ( input()->GetAppModalSurface() == GetVPanel() )
    {
        input()->ReleaseAppModalSurface();
        if ( m_hPreviousModal != 0 )
        {
            vgui::input()->SetAppModalSurface( m_hPreviousModal );
            m_hPreviousModal = 0;
        }
    }

    BaseClass::OnClose();

    FinishClose();
}

void LEditablePanel::FinishClose()
{
    SetVisible( false );

    // if ( m_bDeleteSelfOnClose )
    {
        // Must be last because if vgui is not running then this will call delete this!!!
        MarkForDeletion();
    }
}

/*
** access functions (stack -> C)
*/

LUA_API lua_EditablePanel *lua_toeditablepanel( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );

    if ( phPanel == NULL )
        return NULL;

    return dynamic_cast< lua_EditablePanel * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_EditablePanel *luaL_checkeditablepanel( lua_State *L, int narg )
{
    lua_EditablePanel *d = lua_toeditablepanel( L, narg );

    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "EditablePanel expected, got INVALID_PANEL" );

    return d;
}

LUA_REGISTRATION_INIT( EditablePanel )

LUA_BINDING_BEGIN( EditablePanel, ActivateBuildMode, "class", "Activates build mode for the panel" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->ActivateBuildMode();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, ApplySettings, "class", "Applies settings from key-values" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->ApplySettings( LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 2, "keyValues" ) );
    return 0;
}
LUA_BINDING_END()

// LUA_BINDING_BEGIN( EditablePanel, ChainToAnimationMap, "class|static", "Chains to the animation map" )
//{
//     lua_EditablePanel::ChainToAnimationMap();
//     return 0;
// }
// LUA_BINDING_END()
//
// LUA_BINDING_BEGIN( EditablePanel, ChainToMap, "class|static", "Chains to the map" )
//{
//     lua_EditablePanel::ChainToMap();
//     return 0;
// }
// LUA_BINDING_END()

// Experiment; Disabled because this is not the way we want devs to create panels
// LUA_BINDING_BEGIN( EditablePanel, CreateControlByName, "class", "Creates a control by name" )
//{
//    vgui::Panel *pPanel = LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )
//        ->CreateControlByName( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ) );
//
//    LPanel::PushLuaInstanceSafe( L, pPanel );
//    return 1;
//}
// LUA_BINDING_END( "Panel", "The created panel" )

LUA_BINDING_BEGIN( EditablePanel, DoModal, "class", "Makes the panel modal" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->DoModal();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, IsModal, "class", "Checks if the panel is modal" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->IsModal() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is modal" )

LUA_BINDING_BEGIN( EditablePanel, GetControlInt, "class", "Gets an integer value from a control" )
{
    lua_pushinteger( L,
                     LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )
                         ->GetControlInt(
                             LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "control" ),
                             LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "default" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The control integer value" )

LUA_BINDING_BEGIN( EditablePanel, GetControlString, "class", "Gets a string value from a control" )
{
    lua_pushstring( L,
                    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )
                        ->GetControlString(
                            LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "control" ),
                            LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, "", "default" ) ) );
    return 1;
}
LUA_BINDING_END( "string", "The control string value" )

LUA_BINDING_BEGIN( EditablePanel, GetCurrentKeyFocus, "class", "Gets the current key focus" )
{
    Panel::PushVPanelLuaInstance( L, LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->GetCurrentKeyFocus() );
    return 1;
}
LUA_BINDING_END( "Panel", "The current key focus panel" )

LUA_BINDING_BEGIN( EditablePanel, GetDialogVariables, "class", "Gets the dialog variables" )
{
    lua_pushkeyvalues( L, LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->GetDialogVariables() );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The dialog variables" )

// Experiment; Disabled since this might return a non-Lua panel
// LUA_BINDING_BEGIN( EditablePanel, HasHotkey, "class", "Checks if the panel has a hotkey" )
//{
//    size_t l;
//    Panel *pPanel = LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->HasHotkey( ( wchar_t )LUA_BINDING_ARGUMENT_WITH_EXTRA( luaL_checklstring, 2, &l, "hotkey" ) );
//    pPanel->PushLuaInstance( L ); // TODO: Push correctly
//    return 1;
//}
// LUA_BINDING_END( "Panel", "The panel with the hotkey" )

// LUA_BINDING_BEGIN( EditablePanel, KB_AddBoundKey, "class|static", "Adds a bound key" )
//{
//     lua_EditablePanel::KB_AddBoundKey( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "cmd1" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "cmd2" ) );
//     return 0;
// }
// LUA_BINDING_END()
//
// LUA_BINDING_BEGIN( EditablePanel, KB_ChainToMap, "class", "Chains to the keymap" )
//{
//     lua_EditablePanel::KB_ChainToMap();
//     return 0;
// }
// LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, LoadControlSettings, "class", "Loads control settings" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->LoadControlSettings( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "dialogResourceName" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, NULL, "pathId" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optkeyvalues, 4, NULL, "keyValues" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, LoadControlSettingsAndUserConfig, "class", "Loads control settings and user config" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->LoadControlSettingsAndUserConfig( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "dialogResourceName" ), ( int )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "dialogId" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, LoadUserConfig, "class", "Loads user configuration" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->LoadUserConfig( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "configName" ), ( int )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "dialogId" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, OnRequestFocus, "class", "Handles request focus event" )
{
    Panel *subFocusPanel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "subFocusPanel" );
    Panel *defaultPanel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 3, "defaultPanel" );
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->OnRequestFocus( subFocusPanel->GetVPanel(), defaultPanel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, RegisterControlSettingsFile, "class", "Registers a control settings file" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->RegisterControlSettingsFile( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "dialogResourceName" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, NULL, "pathId" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, RequestFocusNext, "class", "Requests focus for the next panel" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "panel" );
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->RequestFocusNext( panel->GetVPanel() ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the next panel received focus" )

LUA_BINDING_BEGIN( EditablePanel, RequestFocusPrev, "class", "Requests focus for the previous panel" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 2, "panel" );
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->RequestFocusPrev( panel->GetVPanel() ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the previous panel received focus" )

LUA_BINDING_BEGIN( EditablePanel, RequestInfoFromChild, "class", "Requests info from a child panel" )
{
    lua_pushboolean( L,
                     LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->RequestInfoFromChild( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "childName" ), LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 3, "keyValues" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the request was successful" )

LUA_BINDING_BEGIN( EditablePanel, SaveUserConfig, "class", "Saves the user configuration" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->SaveUserConfig();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, SetControlEnabled, "class", "Enables or disables a control" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->SetControlEnabled( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "control" ), LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "enabled" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, SetControlInt, "class", "Sets an integer value for a control" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->SetControlInt( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "control" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, SetControlString, "class", "Sets a string value for a control" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->SetControlString( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "control" ), LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "value" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, SetControlVisible, "class", "Sets a control's visibility" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->SetControlVisible( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "control" ), LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "visible" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, SetDialogVariable, "class", "Sets a dialog variable" )
{
    switch ( lua_type( L, 3 ) )
    {
        case LUA_TNUMBER:
            LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->SetDialogVariable( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ), ( float )LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" ) );
            break;
        case LUA_TSTRING:
        default:
            LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->SetDialogVariable( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ), LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "value" ) );
            break;
    }
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, SetFocusTopLevel, "class", "Sets the focus to the top level" )
{
    LUA_BINDING_ARGUMENT( luaL_checkeditablepanel, 1, "editablePanel" )->SetFocusTopLevel( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "topLevel" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, __index, "class", "Metamethod that is called when a non-existent field is indexed" )
{
    EditablePanel *pEditablePanel = LUA_BINDING_ARGUMENT( lua_toeditablepanel, 1, "editablePanel" );
    LUA_METATABLE_INDEX_CHECK_VALID( L, PanelIsValid );
    LUA_METATABLE_INDEX_CHECK( L, pEditablePanel );

    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LEditablePanel *plEditablePanel = dynamic_cast< LEditablePanel * >( pEditablePanel );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plEditablePanel );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "EditablePanel" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "Panel" );

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( EditablePanel, __newindex, "class", "Metamethod that is called when a new field is added" )
{
    EditablePanel *pEditablePanel = LUA_BINDING_ARGUMENT( lua_toeditablepanel, 1, "editablePanel" );

    if ( pEditablePanel == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LEditablePanel *plEditablePanel = dynamic_cast< LEditablePanel * >( pEditablePanel );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LUA_GET_REF_TABLE( L, plEditablePanel );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, field );
    lua_pop( L, 1 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EditablePanel, __eq, "class", "Metamethod that is called when two panels are compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_toeditablepanel, 1, "editablePanel" ) == LUA_BINDING_ARGUMENT( lua_toeditablepanel, 2, "other" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the panels are equal" )

LUA_BINDING_BEGIN( EditablePanel, __tostring, "class", "Metamethod that is called when the panel is to be converted to a string" )
{
    EditablePanel *pEditablePanel = LUA_BINDING_ARGUMENT( lua_toeditablepanel, 1, "editablePanel" );

    if ( pEditablePanel == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pEditablePanel->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "EditablePanel: \"%s\"", pName );
    }
    return 1;
}
LUA_BINDING_END( "string", "String representation of the panel" )

LUA_BINDING_BEGIN( EditablePanel, __gc, "class", "Metamethod that is called when the panel is garbage collected" )
{
    return PanelCollectGarbage( L );
}
LUA_BINDING_END()

LUA_REGISTRATION_INIT( Panels )

LUA_BINDING_BEGIN( Panels, EditablePanel, "library", "Creates a new editable panel" )
{
    Panel *parent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, VGui_GetClientLuaRootPanel(), "parent" );
    const char *name = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "EditablePanel", "name" );
    lua_EditablePanel *pPanel = new lua_EditablePanel( parent, name, L );
    LEditablePanel::PushLuaInstanceSafe( L, pPanel );
    return 1;
}
LUA_BINDING_END( "EditablePanel", "The new editable panel" )

/*
** Open EditablePanel object
*/
LUALIB_API int luaopen_vgui_EditablePanel( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "EditablePanel" );

    LUA_REGISTRATION_COMMIT( EditablePanel );

    lua_pushstring( L, LUA_PANELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Panels );

    lua_pop( L, 2 );
    return 0;
}
