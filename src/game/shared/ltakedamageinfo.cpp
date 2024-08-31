#include "cbase.h"
#include "takedamageinfo.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "ltakedamageinfo.h"
#include "lbaseentity_shared.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CTakeDamageInfo &lua_todamageinfo( lua_State *L, int idx )
{
    lua_CTakeDamageInfo *info = ( lua_CTakeDamageInfo * )lua_touserdata( L, idx );
    return *info;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushdamageinfo( lua_State *L, lua_CTakeDamageInfo &info )
{
    lua_CTakeDamageInfo *pInfo = ( lua_CTakeDamageInfo * )lua_newuserdata( L, sizeof( lua_CTakeDamageInfo ) );
    *pInfo = info;
    LUA_SAFE_SET_METATABLE( L, LUA_TAKEDAMAGEINFOMETANAME );
}

LUALIB_API lua_CTakeDamageInfo &luaL_checkdamageinfo( lua_State *L, int narg )
{
    lua_CTakeDamageInfo *d = ( lua_CTakeDamageInfo * )luaL_checkudata( L, narg, LUA_TAKEDAMAGEINFOMETANAME );
    return *d;
}

LUA_REGISTRATION_INIT( TakeDamageInfo );

LUA_BINDING_BEGIN( TakeDamageInfo, AddDamage, "class", "Adds damage to the damage info" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).AddDamage( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damage" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, AddDamageType, "class", "Adds a damage type" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).AddDamageType( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damageType" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, AdjustPlayerDamageInflictedForSkillLevel, "class", "Adjusts player-inflicted damage based on skill level" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).AdjustPlayerDamageInflictedForSkillLevel();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, AdjustPlayerDamageTakenForSkillLevel, "class", "Adjusts player-taken damage based on skill level" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).AdjustPlayerDamageTakenForSkillLevel();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, BaseDamageIsValid, "class", "Checks if base damage is valid" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).BaseDamageIsValid() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if base damage is valid, false otherwise." )

LUA_BINDING_BEGIN( TakeDamageInfo, CopyDamageToBaseDamage, "class", "Copies damage to base damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).CopyDamageToBaseDamage();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, DebugGetDamageTypeString, "class", "Gets a string describing the damage type" )
{
    char outbuf[256];
    outbuf[0] = '\0';
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).DebugGetDamageTypeString( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bufferSize" ), outbuf, sizeof( outbuf ) );
    lua_pushstring( L, outbuf );
    return 1;
}
LUA_BINDING_END( "string", "The string describing the damage type." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetAmmoName, "class", "Gets the name of the ammo" )
{
    lua_pushstring( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetAmmoName() );
    return 1;
}
LUA_BINDING_END( "string", "The name of the ammo." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetAmmoType, "class", "Gets the ammo type" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetAmmoType() );
    return 1;
}
LUA_BINDING_END( "integer", "The ammo type." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetAttacker, "class", "Gets the attacker entity" )
{
    CBaseEntity::PushLuaInstanceSafe( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetAttacker() );
    return 1;
}
LUA_BINDING_END( "Entity", "The attacker entity." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetBaseDamage, "class", "Gets the base damage" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetBaseDamage() );
    return 1;
}
LUA_BINDING_END( "number", "The base damage." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetDamage, "class", "Gets the damage" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetDamage() );
    return 1;
}
LUA_BINDING_END( "number", "The damage." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetDamageCustom, "class", "Gets custom damage" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetDamageCustom() );
    return 1;
}
LUA_BINDING_END( "integer", "The custom damage." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetDamageForce, "class", "Gets the force of the damage" )
{
    lua_pushvector( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetDamageForce() );
    return 1;
}
LUA_BINDING_END( "Vector", "The force of the damage." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetDamagePosition, "class", "Gets the position of the damage" )
{
    lua_pushvector( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetDamagePosition() );
    return 1;
}
LUA_BINDING_END( "Vector", "The position of the damage." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetDamageStats, "class", "Gets damage stats" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetDamageStats() );
    return 1;
}
LUA_BINDING_END( "integer", "The damage stats." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetDamageType, "class", "Gets the type of damage" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetDamageType() );
    return 1;
}
LUA_BINDING_END( "integer", "The type of damage." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetInflictor, "class", "Gets the inflictor entity" )
{
    CBaseEntity::PushLuaInstanceSafe( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetInflictor() );
    return 1;
}
LUA_BINDING_END( "Entity", "The inflictor entity." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetMaxDamage, "class", "Gets the maximum damage" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetMaxDamage() );
    return 1;
}
LUA_BINDING_END( "number", "The maximum damage." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetReportedPosition, "class", "Gets the reported position of the damage" )
{
    lua_pushvector( L, LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).GetReportedPosition() );
    return 1;
}
LUA_BINDING_END( "Vector", "The reported position of the damage." )

LUA_BINDING_BEGIN( TakeDamageInfo, GetWeapon, "class", "Gets the weapon entity" )
{
    lua_CTakeDamageInfo damageInfo = LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" );
    CBaseEntity *entity = damageInfo.GetWeapon();

    CBaseCombatWeapon::PushLuaInstanceSafe( L, dynamic_cast< CBaseCombatWeapon * >( entity ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The weapon entity." )

LUA_BINDING_BEGIN( TakeDamageInfo, ScaleDamage, "class", "Scales the damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).ScaleDamage( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "scale" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, ScaleDamageForce, "class", "Scales the damage force" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).ScaleDamageForce( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "scale" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetAmmoType, "class", "Sets the ammo type" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetAmmoType( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "ammoType" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetAttacker, "class", "Sets the attacker entity" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetAttacker( LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "attacker" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetDamage, "class", "Sets the damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetDamage( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damage" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetDamageCustom, "class", "Sets custom damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetDamageCustom( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "customDamage" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetDamageForce, "class", "Sets the force of the damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetDamageForce( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "force" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetDamagePosition, "class", "Sets the position of the damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetDamagePosition( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetDamageStats, "class", "Sets damage stats" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetDamageStats( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "stats" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetDamageType, "class", "Sets the type of damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetDamageType( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damageType" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetInflictor, "class", "Sets the inflictor entity" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetInflictor( LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "inflictor" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetMaxDamage, "class", "Sets the maximum damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetMaxDamage( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "maxDamage" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetReportedPosition, "class", "Sets the reported position of the damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetReportedPosition( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SetWeapon, "class", "Sets the weapon entity" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SetWeapon( LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "weapon" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, SubtractDamage, "class", "Subtracts damage" )
{
    LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ).SubtractDamage( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damage" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfo, __tostring, "class", "Returns a string representation of the TakeDamageInfo instance" )
{
    lua_pushfstring( L, "TakeDamageInfo: %p", luaL_checkudata( L, 1, LUA_TAKEDAMAGEINFOMETANAME ) );
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the TakeDamageInfo instance." )

/*
** TakeDamageInfos library
*/

LUA_REGISTRATION_INIT( TakeDamageInfos )

LUA_BINDING_BEGIN( TakeDamageInfos, Create, "library", "Creates a new TakeDamageInfo object" )
{
    CTakeDamageInfo info;

    if ( lua_gettop( L ) < 4 )
    {
        info = CTakeDamageInfo();
    }
    else if ( lua_gettop( L ) <= 5 )
    {
        lua_CBaseEntity *inflictor = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "inflictor" );
        lua_CBaseEntity *attacker = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "attacker" );
        float damage = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "damage" );
        int damageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "damageType" );
        int killType = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 0, "killType" );

        info = CTakeDamageInfo( inflictor, attacker, damage, damageType, killType );
    }
    else if ( lua_gettop( L ) <= 6 )
    {
        lua_CBaseEntity *inflictor = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "inflictor" );
        lua_CBaseEntity *attacker = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "attacker" );
        lua_CBaseEntity *weapon = LUA_BINDING_ARGUMENT( luaL_checkentity, 3, "weapon" );
        float damage = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "damage" );
        int damageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "damageType" );
        int killType = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 6, 0, "killType" );

        info = CTakeDamageInfo( inflictor, attacker, weapon, damage, damageType, killType );
    }
    else if ( lua_gettop( L ) < 9 )
    {
        lua_CBaseEntity *inflictor = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "inflictor" );
        lua_CBaseEntity *attacker = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "attacker" );
        Vector damageForce = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "damageForce" );
        Vector damagePosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "damagePosition" );
        float damage = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "damage" );
        int damageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "damageType" );
        int killType = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 7, 0, "killType" );
        Vector *reportedPosition = &LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optvector, 8, NULL, "reportedPosition" );

        info = CTakeDamageInfo( inflictor, attacker, damageForce, damagePosition, damage, damageType, killType, reportedPosition );
    }
    else
    {
        lua_CBaseEntity *inflictor = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "inflictor" );
        lua_CBaseEntity *attacker = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "attacker" );
        lua_CBaseEntity *weapon = LUA_BINDING_ARGUMENT( luaL_checkentity, 3, "weapon" );
        Vector damageForce = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "damageForce" );
        Vector damagePosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 5, "damagePosition" );
        float damage = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "damage" );
        int damageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "damageType" );
        int killType = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 8, 0, "killType" );
        Vector *reportedPosition = &LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optvector, 9, NULL, "reportedPosition" );

        info = CTakeDamageInfo( inflictor, attacker, weapon, damageForce, damagePosition, damage, damageType, killType, reportedPosition );
    }

    lua_pushdamageinfo( L, info );
    return 1;
}
LUA_BINDING_END( "TakeDamageInfo", "The new TakeDamageInfo object." )

LUA_BINDING_BEGIN( TakeDamageInfos, ClearMultiDamage, "library", "Clears multi damage" )
{
    ClearMultiDamage();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfos, ApplyMultiDamage, "library", "Applies multi damage" )
{
    ApplyMultiDamage();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfos, AddMultiDamage, "library", "Adds multi damage" )
{
    AddMultiDamage(
        LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ),
        LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfos, ImpulseScale, "library", "Scales the impulse" )
{
    lua_pushnumber( L, ImpulseScale( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "targetMass" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "desiredSpeed" ) ) );

    return 1;
}
LUA_BINDING_END( "number", "The scaled impulse." )

LUA_BINDING_BEGIN( TakeDamageInfos, CalculateExplosiveDamageForce, "library", "Calculates the explosive damage force" )
{
    CalculateExplosiveDamageForce(
        &LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ),
        LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "damageDirection" ),
        LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "damagePosition" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 1.0, "scale" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfos, CalculateBulletDamageForce, "library", "Calculates the bullet damage force" )
{
    CalculateBulletDamageForce(
        &LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bulletType" ),
        LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "damageDirection" ),
        LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "damagePosition" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 1.0, "scale" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfos, CalculateMeleeDamageForce, "library", "Calculates the melee damage force" )
{
    CalculateMeleeDamageForce(
        &LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ),
        LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "damageDirection" ),
        LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "damagePosition" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 1.0, "scale" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( TakeDamageInfos, GuessDamageForce, "library", "Guesses the damage force" )
{
    GuessDamageForce(
        &LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 1, "damageInfo" ),
        LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "damageDirection" ),
        LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "damagePosition" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 1.0, "scale" ) );

    return 0;
}
LUA_BINDING_END()

/*
** Open CTakeDamageInfo object
*/
LUALIB_API int luaopen_CTakeDamageInfo( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_TAKEDAMAGEINFOMETANAME );

    LUA_REGISTRATION_COMMIT( TakeDamageInfo );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_TAKEDAMAGEINFOMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "TakeDamageInfo" */

    LUA_REGISTRATION_COMMIT_LIBRARY( TakeDamageInfos );

    lua_pop( L, 1 );
    return 1;
}
