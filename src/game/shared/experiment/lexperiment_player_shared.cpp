//========= Copyright © 1996-2005, Valve Corporation, All rights reserved.
//============//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"

#include "luamanager.h"
#include "luasrclib.h"
#include "lexperiment_player_shared.h"
#include "lbaseentity_shared.h"
#ifdef CLIENT_DLL
#include "lc_baseanimating.h"
#else
#include "lbaseanimating.h"
#endif
#include "mathlib/lvector.h"

#include "experiment_player_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CExperiment_Player *luaL_toexperimentplayer( lua_State *L, int idx )
{
    CBaseHandle *hPlayer =
        dynamic_cast< CBaseHandle * >( ( CBaseHandle * )lua_touserdata( L, idx ) );
    if ( hPlayer == NULL )
        return NULL;
    return dynamic_cast< lua_CExperiment_Player * >( hPlayer->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CExperiment_Player *luaL_checkexperimentplayer( lua_State *L, int narg )
{
    lua_CExperiment_Player *d = luaL_toexperimentplayer( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "Player expected, got NULL entity" );
    return d;
}

LUALIB_API lua_CExperiment_Player *luaL_optexperimentplayer( lua_State *L, int narg, CExperiment_Player *def )
{
    return luaL_opt( L, luaL_checkexperimentplayer, narg, def );
}

LUA_REGISTRATION_INIT( CExperimentPlayer );

#ifdef CLIENT_DLL
LUA_BINDING_BEGIN( CExperimentPlayer, BecomeRagdollOnClient, "class", "Become ragdoll on client.", "client" )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    CBaseEntity::PushLuaInstanceSafe( L, player->BecomeRagdollOnClient() );
    return 1;
}
LUA_BINDING_END( "entity", "The ragdoll entity" )
#else
LUA_BINDING_BEGIN( CExperimentPlayer, BecomeRagdollOnClient, "class", "Become ragdoll on client.", "server" )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    lua_pushboolean( L, player->BecomeRagdollOnClient( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "force" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player can (and has) become a ragdoll" )
#endif

LUA_BINDING_BEGIN( CExperimentPlayer, CalculateIkLocks, "class", "Calculate IK locks." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    player->CalculateIKLocks( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "currentTime" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CExperimentPlayer, CanSprint, "class", "Check if the player can sprint." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    lua_pushboolean( L, player->CanSprint() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player can sprint" )

LUA_BINDING_BEGIN( CExperimentPlayer, DoAnimationEvent, "class", "Do an animation event." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    int iEvent = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "event" );
    int iData = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 3, 0, "data" );
    player->DoAnimationEvent( ( PlayerAnimEvent_t )iEvent, iData );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CExperimentPlayer, AnimationResetGestureSlot, "class", "Reset a gesture slot." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    player->GetAnimState()->ResetGestureSlot( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "slot" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CExperimentPlayer, AnimationRestartGesture, "class", "Restart a gesture." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    int iSlot = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "slot" );
    int iActivity = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "activity" );
    bool bAutoKill = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "autoKill" );
    player->GetAnimState()->RestartGesture( iSlot, ( Activity )iActivity, bAutoKill );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CExperimentPlayer, AnimationRestartMainSequence, "class", "Restart the main sequence." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    player->GetAnimState()->RestartMainSequence();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CExperimentPlayer, AnimationSetGestureSequence, "class", "Set a gesture sequence." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    int iSlot = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "slot" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "sequence" );
    player->GetAnimState()->AddVCDSequenceToGestureSlot( iSlot, iSequence );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CExperimentPlayer, AnimationSetGestureWeight, "class", "Set a gesture weight." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    int iSlot = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "slot" );
    float flWeight = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "weight" );
    player->GetAnimState()->SetGestureWeight( iSlot, flWeight );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CExperimentPlayer, KeyDown, "class", "Key down." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    player->KeyDown( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" ) );
    return 0;
}
LUA_BINDING_END()

#ifndef CLIENT_DLL
LUA_BINDING_BEGIN( CExperimentPlayer, GetPlayerModelType, "class", "Get the player model type.", "server" )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "entity" );
    lua_pushinteger( L, player->GetPlayerModelType() );
    return 1;
}
LUA_BINDING_END( "number", "The player model type" )
#endif

LUA_BINDING_BEGIN( CExperimentPlayer, __eq, "class", "Metamethod for comparing two entities." )
{
    CExperiment_Player *playerA = LUA_BINDING_ARGUMENT( luaL_toexperimentplayer, 1, "entityA" );
    CExperiment_Player *playerB = LUA_BINDING_ARGUMENT( luaL_toexperimentplayer, 2, "entityB" );

    lua_pushboolean( L, playerA == playerB );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entities are equal" )

LUA_BINDING_BEGIN( CExperimentPlayer, __tostring, "class", "Metamethod for converting the entity to a string." )
{
    CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_toexperimentplayer, 1, "entity" );

    if ( player == NULL )
    {
        lua_pushstring( L, "NULL" );
    }
    else
    {
        lua_pushfstring( L, "CExperiment_Player: %d \"%s\"", player->GetUserID(), player->GetPlayerName() );
    }

    return 1;
}
LUA_BINDING_END( "string", "The string representation of the entity" )

// Experiment; Is this at all useful to Lua?
// static int luasrc_ToExperimentPlayer( lua_State *L )
//{
//    CBaseEntity::PushLuaInstanceSafe( L, ToExperimentPlayer( lua_toentity( L, 1 ) ) );
//    return 1;
//}
//
// static const luaL_Reg CExperiment_Player_funcs[] = {
//    { "ToPlayer", luasrc_ToExperimentPlayer },
//    { NULL, NULL } };

/*
** Open CExperiment_Player object
*/
LUALIB_API int luaopen_CExperiment_Player_shared( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_EXPERIMENTPLAYERLIBNAME );

    LUA_REGISTRATION_COMMIT( CExperimentPlayer );

    lua_pushstring( L, "Entity" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */

    // luaL_register( L, LUA_GNAME, CExperiment_Player_funcs );

    lua_pop( L, 1 );
    return 1;
}
