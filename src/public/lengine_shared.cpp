#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "linetchannelinfo.h"
#include "engine/IEngineSound.h"
#include <gameinfostore.h>
#include <lconvar.h>
#include <lconcommand.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#include <mountsteamcontent.h>

LUA_REGISTRATION_INIT( Engines );

/*
** pgGlobals bindings
*/

LUA_BINDING_BEGIN( Engines, GetAbsoluteFrameTime, "library", "Get the absolute frame time." )
{
    lua_pushnumber( L, gpGlobals->absoluteframetime );
    return 1;
}
LUA_BINDING_END( "number", "The absolute frame time." )

LUA_BINDING_BEGIN( Engines, GetCurrentTime, "library", "Get the current time." )
{
    lua_pushnumber( L, gpGlobals->curtime );
    return 1;
}
LUA_BINDING_END( "number", "The current time." )

LUA_BINDING_BEGIN( Engines, GetSystemTime, "library", "Get the system time." )
{
    lua_pushnumber( L, Plat_FloatTime() );
    return 1;
}
LUA_BINDING_END( "number", "The system time." )

LUA_BINDING_BEGIN( Engines, GetFrameCount, "library", "Get the frame count." )
{
    lua_pushinteger( L, gpGlobals->framecount );
    return 1;
}
LUA_BINDING_END( "integer", "The frame count." )

LUA_BINDING_BEGIN( Engines, GetFrameTime, "library", "Get the frame time." )
{
    lua_pushnumber( L, gpGlobals->frametime );
    return 1;
}
LUA_BINDING_END( "number", "The frame time." )

LUA_BINDING_BEGIN( Engines, GetIntervalPerTick, "library", "Get the interval per tick." )
{
    lua_pushnumber( L, gpGlobals->interval_per_tick );
    return 1;
}
LUA_BINDING_END( "number", "The interval per tick." )

LUA_BINDING_BEGIN( Engines, IsClient, "library", "Check if the game is running on the client." )
{
    lua_pushboolean( L, gpGlobals->IsClient() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the game is running on the client, false otherwise." )

LUA_BINDING_BEGIN( Engines, GetMaxClients, "library", "Get the maximum number of clients." )
{
#ifdef CLIENT_DLL
    lua_pushinteger( L, engine->GetMaxClients() );
#else
    lua_pushinteger( L, gpGlobals->maxClients );
#endif
    return 1;
}
LUA_BINDING_END( "integer", "The maximum number of clients." )

LUA_BINDING_BEGIN( Engines, GetNetworkProtocol, "library", "Get the network protocol." )
{
    lua_pushinteger( L, gpGlobals->network_protocol );
    return 1;
}
LUA_BINDING_END( "integer", "The network protocol." )

LUA_BINDING_BEGIN( Engines, GetRealTime, "library", "Get the real time." )
{
    lua_pushnumber( L, gpGlobals->realtime );
    return 1;
}
LUA_BINDING_END( "number", "The real time." )

LUA_BINDING_BEGIN( Engines, GetSimulatedTicksThisFrame, "library", "Get the number of simulated ticks this frame." )
{
    lua_pushinteger( L, gpGlobals->simTicksThisFrame );
    return 1;
}
LUA_BINDING_END( "integer", "The number of simulated ticks this frame." )

LUA_BINDING_BEGIN( Engines, GetTickCount, "library", "Get the tick count." )
{
    lua_pushinteger( L, gpGlobals->tickcount );
    return 1;
}
LUA_BINDING_END( "integer", "The tick count." )

/*
** Engine bindings
*/

LUA_BINDING_BEGIN( Engines, ChangeTeam, "library", "Change the player's team." )
{
    const char *teamName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "teamName" );

    engine->ChangeTeam( teamName );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, GetAppId, "library", "Get the AppID of the game." )
{
    lua_pushinteger( L, engine->GetAppID() );
    return 1;
}
LUA_BINDING_END( "integer", "The AppID of the game." )

LUA_BINDING_BEGIN( Engines, GetServerAddress, "library", "Get the server address." )
{
    lua_pushstring( L, g_pGameInfoStore->GetServerAddress() );
    return 1;
}
LUA_BINDING_END( "string", "The server address." )

LUA_BINDING_BEGIN( Engines, GetServerName, "library", "Get the server name." )
{
    lua_pushstring( L, g_pGameInfoStore->GetServerName() );
    return 1;
}
LUA_BINDING_END( "string", "The server name." )

LUA_BINDING_BEGIN( Engines, GetLevelName, "library", "Get the name of the current level." )
{
#ifdef CLIENT_DLL
    lua_pushstring( L, engine->GetLevelName() );
#else
    char szMap[MAX_PATH + 1] = "";
    Q_strncpy( szMap, gpGlobals->mapname.ToCStr(), ARRAYSIZE( szMap ) );
    lua_pushstring( L, szMap );
#endif
    return 1;
}
LUA_BINDING_END( "string", "The name of the current level." )

LUA_BINDING_BEGIN( Engines, GetPlayerByAddress, "library", "Get a player by their address." )
{
    const char *address = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "address" );
    CBasePlayer *pPlayer = g_pGameInfoStore->GetPlayerByAddress( address );

    if ( pPlayer )
    {
        CBaseEntity::PushLuaInstanceSafe( L, pPlayer );
        return 1;
    }

    CBaseEntity::PushLuaInstanceSafe( L, NULL );
    return 1;
}
LUA_BINDING_END( "Player", "The player entity." )

LUA_BINDING_BEGIN( Engines, GetGameDir, "library", "Get the game directory." )
{
#ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
#else
    char gamePath[MAX_PATH];
    engine->GetGameDir( gamePath, MAX_PATH );
    Q_StripTrailingSlash( gamePath );
#endif
    lua_pushstring( L, gamePath );

    return 1;
}
LUA_BINDING_END( "string", "The game directory." )

LUA_BINDING_BEGIN( Engines, GetMapEntitiesString, "library", "Get the map entities string." )
{
    lua_pushstring( L, engine->GetMapEntitiesString() );
    return 1;
}
LUA_BINDING_END( "string", "The map entities string." )

LUA_BINDING_BEGIN( Engines, GetMountableGames, "library", "Get a list of mountable games." )
{
    lua_newtable( L );

    CUtlVector< mountableGame_t > &mountableGames = GetMountableGames();

    for ( int i = 0; i < mountableGames.Count(); i++ )
    {
        lua_pushinteger( L, i + 1 );
        lua_newtable( L );

        lua_pushstring( L, "name" );
        lua_pushstring( L, mountableGames[i].name );
        lua_settable( L, -3 );

        lua_pushstring( L, "directoryName" );
        lua_pushstring( L, mountableGames[i].directoryName );
        lua_settable( L, -3 );

        lua_pushstring( L, "isMounted" );
        lua_pushboolean( L, mountableGames[i].isMounted );
        lua_settable( L, -3 );

        lua_pushstring( L, "isOwned" );
        lua_pushboolean( L, mountableGames[i].isOwned );
        lua_settable( L, -3 );

        lua_pushstring( L, "isInstalled" );
        lua_pushboolean( L, mountableGames[i].isInstalled );
        lua_settable( L, -3 );

        lua_pushstring( L, "appId" );
        lua_pushinteger( L, mountableGames[i].appId );
        lua_settable( L, -3 );

        lua_settable( L, -3 );
    }

    return 1;
}
LUA_BINDING_END( "table", "A table of mountable games." )

LUA_BINDING_BEGIN( Engines, IsInEditMode, "library", "Check if the game is in edit mode." )
{
    lua_pushinteger( L, engine->IsInEditMode() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the game is in edit mode, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsLowViolence, "library", "Check if the game is low violence." )
{
    lua_pushboolean( L, engine->IsLowViolence() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the game is low violence, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsPaused, "library", "Check if the game is paused." )
{
    lua_pushboolean( L, engine->IsPaused() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the game is paused, false otherwise." )

LUA_BINDING_BEGIN( Engines, SentenceGroupIndexFromName, "library", "Get the sentence group index from a name." )
{
    const char *groupName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "groupName" );

    lua_pushinteger( L, engine->SentenceGroupIndexFromName( groupName ) );
    return 1;
}
LUA_BINDING_END( "integer", "The sentence group index." )

LUA_BINDING_BEGIN( Engines, SentenceGroupNameFromIndex, "library", "Get the sentence group name from an index." )
{
    int index = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "index" );

    lua_pushstring( L, engine->SentenceGroupNameFromIndex( index ) );
    return 1;
}
LUA_BINDING_END( "string", "The sentence group name." )

LUA_BINDING_BEGIN( Engines, SentenceIndexFromName, "library", "Get the sentence index from a name." )
{
    const char *sentenceName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sentenceName" );

    lua_pushinteger( L, engine->SentenceIndexFromName( sentenceName ) );
    return 1;
}
LUA_BINDING_END( "integer", "The sentence index." )

LUA_BINDING_BEGIN( Engines, SentenceLength, "library", "Get the sentence length." )
{
    int index = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "index" );

    lua_pushnumber( L, engine->SentenceLength( index ) );
    return 1;
}
LUA_BINDING_END( "number", "The sentence length." )

LUA_BINDING_BEGIN( Engines, SentenceNameFromIndex, "library", "Get the sentence name from an index." )
{
    int index = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "index" );

    lua_pushstring( L, engine->SentenceNameFromIndex( index ) );
    return 1;
}
LUA_BINDING_END( "string", "The sentence name." )

LUA_BINDING_BEGIN( Engines, Time, "library", "Get the current time." )
{
    lua_pushnumber( L, engine->Time() );
    return 1;
}
LUA_BINDING_END( "number", "The current time." )

LUA_BINDING_BEGIN( Engines, GetSoundDuration, "library", "Get the duration of a sound." )
{
    const char *soundName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "soundName" );

    float flDuration = enginesound->GetSoundDuration( soundName );
    lua_pushnumber( L, flDuration );
    return 1;
}
LUA_BINDING_END( "number", "The duration of the sound." )

/*
** Open engine library
*/
LUALIB_API int luaopen_engine_shared( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Engines );
    return 1;
}
