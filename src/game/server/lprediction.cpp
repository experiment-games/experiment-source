#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Predictions )

LUA_BINDING_BEGIN( Predictions, IsFirstTimePredicted, "library", "Always returns true on the server. TODO: what should it do?" )
{
    // TODO: What should this return on the server?
    lua_pushboolean( L, true );
    return 1;
}
LUA_BINDING_END( "boolean", "Always true on the server." )

/*
** Open prediction library
*/
LUALIB_API int luaopen_Predictions( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Predictions );
    return 1;
}
