//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Utility code.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "lgametrace.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int luasrc_CanCreateEntityClass (lua_State *L) {
  lua_pushboolean(L, CanCreateEntityClass(luaL_checkstring(L, 1)));
  return 1;
}

static int luasrc_ENTINDEX (lua_State *L) {
  lua_pushinteger(L, ENTINDEX(luaL_checkentity(L, 1)));
  return 1;
}

static int luasrc_INDEXENT (lua_State *L) {
  CBaseEntity *pEntity = CBaseEntity::Instance(INDEXENT(luaL_checkinteger(L, 1)));
  lua_pushentity(L, pEntity);
  return 1;
}

static int luasrc_FNullEnt (lua_State *L) {
  lua_pushboolean(L, FNullEnt(luaL_checkentity(L, 1)->edict()));
  return 1;
}

static int luasrc_FStrEq (lua_State *L) {
  lua_pushboolean(L, FStrEq(luaL_checkstring(L, 1), luaL_checkstring(L, 2)));
  return 1;
}

static int luasrc_Util_SetSize (lua_State *L) {
  UTIL_SetSize(luaL_checkentity(L, 1), luaL_checkvector(L, 2), luaL_checkvector(L, 3));
  return 0;
}

static int luasrc_Util_ClearTrace (lua_State *L) {
  UTIL_ClearTrace(luaL_checktrace(L, 1));
  return 0;
}

static int luasrc_Util_PrecacheDecal (lua_State *L) {
  lua_pushinteger(L, UTIL_PrecacheDecal(luaL_checkstring(L, 1), luaL_optboolean(L, 2, 0)));
  return 1;
}

static int luasrc_Util_GetSimulationInterval (lua_State *L) {
  lua_pushnumber(L, UTIL_GetSimulationInterval());
  return 1;
}

static int luasrc_Util_PlayerByIndex (lua_State *L) {
  lua_pushplayer(L, UTIL_PlayerByIndex(luaL_checkinteger(L, 1)));
  return 1;
}

static int luasrc_Util_GetLocalPlayer (lua_State *L) {
  lua_pushplayer(L, UTIL_GetLocalPlayer());
  return 1;
}

static int luasrc_Util_GetListenServerHost (lua_State *L) {
  lua_pushplayer(L, UTIL_GetListenServerHost());
  return 1;
}

static int luasrc_Util_PlayerByUserId (lua_State *L) {
  lua_pushplayer(L, UTIL_PlayerByUserId(luaL_checkinteger(L, 1)));
  return 1;
}

static int luasrc_Util_PlayerByName (lua_State *L) {
  lua_pushplayer(L, UTIL_PlayerByName(luaL_checkstring(L, 1)));
  return 1;
}

static int luasrc_Util_IsCommandIssuedByServerAdmin (lua_State *L) {
  lua_pushboolean(L, UTIL_IsCommandIssuedByServerAdmin());
  return 1;
}

static int luasrc_Util_EntityByIndex (lua_State *L) {
  lua_pushentity(L, UTIL_EntityByIndex(luaL_checkinteger(L, 1)));
  return 1;
}

static int luasrc_Util_GetPlayerConnectionInfo (lua_State *L) {
  int ping, packetloss;
  UTIL_GetPlayerConnectionInfo(luaL_checkinteger(L, 1), ping, packetloss);
  lua_pushinteger(L, ping);
  lua_pushinteger(L, packetloss);
  return 2;
}

static int luasrc_Util_ClientPVSIsExpanded (lua_State *L) {
  lua_pushboolean(L, UTIL_ClientPVSIsExpanded());
  return 1;
}

static int luasrc_Util_FindClientInPVS (lua_State *L) {
  lua_pushentity(L, UTIL_FindClientInPVS(luaL_checkvector(L, 1), luaL_checkvector(L, 2)));
  return 1;
}

#define MAX_ENTITYARRAY 1024

static int luasrc_Util_EntitiesInBox (lua_State *L) {
  CBaseEntity *pList[MAX_ENTITYARRAY];

  int count = UTIL_EntitiesInBox(pList, luaL_checkinteger(L, 1), luaL_checkvector(L, 2), luaL_checkvector(L, 3), luaL_checkinteger(L, 4));
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
  CBaseEntity *pList[MAX_ENTITYARRAY];

  int count = UTIL_EntitiesInSphere(pList, luaL_checkinteger(L, 1), luaL_checkvector(L, 2), luaL_checknumber(L, 3), luaL_checkinteger(L, 4));
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

static int luasrc_Util_Remove (lua_State *L) {
  UTIL_Remove(luaL_checkentity(L, 1));
  return 0;
}

static int luasrc_Util_DisableRemoveImmediate (lua_State *L) {
  UTIL_DisableRemoveImmediate();
  return 0;
}

static int luasrc_Util_EnableRemoveImmediate (lua_State *L) {
  UTIL_EnableRemoveImmediate();
  return 0;
}

static int luasrc_Util_RemoveImmediate (lua_State *L) {
  UTIL_RemoveImmediate(luaL_checkentity(L, 1));
  return 0;
}

static int luasrc_Util_DropToFloor (lua_State *L) {
  lua_pushinteger(L, UTIL_DropToFloor(luaL_checkentity(L, 1), luaL_checkinteger(L, 2), luaL_optentity(L, 3, NULL)));
  return 1;
}

static int luasrc_Util_SetOrigin (lua_State *L) {
  UTIL_SetOrigin(luaL_checkentity(L, 1), luaL_checkvector(L, 2), luaL_optboolean(L, 3, 0));
  return 0;
}

static int luasrc_Util_EmitAmbientSound (lua_State *L) {
  float duration;
  UTIL_EmitAmbientSound(luaL_checkinteger(L, 1), luaL_checkvector(L, 2), luaL_checkstring(L, 3), luaL_checknumber(L, 4), (soundlevel_t)luaL_checkinteger(L, 5), luaL_checkinteger(L, 6), luaL_checkinteger(L, 7), luaL_optnumber(L, 8, 0.0f), &duration);
  lua_pushnumber(L, duration);
  return 1;
}

static int luasrc_Util_ScreenShake (lua_State *L) {
  UTIL_ScreenShake(luaL_checkvector(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), (ShakeCommand_t)luaL_checkinteger(L, 6), luaL_optboolean(L, 7, 0));
  return 0;
}

static int luasrc_Util_ScreenShakeObject (lua_State *L) {
  UTIL_ScreenShakeObject(luaL_checkentity(L, 1), luaL_checkvector(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6), (ShakeCommand_t)luaL_checkinteger(L, 7), luaL_optboolean(L, 8, 0));
  return 0;
}

static int luasrc_Util_ViewPunch (lua_State *L) {
  UTIL_ViewPunch(luaL_checkvector(L, 1), luaL_checkangle(L, 2), luaL_checknumber(L, 3), luaL_checkboolean(L, 4));
  return 0;
}

static int luasrc_Util_ShowMessage (lua_State *L) {
  UTIL_ShowMessage(luaL_checkstring(L, 1), luaL_checkplayer(L, 2));
  return 0;
}

static int luasrc_Util_ShowMessageAll (lua_State *L) {
  UTIL_ShowMessageAll(luaL_checkstring(L, 1));
  return 0;
}

static int luasrc_Util_MuzzleFlash (lua_State *L) {
  UTIL_MuzzleFlash(luaL_checkvector(L, 1), luaL_checkangle(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));
  return 0;
}

static int luasrc_Util_PointOnLineNearestPoint (lua_State *L) {
  lua_pushvector(L, UTIL_PointOnLineNearestPoint(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkvector(L, 3), luaL_optboolean(L, 4, 0)));
  return 1;
}

static int luasrc_Util_EntityInSolid (lua_State *L) {
  lua_pushinteger(L, UTIL_EntityInSolid(luaL_checkentity(L, 1)));
  return 1;
}

static int luasrc_Util_BloodStream (lua_State *L) {
  UTIL_BloodStream(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));
  return 0;
}

static int luasrc_Util_BloodSpray (lua_State *L) {
  UTIL_BloodSpray(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 4));
  return 0;
}

static int luasrc_Util_RandomBloodVector (lua_State *L) {
  lua_pushvector(L, UTIL_RandomBloodVector());
  return 1;
}

static int luasrc_Util_ImpactTrace (lua_State *L) {
  UTIL_ImpactTrace(&luaL_checktrace(L, 1), luaL_checkinteger(L, 2), const_cast<char *>(luaL_optstring(L, 3, 0)));
  return 0;
}

static int luasrc_Util_PlayerDecalTrace (lua_State *L) {
  UTIL_PlayerDecalTrace(&luaL_checktrace(L, 1), luaL_checkinteger(L, 2));
  return 0;
}

static int luasrc_Util_Smoke (lua_State *L) {
  UTIL_Smoke(luaL_checkvector(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3));
  return 0;
}

static int luasrc_Util_AxisStringToPointDir (lua_State *L) {
  Vector start, dir;
  UTIL_AxisStringToPointDir(start, dir, luaL_checkstring(L, 1));
  lua_pushvector(L, start);
  lua_pushvector(L, dir);
  return 2;
}

static int luasrc_Util_AxisStringToPointPoint (lua_State *L) {
  Vector start, end;
  UTIL_AxisStringToPointPoint(start, end, luaL_checkstring(L, 1));
  lua_pushvector(L, start);
  lua_pushvector(L, end);
  return 2;
}

static int luasrc_Util_AxisStringToUnitDir (lua_State *L) {
  Vector dir;
  UTIL_AxisStringToUnitDir(dir, luaL_checkstring(L, 1));
  lua_pushvector(L, dir);
  return 1;
}

static int luasrc_Util_ClipPunchAngleOffset (lua_State *L) {
  UTIL_ClipPunchAngleOffset(luaL_checkangle(L, 1), luaL_checkplayer(L, 2)->m_Local.m_vecPunchAngle, luaL_checkangle(L, 3));
  return 0;
}

static int luasrc_Util_PredictedPosition (lua_State *L) {
  Vector vecPredictedPosition;
  UTIL_PredictedPosition(luaL_checkentity(L, 1), luaL_checknumber(L, 2), &vecPredictedPosition);
  lua_pushvector(L, vecPredictedPosition);
  return 1;
}

static int luasrc_Util_Beam (lua_State *L) {
  UTIL_Beam(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5), luaL_checkinteger(L, 6), luaL_checknumber(L, 7), luaL_checknumber(L, 8), luaL_checknumber(L, 9), luaL_checkinteger(L, 10), luaL_checknumber(L, 11), luaL_checkinteger(L, 12), luaL_checkinteger(L, 13), luaL_checkinteger(L, 14), luaL_checkinteger(L, 14), luaL_checknumber(L, 15));
  return 0;
}

static int luasrc_Util_IsValidEntity (lua_State *L) {
  lua_pushboolean(L, UTIL_IsValidEntity(luaL_checkentity(L, 1)));
  return 1;
}

static int luasrc_Util_TeamsMatch (lua_State *L) {
  lua_pushboolean(L, UTIL_TeamsMatch(luaL_checkstring(L, 1), luaL_checkstring(L, 2)));
  return 1;
}

static int luasrc_Util_SnapDirectionToAxis (lua_State *L) {
  UTIL_SnapDirectionToAxis(luaL_checkvector(L, 1), luaL_optnumber(L, 2, 0.002f));
  return 0;
}

static int luasrc_Util_PointAtEntity (lua_State *L) {
  lua_pushboolean(L, UTIL_PointAtEntity(luaL_checkentity(L, 1), luaL_checkentity(L, 2)));
  return 1;
}

static int luasrc_Util_TransferPoseParameters (lua_State *L) {
  lua_pushboolean(L, UTIL_TransferPoseParameters(luaL_checkentity(L, 1), luaL_checkentity(L, 2)));
  return 1;
}

static int luasrc_Util_WaterLevel (lua_State *L) {
  lua_pushnumber(L, UTIL_WaterLevel(luaL_checkvector(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
  return 1;
}

static int luasrc_Util_FindWaterSurface (lua_State *L) {
  lua_pushnumber(L, UTIL_FindWaterSurface(luaL_checkvector(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
  return 1;
}

static int luasrc_Util_Bubbles (lua_State *L) {
  UTIL_Bubbles(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkinteger(L, 3));
  return 0;
}

static int luasrc_Util_BubbleTrail (lua_State *L) {
  UTIL_BubbleTrail(luaL_checkvector(L, 1), luaL_checkvector(L, 2), luaL_checkinteger(L, 3));
  return 0;
}

static int luasrc_Util_PrecacheOther (lua_State *L) {
  UTIL_PrecacheOther(luaL_checkstring(L, 1), luaL_optstring(L, 2, 0));
  return 0;
}

static int luasrc_Util_ClientPrintAll (lua_State *L) {
  UTIL_ClientPrintAll(luaL_checkinteger(L, 1), luaL_checkstring(L, 2), luaL_optstring(L, 3, 0), luaL_optstring(L, 4, 0), luaL_optstring(L, 5, 0), luaL_optstring(L, 6, 0));
  return 0;
}

static int luasrc_Util_ValidateSoundName (lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  UTIL_ValidateSoundName((string_t &)name, luaL_checkstring(L, 2));
  lua_pushstring(L, name);
  return 1;
}

static int luasrc_ClientPrint (lua_State *L) {
  ClientPrint(luaL_checkplayer(L, 1), luaL_checkinteger(L, 2), luaL_checkstring(L, 3), luaL_optstring(L, 4, 0), luaL_optstring(L, 5, 0), luaL_optstring(L, 6, 0), luaL_optstring(L, 7, 0));
  return 0;
}

static int luasrc_Util_SayText (lua_State *L) {
  UTIL_SayText(luaL_checkstring(L, 1), luaL_checkplayer(L, 2));
  return 0;
}

static int luasrc_Util_SayTextAll (lua_State *L) {
  UTIL_SayTextAll(luaL_checkstring(L, 1), luaL_optplayer(L, 2, 0), luaL_optboolean(L, 3, 0));
  return 0;
}

static int luasrc_Util_GetCommandClientIndex (lua_State *L) {
  lua_pushinteger(L, UTIL_GetCommandClientIndex());
  return 1;
}

static int luasrc_Util_GetCommandClient (lua_State *L) {
  lua_pushplayer(L, UTIL_GetCommandClient());
  return 1;
}


static const luaL_Reg util_funcs[] = {
  {"CanCreateEntityClass",  luasrc_CanCreateEntityClass},
  {"ENTINDEX",  luasrc_ENTINDEX},
  {"INDEXENT",  luasrc_INDEXENT},
  {"FNullEnt",  luasrc_FNullEnt},
  {"FStrEq",  luasrc_FStrEq},
  // {"UTIL_SetSize",  luasrc_Util_SetSize},
  {"SetSize",  luasrc_Util_SetSize},
  // {"UTIL_ClearTrace",  luasrc_Util_ClearTrace},
  {"ClearTrace",  luasrc_Util_ClearTrace},
  // {"UTIL_PrecacheDecal",  luasrc_Util_PrecacheDecal},
  {"PrecacheDecal",  luasrc_Util_PrecacheDecal},
  // {"UTIL_GetSimulationInterval",  luasrc_Util_GetSimulationInterval},
  {"GetSimulationInterval",  luasrc_Util_GetSimulationInterval},
  // {"UTIL_PlayerByIndex",  luasrc_Util_PlayerByIndex},
  {"PlayerByIndex",  luasrc_Util_PlayerByIndex},
  // {"UTIL_GetLocalPlayer",  luasrc_Util_GetLocalPlayer},
  {"GetLocalPlayer",  luasrc_Util_GetLocalPlayer},
  // {"UTIL_GetListenServerHost",  luasrc_Util_GetListenServerHost},
  {"GetListenServerHost",  luasrc_Util_GetListenServerHost},
  // {"UTIL_PlayerByUserId",  luasrc_Util_PlayerByUserId},
  {"PlayerByUserId",  luasrc_Util_PlayerByUserId},
  // {"UTIL_PlayerByName",  luasrc_Util_PlayerByName},
  {"PlayerByName",  luasrc_Util_PlayerByName},
  // {"UTIL_IsCommandIssuedByServerAdmin",  luasrc_Util_IsCommandIssuedByServerAdmin},
  {"IsCommandIssuedByServerAdmin",  luasrc_Util_IsCommandIssuedByServerAdmin},
  // {"UTIL_EntityByIndex",  luasrc_Util_EntityByIndex},
  {"EntityByIndex",  luasrc_Util_EntityByIndex},
  // {"UTIL_GetPlayerConnectionInfo",  luasrc_Util_GetPlayerConnectionInfo},
  {"GetPlayerConnectionInfo",  luasrc_Util_GetPlayerConnectionInfo},
  // {"UTIL_ClientPVSIsExpanded",  luasrc_Util_ClientPVSIsExpanded},
  {"ClientPVSIsExpanded",  luasrc_Util_ClientPVSIsExpanded},
  // {"UTIL_FindClientInPVS",  luasrc_Util_FindClientInPVS},
  {"FindClientInPVS",  luasrc_Util_FindClientInPVS},
  // {"UTIL_EntitiesInBox",  luasrc_Util_EntitiesInBox},
  {"EntitiesInBox",  luasrc_Util_EntitiesInBox},
  // {"UTIL_EntitiesInSphere",  luasrc_Util_EntitiesInSphere},
  {"EntitiesInSphere",  luasrc_Util_EntitiesInSphere},
  // {"UTIL_Remove",  luasrc_Util_Remove},
  {"Remove",  luasrc_Util_Remove},
  // {"UTIL_DisableRemoveImmediate",  luasrc_Util_DisableRemoveImmediate},
  {"DisableRemoveImmediate",  luasrc_Util_DisableRemoveImmediate},
  // {"UTIL_EnableRemoveImmediate",  luasrc_Util_EnableRemoveImmediate},
  {"EnableRemoveImmediate",  luasrc_Util_EnableRemoveImmediate},
  // {"UTIL_RemoveImmediate",  luasrc_Util_RemoveImmediate},
  {"RemoveImmediate",  luasrc_Util_RemoveImmediate},
  // {"UTIL_DropToFloor",  luasrc_Util_DropToFloor},
  {"DropToFloor",  luasrc_Util_DropToFloor},
  // {"UTIL_SetOrigin",  luasrc_Util_SetOrigin},
  {"SetOrigin",  luasrc_Util_SetOrigin},
  // {"UTIL_EmitAmbientSound",  luasrc_Util_EmitAmbientSound},
  {"EmitAmbientSound",  luasrc_Util_EmitAmbientSound},
  // {"UTIL_ScreenShake",  luasrc_Util_ScreenShake},
  {"ScreenShake",  luasrc_Util_ScreenShake},
  // {"UTIL_ScreenShakeObject",  luasrc_Util_ScreenShakeObject},
  {"ScreenShakeObject",  luasrc_Util_ScreenShakeObject},
  // {"UTIL_ViewPunch",  luasrc_Util_ViewPunch},
  {"ViewPunch",  luasrc_Util_ViewPunch},
  // {"UTIL_ShowMessage",  luasrc_Util_ShowMessage},
  {"ShowMessage",  luasrc_Util_ShowMessage},
  // {"UTIL_ShowMessageAll",  luasrc_Util_ShowMessageAll},
  {"ShowMessageAll",  luasrc_Util_ShowMessageAll},
  // {"UTIL_MuzzleFlash",  luasrc_Util_MuzzleFlash},
  {"MuzzleFlash",  luasrc_Util_MuzzleFlash},
  // {"UTIL_PointOnLineNearestPoint",  luasrc_Util_PointOnLineNearestPoint},
  {"PointOnLineNearestPoint",  luasrc_Util_PointOnLineNearestPoint},
  // {"UTIL_EntityInSolid",  luasrc_Util_EntityInSolid},
  {"EntityInSolid",  luasrc_Util_EntityInSolid},
  // {"UTIL_BloodStream",  luasrc_Util_BloodStream},
  {"BloodStream",  luasrc_Util_BloodStream},
  // {"UTIL_BloodSpray",  luasrc_Util_BloodSpray},
  {"BloodSpray",  luasrc_Util_BloodSpray},
  // {"UTIL_RandomBloodVector",  luasrc_Util_RandomBloodVector},
  {"RandomBloodVector",  luasrc_Util_RandomBloodVector},
  // {"UTIL_ImpactTrace",  luasrc_Util_ImpactTrace},
  {"ImpactTrace",  luasrc_Util_ImpactTrace},
  // {"UTIL_PlayerDecalTrace",  luasrc_Util_PlayerDecalTrace},
  {"PlayerDecalTrace",  luasrc_Util_PlayerDecalTrace},
  // {"UTIL_Smoke",  luasrc_Util_Smoke},
  {"Smoke",  luasrc_Util_Smoke},
  // {"UTIL_AxisStringToPointDir",  luasrc_Util_AxisStringToPointDir},
  {"AxisStringToPointDir",  luasrc_Util_AxisStringToPointDir},
  // {"UTIL_AxisStringToPointPoint",  luasrc_Util_AxisStringToPointPoint},
  {"AxisStringToPointPoint",  luasrc_Util_AxisStringToPointPoint},
  // {"UTIL_AxisStringToUnitDir",  luasrc_Util_AxisStringToUnitDir},
  {"AxisStringToUnitDir",  luasrc_Util_AxisStringToUnitDir},
  // {"UTIL_ClipPunchAngleOffset",  luasrc_Util_ClipPunchAngleOffset},
  {"ClipPunchAngleOffset",  luasrc_Util_ClipPunchAngleOffset},
  // {"UTIL_PredictedPosition",  luasrc_Util_PredictedPosition},
  {"PredictedPosition",  luasrc_Util_PredictedPosition},
  // {"UTIL_Beam",  luasrc_Util_Beam},
  {"Beam",  luasrc_Util_Beam},
  // {"UTIL_IsValidEntity",  luasrc_Util_IsValidEntity},
  {"IsValidEntity",  luasrc_Util_IsValidEntity},
  // {"UTIL_TeamsMatch",  luasrc_Util_TeamsMatch},
  {"TeamsMatch",  luasrc_Util_TeamsMatch},
  // {"UTIL_SnapDirectionToAxis",  luasrc_Util_SnapDirectionToAxis},
  {"SnapDirectionToAxis",  luasrc_Util_SnapDirectionToAxis},
  // {"UTIL_PointAtEntity",  luasrc_Util_PointAtEntity},
  {"PointAtEntity",  luasrc_Util_PointAtEntity},
  // {"UTIL_TransferPoseParameters",  luasrc_Util_TransferPoseParameters},
  {"TransferPoseParameters",  luasrc_Util_TransferPoseParameters},
  // {"UTIL_WaterLevel",  luasrc_Util_WaterLevel},
  {"WaterLevel",  luasrc_Util_WaterLevel},
  // {"UTIL_FindWaterSurface",  luasrc_Util_FindWaterSurface},
  {"FindWaterSurface",  luasrc_Util_FindWaterSurface},
  // {"UTIL_Bubbles",  luasrc_Util_Bubbles},
  {"Bubbles",  luasrc_Util_Bubbles},
  // {"UTIL_BubbleTrail",  luasrc_Util_BubbleTrail},
  {"BubbleTrail",  luasrc_Util_BubbleTrail},
  // {"UTIL_PrecacheOther",  luasrc_Util_PrecacheOther},
  {"PrecacheOther",  luasrc_Util_PrecacheOther},
  // {"UTIL_ClientPrintAll",  luasrc_Util_ClientPrintAll},
  {"ClientPrintAll",  luasrc_Util_ClientPrintAll},
  // {"UTIL_ValidateSoundName",  luasrc_Util_ValidateSoundName},
  {"ValidateSoundName",  luasrc_Util_ValidateSoundName},
  {"ClientPrint",  luasrc_ClientPrint},
  // {"UTIL_SayText",  luasrc_Util_SayText},
  {"SayText",  luasrc_Util_SayText},
  // {"UTIL_SayTextAll",  luasrc_Util_SayTextAll},
  {"SayTextAll",  luasrc_Util_SayTextAll},
  // {"UTIL_GetCommandClientIndex",  luasrc_Util_GetCommandClientIndex},
  {"GetCommandClientIndex",  luasrc_Util_GetCommandClientIndex},
  // {"UTIL_GetCommandClient",  luasrc_Util_GetCommandClient},
  {"GetCommandClient",  luasrc_Util_GetCommandClient},
  {NULL, NULL}
};


LUALIB_API int luaopen_Util (lua_State *L) {
  // luaL_register(L, LUA_GNAME, util_funcs);
  luaL_register(L, LUA_UTILLIBNAME, util_funcs);
  lua_pop(L, 1);
  return 1;
}

