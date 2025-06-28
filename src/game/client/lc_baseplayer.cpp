#include "cbase.h"
#include "c_baseplayer.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseplayer_shared.h"
#include "iclientmode.h"
#include <voice_status.h>
#include "steam/steam_api.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Player )

LUA_BINDING_BEGIN( Player, ShouldDrawLocalPlayer, "class", "Whether the player's player model will be drawn." )
{
    lua_pushboolean( L, g_pClientMode->ShouldDrawLocalPlayer( LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" ) ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the player's player model will be drawn." )

LUA_BINDING_BEGIN( Player, IsMuted, "class", "Whether the player's voice chat is muted in the client." )
{
    CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int playerIndex = player->entindex();
    lua_pushboolean( L, GetClientVoiceMgr()->IsPlayerBlocked( playerIndex ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the player's voice chat is muted." )

LUA_BINDING_BEGIN( Player, SetMuted, "class", "Sets whether the player's voice chat is muted in the client." )
{
    CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int playerIndex = player->entindex();
    bool muted = LUA_BINDING_ARGUMENT( lua_toboolean, 2, "muted" );

    GetClientVoiceMgr()->SetPlayerBlockedState( playerIndex, muted );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ShowProfile, "Opens the player's steam profile page in the steam client browser." )
{
    CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

    CSteamID steamID;
    player->GetSteamID( &steamID );

    if ( steamapicontext && steamapicontext->SteamFriends() )
    {
        steamapicontext->SteamFriends()->ActivateGameOverlayToUser( "steamid", steamID );
    }

    return 0;
}
LUA_BINDING_END()

/*
** Open CBasePlayer object
*/
LUALIB_API int luaopen_CBasePlayer( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEPLAYERMETANAME );

    LUA_REGISTRATION_COMMIT( Player );

    return 1;
}
