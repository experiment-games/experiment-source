#include "cbase.h"
#include "steam_api.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "listeamfriends.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( SteamApiContexts )

LUA_BINDING_BEGIN( SteamApiContexts, SteamFriends, "library", "Get Steam Friends." )
{
    lua_pushsteamfriends( L, steamapicontext->SteamFriends() );
    return 1;
}
LUA_BINDING_END( "SteamFriendsHandle", "The SteamFriends object." )

/*
** Open steamapicontext library
*/
LUALIB_API int luaopen_SteamApiContexts( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( SteamApiContexts );
    return 1;
}
