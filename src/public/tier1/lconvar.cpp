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

LUA_API lua_ConVar *lua_toconvar( lua_State *L, int idx )
{
    lua_ConVar **ppConVar = ( lua_ConVar ** )lua_touserdata( L, idx );
    return *ppConVar;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushconvar( lua_State *L, lua_ConVar *pConVar )
{
    lua_ConVar **ppConVar = ( lua_ConVar ** )lua_newuserdata( L, sizeof( pConVar ) );
    *ppConVar = pConVar;
    LUA_SAFE_SET_METATABLE( L, LUA_CONVARLIBNAME );
}

LUALIB_API lua_ConVar *luaL_checkconvar( lua_State *L, int narg )
{
    lua_ConVar **d = ( lua_ConVar ** )luaL_checkudata( L, narg, LUA_CONVARLIBNAME );
    if ( *d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ConVar expected, got NULL" );
    return *d;
}

LUA_REGISTRATION_INIT()

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
