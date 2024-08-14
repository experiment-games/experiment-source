#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Randoms );

LUA_BINDING_BEGIN( Randoms, RandomFloat, "library", "Generate a random float." )
{
    float min = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 1, 0.0f, "min" );
    float max = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.0f, "max" );

    lua_pushnumber( L, random->RandomFloat( min, max ) );

    return 1;
}
LUA_BINDING_END( "number", "The generated random float." )

LUA_BINDING_BEGIN( Randoms, RandomFloatExponential, "library", "Generate a random float with an exponential distribution." )
{
    float min = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 1, 0.0f, "min" );
    float max = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.0f, "max" );
    float exponent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 1.0f, "exponent" );

    lua_pushnumber( L, random->RandomFloatExp( min, max, exponent ) );

    return 1;
}
LUA_BINDING_END( "number", "The generated random float." )

LUA_BINDING_BEGIN( Randoms, RandomInteger, "library", "Generate a random integer." )
{
    int min = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "min" );
    int max = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "max" );

    lua_pushinteger( L, random->RandomInt( min, max ) );

    return 1;
}
LUA_BINDING_END( "number", "The generated random integer." )

LUA_BINDING_BEGIN( Randoms, SetSeed, "library", "Set the seed for the random number generator." )
{
    int seed = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "seed" );

    random->SetSeed( seed );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Randoms, SharedRandomFloat, "library", "Generate a shared random float. Whenever generating random numbers in shared/predicted code, these SharedRandom* functions have to be used. Each call should specify a unique 'sharedName' that seeds the random number generator. In loops make sure the additionalSeed is increased with the loop counter, otherwise it will always return the same random number" )
{
    const char *sharedName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sharedName" );
    float min = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "min" );
    float max = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "max" );
    int additionalSeed = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 0, "additionalSeed" );

    lua_pushnumber( L, SharedRandomFloat( sharedName, min, max, additionalSeed ) );

    return 1;
}
LUA_BINDING_END( "number", "The generated random float." )

LUA_BINDING_BEGIN( Randoms, SharedRandomInt, "library", "Generate a shared random integer. Whenever generating random numbers in shared/predicted code, these SharedRandom* functions have to be used. Each call should specify a unique 'sharedName' that seeds the random number generator. In loops make sure the additionalSeed is increased with the loop counter, otherwise it will always return the same random number" )
{
    const char *sharedName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sharedName" );
    int min = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "min" );
    int max = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "max" );
    int additionalSeed = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 0, "additionalSeed" );

    lua_pushinteger( L, SharedRandomInt( sharedName, min, max, additionalSeed ) );

    return 1;
}
LUA_BINDING_END( "integer", "The generated random integer." )

LUA_BINDING_BEGIN( Randoms, SharedRandomVector, "library", "Generate a shared random vector. Whenever generating random numbers in shared/predicted code, these SharedRandom* functions have to be used. Each call should specify a unique 'sharedName' that seeds the random number generator. In loops make sure the additionalSeed is increased with the loop counter, otherwise it will always return the same random number" )
{
    const char *sharedName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sharedName" );
    float min = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "min" );
    float max = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "max" );
    int additionalSeed = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 0, "additionalSeed" );

    lua_pushvector( L, SharedRandomVector( sharedName, min, max, additionalSeed ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The generated random vector." )

LUA_BINDING_BEGIN( Randoms, SharedRandomAngle, "library", "Generate a shared random angle. Whenever generating random numbers in shared/predicted code, these SharedRandom* functions have to be used. Each call should specify a unique 'sharedName' that seeds the random number generator. In loops make sure the additionalSeed is increased with the loop counter, otherwise it will always return the same random number" )
{
    const char *sharedName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sharedName" );
    float min = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "min" );
    float max = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "max" );
    int additionalSeed = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 0, "additionalSeed" );

    lua_pushangle( L, SharedRandomAngle( sharedName, min, max, additionalSeed ) );

    return 1;
}
LUA_BINDING_END( "Angle", "The generated random angle." )

#ifdef GAME_DLL

LUA_BINDING_BEGIN( Randoms, RandomBloodVector, "library", "Generate a random blood vector.", "server" )
{
    lua_pushvector( L, UTIL_RandomBloodVector() );
    return 1;
}
LUA_BINDING_END( "Vector", "The generated random blood vector." )

#endif // GAME_DLL

/*
** Open random library
*/
LUALIB_API int luaopen_random( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Randoms );
    return 1;
}
