#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Predictions )

// TODO: These two functions also exist in lc_prediction. Make a shared place for them.
LUA_BINDING_BEGIN( Predictions, GetPredictionPlayer, "library", "Get prediction player." )
{
    CBaseEntity::PushLuaInstanceSafe( L, CBaseEntity::GetPredictionPlayer() );
    return 1;
}
LUA_BINDING_END( "Entity", "The prediction player." )

LUA_BINDING_BEGIN( Predictions, GetPredictionRandomSeed, "library", "Get prediction random seed." )
{
    lua_pushinteger( L, CBaseEntity::GetPredictionRandomSeed() );
    return 1;
}
LUA_BINDING_END( "integer", "The prediction random seed." )

/*
** Open prediction library
*/
LUALIB_API int luaopen_prediction( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Predictions );
    return 1;
}
