//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $NoKeywords: $
//===========================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lconvar.h"
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

LUA_API lua_ConVar *lua_toconvar( lua_State *L, int idx )
{
    lua_ConVar **ppConVar = ( lua_ConVar ** )lua_touserdata( L, idx );
    return *ppConVar;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushconcommand( lua_State *L, lua_ConCommand *pConCommand )
{
    lua_ConCommand **ppConCommand = ( lua_ConCommand ** )lua_newuserdata( L, sizeof( pConCommand ) );
    *ppConCommand = pConCommand;
    LUA_SAFE_SET_METATABLE( L, "ConsoleCommand" );
}

LUA_API void lua_pushconvar( lua_State *L, lua_ConVar *pConVar )
{
    lua_ConVar **ppConVar = ( lua_ConVar ** )lua_newuserdata( L, sizeof( pConVar ) );
    *ppConVar = pConVar;
    LUA_SAFE_SET_METATABLE( L, LUA_CONVARLIBNAME );
}

LUALIB_API lua_ConCommand *luaL_checkconcommand( lua_State *L, int narg )
{
    lua_ConCommand **d = ( lua_ConCommand ** )luaL_checkudata( L, narg, "ConsoleCommand" );

    if ( *d == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ConsoleCommand expected, got NULL" );

    return *d;
}

LUALIB_API lua_ConVar *luaL_checkconvar( lua_State *L, int narg )
{
    lua_ConVar **d = ( lua_ConVar ** )luaL_checkudata( L, narg, LUA_CONVARLIBNAME );
    if ( *d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ConVar expected, got NULL" );
    return *d;
}

static int ConCommand_CanAutoComplete( lua_State *L )
{
    lua_pushboolean( L, luaL_checkconcommand( L, 1 )->CanAutoComplete() );
    return 1;
}

static int ConCommand_IsCommand( lua_State *L )
{
    lua_pushboolean( L, luaL_checkconcommand( L, 1 )->IsCommand() );
    return 1;
}

static int ConCommand_IsValid( lua_State *L )
{
    lua_ConCommand *d = lua_toconcommand( L, 1 );
    lua_pushboolean( L, d != NULL );
    return 1;
}

static int ConCommand___tostring( lua_State *L )
{
    lua_pushfstring( L, "ConCommand: \"%s\"", luaL_checkconcommand( L, 1 )->GetName() );
    return 1;
}

static const luaL_Reg ConCommandmeta[] = {
    { "CanAutoComplete", ConCommand_CanAutoComplete },
    { "IsCommand", ConCommand_IsCommand },
    { "IsValid", ConCommand_IsValid },
    { "__tostring", ConCommand___tostring },
    { NULL, NULL } };

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
        Warning( "ConCommand: ConsoleCommands library table not found.\n" );
        return;
    }

    lua_getfield( L, -1, "Dispatch" );
    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L, 2 );  // pop the function and the table
        Warning( "ConCommand: ConsoleCommands.Dispatch function not found.\n" );
        return;
    }

    lua_remove( L, -2 ); // remove the library table

    // Push the player and command. With arguments as a table
    CBaseEntity::PushLuaInstanceSafe( L, pPlayer );
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
    lua_pop( L, 1 ); // pop the return value

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
/// Looks for the command in the ConCommand database and runs it if found
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

static int luasrc_ConCommand( lua_State *L )
{
    const char *pName = luaL_checkstring( L, 1 );
#ifdef CLIENT_DLL
    bool bIsGameUI = false;
    unsigned short lookup;
    lua_getglobal( L, "GAMEUI" );
    if ( !lua_isnoneornil( L, -1 ) && lua_toboolean( L, -1 ) )
    {
        bIsGameUI = true;

        // Complain about duplicately defined ConCommand names...
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
        // Complain about duplicately defined ConCommand names...
        unsigned short lookup = m_ConCommandDatabase.Find( pName );
        if ( lookup != m_ConCommandDatabase.InvalidIndex() || cvar->FindCommand( pName ) )
        {
            lua_pushconcommand( L, cvar->FindCommand( pName ) );
            return 1;
        }
#ifdef CLIENT_DLL
    }
#endif
    lua_pop( L, 1 );

    ConCommand *pConCommand;
#ifdef CLIENT_DLL
    if ( bIsGameUI )
#if 0
    pConCommand = new ConCommand(strdup(pName), CC_GameUIConCommand, strdup(luaL_optstring(L, 2, 0)), ( int )luaL_optnumber(L, 3, 0), NULL);
#else
        pConCommand = new ConCommand( strdup( pName ), CC_GameUIConCommand, strdup( luaL_optstring( L, 2, 0 ) ), 0, NULL );
#endif
    else
        pConCommand = new ConCommand( strdup( pName ), CC_ConCommand, strdup( luaL_optstring( L, 2, 0 ) ), FCVAR_CLIENTDLL | FCVAR_CLIENTCMD_CAN_EXECUTE | FCVAR_SERVER_CAN_EXECUTE, NULL );
#else
#if 0
    pConCommand = new ConCommand(strdup(pName), CC_ConCommand, strdup(luaL_optstring(L, 2, 0)), ( int )luaL_optnumber(L, 3, 0), NULL);
#else
    pConCommand = new ConCommand( strdup( pName ), CC_ConCommand, strdup( luaL_optstring( L, 2, 0 ) ), FCVAR_GAMEDLL | FCVAR_CLIENTCMD_CAN_EXECUTE, NULL );
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

static const luaL_Reg ConCommand_funcs[] = {
    { "ConsoleCommand", luasrc_ConCommand },
    { NULL, NULL } };

/*
** Open ConCommand object
*/
LUALIB_API int luaopen_ConCommand( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "ConsoleCommand" );
    luaL_register( L, NULL, ConCommandmeta );
    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, "ConsoleCommand" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "ConsoleCommand" */
    luaL_register( L, LUA_GNAME, ConCommand_funcs );
    lua_pop( L, 1 );
    return 1;
}

static int ConVar_AddFlags( lua_State *L )
{
    luaL_checkconvar( L, 1 )->AddFlags( luaL_checknumber( L, 2 ) );
    return 0;
}

static int ConVar_GetBool( lua_State *L )
{
    lua_pushboolean( L, luaL_checkconvar( L, 1 )->GetBool() );
    return 1;
}

static int ConVar_GetDefault( lua_State *L )
{
    lua_pushstring( L, luaL_checkconvar( L, 1 )->GetDefault() );
    return 1;
}

static int ConVar_GetFloat( lua_State *L )
{
    lua_pushnumber( L, luaL_checkconvar( L, 1 )->GetFloat() );
    return 1;
}

static int ConVar_GetHelpText( lua_State *L )
{
    lua_pushstring( L, luaL_checkconvar( L, 1 )->GetHelpText() );
    return 1;
}

static int ConVar_GetInt( lua_State *L )
{
    lua_pushinteger( L, luaL_checkconvar( L, 1 )->GetInt() );
    return 1;
}

static int ConVar_GetMax( lua_State *L )
{
    float maxVal;
    lua_pushboolean( L, luaL_checkconvar( L, 1 )->GetMax( maxVal ) );
    lua_pushnumber( L, maxVal );
    return 2;
}

static int ConVar_GetMin( lua_State *L )
{
    float minVal;
    lua_pushboolean( L, luaL_checkconvar( L, 1 )->GetMin( minVal ) );
    lua_pushnumber( L, minVal );
    return 2;
}

static int ConVar_GetName( lua_State *L )
{
    lua_pushstring( L, luaL_checkconvar( L, 1 )->GetName() );
    return 1;
}

static int ConVar_GetString( lua_State *L )
{
    lua_pushstring( L, luaL_checkconvar( L, 1 )->GetString() );
    return 1;
}

static int ConVar_IsCommand( lua_State *L )
{
    lua_pushboolean( L, luaL_checkconvar( L, 1 )->IsCommand() );
    return 1;
}

static int ConVar_IsFlagSet( lua_State *L )
{
    lua_pushboolean( L, luaL_checkconvar( L, 1 )->IsFlagSet( luaL_checknumber( L, 2 ) ) );
    return 1;
}

static int ConVar_IsRegistered( lua_State *L )
{
    lua_pushboolean( L, luaL_checkconvar( L, 1 )->IsRegistered() );
    return 1;
}

static int ConVar_IsValid( lua_State *L )
{
    lua_ConVar *d = lua_toconvar( L, 1 );
    lua_pushboolean( L, d != NULL );
    return 1;
}

static int ConVar_Revert( lua_State *L )
{
    luaL_checkconvar( L, 1 )->Revert();
    return 0;
}

static int ConVar_SetValue( lua_State *L )
{
    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            luaL_checkconvar( L, 1 )->SetValue( ( float )luaL_checknumber( L, 2 ) );
            break;
        case LUA_TSTRING:
        default:
            luaL_checkconvar( L, 1 )->SetValue( luaL_checkstring( L, 2 ) );
            break;
    }
    return 0;
}

static int ConVar___tostring( lua_State *L )
{
    lua_pushfstring( L, "ConVar: \"%s\" = \"%s\"", luaL_checkconvar( L, 1 )->GetName(), luaL_checkconvar( L, 1 )->GetString() );
    return 1;
}

static const luaL_Reg ConVarmeta[] = {
    { "AddFlags", ConVar_AddFlags },
    { "GetBool", ConVar_GetBool },
    { "GetDefault", ConVar_GetDefault },
    { "GetFloat", ConVar_GetFloat },
    { "GetHelpText", ConVar_GetHelpText },
    { "GetInt", ConVar_GetInt },
    { "GetMax", ConVar_GetMax },
    { "GetMin", ConVar_GetMin },
    { "GetName", ConVar_GetName },
    { "GetString", ConVar_GetString },
    { "IsCommand", ConVar_IsCommand },
    { "IsFlagSet", ConVar_IsFlagSet },
    { "IsRegistered", ConVar_IsRegistered },
    { "IsValid", ConVar_IsValid },
    { "Revert", ConVar_Revert },
    { "SetValue", ConVar_SetValue },
    { "__tostring", ConVar___tostring },
    { NULL, NULL } };

static CUtlDict< ConVar *, unsigned short > m_ConVarDatabase;

static int luasrc_CreateConVar( lua_State *L )
{
    const char *pName = luaL_checkstring( L, 1 );
    // Complain about duplicately defined ConVar names...
    unsigned short lookup = m_ConVarDatabase.Find( pName );
    if ( lookup != m_ConVarDatabase.InvalidIndex() || cvar->FindVar( pName ) )
    {
        lua_pushconvar( L, cvar->FindVar( pName ) );
        return 1;
    }

    ConVar *pConVar = new ConVar( strdup( pName ), luaL_checkstring( L, 2 ), ( int )luaL_optnumber( L, 3, 0 ), strdup( luaL_optstring( L, 4, 0 ) ), luaL_optboolean( L, 5, 0 ), luaL_optnumber( L, 6, 0.0 ), luaL_optboolean( L, 7, 0 ), luaL_optnumber( L, 8, 0 ) );

    lookup = m_ConVarDatabase.Insert( pName, pConVar );
    Assert( lookup != m_ConVarDatabase.InvalidIndex() );
    lua_pushconvar( L, pConVar );
    return 1;
}

static int luasrc_GetConVar( lua_State *L )
{
    const char *pName = luaL_checkstring( L, 1 );
    lua_pushconvar( L, cvar->FindVar( pName ) );
    return 1;
}

void ResetConVarDatabase( void )
{
    for ( int i = m_ConVarDatabase.First(); i != m_ConVarDatabase.InvalidIndex(); i = m_ConVarDatabase.Next( i ) )
    {
        ConVar *pConVar = m_ConVarDatabase[i];
        cvar->UnregisterConCommand( pConVar );
        delete pConVar;
    }
    m_ConVarDatabase.RemoveAll();
}

static const luaL_Reg ConVar_funcs[] = {
    { "CreateConsoleVariable", luasrc_CreateConVar },
    { "GetConsoleVariable", luasrc_GetConVar },
    { NULL, NULL } };

/*
** Open ConVar object
*/
LUALIB_API int luaopen_ConVar( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_CONVARLIBNAME );
    luaL_register( L, NULL, ConVarmeta );
    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_CONVARLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "ConsoleVariable" */
    luaL_register( L, LUA_GNAME, ConVar_funcs );
    lua_pop( L, 1 );
    return 1;
}
