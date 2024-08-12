#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "mathlib/lVector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Entities );

#ifndef CLIENT_DLL

// LUA_BINDING_BEGIN( Entities, AddPostClientMessageEntity, "library", "Adds an entity to the post client message list.", "server" )
//{
//     CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
//
//     gEntList.AddPostClientMessageEntity( entity );
//     return 0;
// }
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Entities, CleanDeleteList, "library", "Cleans up the delete list.", "server" )
{
    gEntList.CleanupDeleteList();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entities, Clear, "library", "Clears the entity list.", "server" )
{
    gEntList.Clear();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entities, FindByClass, "library", "Finds an entity by its class name", "server" )
{
    const char *className = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "className" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 2, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByClassname( startEntity, className ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindByClassNearest, "library", "Finds the nearest entity by its class name", "server" )
{
    const char *className = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "className" );
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
    float maxDistance = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxDistance" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByClassnameNearest( className, position, maxDistance ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindByClassWithin, "library", "Finds an entity by its class name within a radius", "server" )
{
    const char *className = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "className" );
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
    float maxDistance = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxDistance" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 4, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByClassnameWithin( startEntity, className, position, maxDistance ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindByModel, "library", "Finds an entity by its model name", "server" )
{
    const char *modelName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "modelName" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 2, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByModel( startEntity, modelName ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindByName, "library", "Finds an entity by its name", "server" )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 2, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByName( startEntity, name ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindByNameNearest, "library", "Finds the nearest entity by its name", "server" )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
    float maxDistance = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxDistance" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByNameNearest( name, position, maxDistance ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindByNameWithin, "library", "Finds an entity by its name within a radius", "server" )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
    float maxDistance = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxDistance" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 4, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByNameWithin( startEntity, name, position, maxDistance ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindByTarget, "library", "Finds an entity by its target name", "server" )
{
    const char *target = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "target" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 2, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityByTarget( startEntity, target ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindClassNearestFacing, "library", "Finds the nearest entity of a class facing a direction", "server" )
{
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "position" );
    Vector direction = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "direction" );
    float maxDistance = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxDistance" );
    const char *className = LUA_BINDING_ARGUMENT( luaL_checkstring, 4, "className" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityClassNearestFacing( position, direction, maxDistance, strdup( className ) ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindGeneric, "library", "Finds an entity by its generic name", "server" )
{
    const char *generic = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "generic" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 2, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityGeneric( startEntity, generic ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindGenericNearest, "library", "Finds the nearest entity by its generic name", "server" )
{
    const char *generic = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "generic" );
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
    float maxDistance = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxDistance" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityGenericNearest( generic, position, maxDistance ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindGenericWithin, "library", "Finds an entity by its generic name within a radius", "server" )
{
    const char *generic = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "generic" );
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
    float maxDistance = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxDistance" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 4, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityGenericWithin( startEntity, generic, position, maxDistance ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindInSphere, "library", "Finds an entity within a radius", "server" )
{
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "position" );
    float radius = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "radius" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 3, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityInSphere( startEntity, position, radius ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindNearestFacing, "library", "Finds the nearest entity facing a direction", "server" )
{
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "position" );
    Vector direction = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "direction" );
    float maxDistance = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxDistance" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityNearestFacing( position, direction, maxDistance ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, FindProcedural, "library", "Finds an entity by its procedural name", "server" )
{
    const char *procedural = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "procedural" );
    CBaseEntity *startEntity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 2, NULL, "startEntity" );

    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FindEntityProcedural( procedural, startEntity ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity found, or NULL if not found." )

LUA_BINDING_BEGIN( Entities, IsClearing, "library", "Checks if the entity list is being cleared", "server" )
{
    lua_pushboolean( L, gEntList.IsClearingEntities() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the entity list is being cleared, false otherwise." )

LUA_BINDING_BEGIN( Entities, NotifyCreate, "library", "Notifies the entity list that an entity was created", "server" )
{
    CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    gEntList.NotifyCreateEntity( entity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entities, NotifySpawn, "library", "Notifies the entity list that an entity was spawned", "server" )
{
    CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );

    gEntList.NotifySpawn( entity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entities, GetEdictCount, "library", "Gets the number of edicts", "server" )
{
    lua_pushinteger( L, gEntList.NumberOfEdicts() );
    return 1;
}
LUA_BINDING_END( "number", "The number of edicts." )

LUA_BINDING_BEGIN( Entities, ReportFlagsChanged, "library", "Reports that an entity's flags have changed", "server" )
{
    CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    unsigned int flagsOld = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "flagsOld" );
    unsigned int flagsNow = LUA_BINDING_ARGUMENT( luaL_checkinteger, 3, "flagsNow" );

    gEntList.ReportEntityFlagsChanged( entity, flagsOld, flagsNow );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Entities, ResetDeleteList, "library", "Resets the delete list", "server" )
{
    lua_pushinteger( L, gEntList.ResetDeleteList() );
    return 1;
}
LUA_BINDING_END( "number", "The number of entities in the delete list." )

#endif  // CLIENT_DLL

LUA_BINDING_BEGIN( Entities, FirstInList, "library", "Gets the first entity in the list" )
{
#ifdef CLIENT_DLL
    CBaseEntity::PushLuaInstanceSafe( L, ClientEntityList().FirstBaseEntity() );
#else
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.FirstEnt() );
#endif

    return 1;
}
LUA_BINDING_END( "Entity", "The first entity in the entity list." )

LUA_BINDING_BEGIN( Entities, GetAll, "library", "Gets all entities in the list" )
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
        lua_pushinteger( L, ++i );  // 1-based index for Lua
        CBaseEntity::PushLuaInstanceSafe( L, pEnt );
        lua_settable( L, -3 );
    }

    return 1;
}
LUA_BINDING_END( "table", "A table of all entities in the entity list." )

LUA_BINDING_BEGIN( Entities, NextInList, "library", "Gets the next entity in the list" )
{
    CBaseEntity *pCurrent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 1, NULL, "startingEntity" );

#ifdef CLIENT_DLL
    CBaseEntity::PushLuaInstanceSafe( L, ClientEntityList().NextBaseEntity( pCurrent ) );
#else
    CBaseEntity::PushLuaInstanceSafe( L, gEntList.NextEnt( pCurrent ) );
#endif

    return 1;
}
LUA_BINDING_END( "Entity", "The next entity in the entity list." )

LUA_BINDING_BEGIN( Entities, GetCount, "library", "Gets the number of entities in the list" )
{
#ifdef CLIENT_DLL
    lua_pushinteger( L, ClientEntityList().NumberOfEntities() );
#else
    lua_pushinteger( L, gEntList.NumberOfEntities() );
#endif

    return 1;
}
LUA_BINDING_END( "number", "The number of entities in the entity list." )

/*
** Open gEntList library
*/
LUALIB_API int luaopen_Entities( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Entities );
    return 1;
}
