//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lColor.h"
#include "lconvar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int cvar_AllocateDLLIdentifier( lua_State *L )
{
    lua_pushinteger( L, cvar->AllocateDLLIdentifier() );
    return 1;
}

static int cvar_ConsoleColorPrintf( lua_State *L )
{
    cvar->ConsoleColorPrintf( luaL_checkcolor( L, 1 ), luaL_checkstring( L, 2 ) );
    return 0;
}

static int cvar_ConsoleDPrintf( lua_State *L )
{
    cvar->ConsoleDPrintf( luaL_checkstring( L, 1 ) );
    return 0;
}

static int cvar_ConsolePrintf( lua_State *L )
{
    cvar->ConsolePrintf( luaL_checkstring( L, 1 ) );
    return 0;
}

static int cvar_FindVar( lua_State *L )
{
    lua_pushconvar( L, cvar->FindVar( luaL_checkstring( L, 1 ) ) );
    return 1;
}

static int cvar_GetCommandLineValue( lua_State *L )
{
    lua_pushstring( L, cvar->GetCommandLineValue( luaL_checkstring( L, 1 ) ) );
    return 1;
}

static int cvar_RevertFlaggedConVars( lua_State *L )
{
    cvar->RevertFlaggedConVars( luaL_checkint( L, 1 ) );
    return 0;
}

static const luaL_Reg cvarlib[] = {
    { "AllocateDLLIdentifier", cvar_AllocateDLLIdentifier },
    { "ConsoleColorPrintf", cvar_ConsoleColorPrintf },
    { "ConsoleDPrintf", cvar_ConsoleDPrintf },
    { "ConsolePrintf", cvar_ConsolePrintf },
    { "FindVar", cvar_FindVar },
    { "GetCommandLineValue", cvar_GetCommandLineValue },
    { "RevertFlaggedConVars", cvar_RevertFlaggedConVars },
    { NULL, NULL } };

void CV_GlobalChange_Lua( IConVar *var, const char *pOldString, float flOldValue )
{
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
            luasrc_pcall( targetState, 3, 0, 0 );
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
LUALIB_API int luaopen_cvars( lua_State *L )
{
    luaL_register( L, LUA_CVARLIBNAME, cvarlib );
    InstallGlobalChangeCallbacks();
    return 1;
}
