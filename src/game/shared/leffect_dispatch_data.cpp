#include "cbase.h"
#if CLIENT_DLL
#include "c_te_effect_dispatch.h"
#else
#include "te_effect_dispatch.h"
#endif
#include "lua.hpp"
#include "luasrclib.h"
#include "leffect_dispatch_data.h"
#include "lbaseentity_shared.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CEffectData &lua_toeffect( lua_State *L, int idx )
{
    lua_CEffectData *data = ( lua_CEffectData * )lua_touserdata( L, idx );
    return *data;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pusheffect( lua_State *L, lua_CEffectData &data )
{
    lua_CEffectData *pData = ( lua_CEffectData * )lua_newuserdata( L, sizeof( lua_CEffectData ) );
    *pData = data;
    LUA_SAFE_SET_METATABLE( L, LUA_EFFECTDATAMETANAME );
}

LUALIB_API lua_CEffectData &luaL_checkeffect( lua_State *L, int narg )
{
    lua_CEffectData *d = ( lua_CEffectData * )luaL_checkudata( L, narg, LUA_EFFECTDATAMETANAME );
    return *d;
}

LUA_REGISTRATION_INIT( EffectData )

LUA_BINDING_BEGIN( EffectData, GetAngles, "class", "Get the angles of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushangle( L, data.m_vAngles );
    return 1;
}
LUA_BINDING_END( "Angle", "Angles" )

LUA_BINDING_BEGIN( EffectData, GetAttachmentIndex, "class", "Get the attachment ID for the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.m_nAttachmentIndex );
    return 1;
}
LUA_BINDING_END( "integer", "Attachment ID" )

LUA_BINDING_BEGIN( EffectData, GetColor, "class", "Get the byte described as the color of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.m_nColor );
    return 1;
}
LUA_BINDING_END( "integer", "Color" )

LUA_BINDING_BEGIN( EffectData, GetDamageType, "class", "Get the damage type of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.m_nDamageType );
    return 1;
}
LUA_BINDING_END( "integer", "Damage type" )

LUA_BINDING_BEGIN( EffectData, GetEffectNameIndex, "class", "Get the effect name index." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.GetEffectNameIndex() );
    return 1;
}
LUA_BINDING_END( "integer", "Effect name index." )

#ifndef CLIENT_DLL
LUA_BINDING_BEGIN( EffectData, GetEntityIndex, "class", "Get the entity index of the entity the effect is assigned to.", "server" )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.m_nEntIndex );
    return 1;
}
LUA_BINDING_END( "integer", "Entity index." )
#endif

LUA_BINDING_BEGIN( EffectData, GetEntity, "class", "Get the entity the effect is assigned to." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
#ifdef CLIENT_DLL
    CBaseEntity::PushLuaInstanceSafe( L, data.GetEntity() );
#else
    CBaseEntity::PushLuaInstanceSafe( L, CBaseEntity::Instance( data.m_nEntIndex ) );
#endif
    return 1;
}
LUA_BINDING_END( "Entity", "The entity the effect is assigned to." )

LUA_BINDING_BEGIN( EffectData, GetFlags, "class", "Get the flags of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.m_fFlags );
    return 1;
}
LUA_BINDING_END( "integer", "The flags of the effect." )

LUA_BINDING_BEGIN( EffectData, GetHitBox, "class", "Get the hit box ID of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.m_nHitBox );
    return 1;
}
LUA_BINDING_END( "integer", "Hit box ID" )

LUA_BINDING_BEGIN( EffectData, GetMagnitude, "class", "Get the magnitude of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushnumber( L, data.m_flMagnitude );
    return 1;
}
LUA_BINDING_END( "number", "Magnitude" )

LUA_BINDING_BEGIN( EffectData, GetMaterialIndex, "class", "Get the material index of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.m_nMaterial );
    return 1;
}
LUA_BINDING_END( "integer", "Material index" )

LUA_BINDING_BEGIN( EffectData, GetNormal, "class", "Get the normalized direction vector of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushvector( L, data.m_vNormal );
    return 1;
}
LUA_BINDING_END( "Vector", "Normal" )

LUA_BINDING_BEGIN( EffectData, GetOrigin, "class", "Get the origin position of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushvector( L, data.m_vOrigin );
    return 1;
}
LUA_BINDING_END( "Vector", "Origin" )

LUA_BINDING_BEGIN( EffectData, GetRadius, "class", "Get the radius of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushnumber( L, data.m_flRadius );
    return 1;
}
LUA_BINDING_END( "number", "Radius" )

LUA_BINDING_BEGIN( EffectData, GetScale, "class", "Get the scale of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushnumber( L, data.m_flScale );
    return 1;
}
LUA_BINDING_END( "number", "Scale" )

LUA_BINDING_BEGIN( EffectData, GetStart, "class", "Get the start position of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushvector( L, data.m_vStart );
    return 1;
}
LUA_BINDING_END( "Vector", "Start" )

LUA_BINDING_BEGIN( EffectData, GetSurfacePropertyIndex, "class", "Get the surface property index of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.m_nSurfaceProp );
    return 1;
}
LUA_BINDING_END( "integer", "Surface property index" )

LUA_BINDING_BEGIN( EffectData, SetAngles, "class", "Set the angles of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_vAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angles" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetAttachmentIndex, "class", "Set the attachment ID for the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_nAttachmentIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "attachment" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetColor, "class", "Set the byte described as the color of the effect. What this does differs per effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_nColor = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "color" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetDamageType, "class", "Set the damage type of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_nDamageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damageType" );
    return 0;
}
LUA_BINDING_END()

#ifndef CLIENT_DLL
LUA_BINDING_BEGIN( EffectData, SetEntityIndex, "class", "Set the entity index of the entity the effect is assigned to.", "server" )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_nEntIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "entityIndex" );
    return 0;
}
LUA_BINDING_END()
#endif

LUA_BINDING_BEGIN( EffectData, SetEntity, "class", "Set the entity the effect is assigned to." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
#ifdef CLIENT_DLL
    data.m_hEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" );
#else
    data.m_nEntIndex = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" )->entindex();
#endif
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetFlags, "class", "Set the flags of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_fFlags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetHitBox, "class", "Set the hit box ID of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_nHitBox = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "hitBoxIndex" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetMagnitude, "class", "Set the magnitude of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_flMagnitude = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "magnitude" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetMaterialIndex, "class", "Set the material index of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_nMaterial = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "materialIndex" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetNormal, "class", "Set the normalized direction vector of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_vNormal = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "normal" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetOrigin, "class", "Set the origin position of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_vOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetRadius, "class", "Set the radius of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_flRadius = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "radius" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetScale, "class", "Set the scale of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_flScale = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "scale" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetStart, "class", "Set the start position of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_vStart = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "start" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, SetSurfacePropertyIndex, "class", "Set the surface property index of the effect." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    data.m_nSurfaceProp = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "surfaceProperties" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, __index, "class", "Get the entity." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( Q_strcmp( field, "m_fFlags" ) == 0 )
        lua_pushinteger( L, data.m_fFlags );
    else if ( Q_strcmp( field, "m_flMagnitude" ) == 0 )
        lua_pushnumber( L, data.m_flMagnitude );
    else if ( Q_strcmp( field, "m_flRadius" ) == 0 )
        lua_pushnumber( L, data.m_flRadius );
    else if ( Q_strcmp( field, "m_flScale" ) == 0 )
        lua_pushnumber( L, data.m_flScale );
#ifdef CLIENT_DLL
    else if ( Q_strcmp( field, "m_hEntity" ) == 0 )
        CBaseEntity::PushLuaInstanceSafe( L, C_BaseEntity::Instance( data.m_hEntity ) );
#else
    else if ( Q_strcmp( field, "m_nEntIndex" ) == 0 )
        lua_pushinteger( L, data.m_nEntIndex );
#endif
    else if ( Q_strcmp( field, "m_nAttachmentIndex" ) == 0 )
        lua_pushinteger( L, data.m_nAttachmentIndex );
    else if ( Q_strcmp( field, "m_nDamageType" ) == 0 )
        lua_pushinteger( L, data.m_nDamageType );
    else if ( Q_strcmp( field, "m_nHitBox" ) == 0 )
        lua_pushinteger( L, data.m_nHitBox );
    else if ( Q_strcmp( field, "m_nMaterial" ) == 0 )
        lua_pushinteger( L, data.m_nMaterial );
    else if ( Q_strcmp( field, "m_nSurfaceProp" ) == 0 )
        lua_pushinteger( L, data.m_nSurfaceProp );
    else if ( Q_strcmp( field, "m_vAngles" ) == 0 )
        lua_pushangle( L, data.m_vAngles );
    else if ( Q_strcmp( field, "m_vNormal" ) == 0 )
        lua_pushvector( L, data.m_vNormal );
    else if ( Q_strcmp( field, "m_vOrigin" ) == 0 )
        lua_pushvector( L, data.m_vOrigin );
    else if ( Q_strcmp( field, "m_vStart" ) == 0 )
        lua_pushvector( L, data.m_vStart );
    else
    {
        if ( lua_getmetatable( L, 1 ) )
        {
            lua_pushvalue( L, 2 );
            lua_gettable( L, -2 );
        }
        else
        {
            lua_pushnil( L );
        }
    }

    return 1;
}
LUA_BINDING_END( "any", "Value" )

LUA_BINDING_BEGIN( EffectData, __newindex, "class", "Metamethod called when a non-existant field is added" )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( Q_strcmp( field, "m_fFlags" ) == 0 )
        data.m_fFlags = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_flMagnitude" ) == 0 )
        data.m_flMagnitude = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_flRadius" ) == 0 )
        data.m_flRadius = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_flScale" ) == 0 )
        data.m_flScale = luaL_checknumber( L, 3 );
#ifdef CLIENT_DLL
    else if ( Q_strcmp( field, "m_hEntity" ) == 0 )
        data.m_hEntity = lua_toentity( L, 3 )->GetRefEHandle();
#else
    else if ( Q_strcmp( field, "m_nEntIndex" ) == 0 )
        data.m_nEntIndex = luaL_checknumber( L, 3 );
#endif
    else if ( Q_strcmp( field, "m_nAttachmentIndex" ) == 0 )
        data.m_nAttachmentIndex = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_nDamageType" ) == 0 )
        data.m_nDamageType = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_nHitBox" ) == 0 )
        data.m_nHitBox = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_nMaterial" ) == 0 )
        data.m_nMaterial = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_nSurfaceProp" ) == 0 )
        data.m_nSurfaceProp = luaL_checknumber( L, 3 );
    else if ( Q_strcmp( field, "m_vAngles" ) == 0 )
        data.m_vAngles = luaL_checkangle( L, 3 );
    else if ( Q_strcmp( field, "m_vNormal" ) == 0 )
        data.m_vNormal = luaL_checkvector( L, 3 );
    else if ( Q_strcmp( field, "m_vOrigin" ) == 0 )
        data.m_vOrigin = luaL_checkvector( L, 3 );
    else if ( Q_strcmp( field, "m_vStart" ) == 0 )
        data.m_vStart = luaL_checkvector( L, 3 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( EffectData, __tostring, "class", "To string operator." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushfstring( L, "EffectData: %p", &data );
    return 1;
}
LUA_BINDING_END( "string", "String representation of the effect data." )

LUA_REGISTRATION_INIT( Effects )

LUA_BINDING_BEGIN( Effects, Create, "library", "Creates an effect (EffectData)." )
{
    lua_CEffectData data = lua_CEffectData();
    lua_pusheffect( L, data );
    return 1;
}
LUA_BINDING_END( "EffectData", "Effect data." )

LUA_BINDING_BEGIN( Effects, Dispatch, "library", "Dispatches an effect." )
{
    const char *effectName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "effectName" );
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 2, "effectData" );

    DispatchEffect( effectName, data );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Effects, SpawnParticleTracer, "library", "Spawns a tracer particle effect." )
{
    const char *effect = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "effect" );
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "start" );
    Vector end = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "end" );
    bool bWhiz = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "whiz" );
    int entityIndex = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 0, "entityIndex" );
    int attachment = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 6, 0, "attachment" );

    UTIL_ParticleTracer( effect, start, end, entityIndex, attachment, bWhiz );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Effects, SpawnTracer, "library", "Creates a tracer effect." )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector end = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    int entindex = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "entityIndex" );
    int attachment = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 0, "attachment" );
    float velocity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 0, "velocity" );
    bool bWhiz = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 6, false, "shouldWhiz" );
    const char *customTracerName = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 7, NULL, "customTracerName" );
    int particleId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 8, 0, "particleId" );

    UTIL_Tracer( start, end, entindex, attachment, velocity, bWhiz, customTracerName, particleId );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Effects, SpawnBloodDrips, "library", "Creates a blood drip effect." )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );
    Vector direction = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "direction" );
    int color = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "color" );
    int amount = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "amount" );

    UTIL_BloodDrips( origin, direction, color, amount );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Effects, SpawnBloodImpact, "library", "Creates a blood impact effect." )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );
    Vector direction = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "direction" );
    int color = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "color" );
    int amount = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "amount" );

    UTIL_BloodImpact( origin, direction, color, amount );

    return 0;
}
LUA_BINDING_END()

// Experiment; Disabled because we never push trace_t pointers, instead we push it as a table struct
// LUA_BINDING_BEGIN( Effects, SpawnBloodDecalTrace, "library", "Creates a blood decal trace effect." )
//{
//    trace_t *trace = LUA_BINDING_ARGUMENT( luaL_checktrace, 1, "trace" );
//    int color = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "color" );
//
//    UTIL_BloodDecalTrace( &trace, color );
//
//    return 0;
//}
// LUA_BINDING_END()

// Experiment; Disabled because we never push trace_t pointers
// LUA_BINDING_BEGIN( Effects, SpawnDecalTrace, "library", "Creates a decal trace effect." )
//{
//    trace_t *trace = LUA_BINDING_ARGUMENT( luaL_checktrace, 1, "trace" );
//    const char *decalName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "decalName" );
//
//    UTIL_DecalTrace( &trace, decalName );
//
//    return 0;
//}
// LUA_BINDING_END()

#ifdef GAME_DLL

/*
* TODO: Expose the types
#define MUZZLEFLASH_TYPE_DEFAULT	0x00000001
#define MUZZLEFLASH_TYPE_GUNSHIP	0x00000002
#define MUZZLEFLASH_TYPE_STRIDER	0x00000004
*/
LUA_BINDING_BEGIN( Effects, SpawnMuzzleFlash, "library", "Creates a muzzle flash effect. Types can be MUZZLEFLASH_TYPE_DEFAULT(1), MUZZLEFLASH_TYPE_GUNSHIP(2), MUZZLEFLASH_TYPE_STRIDER(4) - enums don't exist yet in Lua", "server" )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );
    QAngle angles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angles" );
    int scale = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "scale" );
    int type = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, MUZZLEFLASH_TYPE_DEFAULT, "type" );

    UTIL_MuzzleFlash( origin, angles, scale, type );

    return 0;
}
LUA_BINDING_END()

/*
* TODO: Expose the types
  DONT_BLEED = -1,

  BLOOD_COLOR_RED = 0,
  BLOOD_COLOR_YELLOW,
  BLOOD_COLOR_GREEN,
  BLOOD_COLOR_MECH,
*/
LUA_BINDING_BEGIN( Effects, SpawnBloodStream, "library", "Creates a blood stream effect. Color can be DONT_BLEED(-1), BLOOD_COLOR_RED(0), BLOOD_COLOR_YELLOW(1), BLOOD_COLOR_GREEN(2), BLOOD_COLOR_MECH(3) - enums don't exist yet in Lua", "server" )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );
    Vector direction = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "direction" );
    int color = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "color" );
    int amount = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "amount" );

    UTIL_BloodStream( origin, direction, color, amount );

    return 0;
}
LUA_BINDING_END()

/*
* TODO: Expose the types
const int FX_BLOODSPRAY_DROPS	= 0x01;
const int FX_BLOODSPRAY_GORE	= 0x02;
const int FX_BLOODSPRAY_CLOUD	= 0x04;
const int FX_BLOODSPRAY_ALL		= 0xFF;
*/
LUA_BINDING_BEGIN( Effects, SpawnBloodSpray, "library", "Creates a blood spray effect.\nColor can be DONT_BLEED(-1), BLOOD_COLOR_RED(0), BLOOD_COLOR_YELLOW(1), BLOOD_COLOR_GREEN(2), BLOOD_COLOR_MECH(3) - enums don't exist yet in Lua\nFlags can be FX_BLOODSPRAY_DROPS(1), FX_BLOODSPRAY_GORE(2), FX_BLOODSPRAY_CLOUD(4), FX_BLOODSPRAY_ALL(255) - enums don't exist yet in Lua", "server" )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );
    Vector direction = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "direction" );
    int color = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "color" );
    int amount = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "amount" );
    int flags = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "flags" );

    UTIL_BloodSpray( origin, direction, color, amount, flags );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Effects, SpawnBubbleTrail, "library", "Creates a bubble trail effect.", "server" )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector end = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    int count = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "count" );

    UTIL_BubbleTrail( start, end, count );

    return 0;
}
LUA_BINDING_END()

#endif  // GAME_DLL

/*
** Open CEffectData object
*/
LUALIB_API int luaopen_CEffectData( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_EFFECTDATAMETANAME );

    LUA_REGISTRATION_COMMIT( EffectData );

    lua_pushstring( L, "EffectData" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "EffectData" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Effects );
    lua_pop( L, 1 ); /* pop metatable */

    return 1;
}
