//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL2.
//
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lexperiment_player_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#include "luamanager.h"
#include "lexperiment_player_shared.h"
#include "lbaseentity_shared.h"
#ifdef CLIENT_DLL
#include "lc_baseanimating.h"
#else
#include "lbaseanimating.h"
#endif
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int CExperiment_Player_GetPlayerModelType( lua_State *L )
{
    lua_pushinteger( L, luaL_checkexperimentplayer( L, 1 )->GetPlayerModelType() );
    return 1;
}

static const luaL_Reg CExperiment_Playermeta[] = {
    { "GetPlayerModelType", CExperiment_Player_GetPlayerModelType },
    { NULL, NULL } };

/*
** Open CExperiment_Player object
*/
LUALIB_API int luaopen_CExperiment_Player( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_EXPERIMENTPLAYERLIBNAME );
    luaL_register( L, NULL, CExperiment_Playermeta );
    return 1;
}
