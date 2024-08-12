#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lColor.h"
#include "lconvar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( ConsoleVariables );

LUA_BINDING_BEGIN( ConsoleVariables, AllocateDllIdentifier, "library", "Allocate a DLL identifier." )
{
    lua_pushinteger( L, cvar->AllocateDLLIdentifier() );
    return 1;
}
LUA_BINDING_END( "integer", "DLL identifier." )

LUA_BINDING_BEGIN( ConsoleVariables, ConsoleColorColorPrint, "library", "Print a message to the console with a specific color." )
{
    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" );

    cvar->ConsoleColorPrintf( color, "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ConsoleVariables, ConsoleDebugPrint, "library", "Print a debug message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );

    cvar->ConsoleDPrintf( "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ConsoleVariables, ConsolePrint, "library", "Print a message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );

    cvar->ConsolePrintf( "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ConsoleVariables, FindVariable, "library", "Find a console variable by name." )
{
    const char *pName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );

    lua_pushconvar( L, cvar->FindVar( pName ) );
    return 1;
}
LUA_BINDING_END( "ConsoleVariable", "Found console variable or NULL." )

LUA_BINDING_BEGIN( ConsoleVariables, GetCommandLineValue, "library", "Get the value of a command line parameter." )
{
    const char *pName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );

    lua_pushstring( L, cvar->GetCommandLineValue( pName ) );
    return 1;
}
LUA_BINDING_END( "string", "Value of the command line parameter." )

LUA_BINDING_BEGIN( ConsoleVariables, RevertFlaggedConVars, "library", "Revert flagged console variables." )
{
    int iFlag = LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "flag" );

    cvar->RevertFlaggedConVars( iFlag );
    return 0;
}
LUA_BINDING_END()

void CV_GlobalChange_Lua( IConVar *var, const char *pOldString, float flOldValue )
{
#ifdef CLIENT_DLL
    lua_State *targetState = nullptr;

    // Hacky workaround for when we load all libs into the menu
    if ( L != nullptr )
    {
        targetState = L;
    }
    else
    {
        targetState = LGameUI;
    }
#else
    lua_State *targetState = L;
#endif

    lua_getglobal( targetState, LUA_CVARLIBNAME );
    if ( lua_istable( targetState, -1 ) )
    {
        lua_getfield( targetState, -1, "CallGlobalChangeCallbacks" );
        if ( lua_isfunction( targetState, -1 ) )
        {
            lua_remove( targetState, -2 );
            lua_pushconvar( targetState, cvar->FindVar( var->GetName() ) );
            lua_pushstring( targetState, pOldString );
            lua_pushnumber( targetState, flOldValue );
            luasrc_pcall( targetState, 3, 0 );
        }
        else
            lua_pop( targetState, 2 );
    }
    else
        lua_pop( targetState, 1 );
}

void InstallGlobalChangeCallbacks( void )
{
    // Install the global cvar callback
    cvar->InstallGlobalChangeCallback( CV_GlobalChange_Lua );
}

void RemoveGlobalChangeCallbacks( void )
{
    // Remove the global cvar callback
    cvar->RemoveGlobalChangeCallback( CV_GlobalChange_Lua );
}

/*
** Open cvar library
*/
LUALIB_API int luaopen_ConsoleVariablesLib( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( ConsoleVariables );

    InstallGlobalChangeCallbacks();

    return 1;
}
