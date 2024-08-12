#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int gpGlobals_absoluteframetime( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->absoluteframetime );
    return 1;
}

static int gpGlobals_curtime( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->curtime );
    return 1;
}

static int gpGlobals_SystemTime( lua_State *L )
{
    lua_pushnumber( L, Plat_FloatTime() );
    return 1;
}

static int gpGlobals_framecount( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->framecount );
    return 1;
}

static int gpGlobals_frametime( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->frametime );
    return 1;
}

static int gpGlobals_interval_per_tick( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->interval_per_tick );
    return 1;
}

static int gpGlobals_IsClient( lua_State *L )
{
    lua_pushboolean( L, gpGlobals->IsClient() );
    return 1;
}

static int gpGlobals_maxClients( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->maxClients );
    return 1;
}

static int gpGlobals_network_protocol( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->network_protocol );
    return 1;
}

static int gpGlobals_realtime( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->realtime );
    return 1;
}

static int gpGlobals_simTicksThisFrame( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->simTicksThisFrame );
    return 1;
}

static int gpGlobals_tickcount( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->tickcount );
    return 1;
}

static const luaL_Reg gpGlobalslib[] = {
    { "AbsoluteFrameTime", gpGlobals_absoluteframetime },
    { "CurrentTime", gpGlobals_curtime },
    { "SystemTime", gpGlobals_SystemTime },
    { "FrameCount", gpGlobals_framecount },
    { "FrameTime", gpGlobals_frametime },
    { "IntervalPerTick", gpGlobals_interval_per_tick },
    { "IsClient", gpGlobals_IsClient },
    { "MaxClients", gpGlobals_maxClients },
    { "NetworkProtocol", gpGlobals_network_protocol },
    { "RealTime", gpGlobals_realtime },
    { "SimulatedTicksThisFrame", gpGlobals_simTicksThisFrame },
    { "TickCount", gpGlobals_tickcount },
    { NULL, NULL } };

/*
** Open gpGlobals library
*/
LUALIB_API int luaopen_gpGlobals( lua_State *L )
{
    luaL_register( L, LUA_GLOBALSLIBNAME, gpGlobalslib );
    return 1;
}
