#include "cbase.h"
#include "c_baseplayer.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseplayer_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int CBasePlayer_GetLocalPlayer( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, CBasePlayer::GetLocalPlayer() );
    return 1;
}

static const luaL_Reg CBasePlayermeta[] = {
    { "GetLocalPlayer", CBasePlayer_GetLocalPlayer },
    { NULL, NULL } };

/*
** Open CBasePlayer object
*/
LUALIB_API int luaopen_CBasePlayer( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEPLAYERLIBNAME );
    luaL_register( L, NULL, CBasePlayermeta );
    return 1;
}
