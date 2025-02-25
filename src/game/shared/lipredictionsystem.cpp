#include "cbase.h"
#include "ipredictionsystem.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( PredictionSystems )

LUA_BINDING_BEGIN( PredictionSystems, SuppressEvents, "library", "Suppresses events." )
{
    IPredictionSystem::SuppressEvents( LUA_BINDING_ARGUMENT( lua_toboolean, 1, "shouldSuppress" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PredictionSystems, SuppressHostEvents, "library", "Suppresses host events." )
{
    IPredictionSystem::SuppressHostEvents( LUA_BINDING_ARGUMENT( lua_toentity, 1, "host" ) );
    return 0;
}
LUA_BINDING_END()

/*
** Open IPredictionSystem library
*/
LUALIB_API int luaopen_PredictionSystems( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( PredictionSystems );
    return 1;
}
