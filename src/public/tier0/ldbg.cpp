//===== Copyright © 1996-2005, Valve Corporation, All rights reserved.
//========//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "lColor.h"
#include <luamanager.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int dbg_SpewActivate( lua_State *L )
{
    SpewActivate( luaL_checkstring( L, 1 ), luaL_checknumber( L, 2 ) );
    return 0;
}

static int dbg_IsSpewActive( lua_State *L )
{
    lua_pushboolean( L,
                     IsSpewActive( luaL_checkstring( L, 1 ), luaL_checknumber( L, 2 ) ) );
    return 1;
}

static int dbg_Msg( lua_State *L )
{
    Msg( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_DMsg( lua_State *L )
{
    DMsg( luaL_checkstring( L, 1 ), luaL_checknumber( L, 2 ), luaL_checkstring( L, 3 ) );
    return 0;
}

static int dbg_Warning( lua_State *L )
{
    Warning( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_DWarning( lua_State *L )
{
    DWarning( luaL_checkstring( L, 1 ), luaL_checknumber( L, 2 ), luaL_checkstring( L, 3 ) );
    return 0;
}

static int dbg_Log( lua_State *L )
{
    Log( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_DLog( lua_State *L )
{
    DLog( luaL_checkstring( L, 1 ), luaL_checknumber( L, 2 ), luaL_checkstring( L, 3 ) );
    return 0;
}

static int dbg_Error( lua_State *L )
{
    Error( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_DevMsg( lua_State *L )
{
    DevMsg( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_DevWarning( lua_State *L )
{
    DevWarning( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_DevLog( lua_State *L )
{
    DevLog( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_ConColorMsg( lua_State *L )
{
    ConColorMsg( luaL_checkcolor( L, 1 ), luaL_checkstring( L, 2 ) );
    return 0;
}

static int dbg_ConMsg( lua_State *L )
{
    ConMsg( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_ConWarning( lua_State *L )
{
    ConWarning( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_ConLog( lua_State *L )
{
    ConLog( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_ConDColorMsg( lua_State *L )
{
    ConDColorMsg( luaL_checkcolor( L, 1 ), luaL_checkstring( L, 2 ) );
    return 0;
}

static int dbg_ConDMsg( lua_State *L )
{
    ConDMsg( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_ConDWarning( lua_State *L )
{
    ConDWarning( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_ConDLog( lua_State *L )
{
    ConDLog( luaL_checkstring( L, 1 ) );
    return 0;
}

static int dbg_NetMsg( lua_State *L )
{
    NetMsg( luaL_checknumber( L, 1 ), luaL_checkstring( L, 2 ) );
    return 0;
}

static int dbg_NetWarning( lua_State *L )
{
    NetWarning( luaL_checknumber( L, 1 ), luaL_checkstring( L, 2 ) );
    return 0;
}

static int dbg_NetLog( lua_State *L )
{
    NetLog( luaL_checknumber( L, 1 ), luaL_checkstring( L, 2 ) );
    return 0;
}

static int dbg_GetRealmColor( lua_State *L )
{
    lua_pushcolor( L, REALM_COLOR );
    return 1;
}

static const luaL_Reg dbg_funcs[] = { { "SpewActivate", dbg_SpewActivate },
                                      { "IsSpewActive", dbg_IsSpewActive },
                                      { "Msg", dbg_Msg },
                                      { "DMsg", dbg_DMsg },
                                      { "Warning", dbg_Warning },
                                      { "DWarning", dbg_DWarning },
                                      { "Log", dbg_Log },
                                      { "DLog", dbg_DLog },
                                      { "Error", dbg_Error },
                                      { "DevMsg", dbg_DevMsg },
                                      { "DevWarning", dbg_DevWarning },
                                      { "DevLog", dbg_DevLog },
                                      { "ConColorMsg", dbg_ConColorMsg },
                                      { "ConMsg", dbg_ConMsg },
                                      { "ConWarning", dbg_ConWarning },
                                      { "ConLog", dbg_ConLog },
                                      { "ConDColorMsg", dbg_ConDColorMsg },
                                      { "ConDMsg", dbg_ConDMsg },
                                      { "ConDWarning", dbg_ConDWarning },
                                      { "ConDLog", dbg_ConDLog },
                                      { "NetMsg", dbg_NetMsg },
                                      { "NetWarning", dbg_NetWarning },
                                      { "NetLog", dbg_NetLog },
                                      { "GetRealmColor", dbg_GetRealmColor },
                                      { NULL, NULL } };

LUALIB_API int luaopen_dbg( lua_State *L )
{
    // luaL_register(L, LUA_DBGLIBNAME, dbg_funcs);
    // Extend the default debug library instead
    lua_getglobal( L, LUA_DBLIBNAME );
    luaL_register( L, NULL, dbg_funcs );
    return 1;
}
