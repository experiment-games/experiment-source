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

static int CExperiment_Player_BecomeRagdollOnClient( lua_State *L )
{
#ifdef CLIENT_DLL
    CBaseEntity::PushLuaInstanceSafe( L, luaL_checkexperimentplayer( L, 1 )->BecomeRagdollOnClient() );
#else
    lua_pushboolean( L, luaL_checkexperimentplayer( L, 1 )->BecomeRagdollOnClient( luaL_checkvector( L, 2 ) ) );
#endif
    return 1;
}

static int CExperiment_Player_CalculateIKLocks( lua_State *L )
{
    luaL_checkexperimentplayer( L, 1 )->CalculateIKLocks( luaL_checknumber( L, 2 ) );
    return 0;
}

static int CExperiment_Player_CanSprint( lua_State *L )
{
    lua_pushboolean( L, luaL_checkexperimentplayer( L, 1 )->CanSprint() );
    return 1;
}

static int CExperiment_Player_DoAnimationEvent( lua_State *L )
{
    luaL_checkexperimentplayer( L, 1 )->DoAnimationEvent( ( PlayerAnimEvent_t )( int )luaL_checknumber( L, 2 ), luaL_optinteger( L, 3, 0 ) );
    return 0;
}

static int CExperiment_Player_AnimationResetGestureSlot( lua_State *L )
{
    luaL_checkexperimentplayer( L, 1 )->GetAnimState()->ResetGestureSlot( ( int )luaL_checknumber( L, 2 ) );
    return 0;
}

static int CExperiment_Player_AnimationRestartGesture( lua_State *L )
{
    luaL_checkexperimentplayer( L, 1 )->GetAnimState()->RestartGesture(
        ( int )luaL_checknumber( L, 2 ),
        ( Activity )( int )luaL_checknumber( L, 3 ),
        ( bool )luaL_optboolean( L, 4, false ) );
    return 0;
}

static int CExperiment_Player_AnimationRestartMainSequence( lua_State *L )
{
    luaL_checkexperimentplayer( L, 1 )->GetAnimState()->RestartMainSequence();
    return 0;
}

static int CExperiment_Player_AnimationSetGestureSequence( lua_State *L )
{
    luaL_checkexperimentplayer( L, 1 )->GetAnimState()->AddVCDSequenceToGestureSlot(
        ( int )luaL_checknumber( L, 2 ),
        ( int )luaL_checknumber( L, 3 ) );
    return 0;
}

static int CExperiment_Player_AnimationSetGestureWeight( lua_State *L )
{
    luaL_checkexperimentplayer( L, 1 )->GetAnimState()->SetGestureWeight(
        ( int )luaL_checknumber( L, 2 ),
        ( float )luaL_checknumber( L, 3 ) );
    return 0;
}

static int CExperiment_Player_KeyDown( lua_State *L )
{
    luaL_checkexperimentplayer( L, 1 )->KeyDown( luaL_checknumber( L, 2 ) );
    return 0;
}

static int CExperiment_Player___index( lua_State *L )
{
    CExperiment_Player *pPlayer = luaL_toexperimentplayer( L, 1 );

    LUA_METATABLE_INDEX_CHECK_VALID( L, CBaseEntity_IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pPlayer );

    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pPlayer );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, LUA_EXPERIMENTPLAYERLIBNAME );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, LUA_BASEPLAYERLIBNAME );

    lua_pushnil( L );
    return 1;
}

static int CExperiment_Player___newindex( lua_State *L )
{
    CExperiment_Player *pPlayer = luaL_toexperimentplayer( L, 1 );

    if ( pPlayer == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index a NULL entity", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    const char *field = luaL_checkstring( L, 2 );

#ifdef CLIENT_DLL
    if ( Q_strcmp( field, "m_fNextThinkPushAway" ) == 0 )
    {
        // TODO: This doesn't exist in SourceSDK2013?
        // pPlayer->m_fNextThinkPushAway = luaL_checknumber(L, 3);
    }
    else
    {
#endif
        LUA_GET_REF_TABLE( L, pPlayer );
        lua_pushvalue( L, 3 );
        lua_setfield( L, -2, field );
        lua_pop( L, 1 );
#ifdef CLIENT_DLL
    }
#endif

    return 0;
}

static int CExperiment_Player___eq( lua_State *L )
{
    lua_pushboolean( L, luaL_toexperimentplayer( L, 1 ) == luaL_toexperimentplayer( L, 2 ) );
    return 1;
}

static int CExperiment_Player___tostring( lua_State *L )
{
    CExperiment_Player *pPlayer = luaL_toexperimentplayer( L, 1 );
    if ( pPlayer == NULL )
        lua_pushstring( L, "NULL" );
    else
        lua_pushfstring( L, "CExperiment_Player: %d \"%s\"", pPlayer->GetUserID(), pPlayer->GetPlayerName() );
    return 1;
}

static const luaL_Reg CExperiment_Playermeta[] = {
    { "BecomeRagdollOnClient", CExperiment_Player_BecomeRagdollOnClient },
    { "CalculateIKLocks", CExperiment_Player_CalculateIKLocks },
    { "CanSprint", CExperiment_Player_CanSprint },
    { "DoAnimationEvent", CExperiment_Player_DoAnimationEvent },
    { "AnimationRestartGesture", CExperiment_Player_AnimationRestartGesture },
    { "AnimationRestartMainSequence", CExperiment_Player_AnimationRestartMainSequence },
    { "AnimationResetGestureSlot", CExperiment_Player_AnimationResetGestureSlot },
    { "AnimationSetGestureSequence", CExperiment_Player_AnimationSetGestureSequence },
    { "AnimationSetGestureWeight", CExperiment_Player_AnimationSetGestureWeight },
    { "KeyDown", CExperiment_Player_KeyDown },
    { "__index", CExperiment_Player___index },
    { "__newindex", CExperiment_Player___newindex },
    { "__eq", CExperiment_Player___eq },
    { "__tostring", CExperiment_Player___tostring },
    { NULL, NULL } };

static int luasrc_ToExperimentPlayer( lua_State *L )
{
    CBaseEntity::PushLuaInstanceSafe( L, ToExperimentPlayer( lua_toentity( L, 1 ) ) );
    return 1;
}

static const luaL_Reg CExperiment_Player_funcs[] = {
    { "ToPlayer", luasrc_ToExperimentPlayer },
    { NULL, NULL } };

/*
** Open CExperiment_Player object
*/
LUALIB_API int luaopen_CExperiment_Player_shared( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_EXPERIMENTPLAYERLIBNAME );
    luaL_register( L, NULL, CExperiment_Playermeta );
    lua_pushstring( L, "Entity" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */
    luaL_register( L, LUA_GNAME, CExperiment_Player_funcs );
    lua_pop( L, 1 );
    return 1;
}
