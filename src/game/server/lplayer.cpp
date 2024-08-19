#include "cbase.h"
#include "luamanager.h"
#include "lbasecombatweapon_shared.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "ltakedamageinfo.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Player )

LUA_BINDING_BEGIN( Player, GiveAmmo, "class", "Give ammo to the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int ammoCount = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "ammoCount" );

    switch ( lua_type( L, 3 ) )
    {
        case LUA_TNUMBER:
        default:
            lua_pushinteger( L, player->GiveAmmo( ammoCount, LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "ammoType" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "shouldSuppressSound" ) ) );
            break;
        case LUA_TSTRING:
            lua_pushinteger( L, player->GiveAmmo( ammoCount, LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "ammoName" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "shouldSuppressSound" ) ) );
            break;
    }

    return 1;
}
LUA_BINDING_END( "integer", "The amount of ammo actually given." )

LUA_BINDING_BEGIN( Player, SetBodyPitch, "class", "Set the body pitch of the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    float pitch = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "pitch" );
    player->SetBodyPitch( pitch );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, CreateViewModel, "class", "Create a view model for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int viewModelIndex = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 2, 0, "viewModelIndex" );
    player->CreateViewModel( viewModelIndex );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, HideViewModels, "class", "Hide all view models for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->HideViewModels();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, DestroyViewModels, "class", "Destroy all view models for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->DestroyViewModels();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, LockPlayerInPlace, "class", "Lock the player in place." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->LockPlayerInPlace();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, UnlockPlayer, "class", "Unlock the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->UnlockPlayer();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ForceRespawn, "class", "Force the player to respawn." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ForceRespawn();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, InitialSpawn, "class", "Perform the initial spawn for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->InitialSpawn();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, InitHud, "class", "Initialize the HUD for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->InitHUD();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, PlayerDeathThink, "class", "Think about death for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->PlayerDeathThink();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, Jump, "class", "Make the player jump." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->Jump();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, Duck, "class", "Make the player duck." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->Duck();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ForceSimulation, "class", "Force the player to simulate." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ForceSimulation();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, DamageEffect, "class", "Apply a damage effect to the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    float damage = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damage" );
    int damageType = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "damageType" );
    player->DamageEffect( damage, damageType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, PauseBonusProgress, "class", "Pause or unpause the bonus progress for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    bool shouldPause = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, true, "shouldPause" );
    player->PauseBonusProgress( shouldPause );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ShowMessage, "class", "Show a message to the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    UTIL_ShowMessage( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" ), player );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetBonusProgress, "class", "Set the bonus progress for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int progress = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "progress" );
    player->SetBonusProgress( progress );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetBonusChallenge, "class", "Set the bonus challenge for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int challenge = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "challenge" );
    player->SetBonusChallenge( challenge );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SnapEyeAngles, "class", "Snap the eye angles of the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    QAngle angles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angles" );
    player->SnapEyeAngles( angles );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetBodyAngles, "class", "Get the body angles of the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushangle( L, player->BodyAngles() );
    return 1;
}
LUA_BINDING_END( "Angle", "The body angles." )

LUA_BINDING_BEGIN( Player, GetBodyTarget, "class", "Get the body target of the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    Vector target = player->BodyTarget( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "target" ), LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "isNoisy" ) );
    lua_pushvector( L, target );
    return 1;
}
LUA_BINDING_END( "Vector", "The body target." )

LUA_BINDING_BEGIN( Player, ShouldFadeOnDeath, "class", "Check if the player should fade on death." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->ShouldFadeOnDeath() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player should fade on death." )

LUA_BINDING_BEGIN( Player, OnEventTakeDamageAlive, "class", "Call to handle taking damage while alive for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    int result = player->OnTakeDamage_Alive( LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 2, "info" ) );
    lua_pushinteger( L, result );
    return 1;
}
LUA_BINDING_END( "integer", "The result of taking damage." )

LUA_BINDING_BEGIN( Player, OnEventDying, "class", "Call to handle dying for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->Event_Dying( LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 2, "info" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, IsFakeClient, "class", "Check if the player is a fake client." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsFakeClient() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is a fake client." )

LUA_BINDING_BEGIN( Player, GetClientIndex, "class", "Get the client index of the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetClientIndex() );
    return 1;
}
LUA_BINDING_END( "integer", "The client index." )

LUA_BINDING_BEGIN( Player, SetPlayerName, "class", "Set the name of the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetPlayerName( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetNetworkIdString, "class", "Get the network ID string of the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushstring( L, player->GetNetworkIDString() );
    return 1;
}
LUA_BINDING_END( "string", "The network ID string." )

LUA_BINDING_BEGIN( Player, ShowViewModel, "class", "Show or hide the view model for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ShowViewModel( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldShow" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ShowCrosshair, "class", "Show or hide the crosshair for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ShowCrosshair( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldShow" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, PackDeadPlayerItems, "class", "Pack the dead player items." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->PackDeadPlayerItems();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, RemoveAllItems, "class", "Remove all items from the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->RemoveAllItems( LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "shouldRemoveSuit" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, IsDead, "class", "Check if the player is dead." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsDead() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is dead." )

LUA_BINDING_BEGIN( Player, HasPhysicsFlag, "class", "Check if the player has a physics flag." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->HasPhysicsFlag( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flag" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player has the physics flag." )

LUA_BINDING_BEGIN( Player, CanUseWeapon, "class", "Check if the player can use a weapon." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->Weapon_CanUse( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player can use the weapon." )

LUA_BINDING_BEGIN( Player, EquipWeapon, "class", "Equip a weapon for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->Weapon_Equip( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, DropWeapon, "class", "Drop a weapon for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->Weapon_Drop(
        LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ),
        &LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optvector, 3, NULL, "position" ),
        &LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optvector, 4, NULL, "velocity" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, DropWeaponInSlot, "class", "Drop the weapon in the specified slot." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->Weapon_DropSlot( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "slot" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetLastWeapon, "class", "Get the previous weapon of the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->GetLastWeapon() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, HasAnyAmmoOfType, "class", "Check if the player has any ammo of the specified type." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->HasAnyAmmoOfType( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "type" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player has any ammo of the specified type." )

LUA_BINDING_BEGIN( Player, RumbleEffect, "class", "Apply a rumble effect to the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->RumbleEffect(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "duration" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "flags" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, IsOnLadder, "class", "Check if the player is on a ladder." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsOnLadder() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player is on a ladder." )

LUA_BINDING_BEGIN( Player, SetFlashlightEnabled, "class", "Enable or disable the flashlight for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetFlashlightEnabled( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldEnable" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, IsFlashlightOn, "class", "Check if the flashlight is on for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->FlashlightIsOn() );
    return 1;
}
LUA_BINDING_END( "integer", "The flashlight state." )

LUA_BINDING_BEGIN( Player, TurnFlashlightOn, "class", "Turn the flashlight on for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->FlashlightTurnOn();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, TurnFlashlightOff, "class", "Turn the flashlight off for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->FlashlightTurnOff();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, IsIlluminatedByFlashlight, "class", "Check if the player is illuminated by the flashlight." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    float flDot;
    lua_pushboolean( L, player->IsIlluminatedByFlashlight( LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "entity" ), &flDot ) );
    lua_pushnumber( L, flDot );
    return 2;
}
LUA_BINDING_END( "boolean", "Whether the player is illuminated by the flashlight.", "number", "The dot product." )

LUA_BINDING_BEGIN( Player, UpdatePlayerSound, "class", "Update the player sound." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->UpdatePlayerSound();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, PlayDeathSound, "class", "Play the death sound for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->DeathSound( LUA_BINDING_ARGUMENT( luaL_checkdamageinfo, 2, "info" ) );
    return 0;
}
LUA_BINDING_END()

// Experiment; SetWeaponAnimType has no existing implementation anywhere
// LUA_BINDING_BEGIN( Player, SetWeaponAnimationType, "class", "Set the weapon animation type for the player." )
//{
//    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
//    player->SetWeaponAnimType( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "extension" ) );
//    return 0;
//}
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ImpulseCommands, "class", "Handle impulse commands for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ImpulseCommands();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, CheatImpulseCommands, "class", "Handle cheat impulse commands for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->CheatImpulseCommands( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "impulse" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, NotifySinglePlayerGameEnding, "class", "Notify the player that the single player game is ending." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->NotifySinglePlayerGameEnding();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, IsSinglePlayerGameEnding, "class", "Check if the single player game is ending for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsSinglePlayerGameEnding() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the single player game is ending." )

LUA_BINDING_BEGIN( Player, StartObserverMode, "class", "Start the observer mode for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->StartObserverMode( LUA_BINDING_ARGUMENT_ENUM( OBSERVER_MODE, 2, "mode " ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the observer mode was started." )

LUA_BINDING_BEGIN( Player, StopObserverMode, "class", "Stop the observer mode for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->StopObserverMode();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ModeWantsSpectatorGui, "class", "Check if the observer mode wants the spectator GUI for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->ModeWantsSpectatorGUI( LUA_BINDING_ARGUMENT_ENUM( OBSERVER_MODE, 2, "mode " ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the observer mode wants the spectator GUI." )

LUA_BINDING_BEGIN( Player, FindNextObserverTarget, "class", "Find the next observer target for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->FindNextObserverTarget( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "reverse" ) ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetNextObserverSearchStartPoint, "class", "Get the next observer search start point for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetNextObserverSearchStartPoint( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "reverse" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The next observer search start point." )

LUA_BINDING_BEGIN( Player, IsValidObserverTarget, "class", "Check if the observer target is valid for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->IsValidObserverTarget( LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "target" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the observer target is valid." )

LUA_BINDING_BEGIN( Player, CheckObserverSettings, "class", "Check the observer settings for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->CheckObserverSettings();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, JumpToPosition, "class", "Jump to the specified position for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->JumptoPosition(
        LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" ),
        LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "angle" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ForceObserverMode, "class", "Force the observer mode for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ForceObserverMode( LUA_BINDING_ARGUMENT_ENUM( OBSERVER_MODE, 2, "mode " ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ValidateCurrentObserverTarget, "class", "Validate the current observer target for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ValidateCurrentObserverTarget();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, StartReplayMode, "class", "Start the replay mode for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->StartReplayMode( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "delay" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "duration" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "entityIndex" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the replay mode was started." )

LUA_BINDING_BEGIN( Player, StopReplayMode, "class", "Stop the replay mode for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->StopReplayMode();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetDelayTicks, "class", "Get the delay ticks for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetDelayTicks() );
    return 1;
}
LUA_BINDING_END( "integer", "The delay ticks." )

LUA_BINDING_BEGIN( Player, GetReplayEntity, "class", "Get the replay entity for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetReplayEntity() );
    return 1;
}
LUA_BINDING_END( "integer", "The replay entity." )

LUA_BINDING_BEGIN( Player, CreateCorpse, "class", "Create a corpse for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->CreateCorpse();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SelectSpawnPoint, "class", "Select a spawn point for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L, player->EntSelectSpawnPoint() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, GetVehicleAnalogControlBias, "class", "Get the vehicle analog control bias for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushinteger( L, player->GetVehicleAnalogControlBias() );
    return 1;
}
LUA_BINDING_END( "integer", "The vehicle analog control bias." )

LUA_BINDING_BEGIN( Player, SetVehicleAnalogControlBias, "class", "Set the vehicle analog control bias for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetVehicleAnalogControlBias( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bias" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, OnVehicleStart, "class", "Handle the vehicle start for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->OnVehicleStart();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, BumpWeapon, "class", "Bump the weapon for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->BumpWeapon( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon was bumped." )

LUA_BINDING_BEGIN( Player, RemoveWeapon, "class", "Remove the weapon for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->RemovePlayerItem( LUA_BINDING_ARGUMENT( luaL_checkweapon, 2, "weapon" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon was removed." )

LUA_BINDING_BEGIN( Player, HasWeapons, "class", "Check if the player has weapons." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_pushboolean( L, player->HasWeapons() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the player has weapons." )

LUA_BINDING_BEGIN( Player, GiveItem, "class", "Give the named weapon to the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    CBaseEntity::PushLuaInstanceSafe( L,
                                      player->GiveNamedItem(
                                          LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ),
                                          LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 3, 0, "amount" ) ) );
    return 1;
}
LUA_BINDING_END( "Entity", "The item given to the player." )

LUA_BINDING_BEGIN( Player, Give, "class", "Give the weapon to the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    const char *className = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "className" );
    bool shouldGiveNoAmmo = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "shouldGiveNoAmmo" );
    CBaseCombatWeapon *weapon = player->Weapon_Create( className );

    //CBaseEntity *entity = player->GiveNamedItem( className, 0 );

    //if ( !entity )
    //{
    //    // Happens when the player already has the weapon, or the weapon is invalid
    //    return 0;
    //}

    //CBaseAnimating *animating = dynamic_cast<CBaseAnimating *>( entity );

    // Let's make sure the weapon has a valid model, if not, set it to the error model
    if ( !weapon->GetModelPtr() )
    {
        weapon->SetModel( "models/error.mdl" );
    }

    player->Weapon_Equip( weapon, !shouldGiveNoAmmo );

    //if ( !weapon )
    //{
    //    // To match the behavior in Garry's Mod, we allow giving any type of entity
    //    // But we only continue if it's a weapon
    //    CBaseEntity::PushLuaInstanceSafe( L, entity );
    //    return 1;
    //}

    CBaseEntity::PushLuaInstanceSafe( L, weapon );
    return 1;
}
LUA_BINDING_END( "Weapon", "The weapon given to the player." )

LUA_BINDING_BEGIN( Player, EnableControl, "class", "Enable or disable control for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->EnableControl( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "shouldEnable" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, CheckTrainUpdate, "class", "Check the train update for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->CheckTrainUpdate();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, EquipSuit, "class", "Equip or remove the suit for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->EquipSuit( LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, true, "shouldPlayEffects" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, RemoveSuit, "class", "Remove the suit for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->RemoveSuit();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SayText, "library", "Say text to a player." )
{
    UTIL_SayText(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "text" ),
        LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetTeam, "class", "Change the team for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ChangeTeam( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "team" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetObserverMode, "class", "Set the observer mode for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->ForceObserverMode( LUA_BINDING_ARGUMENT_ENUM( OBSERVER_MODE, 2, "mode " ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, SetObserverTarget, "class", "Set the observer target for the player." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    player->SetObserverTarget( LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "target" ) );
    return 0;
}
LUA_BINDING_END()
//
// static int luasrc_Util_PlayerDecalTrace( lua_State *L )
//{
//    UTIL_PlayerDecalTrace( &luaL_checktrace( L, 1 ), luaL_checknumber( L, 2 ) );
//    return 0;
//}
//
// static int luasrc_Util_ClipPunchAngleOffset( lua_State *L )
//{
//    UTIL_ClipPunchAngleOffset( luaL_checkangle( L, 1 ), luaL_checkplayer( L, 2 )->m_Local.m_vecPunchAngle, luaL_checkangle( L, 3 ) );
//    return 0;
//}

LUA_BINDING_BEGIN( Player, SprayDecal, "class", "Spray a decal for the player based on a trace with start and end." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    lua_Vector traceStart = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "traceStart" );
    lua_Vector traceEnd = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "traceEnd" );

    trace_t trace;
    UTIL_TraceLine( traceStart, traceEnd, MASK_SOLID_BRUSHONLY, player, COLLISION_GROUP_NONE, &trace );

    UTIL_PlayerDecalTrace( &trace, player->entindex() );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Player, ClampPunchAngleOffset, "class", "Adds angleToClamp with the player's punch angle and clamps it between -clampTo and clampTo." )
{
    lua_CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, 1, "player" );
    QAngle angleToClamp = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "start" );
    QAngle clampTo = LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "clampTo" );

    UTIL_ClipPunchAngleOffset( angleToClamp, player->m_Local.m_vecPunchAngle, clampTo );

    return 0;
}
LUA_BINDING_END()

/*
** Open CBasePlayer object
*/
LUALIB_API int luaopen_CBasePlayer( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEPLAYERMETANAME );

    LUA_REGISTRATION_COMMIT( Player );

    return 1;
}
