#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "listeamfriends.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_ISteamFriends *lua_tosteamfriends( lua_State *L, int idx )
{
    lua_ISteamFriends **ppSteamFriends = ( lua_ISteamFriends ** )lua_touserdata( L, idx );
    return *ppSteamFriends;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushsteamfriends( lua_State *L, ISteamFriends *pSteamFriends )
{
    lua_ISteamFriends **ppSteamFriends = ( lua_ISteamFriends ** )lua_newuserdata( L, sizeof( pSteamFriends ) );
    *ppSteamFriends = pSteamFriends;
    LUA_SAFE_SET_METATABLE( L, LUA_STEAMFRIENDSMETANAME );
}

LUALIB_API lua_ISteamFriends *luaL_checksteamfriends( lua_State *L, int narg )
{
    lua_ISteamFriends *d = ( lua_ISteamFriends * )luaL_checkudata( L, narg, LUA_STEAMFRIENDSMETANAME );

    if ( d == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ISteamFriends expected, got NULL" );

    return d;
}

LUA_REGISTRATION_INIT( SteamFriendsHandle )

LUA_BINDING_BEGIN( SteamFriendsHandle, ActivateGameOverlay, "class", "Activate the Steam overlay to a specific place. Valid dialog options include: 'Friends', 'Community', 'Players', 'Settings', 'OfficialGameGroup', 'Stats', 'Achievements', 'chatroomgroup/nnnn'." )
{
    lua_ISteamFriends *steamFriends = LUA_BINDING_ARGUMENT( luaL_checksteamfriends, 1, "steamFriends" );
    const char *dialogOption = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "dialogOption" );
    steamFriends->ActivateGameOverlay( dialogOption );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( SteamFriendsHandle, GetClanCount, "class", "Get the number of clans the current user is a member of." )
{
    lua_ISteamFriends *steamFriends = LUA_BINDING_ARGUMENT( luaL_checksteamfriends, 1, "steamFriends" );
    lua_pushinteger( L, steamFriends->GetClanCount() );
    return 1;
}
LUA_BINDING_END( "integer", "The number of clans the current user is a member of." )

/*
* TODO: Expose enum and check for it
* enum EFriendFlags
{
  k_EFriendFlagNone			= 0x00,
  k_EFriendFlagBlocked		= 0x01,
  k_EFriendFlagFriendshipRequested	= 0x02,
  k_EFriendFlagImmediate		= 0x04,			// "regular" friend
  k_EFriendFlagClanMember		= 0x08,
  k_EFriendFlagOnGameServer	= 0x10,
  // k_EFriendFlagHasPlayedWith	= 0x20,	// not currently used
  // k_EFriendFlagFriendOfFriend	= 0x40, // not currently used
  k_EFriendFlagRequestingFriendship = 0x80,
  k_EFriendFlagRequestingInfo = 0x100,
  k_EFriendFlagIgnored		= 0x200,
  k_EFriendFlagIgnoredFriend	= 0x400,
  // k_EFriendFlagSuggested		= 0x800,	// not used
  k_EFriendFlagChatMember		= 0x1000,
  k_EFriendFlagAll			= 0xFFFF,
};
*/
LUA_BINDING_BEGIN( SteamFriendsHandle, GetFriendCount, "class", "Get the number of friends the current user has of the specified flag types." )
{
    lua_ISteamFriends *steamFriends = LUA_BINDING_ARGUMENT( luaL_checksteamfriends, 1, "steamFriends" );
    int flag = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flag" );
    lua_pushinteger( L, steamFriends->GetFriendCount( flag ) );
    return 1;
}
LUA_BINDING_END( "integer", "The number of friends the current user has." )

LUA_BINDING_BEGIN( SteamFriendsHandle, GetPersonaName, "class", "Get the persona name of the current user." )
{
    lua_ISteamFriends *steamFriends = LUA_BINDING_ARGUMENT( luaL_checksteamfriends, 1, "steamFriends" );
    lua_pushstring( L, steamFriends->GetPersonaName() );
    return 1;
}
LUA_BINDING_END( "string", "The persona name of the current user." )

LUA_BINDING_BEGIN( SteamFriendsHandle, GetPersonaState, "class", "Get the persona state of the current user." )
{
    lua_ISteamFriends *steamFriends = LUA_BINDING_ARGUMENT( luaL_checksteamfriends, 1, "steamFriends" );
    lua_pushinteger( L, steamFriends->GetPersonaState() );
    return 1;
}
LUA_BINDING_END( "integer", "The persona state of the current user." )

LUA_BINDING_BEGIN( SteamFriendsHandle, SetPersonaName, "class", "Set the persona name of the current user." )
{
    lua_ISteamFriends *steamFriends = LUA_BINDING_ARGUMENT( luaL_checksteamfriends, 1, "steamFriends" );
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" );
    steamFriends->SetPersonaName( name );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( SteamFriendsHandle, __tostring, "class", "Get the persona name of the current user." )
{
    lua_ISteamFriends *steamFriends = LUA_BINDING_ARGUMENT( lua_tosteamfriends, 1, "steamFriends" );

    if ( steamFriends )
        lua_pushfstring( L, "SteamFriendsHandle: %s", steamFriends->GetPersonaName() );
    else
        lua_pushstring( L, "SteamFriendsHandle: NULL" );

    return 1;
}
LUA_BINDING_END( "string", "The persona name of the current user." )

/*
** Open ISteamFriends object
*/
LUALIB_API int luaopen_ISteamFriends( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_STEAMFRIENDSMETANAME );

    LUA_REGISTRATION_COMMIT( SteamFriendsHandle );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_STEAMFRIENDSMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "SteamFriendsHandle" */

    return 1;
}
