#include <cbase.h>

#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <lColor.h>

#include <scripted_controls/lAvatarImage.h>
#include "scripted_controls/lPanel.h"
#include "lbaseplayer_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LAvatarImage::LAvatarImage( Panel *parent, const char *panelName, lua_State *L /* = nullptr */ )
    : CAvatarImagePanel( parent, panelName, L )
{
    m_lua_State = L;
}

/*
** access functions (stack -> C)
*/

LUA_API lua_AvatarImage *lua_toavatarImage( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_AvatarImage * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_AvatarImage *luaL_checkavatarImage( lua_State *L, int narg )
{
    lua_AvatarImage *d = lua_toavatarImage( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "AvatarImage expected, got INVALID_PANEL" );
    return d;
}

LUA_REGISTRATION_INIT( AvatarImage )

LUA_BINDING_BEGIN( AvatarImage, SetPlayer, "class", "Sets the player whose avatar to show in this panel." )
{
    lua_AvatarImage *avatarImage = LUA_BINDING_ARGUMENT( luaL_checkavatarImage, 1, "avatarImage" );
    CBasePlayer *pPlayer = LUA_BINDING_ARGUMENT( luaL_checkplayer, 2, "player" );
    int size = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 3, 32, "size" );
    avatarImage->SetPlayer( pPlayer );
    avatarImage->SetAvatarSize( size, size );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AvatarImage, __index, "class", "Metamethod that is called when a non-existant field is indexed" )
{
    lua_AvatarImage *pAvatarImage = LUA_BINDING_ARGUMENT( lua_toavatarImage, 1, "avatarImage" );
    LUA_METATABLE_INDEX_CHECK_VALID( L, PanelIsValid );
    LUA_METATABLE_INDEX_CHECK( L, pAvatarImage );

    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LAvatarImage *plAvatarImage = dynamic_cast< LAvatarImage * >( pAvatarImage );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, plAvatarImage );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "AvatarImage" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "Panel" );

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( AvatarImage, __newindex, "class", "Metamethod that is called when a new field is added to the panel" )
{
    lua_AvatarImage *pAvatarImage = LUA_BINDING_ARGUMENT( lua_toavatarImage, 1, "avatarImage" );

    if ( pAvatarImage == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LAvatarImage *plAvatarImage = dynamic_cast< LAvatarImage * >( pAvatarImage );

    LUA_GET_REF_TABLE( L, plAvatarImage );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" ) );
    lua_pop( L, 1 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AvatarImage, __eq, "class", "Metamethod that is called when two avatar images are compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_toavatarImage, 1, "avatarImage" ) == LUA_BINDING_ARGUMENT( lua_toavatarImage, 2, "other" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the avatar images are equal" )

LUA_BINDING_BEGIN( AvatarImage, __tostring, "class", "Metamethod that is called when the avatarImage is to be converted to a string" )
{
    lua_AvatarImage *pAvatarImage = LUA_BINDING_ARGUMENT( lua_toavatarImage, 1, "avatarImage" );
    if ( pAvatarImage == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pAvatarImage->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "AvatarImage: \"%s\"", pName );
    }
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AvatarImage, __gc, "class", "Metamethod that is called when the panel is garbage collected" )
{
    return PanelCollectGarbage( L );
}
LUA_BINDING_END()

LUA_REGISTRATION_INIT( Panels )

LUA_BINDING_BEGIN( Panels, AvatarImage, "library", "Creates a new avatarImage" )
{
    Panel *parent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, VGui_GetClientLuaRootPanel(), "parent" );
    const char *name = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "AvatarImage", "name" );

    LAvatarImage *pPanel = new LAvatarImage( parent, name, L );
    LAvatarImage::PushLuaInstanceSafe( L, pPanel );
    return 1;
}
LUA_BINDING_END( "AvatarImage", "The new avatarImage" )

/*
** Open AvatarImage object
*/
LUALIB_API int luaopen_vgui_AvatarImage( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "AvatarImage" );

    LUA_REGISTRATION_COMMIT( AvatarImage );

    lua_pushstring( L, LUA_PANELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Panels );

    // Pop the panel library off the stack
    lua_pop( L, 1 );
    lua_pop( L, 1 );  // Pop the AvatarImage metatable

    return 0;
}
