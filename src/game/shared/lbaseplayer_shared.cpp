#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "steam/steam_api.h"
#include "lbaseplayer_shared.h"
#ifdef CLIENT_DLL
#include "lc_baseanimating.h"
#include <voice_status.h>
#else
#include "lbaseanimating.h"
#endif
#include "lbasecombatweapon_shared.h"
#include "lbaseentity_shared.h"
#include "SoundEmitterSystem/lisoundemittersystembase.h"
#include "mathlib/lvector.h"
#include "lvphysics_interface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CBasePlayer *lua_toplayer( lua_State *L, int idx )
{
    CBaseHandle *phPlayer =
        dynamic_cast< CBaseHandle * >( ( CBaseHandle * )lua_touserdata( L, idx ) );
    if ( phPlayer == NULL )
        return NULL;
    return dynamic_cast< lua_CBasePlayer * >( phPlayer->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CBasePlayer *luaL_checkplayer( lua_State *L, int narg )
{
    lua_CBasePlayer *d = lua_toplayer( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CBasePlayer expected, got NULL entity" );
    return d;
}

LUALIB_API lua_CBasePlayer *luaL_optplayer( lua_State *L, int narg, CBasePlayer *def )
{
    return luaL_opt( L, luaL_checkplayer, narg, def );
}

LUA_REGISTRATION_INIT( CBasePlayer )

LUA_BINDING_BEGIN( CBasePlayer, AbortReload, "class", "Abort the current reload." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->AbortReload();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, AddToPlayerSimulationList, "class", "Add the player to the simulation list." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_CBaseEntity *other = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" );
    player->AddToPlayerSimulationList( other );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, CacheVehicleView, "class", "Cache the vehicle view." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->CacheVehicleView();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, ClearPlayerSimulationList, "class", "Clear the player simulation list." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ClearPlayerSimulationList();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, ClearZoomOwner, "class", "Clear the zoom owner." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ClearZoomOwner();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, CurrentCommandNumber, "class", "Get the current command number." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->CurrentCommandNumber() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, DoMuzzleFlash, "class", "Do the muzzle flash." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->DoMuzzleFlash();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, ExitLadder, "class", "Exit the ladder." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ExitLadder();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, GetEyeAngles, "class", "Get the player's eye angles." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    QAngle v = player->EyeAngles();
    lua_pushangle( L, v );
    return 1;
}
LUA_BINDING_END( "Angle", "The player's eye angles." )

LUA_BINDING_BEGIN( CBasePlayer, GetEyePositionAndVectors, "class", "Get the player's eye position and vectors." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector v1, v2, v3, v4;
    player->EyePositionAndVectors( &v1, &v2, &v3, &v4 );
    lua_pushvector( L, v1 );
    lua_pushvector( L, v2 );
    lua_pushvector( L, v3 );
    lua_pushvector( L, v4 );
    return 4;
}
LUA_BINDING_END( "Vector", "The player's eye position.", "Vector", "The player's eye forward vector.", "Vector", "The player's eye right vector.", "Vector", "The player's eye up vector." )

LUA_BINDING_BEGIN( CBasePlayer, GetEyeVectors, "class", "Get the player's eye vectors." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector v1, v2, v3;
    player->EyeVectors( &v1, &v2, &v3 );
    lua_pushvector( L, v1 );
    lua_pushvector( L, v2 );
    lua_pushvector( L, v3 );
    return 3;
}
LUA_BINDING_END( "Vector", "The player's eye forward vector.", "Vector", "The player's eye right vector.", "Vector", "The player's eye up vector." )

LUA_BINDING_BEGIN( CBasePlayer, FindUseEntity, "class", "Find the entity the player is using." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity *pUseEntity = player->FindUseEntity();
    CBaseEntity::PushLuaInstanceSafe( L, pUseEntity );
    return 1;
}
LUA_BINDING_END( "entity", "The entity the player is using." )

LUA_BINDING_BEGIN( CBasePlayer, GetActiveWeapon, "class", "Get the player's active weapon." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseCombatWeapon *pWeapon = player->GetActiveWeapon();
    CBaseEntity::PushLuaInstanceSafe( L, pWeapon );
    return 1;
}
LUA_BINDING_END( "weapon", "The player's active weapon." )

// FIXME: move to CBaseCombatCharacter
LUA_BINDING_BEGIN( CBasePlayer, GetAmmoCount, "class", "Get the player's ammo count." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
        default:
            lua_pushinteger( L, player->GetAmmoCount( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" ) ) );
            break;
        case LUA_TSTRING:
            lua_pushinteger( L, player->GetAmmoCount( ( char * )LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ) ) );
            break;
    }

    return 1;
}
LUA_BINDING_END( "number", "The player's ammo count." )

// TODO: Is this correct? Is AutoaimVector the same as AimVector in gmod?
LUA_BINDING_BEGIN( CBasePlayer, GetAimVector, "class", "Get the player's autoaim vector." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector v = player->GetAutoaimVector( LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 1, "scale" ) );
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's autoaim vector." )

LUA_BINDING_BEGIN( CBasePlayer, GetBonusChallenge, "class", "Get the player's bonus challenge." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetBonusChallenge() );
    return 1;
}
LUA_BINDING_END( "number", "The player's bonus challenge." )

LUA_BINDING_BEGIN( CBasePlayer, GetBonusProgress, "class", "Get the player's bonus progress." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetBonusProgress() );
    return 1;
}
LUA_BINDING_END( "number", "The player's bonus progress." )

LUA_BINDING_BEGIN( CBasePlayer, GetDeathTime, "class", "Get the player's death time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetDeathTime() );
    return 1;
}
LUA_BINDING_END( "number", "The player's death time." )

LUA_BINDING_BEGIN( CBasePlayer, GetDefaultFov, "class", "Get the player's default FOV." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetDefaultFOV() );
    return 1;
}
LUA_BINDING_END( "number", "The player's default FOV." )

// static int CBasePlayer_GetEfNoInterpParity( lua_State *L )
//{
//     lua_pushinteger( L, luaL_checkplayer( L, 1 )->GetEFNoInterpParity() );
//     return 1;
// }

LUA_BINDING_BEGIN( CBasePlayer, GetFov, "class", "Get the player's FOV." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetFOV() );
    return 1;
}
LUA_BINDING_END( "number", "The player's FOV." )

LUA_BINDING_BEGIN( CBasePlayer, GetFovDistanceAdjustFactor, "class", "Get the player's FOV distance adjust factor." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetFOVDistanceAdjustFactor() );
    return 1;
}
LUA_BINDING_END( "number", "The player's FOV distance adjust factor." )

LUA_BINDING_BEGIN( CBasePlayer, GetFovTime, "class", "Get the player's FOV time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetFOVTime() );
    return 1;
}
LUA_BINDING_END( "number", "The player's FOV time." )

LUA_BINDING_BEGIN( CBasePlayer, GetHands, "class", "Get the player's hands." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetHands() );
    return 1;
}
LUA_BINDING_END( "entity", "The player's hands." )

LUA_BINDING_BEGIN( CBasePlayer, SetHands, "class", "Set the player's hands." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetHands( LUA_BINDING_ARGUMENT( luaL_checkanimating, 2, "hands" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, GetImpulse, "class", "Get the player's impulse." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetImpulse() );
    return 1;
}
LUA_BINDING_END( "number", "The player's impulse." )

LUA_BINDING_BEGIN( CBasePlayer, GetLaggedMovementValue, "class", "Get the player's lagged movement value." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetLaggedMovementValue() );
    return 1;
}
LUA_BINDING_END( "number", "The player's lagged movement value." )

LUA_BINDING_BEGIN( CBasePlayer, GetLastKnownPlaceName, "class", "Get the player's last known place name." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushstring( L, player->GetLastKnownPlaceName() );
    return 1;
}
LUA_BINDING_END( "string", "The player's last known place name." )

LUA_BINDING_BEGIN( CBasePlayer, GetNextAttack, "class", "Get the player's next attack." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetNextAttack() );
    return 1;
}
LUA_BINDING_END( "number", "The player's next attack." )

LUA_BINDING_BEGIN( CBasePlayer, GetObserverMode, "class", "Get the player's observer mode." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetObserverMode() );
    return 1;
}
LUA_BINDING_END( "number", "The player's observer mode." )

LUA_BINDING_BEGIN( CBasePlayer, GetObserverTarget, "class", "Get the player's observer target." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetObserverTarget() );
    return 1;
}
LUA_BINDING_END( "entity", "The player's observer target." )

// Experiment; disabled since this is an internal function
// LUA_BINDING_BEGIN( CBasePlayer, GetOffset_m_Local, "class", "Get the offset of m_Local." )
//{
//    lua_pushinteger( L, CBasePlayer::GetOffset_m_Local() );
//    return 1;
//}
// LUA_BINDING_END( "number", "The offset of m_Local." )

LUA_BINDING_BEGIN( CBasePlayer, GetPlayerLocalData, "class", "Get the player's local data." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_newtable( L );

    lua_pushinteger( L, player->m_Local.m_iHideHUD );
    lua_setfield( L, -2, "m_iHideHUD" );

    lua_pushnumber( L, player->m_Local.m_flFOVRate );
    lua_setfield( L, -2, "m_flFOVRate" );

    lua_pushboolean( L, player->m_Local.m_bDucked );
    lua_setfield( L, -2, "m_bDucked" );
    lua_pushboolean( L, player->m_Local.m_bDucking );
    lua_setfield( L, -2, "m_bDucking" );
    lua_pushboolean( L, player->m_Local.m_bInDuckJump );
    lua_setfield( L, -2, "m_bInDuckJump" );
    lua_pushnumber( L, player->m_Local.m_flDucktime );
    lua_setfield( L, -2, "m_flDucktime" );
    lua_pushnumber( L, player->m_Local.m_flDuckJumpTime );
    lua_setfield( L, -2, "m_flDuckJumpTime" );
    lua_pushnumber( L, player->m_Local.m_flJumpTime );
    lua_setfield( L, -2, "m_flJumpTime" );
    lua_pushinteger( L, player->m_Local.m_nStepside );
    lua_setfield( L, -2, "m_nStepside" );
    lua_pushnumber( L, player->m_Local.m_flFallVelocity );
    lua_setfield( L, -2, "m_flFallVelocity" );
    lua_pushinteger( L, player->m_Local.m_nOldButtons );
    lua_setfield( L, -2, "m_nOldButtons" );

#ifdef CLIENT_DLL
    lua_pushvector( L, player->m_Local.m_vecClientBaseVelocity );
    lua_setfield( L, -2, "m_vecClientBaseVelocity" );
#endif

    QAngle v = player->m_Local.m_vecPunchAngle;

    lua_pushangle( L, v );
    lua_setfield( L, -2, "m_vecPunchAngle" );

    v = player->m_Local.m_vecPunchAngleVel;
    lua_pushangle( L, v );
    lua_setfield( L, -2, "m_vecPunchAngleVel" );

    lua_pushboolean( L, player->m_Local.m_bDrawViewmodel );
    lua_setfield( L, -2, "m_bDrawViewmodel" );
    lua_pushboolean( L, player->m_Local.m_bWearingSuit );
    lua_setfield( L, -2, "m_bWearingSuit" );

    lua_pushboolean( L, player->m_Local.m_bPoisoned );
    lua_setfield( L, -2, "m_bPoisoned" );
    lua_pushnumber( L, player->m_Local.m_flStepSize );
    lua_setfield( L, -2, "m_flStepSize" );

    lua_pushboolean( L, player->m_Local.m_bAllowAutoMovement );
    lua_setfield( L, -2, "m_bAllowAutoMovement" );

    lua_pushboolean( L, player->m_Local.m_bSlowMovement );
    lua_setfield( L, -2, "m_bSlowMovement" );

    return 1;
}
LUA_BINDING_END( "table", "The player's local data." )

LUA_BINDING_BEGIN( CBasePlayer, GetPlayerMaxs, "class", "Get the player's maxs." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector v = player->GetPlayerMaxs();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's maxs." )

LUA_BINDING_BEGIN( CBasePlayer, GetPlayerMins, "class", "Get the player's mins." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector v = player->GetPlayerMins();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's mins." )

LUA_BINDING_BEGIN( CBasePlayer, GetPlayerName, "class", "Get the player's name." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushstring( L, player->GetPlayerName() );
    return 1;
}
LUA_BINDING_END( "string", "The player's name." )

LUA_BINDING_BEGIN( CBasePlayer, GetPreviouslyPredictedOrigin, "class", "Get the player's previously predicted origin." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector v = player->GetPreviouslyPredictedOrigin();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's previously predicted origin." )

LUA_BINDING_BEGIN( CBasePlayer, GetPunchAngle, "class", "Get the player's punch angle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    QAngle v = player->GetPunchAngle();
    lua_pushangle( L, v );
    return 1;
}
LUA_BINDING_END( "Angle", "The player's punch angle." )

LUA_BINDING_BEGIN( CBasePlayer, GetStepSoundCache, "class", "Get the player's step sound cache." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_newtable( L );

    lua_pushinteger( L, 0 );
    lua_newtable( L );
    lua_pushsoundparameters( L, player->m_StepSoundCache[0].m_SoundParameters );
    lua_setfield( L, -2, "m_SoundParameters" );
    lua_pushinteger( L, player->m_StepSoundCache[0].m_usSoundNameIndex );
    lua_setfield( L, -2, "m_usSoundNameIndex" );
    lua_settable( L, -3 );

    lua_pushinteger( L, 1 );
    lua_newtable( L );
    lua_pushsoundparameters( L, player->m_StepSoundCache[1].m_SoundParameters );
    lua_setfield( L, -2, "m_SoundParameters" );
    lua_pushinteger( L, player->m_StepSoundCache[1].m_usSoundNameIndex );
    lua_setfield( L, -2, "m_usSoundNameIndex" );
    lua_settable( L, -3 );

    return 1;
}
LUA_BINDING_END( "table", "The player's step sound cache." )

LUA_BINDING_BEGIN( CBasePlayer, GetStepSoundVelocities, "class", "Get the player's step sound velocities." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    float velwalk, velrun;
    player->GetStepSoundVelocities( &velwalk, &velrun );
    lua_pushnumber( L, velwalk );
    lua_pushnumber( L, velrun );
    return 2;
}
LUA_BINDING_END( "number", "The player's step sound walk velocity.", "number", "The player's step sound run velocity." )

LUA_BINDING_BEGIN( CBasePlayer, GetSwimSoundTime, "class", "Get the player's swim sound time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetSwimSoundTime() );
    return 1;
}
LUA_BINDING_END( "number", "The player's swim sound time." )

LUA_BINDING_BEGIN( CBasePlayer, GetTimeBase, "class", "Get the player's time base." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetTimeBase() );
    return 1;
}
LUA_BINDING_END( "number", "The player's time base." )

LUA_BINDING_BEGIN( CBasePlayer, GetUseEntity, "class", "Get the player's use entity." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetUseEntity() );
    return 1;
}
LUA_BINDING_END( "entity", "The player's use entity." )

LUA_BINDING_BEGIN( CBasePlayer, GetUserId, "class", "Get the player's user ID." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetUserID() );
    return 1;
}
LUA_BINDING_END( "number", "The player's user ID." )

LUA_BINDING_BEGIN( CBasePlayer, GetVehicleEntity, "class", "Get the player's vehicle entity." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetVehicleEntity() );
    return 1;
}
LUA_BINDING_END( "entity", "The player's vehicle entity." )

LUA_BINDING_BEGIN( CBasePlayer, GetViewEntity, "class", "Get the player's view entity." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

#ifdef CLIENT_DLL
    if ( player != C_BasePlayer::GetLocalPlayer() )
    {
        // For now we can only return the view entity clientside, if it's the local player
        luaL_typeerror( L, 1, "local player" );
        return 0;
    }

    CBaseEntity::PushLuaInstanceSafe( L, cl_entitylist->GetEnt( render->GetViewEntity() ) );
#else
    CBaseEntity::PushLuaInstanceSafe( L, player->GetViewEntity() );
#endif

    return 1;
}
LUA_BINDING_END( "entity", "The player's view entity." )

// FIXME: Push CBaseViewModel instead
LUA_BINDING_BEGIN( CBasePlayer, GetViewModel, "class", "Get the player's view model." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetViewModel( ( int )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0, "number" ) ) );
    return 1;
}
LUA_BINDING_END( "entity", "The player's view model." )

LUA_BINDING_BEGIN( CBasePlayer, GetWaterJumpTime, "class", "Get the player's water jump time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetWaterJumpTime() );
    return 1;
}
LUA_BINDING_END( "number", "The player's water jump time." )

LUA_BINDING_BEGIN( CBasePlayer, GetWeapon, "class", "Get the player's weapon." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetWeapon( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" ) ) );
    return 1;
}
LUA_BINDING_END( "entity", "The player's weapon." )

LUA_BINDING_BEGIN( CBasePlayer, HintMessage, "class", "Hint a message to the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->HintMessage( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" ) );
    return 0;
}
LUA_BINDING_END()

// LUA_BINDING_BEGIN( CBasePlayer, IncrementEfNoInterpParity, "class", "Increment the player's EF no interp parity." )
// {
//     lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
//     player->IncrementEFNoInterpParity();
//     return 0;
// }
// LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, IsBot, "class", "Check if the player is a bot." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsBot() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is a bot." )

LUA_BINDING_BEGIN( CBasePlayer, IsHltv, "class", "Check if the player is HLTV." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsHLTV() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is HLTV." )

LUA_BINDING_BEGIN( CBasePlayer, IsInVehicle, "class", "Check if the player is in a vehicle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsInAVehicle() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is in a vehicle." )

LUA_BINDING_BEGIN( CBasePlayer, IsObserver, "class", "Check if the player is an observer." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsObserver() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is an observer." )

LUA_BINDING_BEGIN( CBasePlayer, IsPlayerUnderwater, "class", "Check if the player is underwater." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsPlayerUnderwater() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is underwater." )

LUA_BINDING_BEGIN( CBasePlayer, IsSpeaking, "class", "Check if the player is speaking." )
{
#ifdef CLIENT_DLL
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

    if ( ( player == C_BasePlayer::GetLocalPlayer() && GetClientVoiceMgr()->IsLocalPlayerSpeaking() ) || GetClientVoiceMgr()->IsPlayerSpeaking( player->entindex() ) )
    {
        lua_pushboolean( L, true );
        return 1;
    }
#else
    DevWarning( "CBasePlayer::IsSpeaking is not implemented on the server\n" );
#endif

    lua_pushboolean( L, false );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is speaking." )

LUA_BINDING_BEGIN( CBasePlayer, IsSuitEquipped, "class", "Check if the player has the HEV suit equipped." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsSuitEquipped() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player has the HEV suit equipped." )

LUA_BINDING_BEGIN( CBasePlayer, IsUseableEntity, "class", "Check if the player can use an entity." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L,
                     player->IsUseableEntity(
                         LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" ),
                         LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "use type" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player can use the entity." )

LUA_BINDING_BEGIN( CBasePlayer, ItemPostFrame, "class", "Call the player's item post frame." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ItemPostFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, ItemPreFrame, "class", "Call the player's item pre frame." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ItemPreFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, LeaveVehicle, "class", "Make the player leave their vehicle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->LeaveVehicle();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, GetMaxSpeed, "class", "Get the player's max speed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->MaxSpeed() );
    return 1;
}
LUA_BINDING_END( "number", "The player's max speed." )

// Experiment; not useful in Lua
// LUA_BINDING_BEGIN( CBasePlayer, MyCombatCharacterPointer, "class", "Get the player's combat character pointer." )
//{
//    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
//    CBaseEntity::PushLuaInstanceSafe( L, player->MyCombatCharacterPointer() );
//    return 1;
//}
// LUA_BINDING_END( "entity", "The player's combat character pointer." )

LUA_BINDING_BEGIN( CBasePlayer, PlayerUse, "class", "Make the player use an entity." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->PlayerUse();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, PlayStepSound, "class", "Play a step sound for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->PlayStepSound(
        LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" ),
        &LUA_BINDING_ARGUMENT( lua_tosurfacedata, 3, "surface data" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "volume" ),
        LUA_BINDING_ARGUMENT( luaL_checkboolean, 5, "force" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, PostThink, "class", "Call the player's post think." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->PostThink();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, PreThink, "class", "Call the player's pre think." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->PreThink();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, RemoveAllAmmo, "class", "Remove all ammo from the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->RemoveAllAmmo();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, RemoveAmmo, "class", "Remove ammo from the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

    switch ( lua_type( L, 3 ) )
    {
        case LUA_TNUMBER:
        default:
            player->RemoveAmmo(
                LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "amount" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "ammo type" ) );
            break;
        case LUA_TSTRING:
            player->RemoveAmmo(
                LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "amount" ),
                LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "ammo name" ) );
            break;
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, RemoveFromPlayerSimulationList, "class", "Remove an entity from the player's simulation list." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->RemoveFromPlayerSimulationList( LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, ResetAutoAim, "class", "Reset the player's autoaim." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ResetAutoaim();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, ResetObserverMode, "class", "Reset the player's observer mode." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ResetObserverMode();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SelectItem, "class", "Select an item for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SelectItem(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "item name" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "slot" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SelectLastItem, "class", "Select the player's last item." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SelectLastItem();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetAmmoCount, "class", "Set the player's ammo count." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetAmmoCount(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "amount" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "ammo type" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetAnimation, "class", "Set the player's animation." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetAnimation( ( PLAYER_ANIM )( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "animation" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetAnimationExtension, "class", "Set the player's animation extension." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetAnimationExtension( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "extension" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetBloodColor, "class", "Set the player's blood color." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetBloodColor( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "color" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetFov, "class", "Set the player's field of view." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L,
                     player->SetFOV(
                         LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" ),
                         LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "fov" ),
                         LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "zoom rate" ),
                         LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 0, "zoom time" ) ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetLadderNormal, "class", "Set the player's ladder normal." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetLadderNormal( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "normal" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetMaxSpeed, "class", "Set the player's max speed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetMaxSpeed( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetNextAttack, "class", "Set the player's next attack." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetNextAttack( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetPlayerLocalData, "class", "Set the player's local data." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( Q_strcmp( field, "m_iHideHUD" ) == 0 )
        player->m_Local.m_iHideHUD = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );

    else if ( Q_strcmp( field, "m_flFOVRate" ) == 0 )
        player->m_Local.m_flFOVRate = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );

    else if ( Q_strcmp( field, "m_bDucked" ) == 0 )
        player->m_Local.m_bDucked = ( bool )LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "value" );
    else if ( Q_strcmp( field, "m_bDucking" ) == 0 )
        player->m_Local.m_bDucking = ( bool )LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "value" );
    else if ( Q_strcmp( field, "m_bInDuckJump" ) == 0 )
        player->m_Local.m_bInDuckJump = ( bool )LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "value" );
    else if ( Q_strcmp( field, "m_flDucktime" ) == 0 )
        player->m_Local.m_flDucktime = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );
    else if ( Q_strcmp( field, "m_flDuckJumpTime" ) == 0 )
        player->m_Local.m_flDuckJumpTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );
    else if ( Q_strcmp( field, "m_flJumpTime" ) == 0 )
        player->m_Local.m_flJumpTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );
    else if ( Q_strcmp( field, "m_nStepside" ) == 0 )
        player->m_Local.m_nStepside = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );
    else if ( Q_strcmp( field, "m_flFallVelocity" ) == 0 )
        player->m_Local.m_flFallVelocity = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );
    else if ( Q_strcmp( field, "m_nOldButtons" ) == 0 )
        player->m_Local.m_nOldButtons = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );

#ifdef CLIENT_DLL
    else if ( Q_strcmp( field, "m_vecClientBaseVelocity" ) == 0 )
        player->m_Local.m_vecClientBaseVelocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "value" );
#endif

    else if ( Q_strcmp( field, "m_vecPunchAngle" ) == 0 )
        player->m_Local.m_vecPunchAngle = LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "value" );

    else if ( Q_strcmp( field, "m_vecPunchAngleVel" ) == 0 )
        player->m_Local.m_vecPunchAngleVel = LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "value" );
    else if ( Q_strcmp( field, "m_bDrawViewmodel" ) == 0 )
        player->m_Local.m_bDrawViewmodel = ( bool )LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "value" );
    else if ( Q_strcmp( field, "m_bWearingSuit" ) == 0 )
        player->m_Local.m_bWearingSuit = ( bool )LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "value" );
    else if ( Q_strcmp( field, "m_bPoisoned" ) == 0 )
        player->m_Local.m_bPoisoned = ( bool )LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "value" );
    else if ( Q_strcmp( field, "m_flStepSize" ) == 0 )
        player->m_Local.m_flStepSize = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );
    else if ( Q_strcmp( field, "m_bAllowAutoMovement" ) == 0 )
        player->m_Local.m_bAllowAutoMovement = ( bool )LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "value" );

    else if ( Q_strcmp( field, "m_bSlowMovement" ) == 0 )
        player->m_Local.m_bSlowMovement = ( bool )LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "value" );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetPlayerUnderwater, "class", "Set whether the player is underwater." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetPlayerUnderwater( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "underwater" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetPreviouslyPredictedOrigin, "class", "Set the player's previously predicted origin." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetPreviouslyPredictedOrigin( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetPunchAngle, "class", "Set the player's punch angle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetPunchAngle( LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetStepSoundCache, "class", "Set the player's step sound cache." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int index = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "field" );

    if ( index == 0 )
    {
        if ( Q_strcmp( field, "m_SoundParameters" ) == 0 )
            player->m_StepSoundCache[0].m_SoundParameters = lua_tosoundparameters( L, 4 );
        else if ( Q_strcmp( field, "m_usSoundNameIndex" ) == 0 )
            player->m_StepSoundCache[0].m_usSoundNameIndex = ( unsigned short )luaL_checkinteger( L, 4 );
    }
    else if ( index == 1 )
    {
        if ( Q_strcmp( field, "m_SoundParameters" ) == 0 )
            player->m_StepSoundCache[1].m_SoundParameters = lua_tosoundparameters( L, 4 );
        else if ( Q_strcmp( field, "m_usSoundNameIndex" ) == 0 )
            player->m_StepSoundCache[1].m_usSoundNameIndex = ( unsigned short )luaL_checkinteger( L, 4 );
    }
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetSuitUpdate, "class", "Set the player's suit update." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetSuitUpdate(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "group" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "repeat" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetSwimSoundTime, "class", "Set the player's swim sound time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetSwimSoundTime( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SetWaterJumpTime, "class", "Set the player's water jump time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetWaterJumpTime( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SharedSpawn, "class", "Shared spawn." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SharedSpawn();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, ShouldShowHints, "class", "Should show hints." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->ShouldShowHints() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SimulatePlayerSimulatedEntities, "class", "Simulate player simulated entities." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SimulatePlayerSimulatedEntities();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SmoothViewOnStairs, "class", "Smooth view on stairs." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SmoothViewOnStairs( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "normal" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, SwitchToNextBestWeapon, "class", "Switch to the player's next best weapon." )
{
    lua_pushboolean( L,
                     LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->SwitchToNextBestWeapon( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, UpdateClientData, "class", "Update the player's client data." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->UpdateClientData();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, UpdateUnderwaterState, "class", "Update the player's underwater state." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->UpdateUnderwaterState();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, UsingStandardWeaponsInVehicle, "class", "Check if the player is using standard weapons in a vehicle." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->UsingStandardWeaponsInVehicle() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, ViewPunch, "class", "View punch." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ViewPunch( LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, ViewPunchReset, "class", "View punch reset." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ViewPunchReset( LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0, "time" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, CanSwitchToWeapon, "class", "Check if the player can switch to a weapon." )
{
    lua_pushboolean(
        L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_CanSwitchTo( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player can switch to the weapon." )

LUA_BINDING_BEGIN( CBasePlayer, OwnsWeaponOfType, "class", "Check if the player owns a weapon of a certain type." )
{
    CBaseEntity::PushLuaInstanceSafe( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_OwnsThisType( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "type" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "subtype" ) ) );
    return 1;
}
LUA_BINDING_END( "CBasePlayer", "The weapon the player owns of the specified type." )

LUA_BINDING_BEGIN( CBasePlayer, SetLastWeapon, "class", "Set the player's last weapon." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->Weapon_SetLast( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBasePlayer, WeaponShootPosition, "class", "Get the player's weapon shoot position." )
{
    Vector v = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_ShootPosition();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's weapon shoot position." )

LUA_BINDING_BEGIN( CBasePlayer, ShouldSelectWeapon, "class", "Check if the player should select a weapon." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_ShouldSelectItem( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player should select the weapon." )

LUA_BINDING_BEGIN( CBasePlayer, ShouldSetLastWeapon, "class", "Check if the player should set the last weapon." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_ShouldSetLast( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ), LUA_BINDING_ARGUMENT( luaL_checkweapon, 3, "last" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player should set the last weapon." )

LUA_BINDING_BEGIN( CBasePlayer, SwitchWeapon, "class", "Switch the player's weapon." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_Switch( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "subtype" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon was switched." )

LUA_BINDING_BEGIN( CBasePlayer, TranslateWeaponActivity, "class", "Translate a weapon activity." )
{
    Activity activity = ( Activity )( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "activity" );
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_TranslateActivity( activity, NULL ) );
    return 1;
}
LUA_BINDING_END( "number", "The translated activity." )

LUA_BINDING_BEGIN( CBasePlayer, WeaponCount, "class", "Get the player's weapon count." )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->WeaponCount() );
    return 1;
}
LUA_BINDING_END( "number", "The player's weapon count." )

LUA_BINDING_BEGIN( CBasePlayer, GetAccountId, "class", "Get the player's account ID." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CSteamID steamID = GetSteamIDForPlayerIndex( player->entindex() );
    lua_pushinteger( L, steamID.GetAccountID() );
    return 1;
}
LUA_BINDING_END( "number", "The player's account ID." )

LUA_BINDING_BEGIN( CBasePlayer, GetSteamId, "class", "Get the player's Steam ID." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CSteamID steamID = GetSteamIDForPlayerIndex( player->entindex() );
    lua_pushstring( L, steamID.Render() );
    return 1;
}
LUA_BINDING_END( "string", "The player's Steam ID." )

LUA_BINDING_BEGIN( CBasePlayer, GetSteamId64, "class", "Get the player's Steam ID 64." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CSteamID steamID = GetSteamIDForPlayerIndex( player->entindex() );

    uint64 steamID64 = steamID.ConvertToUint64();
    char steamID64str[20];
    Q_snprintf( steamID64str, sizeof( steamID64str ), "%llu", steamID64 );

    lua_pushstring( L, steamID64str );
    return 1;
}
LUA_BINDING_END( "string", "The player's Steam ID 64." )

LUA_BINDING_BEGIN( CBasePlayer, GetUniqueId, "class", "Get the player's unique ID." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, ( lua_Integer )player->GetUniqueID() );
    return 1;
}
LUA_BINDING_END( "number", "The player's unique ID." )

LUA_BINDING_BEGIN( CBasePlayer, __eq, "class", "Check if two players are equal." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_toplayer, 1, "player" ) == LUA_BINDING_ARGUMENT( lua_toplayer, 2, "player" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the players are equal." )

LUA_BINDING_BEGIN( CBasePlayer, __tostring, "class", "Get the string representation of a player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( lua_toplayer, 1, "player" );

    if ( player == NULL )
        lua_pushstring( L, "NULL" );
    else
        lua_pushfstring( L, "CBasePlayer: %d \"%s\"", player->GetUserID(), player->GetPlayerName() );
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the player." )

// Experiment; Not really useful in Lua, so disabled
// static int luasrc_ToBasePlayer( lua_State *L )
//{
//    CBaseEntity::PushLuaInstanceSafe( L, ToBasePlayer( luaL_checkentity( L, 1 ) ) );
//    return 1;
//}
//
// static const luaL_Reg CBasePlayer_funcs[] = {
//    { "ToBasePlayer", luasrc_ToBasePlayer },
//    { NULL, NULL } };

/*
** Open CBasePlayer object
*/
LUALIB_API int luaopen_CBasePlayer_shared( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_BASEPLAYERMETANAME );

    LUA_REGISTRATION_COMMIT( CBasePlayer );

    lua_pushstring( L, "Entity" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */
    lua_pop( L, 1 );

    return 1;
}

/*
** Open Players library
*/
LUA_REGISTRATION_INIT( Players );

#ifdef CLIENT_DLL
LUA_BINDING_BEGIN( Players, GetLocalPlayer, "library", "Get the local player.", "client" )
{
    CBaseEntity::PushLuaInstanceSafe( L, CBasePlayer::GetLocalPlayer() );
    return 1;
}
LUA_BINDING_END( "CBasePlayer", "The local player." )
#endif

LUALIB_API int luaopen_Players( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Players );

    return 1;
}
