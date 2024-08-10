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

static int engine_ChangeTeam( lua_State *L )
{
    engine->ChangeTeam( luaL_checkstring( L, 1 ) );
    return 0;
}

static int engine_GetAppID( lua_State *L )
{
    lua_pushinteger( L, engine->GetAppID() );
    return 1;
}

static int engine_GetServerAddress( lua_State *L )
{
    lua_pushstring( L, g_pGameInfoStore->GetServerAddress() );
    return 1;
}

static int engine_GetServerName( lua_State *L )
{
    lua_pushstring( L, g_pGameInfoStore->GetServerName() );
    return 1;
}

static int engine_GetLevelName( lua_State *L )
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

static int engine_GetPlayerByAddress( lua_State *L )
{
    CBasePlayer *pPlayer = g_pGameInfoStore->GetPlayerByAddress( luaL_checkstring( L, 1 ) );

    if ( pPlayer )
    {
        CBaseEntity::PushLuaInstanceSafe( L, pPlayer );
        return 1;
    }

    return 0;
}

static int engine_GetGameDir( lua_State *L )
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

static int engine_GetMapEntitiesString( lua_State *L )
{
    lua_pushstring( L, engine->GetMapEntitiesString() );
    return 1;
}

static int engine_GetMountableGames( lua_State *L )
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

static int engine_IsInEditMode( lua_State *L )
{
    lua_pushinteger( L, engine->IsInEditMode() );
    return 1;
}

static int engine_IsLowViolence( lua_State *L )
{
    lua_pushboolean( L, engine->IsLowViolence() );
    return 1;
}

static int engine_IsPaused( lua_State *L )
{
    lua_pushboolean( L, engine->IsPaused() );
    return 1;
}

static int engine_SentenceGroupIndexFromName( lua_State *L )
{
    lua_pushinteger( L, engine->SentenceGroupIndexFromName( luaL_checkstring( L, 1 ) ) );
    return 1;
}

static int engine_SentenceGroupNameFromIndex( lua_State *L )
{
    lua_pushstring( L, engine->SentenceGroupNameFromIndex( luaL_checkinteger( L, 1 ) ) );
    return 1;
}

static int engine_SentenceIndexFromName( lua_State *L )
{
    lua_pushinteger( L, engine->SentenceIndexFromName( luaL_checkstring( L, 1 ) ) );
    return 1;
}

static int engine_SentenceLength( lua_State *L )
{
    lua_pushnumber( L, engine->SentenceLength( luaL_checkinteger( L, 1 ) ) );
    return 1;
}

static int engine_SentenceNameFromIndex( lua_State *L )
{
    lua_pushstring( L, engine->SentenceNameFromIndex( luaL_checkinteger( L, 1 ) ) );
    return 1;
}

static int engine_Time( lua_State *L )
{
    lua_pushnumber( L, engine->Time() );
    return 1;
}

static int engine_GetSoundDuration( lua_State *L )
{
    float flDuration = enginesound->GetSoundDuration( luaL_checkstring( L, 1 ) );
    lua_pushnumber( L, flDuration );
    return 1;
}

static const luaL_Reg enginelib[] = {
    { "ChangeTeam", engine_ChangeTeam },
    { "GetAppID", engine_GetAppID },
    { "GetServerAddress", engine_GetServerAddress },
    { "GetServerName", engine_GetServerName },
    { "GetLevelName", engine_GetLevelName },
    { "GetPlayerByAddress", engine_GetPlayerByAddress },
    { "GetGameDir", engine_GetGameDir },
    { "GetMapEntitiesString", engine_GetMapEntitiesString },
    { "GetMountableGames", engine_GetMountableGames },
    { "IsInEditMode", engine_IsInEditMode },
    { "IsLowViolence", engine_IsLowViolence },
    { "IsPaused", engine_IsPaused },
    { "SentenceGroupIndexFromName", engine_SentenceGroupIndexFromName },
    { "SentenceGroupNameFromIndex", engine_SentenceGroupNameFromIndex },
    { "SentenceIndexFromName", engine_SentenceIndexFromName },
    { "SentenceLength", engine_SentenceLength },
    { "SentenceNameFromIndex", engine_SentenceNameFromIndex },
    { "Time", engine_Time },
    { "GetSoundDuration", engine_GetSoundDuration },
    { NULL, NULL } };

/*
** Open engine library
*/
LUALIB_API int luaopen_engine_shared( lua_State *L )
{
    luaL_register( L, LUA_ENGINELIBNAME, enginelib );
    return 1;
}
