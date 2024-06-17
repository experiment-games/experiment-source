//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "lgametrace.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"



static int luasrc_Util_VecToYaw (lua_State *L) {
  lua_pushnumber(L, UTIL_VecToYaw(luaL_checkvector(L, 1)));
  return 1;
}

static int luasrc_Util_VecToPitch (lua_State *L) {
  lua_pushnumber(L, UTIL_VecToPitch(luaL_checkvector(L, 1)));
  return 1;
}

static int luasrc_Util_YawToVector (lua_State *L) {
  Vector v = UTIL_YawToVector(luaL_checknumber(L, 1));
  lua_pushvector(L, v);
  return 1;
}

static int luasrc_SharedRandomFloat (lua_State *L) {
  lua_pushnumber(L, SharedRandomFloat(luaL_checkstring(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_optint(L, 4, 0)));
  return 1;
}

static int luasrc_SharedRandomInt (lua_State *L) {
  lua_pushinteger(L, SharedRandomInt(luaL_checkstring(L, 1), luaL_checkint(L, 2), luaL_checkint(L, 3), luaL_optint(L, 4, 0)));
  return 1;
}

static int luasrc_SharedRandomVector (lua_State *L) {
  lua_pushvector(L, SharedRandomVector(luaL_checkstring(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_optint(L, 4, 0)));
  return 1;
}

static int luasrc_SharedRandomAngle (lua_State *L) {
  lua_pushangle(L, SharedRandomAngle(luaL_checkstring(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_optint(L, 4, 0)));
  return 1;
}

static int luasrc_Util_TraceLine (lua_State *L) {
  UTIL_TraceLine(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkint(L, 3), lua_toentity(L, 4), luaL_checkint(L, 5), &luaL_checktrace(L, 6));
  return 0;
}

static int luasrc_Util_TraceHull (lua_State *L) {
  UTIL_TraceHull(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkvector(L, 3), luaL_checkvector(L, 4), luaL_checkint(L, 5), luaL_checkentity(L, 6), luaL_checkint(L, 7), &luaL_checktrace(L, 8));
  return 0;
}

static int luasrc_Util_TraceEntity (lua_State *L) {
  UTIL_TraceEntity(luaL_checkentity(L, 1), luaL_checkvector(L, 2), luaL_checkvector(L, 3), luaL_checkint(L, 4), luaL_checkentity(L, 5), luaL_checkint(L, 5), &luaL_checktrace(L, 6));
  return 0;
}

static int luasrc_Util_EntityHasMatchingRootParent (lua_State *L) {
  lua_pushboolean(L, UTIL_EntityHasMatchingRootParent(luaL_checkentity(L, 1), luaL_checkentity(L, 2)));
  return 1;
}

static int luasrc_Util_PointContents (lua_State *L) {
  lua_pushinteger(L, UTIL_PointContents(luaL_checkvector(L, 1)));
  return 1;
}

static int luasrc_Util_TraceModel (lua_State *L) {
  UTIL_TraceModel(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkvector(L, 3), luaL_checkvector(L, 4), luaL_checkentity(L, 5), luaL_checkint(L, 6), &luaL_checktrace(L, 7));
  return 0;
}

static int luasrc_Util_ParticleTracer (lua_State *L) {
  UTIL_ParticleTracer(luaL_checkstring(L, 1), luaL_checkvector(L, 2), luaL_checkvector(L, 3), luaL_optint(L, 4, 0), luaL_optint(L, 5, 0), luaL_optboolean(L, 6, 0));
  return 0;
}

static int luasrc_Util_Tracer (lua_State *L) {
  UTIL_Tracer(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_optint(L, 3, 0), luaL_optint(L, 4, -1), luaL_optnumber(L, 5, 0), luaL_optboolean(L, 6, 0), luaL_optstring(L, 7, 0), luaL_optint(L, 8, 0));
  return 0;
}

static int luasrc_Util_BloodDrips (lua_State *L) {
  UTIL_BloodDrips(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkint(L, 3), luaL_checkint(L, 4));
  return 0;
}

static int luasrc_Util_IsLowViolence (lua_State *L) {
  lua_pushboolean(L, UTIL_IsLowViolence());
  return 1;
}

static int luasrc_Util_ShouldShowBlood (lua_State *L) {
  lua_pushboolean(L, UTIL_ShouldShowBlood(luaL_checkint(L, 1)));
  return 1;
}

static int luasrc_Util_BloodImpact (lua_State *L) {
  UTIL_BloodImpact(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkint(L, 3), luaL_checkint(L, 4));
  return 0;
}

static int luasrc_Util_BloodDecalTrace (lua_State *L) {
  UTIL_BloodDecalTrace(&luaL_checktrace(L, 1), luaL_checkint(L, 2));
  return 0;
}

static int luasrc_Util_DecalTrace (lua_State *L) {
  UTIL_DecalTrace(&luaL_checktrace(L, 1), luaL_checkstring(L, 2));
  return 0;
}

static int luasrc_Util_IsSpaceEmpty (lua_State *L) {
  lua_pushboolean(L, UTIL_IsSpaceEmpty(luaL_checkentity(L, 1), luaL_checkvector(L, 2), luaL_checkvector(L, 3)));
  return 1;
}

static int luasrc_Util_PlayerByIndex (lua_State *L) {
  lua_pushplayer(L, UTIL_PlayerByIndex(luaL_checkint(L, 1)));
  return 1;
}


static const luaL_Reg util_funcs[] = {
  // {"UTIL_VecToYaw",  luasrc_Util_VecToYaw},
  {"VecToYaw",  luasrc_Util_VecToYaw},
  // {"UTIL_VecToPitch",  luasrc_Util_VecToPitch},
  {"VecToPitch",  luasrc_Util_VecToPitch},
  // {"UTIL_YawToVector",  luasrc_Util_YawToVector},
  {"YawToVector",  luasrc_Util_YawToVector},
  {"SharedRandomFloat",  luasrc_SharedRandomFloat},
  {"SharedRandomInt",  luasrc_SharedRandomInt},
  {"SharedRandomVector",  luasrc_SharedRandomVector},
  {"SharedRandomAngle",  luasrc_SharedRandomAngle},
  // {"UTIL_TraceLine",  luasrc_Util_TraceLine},
  {"TraceLine",  luasrc_Util_TraceLine},
  // {"UTIL_TraceHull",  luasrc_Util_TraceHull},
  {"TraceHull",  luasrc_Util_TraceHull},
  // {"UTIL_TraceEntity",  luasrc_Util_TraceEntity},
  {"TraceEntity",  luasrc_Util_TraceEntity},
  // {"UTIL_EntityHasMatchingRootParent",  luasrc_Util_EntityHasMatchingRootParent},
  {"EntityHasMatchingRootParent",  luasrc_Util_EntityHasMatchingRootParent},
  // {"UTIL_PointContents",  luasrc_Util_PointContents},
  {"PointContents",  luasrc_Util_PointContents},
  // {"UTIL_TraceModel",  luasrc_Util_TraceModel},
  {"TraceModel",  luasrc_Util_TraceModel},
  // {"UTIL_ParticleTracer",  luasrc_Util_ParticleTracer},
  {"ParticleTracer",  luasrc_Util_ParticleTracer},
  // {"UTIL_Tracer",  luasrc_Util_Tracer},
  {"Tracer",  luasrc_Util_Tracer},
  // {"UTIL_IsLowViolence",  luasrc_Util_IsLowViolence},
  {"IsLowViolence",  luasrc_Util_IsLowViolence},
  // {"UTIL_ShouldShowBlood",  luasrc_Util_ShouldShowBlood},
  {"ShouldShowBlood",  luasrc_Util_ShouldShowBlood},
  // {"UTIL_BloodDrips",  luasrc_Util_BloodDrips},
  {"BloodDrips",  luasrc_Util_BloodDrips},
  // {"UTIL_BloodImpact",  luasrc_Util_BloodImpact},
  {"BloodImpact",  luasrc_Util_BloodImpact},
  // {"UTIL_BloodDecalTrace",  luasrc_Util_BloodDecalTrace},
  {"BloodDecalTrace",  luasrc_Util_BloodDecalTrace},
  // {"UTIL_DecalTrace",  luasrc_Util_DecalTrace},
  {"DecalTrace",  luasrc_Util_DecalTrace},
  // {"UTIL_IsSpaceEmpty",  luasrc_Util_IsSpaceEmpty},
  {"IsSpaceEmpty",  luasrc_Util_IsSpaceEmpty},
  // {"UTIL_PlayerByIndex",  luasrc_Util_PlayerByIndex},
  {"PlayerByIndex",  luasrc_Util_PlayerByIndex},
  {NULL, NULL}
};


LUALIB_API int luaopen_Util_shared (lua_State *L) {
  // luaL_register(L, LUA_GNAME, util_funcs);
  luaL_register(L, LUA_UTILLIBNAME, util_funcs);
  return 1;
}

