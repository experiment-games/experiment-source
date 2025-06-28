#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "basescripted.h"
#include "mathlib/lVector.h"
#include <ai_basenpc.h>
#include <ai_squad.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( EnemyAi );

LUA_BINDING_BEGIN( EnemyAi, GetScheduleId, "library", "Finds the schedule id by its name." )
{
    const char *scheduleName = luaL_checkstring( L, 1 );

    lua_pushinteger( L, CAI_BaseNPC::GetScheduleID( scheduleName ) );

    return 1;
}
LUA_BINDING_END( "integer", "The schedule enumeration ID or -1 if not found." );

LUA_BINDING_BEGIN( EnemyAi, GetActivityId, "library", "Finds the activity id by its name." )
{
    const char *activityName = luaL_checkstring( L, 1 );
    lua_pushinteger( L, CAI_BaseNPC::GetActivityID( activityName ) );
    return 1;
}
LUA_BINDING_END( "integer", "The activity enumeration ID or -1 if not found." );

LUA_BINDING_BEGIN( EnemyAi, GetConditionId, "library", "Finds the condition id by its name." )
{
    const char *conditionName = luaL_checkstring( L, 1 );
    lua_pushinteger( L, CAI_BaseNPC::GetConditionID( conditionName ) );
    return 1;
}
LUA_BINDING_END( "integer", "The condition enumeration ID or -1 if not found." );

LUA_BINDING_BEGIN( EnemyAi, GetTaskId, "library", "Finds the task id by its name." )
{
    const char *taskName = luaL_checkstring( L, 1 );
    lua_pushinteger( L, CAI_BaseNPC::GetTaskID( taskName ) );
    return 1;
}
LUA_BINDING_END( "integer", "The task enumeration ID or -1 if not found." );

// No use for this function in Lua I think?
//LUA_BINDING_BEGIN( EnemyAi, GetSquadSlotId, "library", "Finds the squad slot id by its name." )
//{
//    const char *slotName = luaL_checkstring( L, 1 );
//    lua_pushinteger( L, CAI_BaseNPC::GetSquadSlotID( slotName ) );
//    return 1;
//}
//LUA_BINDING_END( "integer", "The squad slot enumeration ID or -1 if not found." );

LUA_BINDING_BEGIN( EnemyAi, GetSquadLeader, "library", "Returns the squad leader of the given squad." )
{
    const char *squadName = luaL_checkstring( L, 1 );

    if ( !squadName || !squadName[0] )
    {
        CBaseEntity::PushLuaInstanceSafe( L, NULL );
        return 1;
    }

    CAI_Squad *pSquad = g_AI_SquadManager.FindSquad( MAKE_STRING( squadName ) );
    
    if ( !pSquad )
    {
        CBaseEntity::PushLuaInstanceSafe( L, NULL );
        return 1;
    }

    CAI_BaseNPC *pLeader = pSquad->GetLeader();

    if ( !pLeader )
    {
        CBaseEntity::PushLuaInstanceSafe( L, NULL );
        return 1;
    }

    CBaseEntity::PushLuaInstanceSafe( L, pLeader );
    return 1;
}
LUA_BINDING_END( "Entity", "The squad leader of the given squad or null if not found." );

LUA_BINDING_BEGIN( EnemyAi, GetSquadMemberCount, "library", "Returns the number of members in the given squad." )
{
    const char *squadName = luaL_checkstring( L, 1 );

    if ( !squadName || !squadName[0] )
    {
        lua_pushinteger( L, 0 );
        return 1;
    }

    CAI_Squad *pSquad = g_AI_SquadManager.FindSquad( MAKE_STRING( squadName ) );

    if ( !pSquad )
    {
        lua_pushinteger( L, 0 );
        return 1;
    }

    lua_pushinteger( L, pSquad->NumMembers() );
    return 1;
}
LUA_BINDING_END( "integer", "The number of members in the given squad." );

LUA_BINDING_BEGIN( EnemyAi, GetSquadMembers, "library", "Returns a table of squad members for the given squad." )
{
    const char *squadName = luaL_checkstring( L, 1 );
    if ( !squadName || !squadName[0] )
    {
        lua_newtable( L );
        return 1;
    }
    CAI_Squad *pSquad = g_AI_SquadManager.FindSquad( MAKE_STRING( squadName ) );
    if ( !pSquad )
    {
        lua_newtable( L );
        return 1;
    }

    lua_newtable( L );
    
    AISquadIter_t iter;
    CAI_BaseNPC *pSquadmate = pSquad->GetFirstMember( &iter );
    int index = 1;  // Lua tables are 1-indexed

    while ( pSquadmate )
    {
        // Ignore squadmates that can't take damage. This is primarily to ignore npc_enemyfinders.
        if ( pSquadmate->m_takedamage != DAMAGE_NO )
        {
            if ( pSquadmate != NULL )
            {
                CBaseEntity::PushLuaInstanceSafe( L, pSquadmate );
                lua_rawseti( L, -2, index++ );  // Set the value at index in the table
            }
        }

        pSquadmate = pSquad->GetNextMember( &iter );
    }

    return 1;
}
LUA_BINDING_END( "table", "A table of squad members for the given squad." );

/*
** Open gEntList library
*/
LUALIB_API int luaopen_EnemyAi( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( EnemyAi );
    return 1;
}
