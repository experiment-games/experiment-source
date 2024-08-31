#include "cbase.h"
#ifdef CLIENT_DLL
#include "prediction.h"
#endif
#include "luamanager.h"
#include "luasrclib.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Predictions )

LUA_BINDING_BEGIN( Predictions, GetPredictionPlayer, "library", "Get prediction player." )
{
    CBasePlayer::PushLuaInstanceSafe( L, CBaseEntity::GetPredictionPlayer() );
    return 1;
}
LUA_BINDING_END( "Player", "The prediction player." )

LUA_BINDING_BEGIN( Predictions, GetPredictionRandomSeed, "library", "Get prediction random seed." )
{
    lua_pushinteger( L, CBaseEntity::GetPredictionRandomSeed() );
    return 1;
}
LUA_BINDING_END( "integer", "The prediction random seed." )

/*
** Open prediction library
*/
LUALIB_API int luaopen_PredictionShared( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Predictions );
    return 1;
}
