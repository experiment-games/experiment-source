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
    LUA_SAFE_SET_METATABLE( L, "CEffectData" );
}

LUALIB_API lua_CEffectData &luaL_checkeffect( lua_State *L, int narg )
{
    lua_CEffectData *d = ( lua_CEffectData * )luaL_checkudata( L, narg, "CEffectData" );
    return *d;
}

LUA_REGISTRATION_INIT( CEffectData )

#ifdef CLIENT_DLL
LUA_BINDING_BEGIN( CEffectData, GetEntityIndex, "class", "Get the entity index.", "client" )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.entindex() );
    return 1;
}
LUA_BINDING_END( "number", "Entity index." )
#endif

LUA_BINDING_BEGIN( CEffectData, GetEffectNameIndex, "class", "Get the effect name index." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushinteger( L, data.GetEffectNameIndex() );
    return 1;
}
LUA_BINDING_END( "number", "Effect name index." )

#ifdef CLIENT_DLL
LUA_BINDING_BEGIN( CEffectData, GetEntity, "class", "Get the entity.", "client" )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    CBaseEntity::PushLuaInstanceSafe( L, data.GetEntity() );
    return 1;
}
LUA_BINDING_END( "entity", "Entity." )
#endif

LUA_BINDING_BEGIN( CEffectData, __index, "class", "Get the entity." )
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

LUA_BINDING_BEGIN( CEffectData, __newindex, "class", "Metamethod called when a non-existant field is added" )
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

LUA_BINDING_BEGIN( CEffectData, __tostring, "class", "To string operator." )
{
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 1, "effectData" );
    lua_pushfstring( L, "CEffectData: %p", &data );
    return 1;
}
LUA_BINDING_END( "string", "String representation of the effect data." )

LUA_REGISTRATION_INIT( _G )

LUA_BINDING_BEGIN( _G, CEffectData, "library", "Creates an effect." )
{
    CEffectData data = CEffectData();
    lua_pusheffect( L, data );
    return 1;
}
LUA_BINDING_END( "EffectData", "Effect data." )

LUA_BINDING_BEGIN( _G, DispatchEffect, "library", "Dispatches an effect." )
{
    const char *effectName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "effectName" );
    lua_CEffectData &data = LUA_BINDING_ARGUMENT( luaL_checkeffect, 2, "effectData" );

    DispatchEffect( effectName, data );
    return 0;
}
LUA_BINDING_END()

/*
** Open CEffectData object
*/
LUALIB_API int luaopen_CEffectData( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_EFFECTDATALIBNAME );

    LUA_REGISTRATION_COMMIT( CEffectData );

    lua_pushstring( L, "Effect" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Effect" */

    LUA_REGISTRATION_COMMIT_LIBRARY( _G );
    lua_pop( L, 1 ); /* pop metatable */

    return 1;
}
