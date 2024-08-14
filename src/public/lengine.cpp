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

// Engine interfaces.
// IVEngineServer	*engine = NULL;

LUA_REGISTRATION_INIT( Engines );

LUA_BINDING_BEGIN( Engines, AllowEdictReuse, "library", "Allow immediate edict reuse." )
{
    engine->AllowImmediateEdictReuse();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, ChangeLevel, "library", "Change the level." )
{
    const char *levelName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "levelName" );
    const char *transitionMap = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, NULL, "transitionMap" );

    engine->ChangeLevel( levelName, transitionMap );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, CheckAreasConnected, "library", "Check if two areas are connected." )
{
    int startArea = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "startArea" );
    int endArea = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "endArea" );

    lua_pushboolean( L, engine->CheckAreasConnected( startArea, endArea ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the areas are connected, false otherwise." )

LUA_BINDING_BEGIN( Engines, CopyFile, "library", "Copy a file." )
{
    const char *source = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "source" );
    const char *destination = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "destination" );

    lua_pushboolean( L, engine->CopyFile( source, destination ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the file was copied, false otherwise." )

LUA_BINDING_BEGIN( Engines, CreateFakeClient, "library", "Create a fake client." )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );

    edict_t *edict = engine->CreateFakeClient( name );

    if ( !edict )
    {
        Msg( "Failed to create Bot.\n" );
        return 0;
    }

    CBasePlayer *player = ( ( CBasePlayer * )CBaseEntity::Instance( edict ) );
    CBaseEntity::PushLuaInstanceSafe( L, player );

    return 1;
}
LUA_BINDING_END( "Entity", "The created fake client." )

LUA_BINDING_BEGIN( Engines, ForceExactFile, "library", "Force a file to be exact." )
{
    const char *fileName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "fileName" );

    engine->ForceExactFile( fileName );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, ForceSimpleMaterial, "library", "Force a material to be simple." )
{
    const char *materialName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "materialName" );

    engine->ForceSimpleMaterial( materialName );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, GetClientConsoleVariableValue, "library", "Get a client's ConsoleVariable value." )
{
    int playerIndex;

    if ( lua_toplayer( L, 1 ) )
        playerIndex = LUA_BINDING_ARGUMENT( lua_toplayer, 1, "playerOrIndex" )->entindex();
    else
        playerIndex = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "playerOrIndex" );

    const char *conVarName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "conVarName" );

    lua_pushstring( L, engine->GetClientConVarValue( playerIndex, conVarName ) );

    return 1;
}
LUA_BINDING_END( "string", "The client's ConsoleVariable value." )

LUA_BINDING_BEGIN( Engines, GetClientConsoleVariableValueAsNumber, "library", "Get a client's ConsoleVariable value as a number." )
{
    int playerIndex;

    if ( lua_toplayer( L, 1 ) )
        playerIndex = LUA_BINDING_ARGUMENT( lua_toplayer, 1, "playerOrIndex" )->entindex();
    else
        playerIndex = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "playerOrIndex" );

    const char *conVarName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "conVarName" );

    lua_pushnumber( L, Q_atoi( engine->GetClientConVarValue( playerIndex, conVarName ) ) );

    return 1;
}
LUA_BINDING_END( "number", "The client's ConsoleVariable value as a number." )

LUA_BINDING_BEGIN( Engines, GetEntityCount, "library", "Get the entity count." )
{
    lua_pushinteger( L, engine->GetEntityCount() );

    return 1;
}
LUA_BINDING_END( "integer", "The entity count." )

LUA_BINDING_BEGIN( Engines, GetMostRecentlyLoadedFileName, "library", "Get the most recently loaded file name." )
{
    lua_pushstring( L, engine->GetMostRecentlyLoadedFileName() );

    return 1;
}
LUA_BINDING_END( "string", "The most recently loaded file name." )

LUA_BINDING_BEGIN( Engines, GetPlayerNetInfo, "library", "Get the player's net info." )
{
    int playerIndex = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "playerIndex" );

    lua_pushnetchannel( L, engine->GetPlayerNetInfo( playerIndex ) );

    return 1;
}
LUA_BINDING_END( "netchannel", "The player's net info." )

LUA_BINDING_BEGIN( Engines, GetSaveFileName, "library", "Get the save file name." )
{
    lua_pushstring( L, engine->GetSaveFileName() );

    return 1;
}
LUA_BINDING_END( "string", "The save file name." )

LUA_BINDING_BEGIN( Engines, IndexOfEdict, "library", "Get the index of an edict." )
{
    CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    lua_pushinteger( L, engine->IndexOfEdict( entity->edict() ) );

    return 1;
}
LUA_BINDING_END( "integer", "The index of the edict." )

LUA_BINDING_BEGIN( Engines, InsertServerCommand, "library", "Insert a server command." )
{
    const char *command = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "command" );

    engine->InsertServerCommand( command );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, IsDecalPrecached, "library", "Check if a decal is precached." )
{
    const char *decalName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "decalName" );

    lua_pushboolean( L, engine->IsDecalPrecached( decalName ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the decal is precached, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsDedicatedServer, "library", "Check if the server is dedicated." )
{
    lua_pushboolean( L, engine->IsDedicatedServer() );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the server is dedicated, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsGenericPrecached, "library", "Check if a generic is precached." )
{
    const char *genericName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "genericName" );

    lua_pushboolean( L, engine->IsGenericPrecached( genericName ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the generic is precached, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsInCommentaryMode, "library", "Check if the game is in commentary mode." )
{
    lua_pushboolean( L, engine->IsInCommentaryMode() );

    return 1;
}
LUA_BINDING_END( "boolean", "True if in commentary mode." )

LUA_BINDING_BEGIN( Engines, IsInternalBuild, "library", "Check if the build is internal." )
{
    lua_pushboolean( L, engine->IsInternalBuild() );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the build is internal, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsMapValid, "library", "Check if a map is valid." )
{
    const char *mapName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "mapName" );

    lua_pushboolean( L, engine->IsMapValid( mapName ) );

    return 1;
}
LUA_BINDING_END( "integer", "True if the map is valid, false otherwise." )

LUA_BINDING_BEGIN( Engines, IsModelPrecached, "library", "Check if a model is precached." )
{
    const char *modelName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "modelName" );

    lua_pushboolean( L, engine->IsModelPrecached( modelName ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the model is precached, false otherwise." )

LUA_BINDING_BEGIN( Engines, LightStyle, "library", "Set a light style." )
{
    int styleIndex = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "styleIndex" );
    const char *styleValue = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "styleValue" );

    engine->LightStyle( styleIndex, styleValue );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, LoadAdjacentEnts, "library", "Load adjacent entities." )
{
    const char *mapName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "mapName" );
    const char *baseMapName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "baseMapName" );

    engine->LoadAdjacentEnts( mapName, baseMapName );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, LoadGameState, "library", "Load the game state." )
{
    const char *fileName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "fileName" );
    bool isDemo = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "isDemo" );

    lua_pushboolean( L, engine->LoadGameState( fileName, isDemo ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the game state was loaded, false otherwise." )

LUA_BINDING_BEGIN( Engines, LockNetworkStringTables, "library", "Lock the network string tables." )
{
    bool lock = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "lock" );

    lua_pushboolean( L, engine->LockNetworkStringTables( lock ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the network string tables were locked, false otherwise." )

LUA_BINDING_BEGIN( Engines, LogPrint, "library", "Print a message to the log." )
{
    const char *message = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );

    engine->LogPrint( message );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, MultiplayerEndGame, "library", "End the multiplayer game." )
{
    engine->MultiplayerEndGame();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, NotifyEdictFlagsChange, "library", "Notify of an edict flags change." )
{
    int flags = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "flags" );

    engine->NotifyEdictFlagsChange( flags );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, PrecacheDecal, "library", "Precache a decal." )
{
    const char *decalName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "decalName" );
    bool preload = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "preload" );

    lua_pushinteger( L, engine->PrecacheDecal( decalName, preload ) );

    return 1;
}
LUA_BINDING_END( "integer", "The decal index." )

LUA_BINDING_BEGIN( Engines, PrecacheGeneric, "library", "Precache a generic." )
{
    const char *genericName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "genericName" );
    bool preload = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "preload" );

    lua_pushinteger( L, engine->PrecacheGeneric( genericName, preload ) );

    return 1;
}
LUA_BINDING_END( "integer", "The generic index." )

LUA_BINDING_BEGIN( Engines, PrecacheModel, "library", "Precache a model." )
{
    const char *modelName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "modelName" );
    bool preload = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "preload" );

    lua_pushinteger( L, engine->PrecacheModel( modelName, preload ) );

    return 1;
}
LUA_BINDING_END( "integer", "The model index." )

LUA_BINDING_BEGIN( Engines, PrecacheSentenceFile, "library", "Precache a sentence file." )
{
    const char *sentenceFileName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sentenceFileName" );
    bool preload = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "preload" );

    lua_pushinteger( L, engine->PrecacheSentenceFile( sentenceFileName, preload ) );

    return 1;
}
LUA_BINDING_END( "integer", "The sentence file index." )

LUA_BINDING_BEGIN( Engines, ServerCommand, "library", "Run a server command." )
{
    const char *command = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "command" );

    if ( TryRunConsoleCommand( command ) )
        return 0;

    engine->ServerCommand( command );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, ServerExecute, "library", "Execute the server." )
{
    engine->ServerExecute();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, SetAreaPortalState, "library", "Set the area portal state." )
{
    int portalIndex = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "portalIndex" );
    int isOpen = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "isOpen" );

    engine->SetAreaPortalState( portalIndex, isOpen );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, SetDedicatedServerBenchmarkMode, "library", "Set the dedicated server benchmark mode." )
{
    bool enabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "enabled" );

    engine->SetDedicatedServerBenchmarkMode( enabled );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Engines, GetSimulationInterval, "library", "Get the simulation interval." )
{
    lua_pushnumber( L, UTIL_GetSimulationInterval() );
    return 1;
}
LUA_BINDING_END( "number", "The simulation interval." )

LUA_BINDING_BEGIN( Engines, GetListenServerHost, "library", "Get the local player on a listen server - this is for multiplayer use only." )
{
    CBaseEntity::PushLuaInstanceSafe( L, UTIL_GetListenServerHost() );
    return 1;
}
LUA_BINDING_END( "Entity", "The listen server host." )

/*
** Open engine library
*/
LUALIB_API int luaopen_engine( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Engines );
    return 1;
}
