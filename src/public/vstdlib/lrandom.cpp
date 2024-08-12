#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Randoms );

LUA_BINDING_BEGIN( Randoms, RandomFloat, "library", "Generate a random float.", "shared" )
{
    float min = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 1, 0.0f, "min" );
    float max = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.0f, "max" );

    lua_pushnumber( L, random->RandomFloat( min, max ) );

    return 1;
}
LUA_BINDING_END( "number", "The generated random float." )

LUA_BINDING_BEGIN( Randoms, RandomFloatExponential, "library", "Generate a random float with an exponential distribution.", "shared" )
{
    float min = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 1, 0.0f, "min" );
    float max = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.0f, "max" );
    float exponent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 1.0f, "exponent" );

    lua_pushnumber( L, random->RandomFloatExp( min, max, exponent ) );

    return 1;
}
LUA_BINDING_END( "number", "The generated random float." )

LUA_BINDING_BEGIN( Randoms, RandomInteger, "library", "Generate a random integer.", "shared" )
{
    int min = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "min" );
    int max = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "max" );

    lua_pushinteger( L, random->RandomInt( min, max ) );

    return 1;
}
LUA_BINDING_END( "number", "The generated random integer." )

LUA_BINDING_BEGIN( Randoms, SetSeed, "library", "Set the seed for the random number generator.", "shared" )
{
    int seed = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "seed" );

    random->SetSeed( seed );

    return 0;
}
LUA_BINDING_END()

/*
** Open random library
*/
LUALIB_API int luaopen_random( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Randoms );
    return 1;
}
