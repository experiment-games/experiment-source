#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "steam/steam_api.h"
#include "lbaseplayer_shared.h"
#ifdef CLIENT_DLL
#include "lc_baseanimating.h"
#include <voice_status.h>
#include "c_playerresource.h"
#else
#include "lbaseanimating.h"
#include "player_resource.h"
#include "ilagcompensationmanager.h"
#endif
#include "lbasecombatweapon_shared.h"
#include "lbaseentity_shared.h"
#include "SoundEmitterSystem/lisoundemittersystembase.h"
#include "mathlib/lvector.h"
#include "lvphysics_interface.h"
#include <lusercmd.h>
#include <weapon_experimentbase_scriptedweapon.h>
#include "engine/IEngineSound.h"
#include <lexperiment_player_shared.h>

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
        luaL_argerror( L, narg, "Player expected, got NULL entity" );
    return d;
}

LUALIB_API lua_CBasePlayer *luaL_optplayer( lua_State *L, int narg, lua_CBasePlayer *def )
{
    return luaL_opt( L, luaL_checkplayer, narg, def );
}

LUA_REGISTRATION_INIT( Player )

LUA_BINDING_BEGIN( Player, AddToPlayerSimulationList, "class", "Add the player to the simulation list." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_CBaseEntity *other = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" );
    player->AddToPlayerSimulationList( other );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, CacheVehicleView, "class", "Cache the vehicle view." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->CacheVehicleView();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ClearPlayerSimulationList, "class", "Clear the player simulation list." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ClearPlayerSimulationList();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ClearZoomOwner, "class", "Clear the zoom owner." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ClearZoomOwner();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, CurrentCommandNumber, "class", "Get the current command number." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->CurrentCommandNumber() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ExitLadder, "class", "Exit the ladder." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ExitLadder();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetEyePositionAndVectors, "class", "Get the player's eye position and vectors." )
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

LUA_BINDING_BEGIN( Player, GetEyeVectors, "class", "Get the player's eye vectors." )
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

LUA_BINDING_BEGIN( Player, FindUseEntity, "class", "Find the entity the player is using." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity *pUseEntity = player->FindUseEntity();
    CBaseEntity::PushLuaInstanceSafe( L, pUseEntity );
    return 1;
}
LUA_BINDING_END( "Entity", "The entity the player is using." )

LUA_BINDING_BEGIN( Player, GetActiveWeapon, "class", "Get the player's active weapon." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseCombatWeapon *pWeapon = player->GetActiveWeapon();

    if ( !CExperimentScriptedWeapon::IsValidWeapon( pWeapon ) )
    {
        CBaseCombatWeapon::PushLuaInstanceSafe( L, NULL );
        return 1;
    }

    CBaseCombatWeapon::PushLuaInstanceSafe( L, pWeapon );
    return 1;
}
LUA_BINDING_END( "Weapon", "The player's active weapon." )

LUA_BINDING_BEGIN( Player, GetArmor, "class", "Get the player's armor value." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->ArmorValue() );
    return 1;
}
LUA_BINDING_END( "integer", "The player's armor value." )

// FIXME: move to CBaseCombatCharacter
LUA_BINDING_BEGIN( Player, GetAmmoCount, "class", "Get the player's ammo count." )
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
LUA_BINDING_END( "integer", "The player's ammo count." )

// TODO: Is this correct? Is AutoaimVector the same as AimVector in gmod?
LUA_BINDING_BEGIN( Player, GetAimVector, "class", "Get the player's autoaim vector." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    // Commented to prevent errors with the Helix gamemode which has a type in cl_hooks.lua: `lastTrace.endpos = lastTrace.start + client:GetAimVector(client) * 160`
    // Vector v = player->GetAutoaimVector( LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 1, "scale" ) );
    Vector v = player->GetAutoaimVector( 1 );
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's autoaim vector." )

LUA_BINDING_BEGIN( Player, GetBonusChallenge, "class", "Get the player's bonus challenge." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetBonusChallenge() );
    return 1;
}
LUA_BINDING_END( "integer", "The player's bonus challenge." )

LUA_BINDING_BEGIN( Player, GetBonusProgress, "class", "Get the player's bonus progress." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetBonusProgress() );
    return 1;
}
LUA_BINDING_END( "integer", "The player's bonus progress." )

LUA_BINDING_BEGIN( Player, GetDeathTime, "class", "Get the player's death time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetDeathTime() );
    return 1;
}
LUA_BINDING_END( "number", "The player's death time." )

LUA_BINDING_BEGIN( Player, GetDefaultFov, "class", "Get the player's default FOV." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetDefaultFOV() );
    return 1;
}
LUA_BINDING_END( "integer", "The player's default FOV." )

// static int CBasePlayer_GetEfNoInterpParity( lua_State *L )
//{
//     lua_pushinteger( L, luaL_checkplayer( L, 1 )->GetEFNoInterpParity() );
//     return 1;
// }

LUA_BINDING_BEGIN( Player, GetFov, "class", "Get the player's FOV." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetFOV() );
    return 1;
}
LUA_BINDING_END( "number", "The player's FOV." )

LUA_BINDING_BEGIN( Player, GetFovDistanceAdjustFactor, "class", "Get the player's FOV distance adjust factor." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetFOVDistanceAdjustFactor() );
    return 1;
}
LUA_BINDING_END( "number", "The player's FOV distance adjust factor." )

LUA_BINDING_BEGIN( Player, GetFovTime, "class", "Get the player's FOV time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetFOVTime() );
    return 1;
}
LUA_BINDING_END( "number", "The player's FOV time." )

LUA_BINDING_BEGIN( Player, SetDsp, "class", "Set the player's DSP." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int dspEffectId = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "dspEffectId" );
    bool fastReset = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "fastReset" );

    CSingleUserRecipientFilter user( player );
    enginesound->SetPlayerDSP( user, dspEffectId, fastReset );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetHands, "class", "Get the player's hands." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetHands() );
    return 1;
}
LUA_BINDING_END( "Entity", "The player's hands." )

LUA_BINDING_BEGIN( Player, SetHands, "class", "Set the player's hands." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetHands( LUA_BINDING_ARGUMENT( luaL_checkanimating, 2, "hands" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetCurrentCommand, "class", "Returns the last command which was sent by the current prediction player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
#ifdef CLIENT_DLL
    lua_pushusercmd( L, player->m_pCurrentCommand );
#else
    lua_pushusercmd( L, player->GetCurrentCommand() );
#endif
    return 1;
}
LUA_BINDING_END( "UserCommand", "The last user command." )

LUA_BINDING_BEGIN( Player, GetImpulse, "class", "Get the player's impulse." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetImpulse() );
    return 1;
}
LUA_BINDING_END( "integer", "The player's impulse." )

LUA_BINDING_BEGIN( Player, GetLaggedMovementValue, "class", "Get the player's lagged movement value." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetLaggedMovementValue() );
    return 1;
}
LUA_BINDING_END( "number", "The player's lagged movement value." )

LUA_BINDING_BEGIN( Player, GetLastKnownPlaceName, "class", "Get the player's last known place name." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushstring( L, player->GetLastKnownPlaceName() );
    return 1;
}
LUA_BINDING_END( "string", "The player's last known place name." )

LUA_BINDING_BEGIN( Player, GetNextAttack, "class", "Get the player's next attack." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetNextAttack() );
    return 1;
}
LUA_BINDING_END( "number", "The player's next attack." )

LUA_BINDING_BEGIN( Player, GetObserverMode, "class", "Get the player's observer mode." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetObserverMode() );
    return 1;
}
LUA_BINDING_END( "enumeration/OBSERVER_MODE", "The player's observer mode." )

LUA_BINDING_BEGIN( Player, GetObserverTarget, "class", "Get the player's observer target." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetObserverTarget() );
    return 1;
}
LUA_BINDING_END( "Entity", "The player's observer target." )

// Experiment; disabled since this is an internal function
// LUA_BINDING_BEGIN( Player, GetOffset_m_Local, "class", "Get the offset of m_Local." )
//{
//    lua_pushinteger( L, lua_CBasePlayer::GetOffset_m_Local() );
//    return 1;
//}
// LUA_BINDING_END( "integer", "The offset of m_Local." )

LUA_BINDING_BEGIN( Player, GetPing, "class", "Get the player's ping." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

#ifdef CLIENT_DLL
    lua_pushinteger( L, g_PR->GetPing( player->entindex() ) );
#else
    lua_pushinteger( L, g_pPlayerResource->GetPing( player->entindex() ) );
#endif
    return 1;
}
LUA_BINDING_END( "integer", "The player's ping." )

// TODO: We should really have accessors for these
LUA_BINDING_BEGIN( Player, GetPlayerLocalData, "class", "Get the player's local data." )
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

LUA_BINDING_BEGIN( Player, GetPlayerMaxs, "class", "Get the player's maxs." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector v = player->GetPlayerMaxs();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's maxs." )

LUA_BINDING_BEGIN( Player, GetPlayerMins, "class", "Get the player's mins." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector v = player->GetPlayerMins();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's mins." )

LUA_BINDING_BEGIN( Player, GetPlayerName, "class", "Get the player's name." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushstring( L, player->GetPlayerName() );
    return 1;
}
LUA_BINDING_END( "string", "The player's name." )

LUA_BINDING_BEGIN( Player, GetPreviouslyPredictedOrigin, "class", "Get the player's previously predicted origin." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector v = player->GetPreviouslyPredictedOrigin();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's previously predicted origin." )

LUA_BINDING_BEGIN( Player, GetPunchAngle, "class", "Get the player's punch angle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    QAngle v = player->GetPunchAngle();
    lua_pushangle( L, v );
    return 1;
}
LUA_BINDING_END( "Angle", "The player's punch angle." )

LUA_BINDING_BEGIN( Player, GetStepSoundCache, "class", "Get the player's step sound cache." )
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

LUA_BINDING_BEGIN( Player, GetStepSoundVelocities, "class", "Get the player's step sound velocities." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    float velwalk, velrun;
    player->GetStepSoundVelocities( &velwalk, &velrun );
    lua_pushnumber( L, velwalk );
    lua_pushnumber( L, velrun );
    return 2;
}
LUA_BINDING_END( "number", "The player's step sound walk velocity.", "number", "The player's step sound run velocity." )

LUA_BINDING_BEGIN( Player, GetSwimSoundTime, "class", "Get the player's swim sound time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetSwimSoundTime() );
    return 1;
}
LUA_BINDING_END( "number", "The player's swim sound time." )

LUA_BINDING_BEGIN( Player, GetTimeBase, "class", "Get the player's time base." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetTimeBase() );
    return 1;
}
LUA_BINDING_END( "number", "The player's time base." )

LUA_BINDING_BEGIN( Player, GetUseEntity, "class", "Get the player's use entity." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetUseEntity() );
    return 1;
}
LUA_BINDING_END( "Entity", "The player's use entity." )

LUA_BINDING_BEGIN( Player, GetUserId, "class", "Get the player's user ID." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetUserID() );
    return 1;
}
LUA_BINDING_END( "integer", "The player's user ID." )

LUA_BINDING_BEGIN( Player, GetVehicleEntity, "class", "Get the player's vehicle entity." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetVehicleEntity() );
    return 1;
}
LUA_BINDING_END( "Entity", "The player's vehicle entity." )

LUA_BINDING_BEGIN( Player, GetViewEntity, "class", "Get the player's view entity." )
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
LUA_BINDING_END( "Entity", "The player's view entity." )

// FIXME: Push CBaseViewModel instead
LUA_BINDING_BEGIN( Player, GetViewModel, "class", "Get the player's view model." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetViewModel( ( int )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0, "viewModelIndex" ) ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The player's view model." )

LUA_BINDING_BEGIN( Player, GetWaterJumpTime, "class", "Get the player's water jump time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetWaterJumpTime() );
    return 1;
}
LUA_BINDING_END( "number", "The player's water jump time." )

LUA_BINDING_BEGIN( Player, GetWeapon, "class", "Get the player's weapon." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    const char *className = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "className" );
    CBaseCombatWeapon *foundWeapon = nullptr;

    for ( int i = 0; i < player->WeaponCount(); i++ )
    {
        CBaseCombatWeapon *weapon = player->GetWeapon( i );

        // Stop as soon as we've reached the end of valid weapons
        if ( !weapon || !CExperimentScriptedWeapon::IsValidWeapon( weapon ) )
            break;

        if ( FStrEq( weapon->GetClassname(), className ) )
        {
            foundWeapon = weapon;
            break;
        }
    }

    CBaseCombatWeapon::PushLuaInstanceSafe( L, foundWeapon );
    return 1;
}
LUA_BINDING_END( "Entity", "The player's weapon." )

LUA_BINDING_BEGIN( Player, GetWeaponInSlot, "class", "Get the player's weapon in a slot." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int slot = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "slot" );
    CBaseCombatWeapon *weapon = player->GetWeapon( slot );

    if ( !CExperimentScriptedWeapon::IsValidWeapon( weapon ) )
    {
        CBaseCombatWeapon::PushLuaInstanceSafe( L, nullptr );
        return 1;
    }

    CBaseCombatWeapon::PushLuaInstanceSafe( L, weapon );
    return 1;
}
LUA_BINDING_END( "Entity", "The player's weapon in the slot." )

LUA_BINDING_BEGIN( Player, GetWeapons, "class", "Get the player's weapons." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_newtable( L );

    for ( int i = 0; i < player->WeaponCount(); i++ )
    {
        CBaseCombatWeapon *weapon = player->GetWeapon( i );

        // Stop as soon as we've reached the end of valid weapons
        if ( !weapon || !CExperimentScriptedWeapon::IsValidWeapon( weapon ) )
            break;

        CBaseCombatWeapon::PushLuaInstanceSafe( L, weapon );
        lua_rawseti( L, -2, i + 1 );
    }

    return 1;
}
LUA_BINDING_END( "table", "The player's weapons." )

LUA_BINDING_BEGIN( Player, HintMessage, "class", "Hint a message to the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->HintMessage( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, IsFlashlightOn, "class", "Check if the flashlight is on for the player." )
{
    lua_CExperiment_Player *player = LUA_BINDING_ARGUMENT( luaL_checkexperimentplayer, 1, "player" );
    lua_pushboolean( L, player->FlashlightIsOn() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the flashlight is on, false otherwise." )

// LUA_BINDING_BEGIN( Player, IncrementEfNoInterpParity, "class", "Increment the player's EF no interp parity." )
// {
//     lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
//     player->IncrementEFNoInterpParity();
//     return 0;
// }
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, IsBot, "class", "Check if the player is a bot." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsBot() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is a bot." )

LUA_BINDING_BEGIN( Player, IsHltv, "class", "Check if the player is HLTV." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsHLTV() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is HLTV." )

LUA_BINDING_BEGIN( Player, IsInVehicle, "class", "Check if the player is in a vehicle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsInAVehicle() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is in a vehicle." )

LUA_BINDING_BEGIN( Player, IsObserver, "class", "Check if the player is an observer." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsObserver() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is an observer." )

LUA_BINDING_BEGIN( Player, IsPlayerUnderwater, "class", "Check if the player is underwater." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsPlayerUnderwater() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is underwater." )

LUA_BINDING_BEGIN( Player, IsSpeaking, "class", "Check if the player is speaking." )
{
#ifdef CLIENT_DLL
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

    if ( ( player == C_BasePlayer::GetLocalPlayer() && GetClientVoiceMgr()->IsLocalPlayerSpeaking() ) || GetClientVoiceMgr()->IsPlayerSpeaking( player->entindex() ) )
    {
        lua_pushboolean( L, true );
        return 1;
    }
#else
    DevWarning( "Player::IsSpeaking is not implemented on the server\n" );
#endif

    lua_pushboolean( L, false );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is speaking." )

LUA_BINDING_BEGIN( Player, IsSuitEquipped, "class", "Check if the player has the HEV suit equipped." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsSuitEquipped() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player has the HEV suit equipped." )

LUA_BINDING_BEGIN( Player, IsUseableEntity, "class", "Check if the player can use an entity." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L,
                     player->IsUseableEntity(
                         LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" ),
                         LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "use type" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player can use the entity." )

LUA_BINDING_BEGIN( Player, ItemPostFrame, "class", "Call the player's item post frame." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ItemPostFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ItemPreFrame, "class", "Call the player's item pre frame." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ItemPreFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, LeaveVehicle, "class", "Make the player leave their vehicle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->LeaveVehicle();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetMaxSpeed, "class", "Get the player's max speed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetMaxSpeed() );
    return 1;
}
LUA_BINDING_END( "number", "The player's max speed." )

// Experiment; not useful in Lua
// LUA_BINDING_BEGIN( Player, MyCombatCharacterPointer, "class", "Get the player's combat character pointer." )
//{
//    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
//    CBaseEntity::PushLuaInstanceSafe( L, player->MyCombatCharacterPointer() );
//    return 1;
//}
// LUA_BINDING_END( "Entity", "The player's combat character pointer." )

LUA_BINDING_BEGIN( Player, PlayerUse, "class", "Make the player use an entity." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->PlayerUse();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, PlayStepSound, "class", "Play a step sound for the player." )
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

LUA_BINDING_BEGIN( Player, PostThink, "class", "Call the player's post think." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->PostThink();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, PreThink, "class", "Call the player's pre think." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->PreThink();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, RemoveAllAmmo, "class", "Remove all ammo from the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->RemoveAllAmmo();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, RemoveAmmo, "class", "Remove ammo from the player." )
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

LUA_BINDING_BEGIN( Player, RemoveFromPlayerSimulationList, "class", "Remove an entity from the player's simulation list." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->RemoveFromPlayerSimulationList( LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ResetAutoAim, "class", "Reset the player's autoaim." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ResetAutoaim();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ResetObserverMode, "class", "Reset the player's observer mode." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ResetObserverMode();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SelectItem, "class", "Select an item for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SelectItem(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "item name" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "slot" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SelectLastItem, "class", "Select the player's last item." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SelectLastItem();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetAmmoCount, "class", "Set the player's ammo count." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetAmmoCount(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "amount" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "ammo type" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetAnimation, "class", "Set the player's animation." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetAnimation( LUA_BINDING_ARGUMENT_ENUM( PLAYER_ANIM, 2, "animation" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetAnimationExtension, "class", "Set the player's animation extension." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetAnimationExtension( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "extension" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetBloodColor, "class", "Set the player's blood color." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetBloodColor( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "color" ) );
    return 0;
}
LUA_BINDING_END()

#define PLAYER_THEMSELVES nullptr
LUA_BINDING_BEGIN( Player, SetFov, "class", "Set the player's field of view." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_CBaseEntity *requester = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optentity, 4, PLAYER_THEMSELVES, "requester" );
    lua_pushboolean( L,
                     player->SetFOV(
                         requester ? requester : player,
                         LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "fov" ),
                         LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "transitionTime" ) ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetLadderNormal, "class", "Set the player's ladder normal." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetLadderNormal( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "normal" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, StartLagCompensation, "class", "Set the player's lag compensation. On the client this does nothing." )
{
#ifndef CLIENT_DLL
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

    // Move other players back to history positions based on local player's lag
    lagcompensation->StartLagCompensation( player,
                                           player->GetCurrentCommand() );
#endif
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, FinishLagCompensation, "class", "Set the player's lag compensation. On the client this does nothing." )
{
#ifndef CLIENT_DLL
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );

    lagcompensation->FinishLagCompensation( player );
#endif
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetMaxSpeed, "class", "Set the player's current max speed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetMaxSpeed( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetWalkSpeed, "class", "Set the player's walking speed (for when they press ALT)." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetWalkSpeed( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetWalkSpeed, "class", "Get the player's walking speed (for when they press ALT)." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetWalkSpeed() );
    return 1;
}
LUA_BINDING_END( "number", "The player's walking speed." )

LUA_BINDING_BEGIN( Player, SetNormalSpeed, "class", "Set the player's normal speed (for when neither sprinting, walking nor crouching)" )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetNormalSpeed( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetNormalSpeed, "class", "Get the player's normal speed (for when neither sprinting, walking nor crouching)" )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetNormalSpeed() );
    return 1;
}
LUA_BINDING_END( "number", "The player's normal speed." )

LUA_BINDING_BEGIN( Player, SetRunSpeed, "class", "Set the player's sprint speed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetRunSpeed( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetRunSpeed, "class", "Get the player's sprint speed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetRunSpeed() );
    return 1;
}
LUA_BINDING_END( "number", "The player's sprint speed." )

LUA_BINDING_BEGIN( Player, SetLadderClimbSpeed, "class", "Set the player's ladder climb speed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetLadderClimbSpeed( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetLadderClimbSpeed, "class", "Get the player's ladder climb speed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetLadderClimbSpeed() );
    return 1;
}
LUA_BINDING_END( "number", "The player's ladder climb speed." )

LUA_BINDING_BEGIN( Player, SetCrouchWalkFraction, "class", "Set the fraction by which player's walk speed will be multiplied when crouched." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetCrouchWalkFraction( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speed" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetCrouchWalkFraction, "class", "Get the fraction by which player's walk speed will be multiplied when crouched." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetCrouchWalkFraction() );
    return 1;
}
LUA_BINDING_END( "number", "The player's crouched walk speed fraction." )

LUA_BINDING_BEGIN( Player, SetJumpPower, "class", "Set the player's jump power. Is multiplied by jumpFactor of the surface the player is standing on. Supposedly (according to a Valve comment inside the SDK) 160 power equals about 21 units jump height." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetJumpPower( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "power" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetJumpPower, "class", "Get the player's jump power." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetJumpPower() );
    return 1;
}
LUA_BINDING_END( "number", "The player's jump power." )

LUA_BINDING_BEGIN( Player, SetDuckSpeed, "class", "Set the player's duck speed. This **should not** suffer from this bug that Garry's Mod has: https://github.com/Facepunch/garrysmod-issues/issues/2722" )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetDuckSpeedInMilliseconds( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "speedInMilliseconds" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetDuckSpeed, "class", "Get the player's duck speed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetDuckSpeedInMilliseconds() );
    return 1;
}
LUA_BINDING_END( "number", "The player's duck speed." )

LUA_BINDING_BEGIN( Player, SetUnDuckFraction, "class", "Set the player's unduck fraction. If it's 0.5 then unducking will take half the time it takes to duck." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetUnDuckFraction( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "fraction" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetUnDuckFraction, "class", "Get the player's unduck fraction." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushnumber( L, player->GetUnDuckFraction() );
    return 1;
}
LUA_BINDING_END( "number", "The player's unduck fraction." )

LUA_BINDING_BEGIN( Player, SetNextAttack, "class", "Set the player's next attack." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetNextAttack( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" ) );
    return 0;
}
LUA_BINDING_END()

// TODO: We should really have accessor methods for each of these
LUA_BINDING_BEGIN( Player, SetPlayerLocalData, "class", "Set the player's local data." )
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

LUA_BINDING_BEGIN( Player, SetPlayerUnderwater, "class", "Set whether the player is underwater." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetPlayerUnderwater( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "underwater" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetPreviouslyPredictedOrigin, "class", "Set the player's previously predicted origin." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetPreviouslyPredictedOrigin( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetPunchAngle, "class", "Set the player's punch angle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetPunchAngle( LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetStepSoundCache, "class", "Set the player's step sound cache." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int index = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "field" );

    if ( index == 0 )
    {
        if ( Q_strcmp( field, "m_SoundParameters" ) == 0 )
            player->m_StepSoundCache[0].m_SoundParameters = lua_tosoundparameters( L, 4 );
        else if ( Q_strcmp( field, "m_usSoundNameIndex" ) == 0 )
            player->m_StepSoundCache[0].m_usSoundNameIndex = ( unsigned short )luaL_checknumber( L, 4 );
    }
    else if ( index == 1 )
    {
        if ( Q_strcmp( field, "m_SoundParameters" ) == 0 )
            player->m_StepSoundCache[1].m_SoundParameters = lua_tosoundparameters( L, 4 );
        else if ( Q_strcmp( field, "m_usSoundNameIndex" ) == 0 )
            player->m_StepSoundCache[1].m_usSoundNameIndex = ( unsigned short )luaL_checknumber( L, 4 );
    }
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetSuitUpdate, "class", "Set the player's suit update." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetSuitUpdate(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "group" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "repeat" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetSwimSoundTime, "class", "Set the player's swim sound time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetSwimSoundTime( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetWaterJumpTime, "class", "Set the player's water jump time." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetWaterJumpTime( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SharedSpawn, "class", "Shared spawn." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SharedSpawn();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ShouldShowHints, "class", "Should show hints." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->ShouldShowHints() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SimulatePlayerSimulatedEntities, "class", "Simulate player simulated entities." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SimulatePlayerSimulatedEntities();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SmoothViewOnStairs, "class", "Smooth view on stairs." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SmoothViewOnStairs( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "normal" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SwitchToNextBestWeapon, "class", "Switch to the player's next best weapon." )
{
    lua_pushboolean( L,
                     LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->SwitchToNextBestWeapon( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, UpdateClientData, "class", "Update the player's client data." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->UpdateClientData();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, UpdateUnderwaterState, "class", "Update the player's underwater state." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->UpdateUnderwaterState();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, UsingStandardWeaponsInVehicle, "class", "Check if the player is using standard weapons in a vehicle." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->UsingStandardWeaponsInVehicle() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ViewPunch, "class", "View punch." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ViewPunch( LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ViewPunchReset, "class", "View punch reset." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ViewPunchReset( LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0, "time" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetViewPunchAngles, "class", "Get the player's view punch angle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushangle( L, player->m_Local.m_vecPunchAngle );
    return 1;
}
LUA_BINDING_END( "Angle", "The player's view punch angle." )

LUA_BINDING_BEGIN( Player, SetViewPunchAngles, "class", "Set the player's view punch angle." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->m_Local.m_vecPunchAngle = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, CanSwitchToWeapon, "class", "Check if the player can switch to a weapon." )
{
    lua_pushboolean(
        L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_CanSwitchTo( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player can switch to the weapon." )

LUA_BINDING_BEGIN( Player, HasWeapon, "class", "Check if the player has the named weapon." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    const char *className = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" );

    lua_pushboolean( L, player->HasNamedPlayerItem( className ) != nullptr );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player has the weapon." )

LUA_BINDING_BEGIN( Player, OwnsWeaponOfType, "class", "Check if the player owns a weapon of a certain type." )
{
    CBaseCombatWeapon::PushLuaInstanceSafe( L,
                                            LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )
                                                ->Weapon_OwnsThisType(
                                                    LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "type" ),
                                                    LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "subType" ) ) );
    return 1;
}
LUA_BINDING_END( "Weapon", "The weapon the player owns of the specified type." )

LUA_BINDING_BEGIN( Player, SetLastWeapon, "class", "Set the player's last weapon." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->Weapon_SetLast( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, IsKeyDown, "class", "Check if a key is down." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int keys = ( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "keys" );
    lua_pushboolean( L, player->m_nButtons & keys );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the key is down." )

LUA_BINDING_BEGIN( Player, WasKeyDown, "class", "Get if the specified key was down one tick before the current." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int keys = ( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "keys" );
    lua_pushboolean( L, player->m_afButtonLast & keys );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the key was down." )

LUA_BINDING_BEGIN( Player, WasKeyPressed, "class", "Check if a key was pressed." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int keys = ( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "keys" );
    lua_pushboolean( L, player->m_afButtonPressed & keys );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the key was pressed." )

LUA_BINDING_BEGIN( Player, WasKeyReleased, "class", "Check if a key was released." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int keys = ( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "keys" );
    lua_pushboolean( L, player->m_afButtonReleased & keys );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the key was released." )

LUA_BINDING_BEGIN( Player, GetWeaponShootPosition, "class", "Get the player's weapon shoot position." )
{
    Vector v = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_ShootPosition();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "The player's weapon shoot position." )

LUA_BINDING_BEGIN( Player, ShouldSelectWeapon, "class", "Check if the player should select a weapon." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_ShouldSelectItem( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player should select the weapon." )

LUA_BINDING_BEGIN( Player, ShouldSetLastWeapon, "class", "Check if the player should set the last weapon." )
{
    lua_pushboolean( L,
                     LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )
                         ->Weapon_ShouldSetLast(
                             LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ),
                             LUA_BINDING_ARGUMENT( luaL_checkweapon, 3, "last" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player should set the last weapon." )

LUA_BINDING_BEGIN( Player, SwitchWeapon, "class", "Switch the player's weapon." )
{
    lua_pushboolean( L,
                     LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )
                         ->Weapon_Switch(
                             LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ),
                             LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "subType" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon was switched." )

LUA_BINDING_BEGIN( Player, TranslateWeaponActivity, "class", "Translate a weapon activity." )
{
    Activity activity = LUA_BINDING_ARGUMENT_ENUM( Activity, 2, "activity" );
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->Weapon_TranslateActivity( activity, NULL ) );
    return 1;
}
LUA_BINDING_END( "enumeration/ACTIVITY", "The translated activity." )

LUA_BINDING_BEGIN( Player, WeaponCount, "class", "Get the player's weapon count." )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" )->WeaponCount() );
    return 1;
}
LUA_BINDING_END( "integer", "The player's weapon count." )

LUA_BINDING_BEGIN( Player, GetAccountId, "class", "Get the player's account ID." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CSteamID steamID = GetSteamIDForPlayerIndex( player->entindex() );
    lua_pushinteger( L, steamID.GetAccountID() );
    return 1;
}
LUA_BINDING_END( "integer", "The player's account ID." )

LUA_BINDING_BEGIN( Player, GetSteamId, "class", "Get the player's Steam ID." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CSteamID steamID = GetSteamIDForPlayerIndex( player->entindex() );
    lua_pushstring( L, steamID.Render() );
    return 1;
}
LUA_BINDING_END( "string", "The player's Steam ID." )

LUA_BINDING_BEGIN( Player, GetSteamId64, "class", "Get the player's Steam ID 64." )
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

LUA_BINDING_BEGIN( Player, GetUniqueId, "class", "Get the player's unique ID." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, ( lua_Integer )player->GetUniqueID() );
    return 1;
}
LUA_BINDING_END( "integer", "The player's unique ID." )

// Experiment; We only let CBaseEntity determine equality, which should be fine since they're pointers to the same entity. Disabled:
// LUA_BINDING_BEGIN( Player, __eq, "class", "Check if two players are equal." )
//{
//    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_toplayer, 1, "player" ) == LUA_BINDING_ARGUMENT( lua_toplayer, 2, "other" ) );
//    return 1;
//}
// LUA_BINDING_END( "boolean", "Whether the players are equal." )
//
// LUA_BINDING_BEGIN( Player, __tostring, "class", "Get the string representation of a player." )
//{
//    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( lua_toplayer, 1, "player" );
//
//    if ( player == NULL )
//        lua_pushstring( L, "NULL" );
//    else
//        lua_pushfstring( L, "Player: %d \"%s\" (%d)", player->GetUserID(), player->GetPlayerName(), player->entindex() );
//    return 1;
//}
// LUA_BINDING_END( "string", "The string representation of the player." )

// Experiment; Not really useful in Lua, so disabled
// static int luasrc_ToBasePlayer( lua_State *L )
//{
//    CBasePlayer::PushLuaInstanceSafe( L, ToBasePlayer( luaL_checkentity( L, 1 ) ) );
//    return 1;
//}

/*
** Open CBasePlayer object
*/
LUALIB_API int luaopen_CBasePlayer_shared( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_BASEPLAYERMETANAME );

    LUA_REGISTRATION_COMMIT( Player );

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
    CBasePlayer::PushLuaInstanceSafe( L, CBasePlayer::GetLocalPlayer() );
    return 1;
}
LUA_BINDING_END( "Player", "The local player." )

LUA_BINDING_BEGIN( Players, IsPlayerIndex, "library", "Check if index is a player index", "client" )
{
    int index = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "index" );
    lua_pushboolean( L, IsPlayerIndex( index ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if player index, false otherwise." )

LUA_BINDING_BEGIN( Players, GetLocalPlayerIndex, "library", "Get local player index", "client" )
{
    lua_pushinteger( L, GetLocalPlayerIndex() );
    return 1;
}
LUA_BINDING_END( "integer", "Local player index." )

LUA_BINDING_BEGIN( Players, IsLocalPlayerSpectator, "library", "Check if local player is a spectator", "client" )
{
    lua_pushboolean( L, IsLocalPlayerSpectator() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if local player is a spectator." )

LUA_BINDING_BEGIN( Players, GetSpectatorMode, "library", "Get spectator mode of the local player", "client" )
{
    lua_pushinteger( L, GetSpectatorMode() );
    return 1;
}
LUA_BINDING_END( "integer", "Spectator mode of the local player." )

LUA_BINDING_BEGIN( Players, GetSpectatorTarget, "library", "Get the spectator target of the local player", "client" )
{
    lua_pushinteger( L, GetSpectatorTarget() );
    return 1;
}
LUA_BINDING_END( "integer", "Spectator target of the local player." )

LUA_BINDING_BEGIN( Players, GetLocalPlayerTeam, "library", "Get the team of the local player", "client" )
{
    lua_pushinteger( L, GetLocalPlayerTeam() );
    return 1;
}
LUA_BINDING_END( "integer", "Team of the local player." )
#endif

LUA_BINDING_BEGIN( Players, GetAllBots, "library", "Get all bots." )
{
    lua_newtable( L );

    int index = 1;
    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );

        if ( pPlayer->IsBot() )
        {
            CBasePlayer::PushLuaInstanceSafe( L, pPlayer );
            lua_rawseti( L, -2, index );
            index++;
        }
    }

    return 1;
}
LUA_BINDING_END( "table", "All bots." )

LUA_BINDING_BEGIN( Players, GetAllHumans, "library", "Get all humans." )
{
    lua_newtable( L );

    int index = 1;
    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );

        if ( pPlayer && !pPlayer->IsBot() )
        {
            CBasePlayer::PushLuaInstanceSafe( L, pPlayer );
            lua_rawseti( L, -2, index );
            index++;
        }
    }

    return 1;
}
LUA_BINDING_END( "table", "All humans." )

LUA_BINDING_BEGIN( Players, GetAll, "library", "Get all players." )
{
    lua_newtable( L );

    int index = 1;
    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
        if ( pPlayer )
        {
            CBasePlayer::PushLuaInstanceSafe( L, pPlayer );
            lua_rawseti( L, -2, index );
            index++;
        }
    }

    return 1;
}
LUA_BINDING_END( "table", "All players." )

LUA_BINDING_BEGIN( Players, GetCount, "library", "Get the number of players." )
{
    int count = 0;
    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        if ( UTIL_PlayerByIndex( i ) )
            count++;
    }

    lua_pushinteger( L, count );
    return 1;
}
LUA_BINDING_END( "integer", "The number of players." )

LUA_BINDING_BEGIN( Players, FindByIndex, "library", "Get a player by index." )
{
    CBasePlayer::PushLuaInstanceSafe( L, UTIL_PlayerByIndex( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "index" ) ) );
    return 1;
}
LUA_BINDING_END( "Player", "The player." )

LUA_BINDING_BEGIN( Players, FindByUserId, "library", "Get a player by user ID." )
{
    CBasePlayer::PushLuaInstanceSafe( L, UTIL_PlayerByUserId( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "user ID" ) ) );
    return 1;
}
LUA_BINDING_END( "Player", "The player." )

LUA_BINDING_BEGIN( Players, FindByName, "library", "Get a player by name." )
{
    CBasePlayer::PushLuaInstanceSafe( L, UTIL_PlayerByName( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" ) ) );
    return 1;
}
LUA_BINDING_END( "Player", "The player." )

#define STEAM_BASELINE Q_atoui64( "76561197960265728" )

LUA_BINDING_BEGIN( Players, FindBySteamId, "library", "Get a player by Steam ID." )
{
    const char *classicSteamID = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "Steam ID" );

    // Converts STEAM_{eUniverse}:{eAccountType}:{unAccountID} to a 64-bit SteamID
    int eUniverse, eAccountType;
    uint64 unAccountID;
    sscanf( classicSteamID, "STEAM_%d:%d:%llu", &eUniverse, &eAccountType, &unAccountID );
    uint64 product = unAccountID * 2;
    uint64 steamID = STEAM_BASELINE + product + eAccountType;

    CBasePlayer::PushLuaInstanceSafe( L, UTIL_PlayerBySteamID( steamID ) );
    return 1;
}
LUA_BINDING_END( "Player", "The player." )

LUA_BINDING_BEGIN( Players, FindBySteamId64, "library", "Get a player by Steam ID 64." )
{
    uint64 id = Q_atoui64( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "Steam ID 64" ) );
    CSteamID steamID( id );
    CBasePlayer::PushLuaInstanceSafe( L, UTIL_PlayerBySteamID( steamID ) );
    return 1;
}
LUA_BINDING_END( "Player", "The player." )

LUA_BINDING_BEGIN( Players, FindByUniqueID, "library", "Get a player by unique ID." )
{
    uint uniqueID = Q_atoi( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "unique ID" ) );

    CBasePlayer *player = NULL;

    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        player = UTIL_PlayerByIndex( i );
        if ( player && player->GetUniqueID() == uniqueID )
        {
            player = UTIL_PlayerByIndex( i );
            break;
        }
    }

    CBasePlayer::PushLuaInstanceSafe( L, player );
    return 1;
}
LUA_BINDING_END( "Player", "The player." )

LUA_BINDING_BEGIN( Players, ClientPrint, "library", "Print message to client" )
{
    CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int msgDest = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "msgDest" );
    const char *message = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "message" );
    const char *param1 = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 4, 0, "param1" );
    const char *param2 = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 5, 0, "param2" );
    const char *param3 = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 6, 0, "param3" );
    const char *param4 = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 7, 0, "param4" );
    ClientPrint( player, msgDest, message, param1, param2, param3, param4 );
    return 0;
}
LUA_BINDING_END()

#ifdef GAME_DLL

LUA_BINDING_BEGIN( Players, IsTeamEqual, "library", "Check if two teams are equal." )
{
    lua_pushboolean( L,
                     UTIL_TeamsMatch(
                         LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "team 1" ),
                         LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "team 2" ) ) );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the teams are equal." )

LUA_BINDING_BEGIN( Players, SayTextToAll, "library", "Say text to all players." )
{
    UTIL_SayTextAll(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "text" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optplayer, 2, 0, "speakingPlayer" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, true, "withoutStringLookup" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Players, ClientPrintToAll, "library", "Print text to all clients, optionally substituting 4 strings. The type decides where to show it: HUD_PRINTCENTER (1), HUD_PRINTNOTIFY (2), HUD_PRINTTALK (3), HUD_PRINTCONSOLE (4) - Enums don't exist yet" )  // TODO: Expose these enums
{
    UTIL_ClientPrintAll(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "messageType" ),
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, NULL, "subsitution1" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 4, NULL, "subsitution2" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 5, NULL, "subsitution3" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 6, NULL, "subsitution4" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Players, ShowMessageToAll, "library", "Show a message to all players." )
{
    UTIL_ShowMessageAll( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Players, ViewPunchAll, "library", "Punches the view of all clients within radius. If radius is 0, punches all clients.." )
{
    UTIL_ViewPunch(
        LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" ),
        LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "radius" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "shouldPunchPlayersInAir" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Players, GetPlayerConnectionInfo, "library", "Get the connection info of a player." )
{
    int ping, packetloss;
    UTIL_GetPlayerConnectionInfo( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "index" ), ping, packetloss );
    lua_pushinteger( L, ping );
    lua_pushinteger( L, packetloss );
    return 2;
}
LUA_BINDING_END( "integer", "The player's ping.", "integer", "The player's packet loss." )

#endif  // GAME_DLL

LUALIB_API int luaopen_Players( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Players );

    return 1;
}
