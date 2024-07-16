//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL2.
//
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lhl2mp_player_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#include "luamanager.h"
#include "lhl2mp_player_shared.h"
#include "lbaseentity_shared.h"
#ifdef CLIENT_DLL
#include "lc_baseanimating.h"
#else
#include "lbaseanimating.h"
#endif
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int CHL2MP_Player_GetPlayerModelType( lua_State *L )
{
    lua_pushinteger( L, luaL_checkhl2mpplayer( L, 1 )->GetPlayerModelType() );
    return 1;
}

static const luaL_Reg CHL2MP_Playermeta[] = {
    { "GetPlayerModelType", CHL2MP_Player_GetPlayerModelType },
    { NULL, NULL } };

/*
** Open CHL2MP_Player object
*/
LUALIB_API int luaopen_CHL2MP_Player( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_HL2MPPLAYERLIBNAME );
    luaL_register( L, NULL, CHL2MP_Playermeta );
    return 1;
}
