#include <cbase.h>

#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <lColor.h>

#include "scripted_controls/lPanel.h"
#include "scripted_controls/lCheckButton.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LCheckButton::LCheckButton( Panel *parent, const char *panelName, const char *text, lua_State *L /* = nullptr */ )
    : CheckButton( parent, panelName, text, L )
{
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
LCheckButton::~LCheckButton()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void LCheckButton::OnCheckButtonChecked()
{
#ifdef LUA_SDK
    LUA_CALL_PANEL_METHOD_BEGIN( "OnCheckButtonChecked" );
    LUA_CALL_PANEL_METHOD_END( 0, 0 );
#endif
}

/*
** access functions (stack -> C)
*/

LUA_API lua_CheckButton *lua_tocheckbutton( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_CheckButton * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CheckButton *luaL_checkcheckbutton( lua_State *L, int narg )
{
    lua_CheckButton *d = lua_tocheckbutton( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CheckButton expected, got INVALID_PANEL" );
    return d;
}

LUA_REGISTRATION_INIT( CheckButton )

//LUA_BINDING_BEGIN( CheckButton, ChainToAnimationMap, "class|static", "Chains the button to an animation map" )
//{
//    lua_CheckButton::ChainToAnimationMap();
//    return 0;
//}
//LUA_BINDING_END()
//
//LUA_BINDING_BEGIN( CheckButton, ChainToMap, "class|static", "Chains the button to a map" )
//{
//    lua_CheckButton::ChainToMap();
//    return 0;
//}
//LUA_BINDING_END()
//
//LUA_BINDING_BEGIN( CheckButton, KB_AddBoundKey, "class|static", "Adds a bound key to the button" )
//{
//    const char *key = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "key" );
//    int unknownA = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "unknownA" );
//    int unknownB = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "unknownB" );
//    lua_CheckButton::KB_AddBoundKey( key, unknownA, unknownB );
//    return 0;
//}
//LUA_BINDING_END()
//
//LUA_BINDING_BEGIN( CheckButton, KB_ChainToMap, "class|static", "Chains the button to a keyboard map" )
//{
//    lua_CheckButton::KB_ChainToMap();
//    return 0;
//}
//LUA_BINDING_END()

LUA_BINDING_BEGIN( CheckButton, GetDisabledBackgroundColor, "class", "Gets the disabled background color" )
{
    lua_CheckButton *checkButton = LUA_BINDING_ARGUMENT( luaL_checkcheckbutton, 1, "checkButton" );
    lua_pushcolor( L, checkButton->GetDisabledBgColor() );
    return 1;
}
LUA_BINDING_END( "Color", "The disabled background color" )

LUA_BINDING_BEGIN( CheckButton, GetDisabledForegroundColor, "class", "Gets the disabled foreground color" )
{
    lua_CheckButton *checkButton = LUA_BINDING_ARGUMENT( luaL_checkcheckbutton, 1, "checkButton" );
    lua_pushcolor( L, checkButton->GetDisabledFgColor() );
    return 1;
}
LUA_BINDING_END( "Color", "The disabled foreground color" )

LUA_BINDING_BEGIN( CheckButton, SetCheckButtonCheckable, "class", "Sets whether the button is checkable" )
{
    lua_CheckButton *checkButton = LUA_BINDING_ARGUMENT( luaL_checkcheckbutton, 1, "checkButton" );
    bool checkable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "checkable" );
    checkButton->SetCheckButtonCheckable( checkable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CheckButton, __index, "class", "Metamethod that is called when a non-existent field is indexed" )
{
    CheckButton *pCheckButton = LUA_BINDING_ARGUMENT( lua_tocheckbutton, 1, "checkButton" );
    LUA_METATABLE_INDEX_CHECK_VALID( L, PanelIsValid );
    LUA_METATABLE_INDEX_CHECK( L, pCheckButton );

    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LCheckButton *plCheckButton = dynamic_cast< LCheckButton * >( pCheckButton );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plCheckButton );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "CheckButton" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "Button" );

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( CheckButton, __newindex, "class", "Metamethod that is called when a new field is added to the panel" )
{
    CheckButton *pCheckButton = LUA_BINDING_ARGUMENT( lua_tocheckbutton, 1, "checkButton" );

    if ( pCheckButton == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LCheckButton *plCheckButton = dynamic_cast< LCheckButton * >( pCheckButton );

    LUA_GET_REF_TABLE( L, plCheckButton );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" ) );
    lua_pop( L, 1 );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CheckButton, __eq, "class", "Metamethod that is called when two buttons are compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_tocheckbutton, 1, "checkButton" ) == LUA_BINDING_ARGUMENT( lua_tocheckbutton, 2, "other" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the buttons are equal" )

LUA_BINDING_BEGIN( CheckButton, __tostring, "class", "Metamethod that is called when the button is to be converted to a string" )
{
    CheckButton *pCheckButton = LUA_BINDING_ARGUMENT( lua_tocheckbutton, 1, "checkButton" );
    if ( pCheckButton == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pCheckButton->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "CheckButton: \"%s\"", pName );
    }
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the button" )

LUA_REGISTRATION_INIT( Panels )

LUA_BINDING_BEGIN( Panels, CheckButton, "library", "Creates a new CheckButton panel" )
{
    Panel *parent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, VGui_GetClientLuaRootPanel(), "parent" );
    const char *name = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "CheckButton", "name" );
    const char *text = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, "", "text" );

    LCheckButton *pPanel = new LCheckButton( parent, name, text, L );
    LCheckButton::PushLuaInstanceSafe( L, pPanel );
    return 1;
}
LUA_BINDING_END( "CheckButton", "The new CheckButton Panel" )

/*
** Open CheckButton object
*/
LUALIB_API int luaopen_vgui_CheckButton( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "CheckButton" );

    LUA_REGISTRATION_COMMIT( CheckButton );

    lua_pushstring( L, LUA_PANELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Panels );

    lua_pop( L, 2 );
    return 0;
}
