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
    LUA_SAFE_SET_METATABLE( L, LUA_CONVARMETANAME );
}

LUALIB_API lua_ConVar *luaL_checkconvar( lua_State *L, int narg )
{
    lua_ConVar **d = ( lua_ConVar ** )luaL_checkudata( L, narg, LUA_CONVARMETANAME );
    if ( *d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ConsoleVariable expected, got NULL" );
    return *d;
}

LUA_REGISTRATION_INIT( ConsoleVariable )

LUA_BINDING_BEGIN( ConsoleVariable, AddFlags, "class", "Adds flags to the ConsoleVariable" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    pConVar->AddFlags( luaL_checknumber( L, 2 ) );
    return 0;
}
LUA_BINDING_END( "void", "Adds flags to the ConsoleVariable" )

LUA_BINDING_BEGIN( ConsoleVariable, GetBool, "class", "Gets the value of the ConsoleVariable as a boolean" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushboolean( L, pConVar->GetBool() );
    return 1;
}
LUA_BINDING_END( "boolean", "Gets the value of the ConsoleVariable as a boolean" )

LUA_BINDING_BEGIN( ConsoleVariable, GetDefault, "class", "Gets the default value of the ConsoleVariable" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushstring( L, pConVar->GetDefault() );
    return 1;
}
LUA_BINDING_END( "string", "Gets the default value of the ConsoleVariable" )

LUA_BINDING_BEGIN( ConsoleVariable, GetFloat, "class", "Gets the value of the ConsoleVariable as a float" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushnumber( L, pConVar->GetFloat() );
    return 1;
}
LUA_BINDING_END( "number", "Gets the value of the ConsoleVariable as a float" )

LUA_BINDING_BEGIN( ConsoleVariable, GetHelpText, "class", "Gets the help text of the ConsoleVariable" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushstring( L, pConVar->GetHelpText() );
    return 1;
}
LUA_BINDING_END( "string", "Gets the help text of the ConsoleVariable" )

LUA_BINDING_BEGIN( ConsoleVariable, GetInteger, "class", "Gets the value of the ConsoleVariable as an integer" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushinteger( L, pConVar->GetInt() );
    return 1;
}
LUA_BINDING_END( "integer", "Gets the value of the ConsoleVariable as an integer" )

LUA_BINDING_BEGIN( ConsoleVariable, GetMax, "class", "Gets the maximum value of the ConsoleVariable" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    float maxVal;
    lua_pushboolean( L, pConVar->GetMax( maxVal ) );
    lua_pushnumber( L, maxVal );
    return 2;
}
LUA_BINDING_END( "boolean, number", "Gets the maximum value of the ConsoleVariable" )

LUA_BINDING_BEGIN( ConsoleVariable, GetMin, "class", "Gets the minimum value of the ConsoleVariable" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    float minVal;
    lua_pushboolean( L, pConVar->GetMin( minVal ) );
    lua_pushnumber( L, minVal );
    return 2;
}
LUA_BINDING_END( "boolean, number", "Gets the minimum value of the ConsoleVariable" )

LUA_BINDING_BEGIN( ConsoleVariable, GetName, "class", "Gets the name of the ConsoleVariable" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushstring( L, pConVar->GetName() );
    return 1;
}
LUA_BINDING_END( "string", "Gets the name of the ConsoleVariable" )

LUA_BINDING_BEGIN( ConsoleVariable, GetString, "class", "Gets the value of the ConsoleVariable as a string" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushstring( L, pConVar->GetString() );
    return 1;
}
LUA_BINDING_END( "string", "Gets the value of the ConsoleVariable as a string" )

LUA_BINDING_BEGIN( ConsoleVariable, IsCommand, "class", "Whether the ConsoleVariable is a command (and not a ConsoleVariable)" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushboolean( L, pConVar->IsCommand() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the ConsoleVariable is a command (and not a ConsoleVariable)" )

LUA_BINDING_BEGIN( ConsoleVariable, IsFlagSet, "class", "Whether the ConsoleVariable has a flag set" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushboolean( L, pConVar->IsFlagSet( luaL_checknumber( L, 2 ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the ConsoleVariable has a flag set" )

LUA_BINDING_BEGIN( ConsoleVariable, IsRegistered, "class", "Whether the ConsoleVariable is registered" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushboolean( L, pConVar->IsRegistered() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the ConsoleVariable is registered" )

LUA_BINDING_BEGIN( ConsoleVariable, IsValid, "class", "Whether the ConsoleVariable is valid" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( lua_toconvar, 1, "consoleVariable" );
    lua_pushboolean( L, pConVar != NULL );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the ConsoleVariable is valid" )

LUA_BINDING_BEGIN( ConsoleVariable, Revert, "class", "Reverts the ConsoleVariable to its default value" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    pConVar->Revert();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ConsoleVariable, SetValue, "class", "Sets the value of the ConsoleVariable" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            pConVar->SetValue( ( float )luaL_checknumber( L, 2 ) );
            break;
        case LUA_TBOOLEAN:
            pConVar->SetValue( lua_toboolean( L, 2 ) ? 1 : 0 );
            break;
        case LUA_TSTRING:
        default:
            pConVar->SetValue( luaL_checkstring( L, 2 ) );
            break;
    }
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ConsoleVariable, __tostring, "class", "Returns a string representation of the ConsoleVariable" )
{
    lua_ConVar *pConVar = LUA_BINDING_ARGUMENT( luaL_checkconvar, 1, "consoleVariable" );
    lua_pushfstring( L, "ConsoleVariable: \"%s\" = \"%s\"", pConVar->GetName(), pConVar->GetString() );
    return 1;
}
LUA_BINDING_END( "string", "Returns a string representation of the ConsoleVariable" )

static CUtlDict< ConVar *, unsigned short > m_ConVarDatabase;

LUA_REGISTRATION_INIT( ConsoleVariables )

LUA_BINDING_BEGIN( ConsoleVariables, Create, "library", "Creates a console variable or returns the existing one with the given name" )
{
    const char *pName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    const char *pValue = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "value" );
    int flags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "flags" );
    const char *pHelpText = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 4, "", "helpText" );
    bool bMin = lua_isnumber( L, 5 );
    float min = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 0.0, "minimum" );
    bool bMax = lua_isnumber( L, 6 );
    float max = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 6, 0, "maximum" );

    unsigned short lookup = m_ConVarDatabase.Find( pName );
    if ( lookup != m_ConVarDatabase.InvalidIndex() || cvar->FindVar( pName ) )
    {
        lua_pushconvar( L, cvar->FindVar( pName ) );
        return 1;
    }

    ConVar *pConVar = new ConVar( strdup( pName ), pValue, flags, strdup( pHelpText ), bMin, min, bMax, max );

    lookup = m_ConVarDatabase.Insert( pName, pConVar );
    Assert( lookup != m_ConVarDatabase.InvalidIndex() );
    lua_pushconvar( L, pConVar );
    return 1;
}
LUA_BINDING_END( "ConsoleVariable", "The ConsoleVariable created or found" )

LUA_BINDING_BEGIN( ConsoleVariables, Get, "library", "Gets a console variable" )
{
    const char *pName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    lua_pushconvar( L, cvar->FindVar( pName ) );
    return 1;
}
LUA_BINDING_END( "ConsoleVariable", "The ConsoleVariable found" )

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

/*
** Open ConsoleVariable object
*/
LUALIB_API int luaopen_ConsoleVariableMeta( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_CONVARMETANAME );
    
    LUA_REGISTRATION_COMMIT( ConsoleVariable );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_CONVARMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "ConsoleVariable" */

    LUA_REGISTRATION_COMMIT_LIBRARY( ConsoleVariables );

    lua_pop( L, 1 );
    return 1;
}
