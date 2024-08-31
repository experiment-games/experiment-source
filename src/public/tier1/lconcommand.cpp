#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lconcommand.h"
#include "lbaseplayer_shared.h"
#include "datacache/imdlcache.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_ConCommand *lua_toconcommand( lua_State *L, int idx )
{
    lua_ConCommand **ppConCommand = ( lua_ConCommand ** )lua_touserdata( L, idx );
    return *ppConCommand;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushconcommand( lua_State *L, lua_ConCommand *pConCommand )
{
    lua_ConCommand **ppConCommand = ( lua_ConCommand ** )lua_newuserdata( L, sizeof( pConCommand ) );
    *ppConCommand = pConCommand;
    LUA_SAFE_SET_METATABLE( L, LUA_CONCOMMANDMETANAME );
}

LUALIB_API lua_ConCommand *luaL_checkconcommand( lua_State *L, int narg )
{
    lua_ConCommand **d = ( lua_ConCommand ** )luaL_checkudata( L, narg, LUA_CONCOMMANDMETANAME );

    if ( *d == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ConsoleCommand expected, got NULL" );

    return *d;
}

LUA_REGISTRATION_INIT( ConsoleCommand )

LUA_BINDING_BEGIN( ConsoleCommand, CanAutoComplete, "class", "Whether the command can be auto-completed" )
{
    lua_ConCommand *pConCommand = LUA_BINDING_ARGUMENT( luaL_checkconcommand, 1, "consoleCommand" );
    lua_pushboolean( L, pConCommand->CanAutoComplete() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the command can be auto-completed" )

LUA_BINDING_BEGIN( ConsoleCommand, IsCommand, "class", "Whether the command is a command (and not a ConsoleVariable)" )
{
    lua_ConCommand *pConCommand = LUA_BINDING_ARGUMENT( luaL_checkconcommand, 1, "consoleCommand" );
    lua_pushboolean( L, pConCommand->IsCommand() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the command is a command" )

LUA_BINDING_BEGIN( ConsoleCommand, IsValid, "class", "Whether the command is valid" )
{
    lua_ConCommand *pConCommand = LUA_BINDING_ARGUMENT( luaL_checkconcommand, 1, "consoleCommand" );
    lua_pushboolean( L, pConCommand != NULL );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the command is valid" )

LUA_BINDING_BEGIN( ConsoleCommand, __tostring, "class", "Returns a string representation of the command" )
{
    lua_ConCommand *pConCommand = LUA_BINDING_ARGUMENT( luaL_checkconcommand, 1, "consoleCommand" );
    lua_pushfstring( L, "ConsoleCommand: \"%s\"", pConCommand->GetName() );
    return 1;
}
LUA_BINDING_END( "string", "Returns a string representation of the command" )

#ifdef CLIENT_DLL
static CUtlDict< ConCommand *, unsigned short > m_GameUIConCommandDatabase;
#endif
static CUtlDict< ConCommand *, unsigned short > m_ConCommandDatabase;

static void LuaRunConCommand( lua_State *L, const CCommand &args )
{
#ifdef CLIENT_DLL
    C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
#else
    CBasePlayer *pPlayer = UTIL_GetCommandClient();
#endif
    const char *pCmd = args[0];

    // Is the client spawned yet?
    // if ( !pPlayer )
    // 	return;

    MDLCACHE_CRITICAL_SECTION();  // TODO: (Experiment) Why is this here?

    lua_getglobal( L, LUA_CONCOMMANDLIBNAME );
    if ( !lua_istable( L, -1 ) )
    {
        lua_pop( L, 1 );  // pop the nil value
        Warning( "ConsoleCommand: ConsoleCommands library table not found.\n" );
        return;
    }

    lua_getfield( L, -1, "Dispatch" );
    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L, 2 );  // pop the function and the table
        Warning( "ConsoleCommand: ConsoleCommands.Dispatch function not found.\n" );
        return;
    }

    lua_remove( L, -2 );  // remove the library table

    // Push the player and command. With arguments as a table
#ifdef CLIENT_DLL
    if ( L == LGameUI )
        lua_pushnil( L );
    else
#endif
        CBasePlayer::PushLuaInstanceSafe( L, pPlayer );
    lua_pushstring( L, pCmd );
    lua_newtable( L );
    for ( int i = 1; i < args.ArgC(); i++ )
    {
        lua_pushinteger( L, i );
        lua_pushstring( L, args[i] );
        lua_settable( L, -3 );
    }
    luasrc_pcall( L, 3, 1 );

    if ( !lua_isboolean( L, -1 ) )
    {
        lua_pop( L, 1 );  // pop the return value
        return;
    }

    bool res = ( bool )luaL_checkboolean( L, -1 );
    lua_pop( L, 1 );  // pop the return value

    if ( res )
    {
        return;
    }

#ifndef CLIENT_DLL
    // If we failed, see if we can't get some helpful information to the user
    if ( Q_strlen( pCmd ) > 128 )
    {
        ClientPrint( pPlayer, HUD_PRINTCONSOLE, "Console command too long.\n" );
    }
    else
    {
        ClientPrint( pPlayer, HUD_PRINTCONSOLE, UTIL_VarArgs( "Unknown command: %s\n", pCmd ) );
    }
#endif
}
#ifdef CLIENT_DLL
// Andrew; ugh.
// Experiment; pass the correct lua state to the function when in the gameui
void CC_GameUIConCommand( const CCommand &args )
{
    LuaRunConCommand( LGameUI, args );
}
#endif

void CC_ConCommand( const CCommand &args )
{
    LuaRunConCommand( L, args );
}

/// <summary>
/// Looks for the command in the ConsoleCommand database and runs it if found
/// The command string is expected to be in the format "command arg1 arg2 arg3 ..."
/// </summary>
/// <param name="pszCommandString"></param>
/// <returns>Whether the command was found and executed</returns>
bool TryRunConsoleCommand( const char *pszCommandString )
{
    CCommand args;
    args.Tokenize( pszCommandString );

    if ( args.ArgC() == 0 )
    {
        return false;
    }

    // TODO: GameUI find the command

    const char *pName = args[0];
    unsigned short lookup = m_ConCommandDatabase.Find( pName );
    if ( lookup == m_ConCommandDatabase.InvalidIndex() || !cvar->FindCommand( pName ) )
    {
        return false;
    }

    LuaRunConCommand( L, args );
    return true;
}

LUA_REGISTRATION_INIT( ConsoleCommands )

LUA_BINDING_BEGIN( ConsoleCommands, Create, "library", "Creates a console command or returns the existing one with the given name" )
{
    const char *pName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    const char *pHelpString = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "", "helpString" );
    int flags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "flags" );

    // On the client we may check the GameUI database instead of the main one if GAMEUI is true
#ifdef CLIENT_DLL
    bool bIsGameUI = false;
    unsigned short lookup;
    lua_getglobal( L, "GAMEUI" );
    if ( !lua_isnoneornil( L, -1 ) && lua_toboolean( L, -1 ) )
    {
        bIsGameUI = true;

        lookup = m_GameUIConCommandDatabase.Find( pName );
        if ( lookup != m_GameUIConCommandDatabase.InvalidIndex() || cvar->FindCommand( pName ) )
        {
            lua_pushconcommand( L, cvar->FindCommand( pName ) );
            return 1;
        }
    }
    else
    {
#endif
        unsigned short lookup = m_ConCommandDatabase.Find( pName );
        if ( lookup != m_ConCommandDatabase.InvalidIndex() || cvar->FindCommand( pName ) )
        {
            lua_pushconcommand( L, cvar->FindCommand( pName ) );
            return 1;
        }
#ifdef CLIENT_DLL
    }
    lua_pop( L, 1 );  // pop the GAMEUI value
#endif

    ConCommand *pConCommand;
#ifdef CLIENT_DLL
    if ( bIsGameUI )
#if 1  // TODO: (Experiment) Why was this disabled?
        pConCommand = new ConCommand( strdup( pName ), CC_GameUIConCommand, strdup( pHelpString ), ( int )flags, NULL );
#else
        pConCommand = new ConCommand( strdup( pName ), CC_GameUIConCommand, strdup( pHelpString ), 0, NULL );
#endif
    else
        pConCommand = new ConCommand( strdup( pName ), CC_ConCommand, strdup( pHelpString ), FCVAR_CLIENTDLL | FCVAR_CLIENTCMD_CAN_EXECUTE | FCVAR_SERVER_CAN_EXECUTE, NULL );
#else
#if 1  // Experiment; FCVAR_GAMEDLL | FCVAR_CLIENTCMD_CAN_EXECUTE must be set for UTIL_GetCommandClient to get the player running the command
    pConCommand = new ConCommand( strdup( pName ), CC_ConCommand, strdup( pHelpString ), ( ( int )flags | FCVAR_GAMEDLL | FCVAR_CLIENTCMD_CAN_EXECUTE ), NULL );
#else
    pConCommand = new ConCommand( strdup( pName ), CC_ConCommand, strdup( pHelpString ), FCVAR_GAMEDLL | FCVAR_CLIENTCMD_CAN_EXECUTE, NULL );
#endif
#endif

#ifdef CLIENT_DLL
    if ( bIsGameUI )
    {
        lookup = m_GameUIConCommandDatabase.Insert( pName, pConCommand );
        Assert( lookup != m_GameUIConCommandDatabase.InvalidIndex() );
    }
    else
    {
#endif
        lookup = m_ConCommandDatabase.Insert( pName, pConCommand );
        Assert( lookup != m_ConCommandDatabase.InvalidIndex() );
#ifdef CLIENT_DLL
    }
#endif
    lua_pushconcommand( L, pConCommand );
    return 1;
}
LUA_BINDING_END( "ConsoleCommand", "Created console command (returns existing command if one with the same name exists)" )

#ifdef GAME_DLL

LUA_BINDING_BEGIN( ConsoleCommands, IsCommandIssuedByServerAdmin, "library", "Whether the command was issued by a server admin", "server" )
{
    lua_pushboolean( L, UTIL_IsCommandIssuedByServerAdmin() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the command was issued by a server admin" )

LUA_BINDING_BEGIN( ConsoleCommands, GetCommandClientIndex, "library", "Get the index of the client that issued the command", "server" )
{
    lua_pushinteger( L, UTIL_GetCommandClientIndex() );
    return 1;
}
LUA_BINDING_END( "integer", "The index of the client that issued the command" )

LUA_BINDING_BEGIN( ConsoleCommands, GetCommandClient, "library", "Get the client that issued the command", "server" )
{
    CBasePlayer::PushLuaInstanceSafe( L, UTIL_GetCommandClient() );
    return 1;
}
LUA_BINDING_END( "Player", "The client that issued the command" )

#endif  // GAME_DLL

#ifdef CLIENT_DLL
void ResetGameUIConCommandDatabase( void )
{
    for ( int i = m_GameUIConCommandDatabase.First(); i != m_GameUIConCommandDatabase.InvalidIndex(); i = m_GameUIConCommandDatabase.Next( i ) )
    {
        ConCommand *pConCommand = m_GameUIConCommandDatabase[i];
        cvar->UnregisterConCommand( pConCommand );
        delete pConCommand;
    }
    m_GameUIConCommandDatabase.RemoveAll();
}
#endif

void ResetConCommandDatabase( void )
{
    for ( int i = m_ConCommandDatabase.First(); i != m_ConCommandDatabase.InvalidIndex(); i = m_ConCommandDatabase.Next( i ) )
    {
        ConCommand *pConCommand = m_ConCommandDatabase[i];
        cvar->UnregisterConCommand( pConCommand );
        delete pConCommand;
    }
    m_ConCommandDatabase.RemoveAll();
}

/*
** Open ConsoleCommand object
*/
LUALIB_API int luaopen_ConsoleCommandMeta( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_CONCOMMANDMETANAME );

    LUA_REGISTRATION_COMMIT( ConsoleCommand );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_CONCOMMANDMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "ConsoleCommand" */

    LUA_REGISTRATION_COMMIT_LIBRARY( ConsoleCommands );

    lua_pop( L, 1 );
    return 1;
}
