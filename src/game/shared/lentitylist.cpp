//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "mathlib/lVector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifndef CLIENT_DLL
// static int gEntList_AddPostClientMessageEntity (lua_State *L) {
//   gEntList.AddPostClientMessageEntity(luaL_checkentity(L, 1));
//   return 0;
// }

static int gEntList_CleanupDeleteList( lua_State *L )
{
    gEntList.CleanupDeleteList();
    return 0;
}

static int gEntList_Clear( lua_State *L )
{
    gEntList.Clear();
    return 0;
}

/// <luadoc>
///     <summary>
///         Finds an entity by the given classname, it starts at the entity given in the first argument.
///     </summary>
///     <library>%LUA_ENTLISTLIBNAME%</library>
///     <function>FindEntityByClassname</function>
///     <param name="entity">The entity to start searching from.</param>
///     <param name="classname">The classname of the entity to find.</param>
///     <returns>An entity with the given classname.</returns>
///     <realm>server</realm>
///     <example>
///         Finds an entity by the classname "npc_zombie".
///         By providing NULL as the first argument, it will search from the beginning of the entity list.
///         <code>
///             local entity = EntityList.FindEntityByClassname( NULL, "npc_zombie" )
///             print( entity:GetClassname(), entity:GetPosition() )
///         </code>
///     </example>
///     <example>
///         Finds an entity by the classname "player", starting from the player with the name "Anne".
///         <code>
///            local entity = EntityList.FindEntityByClassname( Util.PlayerByName( "Anne" ), "player" )
///            print( entity:GetClassname(), entity:GetPosition() )
///         </code>
///     </example>
/// </luadoc>
static int gEntList_FindEntityByClassname( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByClassname( lua_toentity( L, 1 ), luaL_checkstring( L, 2 ) ) );
    return 1;
}

/// <luadoc>
///     <summary>
///         Finds an entity by the given classname, it narrows the search within the radius of the given vector.
///     </summary>
///     <library>%LUA_ENTLISTLIBNAME%</library>
///     <function>FindEntityByClassnameNearest</function>
///     <param name="classname">The classname of the entity to find.</param>
///     <param name="origin">The origin to search from.</param>
///     <param name="radius">The radius to search within.</param>
///     <returns>An entity with the given classname.</returns>
///     <realm>server</realm>
/// </luadoc>
static int gEntList_FindEntityByClassnameNearest( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByClassnameNearest( luaL_checkstring( L, 1 ), luaL_checkvector( L, 2 ), luaL_checknumber( L, 3 ) ) );
    return 1;
}

static int gEntList_FindEntityByClassnameWithin( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByClassnameWithin( lua_toentity( L, 1 ), luaL_checkstring( L, 2 ), luaL_checkvector( L, 3 ), luaL_checknumber( L, 4 ) ) );
    return 1;
}

static int gEntList_FindEntityByModel( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByModel( lua_toentity( L, 1 ), luaL_checkstring( L, 2 ) ) );
    return 1;
}

static int gEntList_FindEntityByName( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByName( lua_toentity( L, 1 ), luaL_checkstring( L, 2 ), luaL_optentity( L, 3, 0 ), luaL_optentity( L, 4, 0 ), luaL_optentity( L, 5, 0 ) ) );
    return 1;
}

static int gEntList_FindEntityByNameNearest( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByNameNearest( luaL_checkstring( L, 1 ), luaL_checkvector( L, 2 ), luaL_checknumber( L, 3 ), luaL_optentity( L, 4, 0 ), luaL_optentity( L, 5, 0 ), luaL_optentity( L, 6, 0 ) ) );
    return 1;
}

static int gEntList_FindEntityByNameWithin( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByNameWithin( lua_toentity( L, 1 ), luaL_checkstring( L, 2 ), luaL_checkvector( L, 3 ), luaL_checknumber( L, 4 ), luaL_optentity( L, 5, 0 ), luaL_optentity( L, 6, 0 ), luaL_optentity( L, 7, 0 ) ) );
    return 1;
}

static int gEntList_FindEntityByTarget( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByTarget( lua_toentity( L, 1 ), luaL_checkstring( L, 2 ) ) );
    return 1;
}

static int gEntList_FindEntityClassNearestFacing( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityClassNearestFacing( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ), luaL_checknumber( L, 3 ), ( char * )luaL_checkstring( L, 4 ) ) );
    return 1;
}

static int gEntList_FindEntityGeneric( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityGeneric( lua_toentity( L, 1 ), luaL_checkstring( L, 2 ), luaL_optentity( L, 3, 0 ), luaL_optentity( L, 4, 0 ), luaL_optentity( L, 5, 0 ) ) );
    return 1;
}

static int gEntList_FindEntityGenericNearest( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityGenericNearest( luaL_checkstring( L, 1 ), luaL_checkvector( L, 2 ), luaL_checknumber( L, 3 ), luaL_optentity( L, 4, 0 ), luaL_optentity( L, 5, 0 ), luaL_optentity( L, 6, 0 ) ) );
    return 1;
}

static int gEntList_FindEntityGenericWithin( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityGenericWithin( lua_toentity( L, 1 ), luaL_checkstring( L, 2 ), luaL_checkvector( L, 3 ), luaL_checknumber( L, 4 ), luaL_optentity( L, 5, 0 ), luaL_optentity( L, 6, 0 ), luaL_optentity( L, 7, 0 ) ) );
    return 1;
}

static int gEntList_FindEntityInSphere( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityInSphere( lua_toentity( L, 1 ), luaL_checkvector( L, 2 ), luaL_checknumber( L, 3 ) ) );
    return 1;
}

static int gEntList_FindEntityNearestFacing( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityNearestFacing( luaL_checkvector( L, 1 ), luaL_checkvector( L, 2 ), luaL_checknumber( L, 3 ) ) );
    return 1;
}

static int gEntList_FindEntityProcedural( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityProcedural( luaL_checkstring( L, 1 ), luaL_optentity( L, 2, 0 ), luaL_optentity( L, 3, 0 ), luaL_optentity( L, 4, 0 ) ) );
    return 1;
}

static int gEntList_IsClearingEntities( lua_State *L )
{
    lua_pushboolean( L, gEntList.IsClearingEntities() );
    return 1;
}

static int gEntList_NotifyCreateEntity( lua_State *L )
{
    gEntList.NotifyCreateEntity( luaL_checkentity( L, 1 ) );
    return 0;
}

static int gEntList_NotifySpawn( lua_State *L )
{
    gEntList.NotifySpawn( luaL_checkentity( L, 1 ) );
    return 0;
}

static int gEntList_NumberOfEdicts( lua_State *L )
{
    lua_pushinteger( L, gEntList.NumberOfEdicts() );
    return 1;
}

// static int gEntList_PostClientMessagesSent (lua_State *L) {
//   gEntList.PostClientMessagesSent();
//   return 0;
// }

static int gEntList_ReportEntityFlagsChanged( lua_State *L )
{
    gEntList.ReportEntityFlagsChanged( luaL_checkentity( L, 1 ), ( unsigned int )luaL_checkinteger( L, 2 ), ( unsigned int )luaL_checkinteger( L, 3 ) );
    return 0;
}

static int gEntList_ResetDeleteList( lua_State *L )
{
    lua_pushinteger( L, gEntList.ResetDeleteList() );
    return 1;
}
#endif

static int gEntList_FirstEnt( lua_State *L )
{
#ifdef CLIENT_DLL
    CBaseEntity::PushLuaInstanceSafe( L, ClientEntityList().FirstBaseEntity() );
#else
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FirstEnt() );
#endif

    return 1;
}

static int gEntList_GetAllEntities( lua_State *L )
{
    lua_newtable( L );

    CBaseEntity *pEnt = NULL;
    int i = 0;
#ifdef CLIENT_DLL
    while ( ( pEnt = ClientEntityList().NextBaseEntity( pEnt ) ) != NULL )
#else
    while ( ( pEnt = gEntList.NextEnt( pEnt ) ) != NULL )
#endif
    {
        lua_pushinteger( L, ++i );  // 1-based index
        CBaseEntity::PushLuaInstanceSafe( L, pEnt );
        lua_settable( L, -3 );
    }

    return 1;
}

static int gEntList_NextEnt( lua_State *L )
{
#ifdef CLIENT_DLL
    CBaseEntity::PushLuaInstanceSafe( L, ClientEntityList().NextBaseEntity( lua_toentity( L, 1 ) ) );
#else
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.NextEnt( lua_toentity( L, 1 ) ) );
#endif

    return 1;
}

static int gEntList_NumberOfEntities( lua_State *L )
{
#ifdef CLIENT_DLL
    lua_pushinteger( L, ClientEntityList().NumberOfEntities() );
#else
    lua_pushinteger( L, gEntList.NumberOfEntities() );
#endif

    return 1;
}

static const luaL_Reg gEntListlib[] = {
#ifndef CLIENT_DLL
    //{"AddPostClientMessageEntity",   gEntList_AddPostClientMessageEntity},
    { "CleanupDeleteList", gEntList_CleanupDeleteList },
    { "Clear", gEntList_Clear },
    { "FindEntityByClassname", gEntList_FindEntityByClassname },
    { "FindEntityByClassnameNearest", gEntList_FindEntityByClassnameNearest },
    { "FindEntityByClassnameWithin", gEntList_FindEntityByClassnameWithin },
    { "FindEntityByModel", gEntList_FindEntityByModel },
    { "FindEntityByName", gEntList_FindEntityByName },
    { "FindEntityByNameNearest", gEntList_FindEntityByNameNearest },
    { "FindEntityByNameWithin", gEntList_FindEntityByNameWithin },
    { "FindEntityByTarget", gEntList_FindEntityByTarget },
    { "FindEntityClassNearestFacing", gEntList_FindEntityClassNearestFacing },
    { "FindEntityGeneric", gEntList_FindEntityGeneric },
    { "FindEntityGenericNearest", gEntList_FindEntityGenericNearest },
    { "FindEntityGenericWithin", gEntList_FindEntityGenericWithin },
    { "FindEntityInSphere", gEntList_FindEntityInSphere },
    { "FindEntityNearestFacing", gEntList_FindEntityNearestFacing },
    { "FindEntityProcedural", gEntList_FindEntityProcedural },
    { "IsClearingEntities", gEntList_IsClearingEntities },
    { "NotifyCreateEntity", gEntList_NotifyCreateEntity },
    //{"PostClientMessagesSent",   gEntList_PostClientMessagesSent},
    { "ReportEntityFlagsChanged", gEntList_ReportEntityFlagsChanged },
    { "ResetDeleteList", gEntList_ResetDeleteList },
    { "NotifySpawn", gEntList_NotifySpawn },
    { "GetEdictCount", gEntList_NumberOfEdicts },
#endif
    { "FirstEnt", gEntList_FirstEnt },
    { "GetAllEntities", gEntList_GetAllEntities },
    { "GetEntityCount", gEntList_NumberOfEntities },
    { "NextEnt", gEntList_NextEnt },
    { NULL, NULL } };

/*
** Open gEntList library
*/
LUALIB_API int luaopen_EntityList( lua_State *L )
{
    luaL_register( L, LUA_ENTLISTLIBNAME, gEntListlib );
    return 1;
}
