//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "lbaseplayer_shared.h"
#include "lbaseentity_shared.h"
#include "lgametrace.h"
#include "mathlib/lvector.h"
#include "vgui/lvgui.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"



static int luasrc_IsPlayerIndex (lua_State *L) {
  lua_pushboolean(L, IsPlayerIndex(luaL_checkint(L, 1)));
  return 1;
}

static int luasrc_GetLocalPlayerIndex (lua_State *L) {
  lua_pushinteger(L, GetLocalPlayerIndex());
  return 1;
}

static int luasrc_IsLocalPlayerSpectator (lua_State *L) {
  lua_pushboolean(L, IsLocalPlayerSpectator());
  return 1;
}

static int luasrc_GetSpectatorMode (lua_State *L) {
  lua_pushinteger(L, GetSpectatorMode());
  return 1;
}

static int luasrc_GetSpectatorTarget (lua_State *L) {
  lua_pushinteger(L, GetSpectatorTarget());
  return 1;
}

static int luasrc_GetLocalPlayerTeam (lua_State *L) {
  lua_pushinteger(L, GetLocalPlayerTeam());
  return 1;
}

static int luasrc_NormalizeAngles (lua_State *L) {
  NormalizeAngles(luaL_checkangle(L, 1));
  return 0;
}

static int luasrc_InterpolateAngles (lua_State *L) {
  InterpolateAngles(luaL_checkangle(L, 1), luaL_checkangle(L, 2), luaL_checkangle(L, 3), luaL_checknumber(L, 4));
  return 0;
}

static int luasrc_ScreenHeight (lua_State *L) {
  lua_pushinteger(L, ScreenHeight());
  return 1;
}

static int luasrc_ScreenWidth (lua_State *L) {
  lua_pushinteger(L, ScreenWidth());
  return 1;
}

static int luasrc_Util_ComputeStringWidth (lua_State *L) {
  lua_pushinteger(L, UTIL_ComputeStringWidth(*(vgui::HFont *)luaL_checkfont(L, 1), luaL_checkstring(L, 2)));
  return 1;
}

static int luasrc_Util_AngleDiff (lua_State *L) {
  lua_pushnumber(L, UTIL_AngleDiff(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

static int luasrc_Util_Bubbles (lua_State *L) {
  UTIL_Bubbles(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkint(L, 3));
  return 0;
}

static int luasrc_Util_ScreenShake (lua_State *L) {
  UTIL_ScreenShake(luaL_checkvector(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), (ShakeCommand_t)luaL_checkint(L, 6), luaL_optboolean(L, 7, 0));
  return 0;
}

static int luasrc_Util_Tracer (lua_State *L) {
  UTIL_Tracer(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkint(L, 3), luaL_checkint(L, 4), luaL_checknumber(L, 5), luaL_checkboolean(L, 6), luaL_checkstring(L, 7));
  return 0;
}

static int luasrc_Util_Smoke (lua_State *L) {
  UTIL_Smoke(luaL_checkvector(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3));
  return 0;
}

static int luasrc_Util_ImpactTrace (lua_State *L) {
  UTIL_ImpactTrace(&luaL_checktrace(L, 1), luaL_checkint(L, 2), (char *)luaL_optstring(L, 3, 0));
  return 0;
}

static int luasrc_Util_SetOrigin (lua_State *L) {
  UTIL_SetOrigin(luaL_checkentity(L, 1), luaL_checkvector(L, 2));
  return 0;
}

static int luasrc_Util_PrecacheOther (lua_State *L) {
  UTIL_PrecacheOther(luaL_checkstring(L, 1));
  return 0;
}

static int luasrc_ClientPrint (lua_State *L) {
  ClientPrint(luaL_checkplayer(L, 1), luaL_checkint(L, 2), luaL_checkstring(L, 3), luaL_optstring(L, 4, 0), luaL_optstring(L, 5, 0), luaL_optstring(L, 6, 0), luaL_optstring(L, 7, 0));
  return 0;
}

#define MAX_ENTITYARRAY 1024

static int luasrc_Util_EntitiesInBox (lua_State *L) {
  C_BaseEntity *pList[MAX_ENTITYARRAY];

  int count = UTIL_EntitiesInBox(pList, luaL_checkint(L, 1), luaL_checkvector(L, 2), luaL_checkvector(L, 3), luaL_checkint(L, 4), luaL_optint(L, 5, PARTITION_CLIENT_NON_STATIC_EDICTS));
  lua_pushinteger(L, count);
  lua_newtable(L);
  for( int i = 0 ; i < count ; i++ )
  {
	  lua_pushinteger(L, i);
	  lua_pushentity(L, pList[ i ]);
	  lua_settable(L, -3);
  }
  return 2;
}

static int luasrc_Util_EntitiesInSphere (lua_State *L) {
  C_BaseEntity *pList[MAX_ENTITYARRAY];

  int count = UTIL_EntitiesInSphere(pList, luaL_checkint(L, 1), luaL_checkvector(L, 2), luaL_checknumber(L, 3), luaL_checkint(L, 4), luaL_optint(L, 5, PARTITION_CLIENT_NON_STATIC_EDICTS));
  lua_pushinteger(L, count);
  lua_newtable(L);
  for( int i = 0 ; i < count ; i++ )
  {
	  lua_pushinteger(L, i);
	  lua_pushentity(L, pList[ i ]);
	  lua_settable(L, -3);
  }
  return 2;
}

static int luasrc_Util_SafeName (lua_State *L) {
  lua_pushstring(L, UTIL_SafeName(luaL_checkstring(L, 1)));
  return 1;
}

static int luasrc_Util_BoundToWorldSize (lua_State *L) {
  UTIL_BoundToWorldSize(&luaL_checkvector(L, 1));
  return 0;
}


static const luaL_Reg util_funcs[] = {
  {"IsPlayerIndex",  luasrc_IsPlayerIndex},
  {"GetLocalPlayerIndex",  luasrc_GetLocalPlayerIndex},
  {"IsLocalPlayerSpectator",  luasrc_IsLocalPlayerSpectator},
  {"GetSpectatorMode",  luasrc_GetSpectatorMode},
  {"GetSpectatorTarget",  luasrc_GetSpectatorTarget},
  {"NormalizeAngles",  luasrc_NormalizeAngles},
  {"InterpolateAngles",  luasrc_InterpolateAngles},
  {"ScreenHeight",  luasrc_ScreenHeight},
  {"ScreenWidth",  luasrc_ScreenWidth},
  {"ComputeStringWidth", luasrc_Util_ComputeStringWidth},
  {"AngleDiff",  luasrc_Util_AngleDiff},
  {"Bubbles",  luasrc_Util_Bubbles},
  {"ScreenShake",  luasrc_Util_ScreenShake},
  {"Tracer",  luasrc_Util_Tracer},
  {"Smoke",  luasrc_Util_Smoke},
  {"ImpactTrace",  luasrc_Util_ImpactTrace},
  {"SetOrigin",  luasrc_Util_SetOrigin},
  {"PrecacheOther",  luasrc_Util_PrecacheOther},
  {"ClientPrint",  luasrc_ClientPrint},
  {"EntitiesInBox",  luasrc_Util_EntitiesInBox},
  {"EntitiesInSphere",  luasrc_Util_EntitiesInSphere},
  {"SafeName",  luasrc_Util_SafeName},
  {"BoundToWorldSize",  luasrc_Util_BoundToWorldSize},
  {NULL, NULL}
};


LUALIB_API int luaopen_Util (lua_State *L) {
  luaL_register(L, LUA_UTILLIBNAME, util_funcs);
  return 1;
}

