#include "cbase.h"
#include "ammodef.h"
#include "luamanager.h"
#include "lshareddefs.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_FireBulletsInfo_t lua_tofirebulletsinfo( lua_State *L, int argIndex, lua_CBaseEntity *self )
{
    // Let's ensure that the bullet info is a table
    luaL_checktype( L, argIndex, LUA_TTABLE );

    FireBulletsInfo_t info;

    // Internally this field seems to be used for game stats
    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "IsPrimaryAttack", "m_bPrimaryAttack" );
    info.m_bPrimaryAttack = lua_isnil( L, -1 ) ? true : luaL_checkboolean( L, -1 );
    lua_pop( L, 1 );  // pop the IsPrimaryAttack value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Force", "m_flDamageForceScale" );
    info.m_flDamageForceScale = lua_isnil( L, -1 ) ? 1.0f : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Force value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Distance", "m_flDistance" );
    info.m_flDistance = lua_isnil( L, -1 ) ? 56756.0f : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Distance value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "AmmoType", "m_iAmmoType" );
    if ( lua_isnumber( L, -1 ) )
        info.m_iAmmoType = luaL_checknumber( L, -1 );
    else if ( lua_isstring( L, -1 ) )
    {
        const char *ammoName = luaL_checkstring( L, -1 );
        info.m_iAmmoType = GetAmmoDef()->Index( ammoName );
    }
    else
        info.m_iAmmoType = 0;
    lua_pop( L, 1 );  // pop the AmmoType value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Damage", "m_flDamage" );
    info.m_flDamage = lua_isnil( L, -1 ) ? 1.0f : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Damage value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "PlayerDamage", "m_iPlayerDamage" );
    info.m_iPlayerDamage = lua_isnil( L, -1 ) ? info.m_flDamage : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the PlayerDamage value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "AmountOfBullets", "Num" );
    info.m_iShots = lua_isnil( L, -1 ) ? 1 : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the AmountOfBullets value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "TracerFrequency", "Tracer" );
    info.m_iTracerFreq = lua_isnil( L, -1 ) ? 1 : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Tracer value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "TracerName", "m_pszTracerType" );
    info.m_pszTracerType = lua_isnil( L, -1 ) ? NULL : strdup( luaL_checkstring( L, -1 ) );
    lua_pop( L, 1 );  // pop the TracerName value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Flags", "m_nFlags" );
    info.m_nFlags = lua_isnil( L, -1 ) ? 0 : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Flags value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "IgnoreEntity", "m_pAdditionalIgnoreEnt" );
    info.m_pAdditionalIgnoreEnt = lua_isnil( L, -1 ) ? NULL : lua_toentity( L, -1 );
    lua_pop( L, 1 );  // pop the IgnoreEntity value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Attacker", "m_pAttacker" );
    info.m_pAttacker = lua_isnil( L, -1 ) ? self : lua_toentity( L, -1 );
    lua_pop( L, 1 );  // pop the Attacker value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Direction", "Dir" );
    info.m_vecDirShooting = lua_isnil( L, -1 ) ? vec3_origin : luaL_checkvector( L, -1 );
    lua_pop( L, 1 );  // pop the Direction value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Spread", "m_vecSpread" );
    info.m_vecSpread = lua_isnil( L, -1 ) ? vec3_origin : luaL_checkvector( L, -1 );
    lua_pop( L, 1 );  // pop the Spread value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Origin", "Src" );
    info.m_vecSrc = lua_isnil( L, -1 ) ? vec3_origin : luaL_checkvector( L, -1 );
    lua_pop( L, 1 );  // pop the Origin value

    return info;
}

LUA_API void lua_toemitsound( lua_State *L, int argIndex, EmitSound_t &ep )
{
    // Let's ensure that the emit sound info is a table
    luaL_checktype( L, argIndex, LUA_TTABLE );

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Channel", "m_nChannel" );
    ep.m_nChannel = lua_isnil( L, -1 ) ? CHAN_AUTO : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Channel value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "SoundName", "m_pSoundName" );
    ep.m_pSoundName = lua_isnil( L, -1 ) ? NULL : luaL_checkstring( L, -1 );
    lua_pop( L, 1 );  // pop the SoundName value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Volume", "m_flVolume" );
    ep.m_flVolume = lua_isnil( L, -1 ) ? 1.0f : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Volume value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "SoundLevel", "m_SoundLevel" );
    ep.m_SoundLevel = lua_isnil( L, -1 ) ? SNDLVL_NORM : ( soundlevel_t )( int )luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the SoundLevel value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Flags", "m_nFlags" );
    ep.m_nFlags = lua_isnil( L, -1 ) ? 0 : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Flags value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Pitch", "m_nPitch" );
    ep.m_nPitch = lua_isnil( L, -1 ) ? PITCH_NORM : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the Pitch value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "Origin", "m_pOrigin" );
    ep.m_pOrigin = lua_isnil( L, -1 ) ? NULL : new Vector( luaL_checkvector( L, -1 ) );
    lua_pop( L, 1 );  // pop the Origin value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "SoundTime", "m_flSoundTime" );
    ep.m_flSoundTime = lua_isnil( L, -1 ) ? 0.0f : luaL_checknumber( L, -1 );
    lua_pop( L, 1 );  // pop the SoundTime value

    GET_FIELD_WITH_COMPATIBILITY( L, argIndex, "SoundDuration", "m_pflSoundDuration" );
    if ( !lua_isnil( L, -1 ) )
    {
        float duration = luaL_checknumber( L, -1 );
        ep.m_pflSoundDuration = &duration;
    }
    lua_pop( L, 1 );  // pop the SoundDuration value
}
