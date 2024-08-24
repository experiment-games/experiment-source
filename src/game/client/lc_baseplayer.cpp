#include "cbase.h"
#include "c_baseplayer.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseplayer_shared.h"
#include "iclientmode.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Player )

LUA_BINDING_BEGIN( Player, ShouldDrawLocalPlayer, "class", "Whether the player's player model will be drawn." )
{
    lua_pushboolean( L, g_pClientMode->ShouldDrawLocalPlayer( LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the player's player model will be drawn." )

/*
** Open CBasePlayer object
*/
LUALIB_API int luaopen_CBasePlayer( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEPLAYERMETANAME );

    LUA_REGISTRATION_COMMIT( Player );

    return 1;
}
