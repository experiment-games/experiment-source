/*
    Original code by Team Sandbox:
        Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

    Modified for Experiment.
*/

#include "cbase.h"
#include "lua.hpp"

#include "luasrclib.h"
#include "lauxlib.h"

typedef struct luaL_RegForState
{
    const char *name;
    lua_CFunction func;
    bool availableInGameUI;
} luaL_RegForState;

static const luaL_RegForState luasrclibs[] = {
    { LUA_BASEANIMATINGLIBNAME, luaopen_CBaseAnimating, false },
    { LUA_BASECOMBATWEAPONLIBNAME, luaopen_CBaseCombatWeapon, false },
    { LUA_BASEENTITYLIBNAME, luaopen_CBaseEntity, false },
    { LUA_BASEENTITYLIBNAME, luaopen_CBaseEntity_shared, false },
    { LUA_BASEPLAYERLIBNAME, luaopen_CBasePlayer, false },
    { LUA_BASEPLAYERLIBNAME, luaopen_CBasePlayer_shared, false },
    { LUA_EFFECTDATALIBNAME, luaopen_CEffectData, false },
    { LUA_GAMETRACELIBNAME, luaopen_CGameTrace, false },
#ifndef CLIENT_DLL
    { LUA_HL2MPPLAYERLIBNAME, luaopen_CHL2MP_Player, false },
#endif
    { LUA_HL2MPPLAYERLIBNAME, luaopen_CHL2MP_Player_shared, false },
    { LUA_COLORLIBNAME, luaopen_Color, true },
    { LUA_CONCOMMANDLIBNAME, luaopen_ConCommand, true },
    { LUA_CONTENTSLIBNAME, luaopen_CONTENTS, true },
    { LUA_CONVARLIBNAME, luaopen_ConVar, true },
    { LUA_PASFILTERLIBNAME, luaopen_CPASFilter, false },
    { LUA_RECIPIENTFILTERLIBNAME, luaopen_CRecipientFilter },
    { LUA_TAKEDAMAGEINFOLIBNAME, luaopen_CTakeDamageInfo },
    { LUA_CVARLIBNAME, luaopen_cvars, false },
    { LUA_DBGLIBNAME, luaopen_dbg, true },
    { LUA_DEBUGOVERLAYLIBNAME, luaopen_debugoverlay, false },
    { LUA_ENGINELIBNAME, luaopen_engine, true },
#ifdef CLIENT_DLL
    // FIXME: obsolete? should be passing VPANELs, but passes Panel instead,
    // which always ends up being invalid (we can't access them by pointer)
    { LUA_ENGINEVGUILIBNAME, luaopen_enginevgui, true },
#endif
    { LUA_FCVARLIBNAME, luaopen_FCVAR, true },
    { LUA_FILESYSTEMLIBNAME, luaopen_filesystem, true },
#ifdef CLIENT_DLL
    { LUA_FONTFLAGLIBNAME, luaopen_FONTFLAG, true },
#endif
#ifndef CLIENT_DLL
    { LUA_ENTLISTLIBNAME, luaopen_gEntList, false },
#endif
    { LUA_GLOBALSLIBNAME, luaopen_gpGlobals, true },
#ifdef CLIENT_DLL
    { LUA_CLIENTSHADOWMGRLIBNAME, luaopen_g_pClientShadowMgr, false },
    { LUA_FONTLIBNAME, luaopen_HFont, true },
    { LUA_HSCHEMELIBNAME, luaopen_HScheme, true },
#endif
    { LUA_MATERIALLIBNAME, luaopen_IMaterial, true },
    { LUA_MOVEHELPERLIBNAME, luaopen_IMoveHelper, false },
    { LUA_INLIBNAME, luaopen_IN },
#ifndef CLIENT_DLL
    { LUA_NETCHANNELINFOLIBNAME, luaopen_INetChannelInfo, false },
#endif
    { LUA_INETWORKSTRINGTABLELIBNAME, luaopen_INetworkStringTable, false },
#ifdef CLIENT_DLL
    { LUA_INPUTLIBNAME, luaopen_input, true },
#endif
    { LUA_PHYSICSOBJECTLIBNAME, luaopen_IPhysicsObject, false },
    { LUA_PHYSICSSURFACEPROPSLIBNAME, luaopen_IPhysicsSurfaceProps, false },
    { LUA_PREDICTIONSYSTEMLIBNAME, luaopen_IPredictionSystem, false },
#ifdef CLIENT_DLL
    { LUA_ISCHEMELIBNAME, luaopen_IScheme, true },
#endif
    { LUA_STEAMFRIENDSLIBNAME, luaopen_ISteamFriends, true },
    { LUA_KEYVALUESLIBNAME, luaopen_KeyValues, true },
    { LUA_MASKLIBNAME, luaopen_MASK, true },
    { LUA_MATHLIBLIBNAME, luaopen_mathlib, true },
    { LUA_MATRIXLIBNAME, luaopen_matrix3x4_t, true },
    { LUA_NETWORKSTRINGTABLELIBNAME, luaopen_networkstringtable, true },
    { LUA_PHYSENVLIBNAME, luaopen_physenv, false },
#ifdef CLIENT_DLL
    { LUA_PREDICTIONLIBNAME, luaopen_prediction, false },
#endif
    { LUA_QANGLELIBNAME, luaopen_QAngle, true },
    { LUA_RANDOMLIBNAME, luaopen_random, true },
#ifndef CLIENT_DLL
    { LUA_RESOURCESLIBNAME, luaopen_resources, false },
#endif
#ifdef CLIENT_DLL
    { LUA_SCHEMELIBNAME, luaopen_scheme, true },
#endif
    { LUA_STEAMAPICONTEXTLIBNAME, luaopen_steamapicontext, true },
    { LUA_SURFLIBNAME, luaopen_SURF, true },
#ifdef CLIENT_DLL
    { LUA_SURFACELIBNAME, luaopen_surface, true },
#endif
    { LUA_UTILLIBNAME, luaopen_Util, false },
    { LUA_UTILLIBNAME, luaopen_Util_shared, false },
    { LUA_VECTORLIBNAME, luaopen_Vector, true },
#ifdef CLIENT_DLL
    { LUA_VGUILIBNAME, luaopen_vgui, true },
#endif
    { LUA_VMATRIXLIBNAME, luaopen_VMatrix, true },

#ifdef CLIENT_DLL
    { LUA_BFREADLIBNAME, luaopen_bf_read, false },
#endif
    { LUA_UMSGLIBNAME, luaopen_umsg, false },

    { LUA_PARTICLESYSTEMLIBNAME, luaopen_ParticleSystem, false},
    { LUA_SYSTEMLIBNAME, luaopen_system, true },
    { LUA_RENDERLIBNAME, luaopen_render, true },
    { LUA_ITEXTURELIBNAME, luaopen_ITexture, true },
    { NULL, NULL, NULL } };

LUALIB_API void luasrc_openlibs( lua_State *L, bool isStateGameUI )
{
    const luaL_RegForState *lib = luasrclibs;
    for ( ; lib->func; lib++ )
    {
        if ( isStateGameUI && !lib->availableInGameUI )
            continue;

        lua_pushcfunction( L, lib->func );
        lua_pushstring( L, lib->name );
        lua_call( L, 1, 0 );
    }
}
