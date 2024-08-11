//========= Copyright © 1996-2005, Valve Corporation, All rights reserved.
//============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbasecombatweapon_shared.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseCombatWeapon *lua_toweapon( lua_State *L, int idx )
{
    CBaseHandle *hWeapon =
        dynamic_cast< CBaseHandle * >( ( CBaseHandle * )lua_touserdata( L, idx ) );
    if ( hWeapon == NULL )
        return NULL;
    return dynamic_cast< lua_CBaseCombatWeapon * >( hWeapon->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CBaseCombatWeapon *luaL_checkweapon( lua_State *L, int narg )
{
    lua_CBaseCombatWeapon *d = lua_toweapon( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CBaseCombatWeapon expected, got NULL entity" );
    return d;
}

LUA_REGISTRATION_INIT( CBaseCombatWeapon )

LUA_BINDING_BEGIN( CBaseCombatWeapon, AbortReload, "class", "Abort reload." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->AbortReload();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, ActivityListCount, "class", "Activity list count." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iCount = 0;
    pWeapon->ActivityList( iCount );
    lua_pushinteger( L, iCount );
    return 1;
}
LUA_BINDING_END( "integer", "Number of activities in the list." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, ActivityOverride, "class", "Asks which Activity override the weapon wants." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iActivity = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "activity" );
    bool bRequired = false;
    lua_pushinteger( L, pWeapon->ActivityOverride( ( Activity )iActivity, &bRequired ) );
    lua_pushboolean( L, bRequired );
    return 2;
}
LUA_BINDING_END( "integer", "Override activity ID.", "boolean", "Whether the activity is required." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, AddViewKick, "class", "Add view kick." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->AddViewKick();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, AllowsAutoSwitchFrom, "class", "Allows auto switch from." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->AllowsAutoSwitchFrom() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon allows auto switch from." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, AllowsAutoSwitchTo, "class", "Allows auto switch to." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->AllowsAutoSwitchTo() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon allows auto switch to." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, CalcViewmodelBob, "class", "Calculate viewmodel bob." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushnumber( L, pWeapon->CalcViewmodelBob() );
    return 1;
}
LUA_BINDING_END( "number", "Viewmodel bob." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, CanBePickedUpByNPCs, "class", "Can be picked up by NPCs." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->CanBePickedUpByNPCs() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon can be picked up by NPCs." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, CanBeSelected, "class", "Can be selected." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->CanBeSelected() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon can be selected." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, CanDeploy, "class", "Can deploy." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->CanDeploy() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon can deploy." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, CanHolster, "class", "Can holster." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->CanHolster() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon can holster." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, CanLower, "class", "Can lower." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->CanLower() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon can lower." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, CheckReload, "class", "Check reload." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->CheckReload();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, Clip1, "class", "The amount of ammo in the primary clip." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->Clip1() );
    return 1;
}
LUA_BINDING_END( "integer", "The amount of ammo in the primary clip." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, Clip2, "class", "The amount of ammo in the secondary clip." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->Clip2() );
    return 1;
}
LUA_BINDING_END( "integer", "The amount of ammo in the secondary clip." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, DefaultDeploy, "class", "Default deploy." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    const char *pszViewModel = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "viewModel" );
    const char *pszWorldModel = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "worldModel" );
    int iActivity = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "activity" );
    const char *pszAnimExtension = LUA_BINDING_ARGUMENT( luaL_checkstring, 5, "animationExtension" );
    lua_pushboolean( L, pWeapon->DefaultDeploy( strdup( pszViewModel ), strdup( pszWorldModel ), iActivity, strdup( pszAnimExtension ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon was successfully deployed." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, DefaultReload, "class", "Default reload." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iClipSize1 = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "clipSize1" );
    int iClipSize2 = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "clipSize2" );
    int iActivity = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "activity" );
    lua_pushboolean( L, pWeapon->DefaultReload( iClipSize1, iClipSize2, iActivity ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon was successfully reloaded." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, DefaultTouch, "class", "Default touch." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_CBaseEntity *pOther = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "touchingEntity" );
    pWeapon->DefaultTouch( pOther );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, Deploy, "class", "Deploy." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->Deploy() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon was successfully deployed." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, DisplayAltFireHudHint, "class", "Display alt fire HUD hint." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->DisplayAltFireHudHint();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, DisplayReloadHudHint, "class", "Display reload HUD hint." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->DisplayReloadHudHint();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, Drop, "class", "Drop." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    Vector vecThrowVelocity;

    if ( lua_isvector( L, 2 ) )
        vecThrowVelocity = LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkvector, 2, "velocity" );
    else
        vecThrowVelocity = vec3_origin;

    pWeapon->Drop( vecThrowVelocity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, FinishReload, "class", "Finish reload." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->FinishReload();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetActivity, "class", "Get activity." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetActivity() );
    return 1;
}
LUA_BINDING_END( "integer", "Activity ID." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetAnimPrefix, "class", "Get animation prefix." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushstring( L, pWeapon->GetAnimPrefix() );
    return 1;
}
LUA_BINDING_END( "string", "Animation prefix." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetBulletType, "class", "Get bullet type." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetBulletType() );
    return 1;
}
LUA_BINDING_END( "integer", "Bullet type." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetDamage, "class", "Get damage." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    float flDamage = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "damage" );
    int iLocation = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "location" );
    lua_pushnumber( L, pWeapon->GetDamage( flDamage, iLocation ) );
    return 1;
}
LUA_BINDING_END( "number", "Damage." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetDeathNoticeName, "class", "Get death notice name." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushstring( L, pWeapon->GetDeathNoticeName() );
    return 1;
}
LUA_BINDING_END( "string", "Death notice name." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetDefaultAnimSpeed, "class", "Get default animation speed." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushnumber( L, pWeapon->GetDefaultAnimSpeed() );
    return 1;
}
LUA_BINDING_END( "number", "Default animation speed." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetDefaultClip1, "class", "Get default clip 1." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetDefaultClip1() );
    return 1;
}
LUA_BINDING_END( "integer", "Default clip 1." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetDefaultClip2, "class", "Get default clip 2." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetDefaultClip2() );
    return 1;
}
LUA_BINDING_END( "integer", "Default clip 2." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetDrawActivity, "class", "Get draw activity." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetDrawActivity() );
    return 1;
}
LUA_BINDING_END( "integer", "Draw activity ID." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetFireRate, "class", "Get fire rate." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushnumber( L, pWeapon->GetFireRate() );
    return 1;
}
LUA_BINDING_END( "number", "Fire rate." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetIdealActivity, "class", "Get ideal activity." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetIdealActivity() );
    return 1;
}
LUA_BINDING_END( "integer", "Ideal activity ID." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetIdealSequence, "class", "Get ideal sequence." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetIdealSequence() );
    return 1;
}
LUA_BINDING_END( "integer", "Ideal sequence ID." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetMaxAutoAimDeflection, "class", "Get max auto aim deflection." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushnumber( L, pWeapon->GetMaxAutoAimDeflection() );
    return 1;
}
LUA_BINDING_END( "number", "Max auto aim deflection." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetMaxBurst, "class", "Get max burst." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetMaxBurst() );
    return 1;
}
LUA_BINDING_END( "integer", "Max burst." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetMaxClip1, "class", "Get max clip 1." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetMaxClip1() );
    return 1;
}
LUA_BINDING_END( "integer", "Max clip 1." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetMaxClip2, "class", "Get max clip 2." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetMaxClip2() );
    return 1;
}
LUA_BINDING_END( "integer", "Max clip 2." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetMaxRestTime, "class", "Get max rest time." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushnumber( L, pWeapon->GetMaxRestTime() );
    return 1;
}
LUA_BINDING_END( "number", "Max rest time." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetMinBurst, "class", "Get min burst." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetMinBurst() );
    return 1;
}
LUA_BINDING_END( "integer", "Min burst." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetMinRestTime, "class", "Get min rest time." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushnumber( L, pWeapon->GetMinRestTime() );
    return 1;
}
LUA_BINDING_END( "number", "Min rest time." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetClassName, "class", "Get class name." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushstring( L, pWeapon->GetName() );
    return 1;
}
LUA_BINDING_END( "string", "Class name." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetOwner, "class", "Get owner." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    CBaseEntity::PushLuaInstanceSafe( L, ( CBasePlayer * )pWeapon->GetOwner() );
    return 1;
}
LUA_BINDING_END( "entity", "Owner entity." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetSlotPosition, "class", "Get the weapon slot position in the selection screen." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetPosition() );
    return 1;
}
LUA_BINDING_END( "integer", "Position in the selection screen." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetPrimaryAmmoCount, "class", "Get the amount of primary ammo." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetPrimaryAmmoCount() );
    return 1;
}
LUA_BINDING_END( "integer", "Amount of primary ammo." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetPrimaryAmmoType, "class", "Get the primary ammo type." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetPrimaryAmmoType() );
    return 1;
}
LUA_BINDING_END( "integer", "Primary ammo type." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetPrimaryAttackActivity, "class", "Get the primary attack activity." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetPrimaryAttackActivity() );
    return 1;
}
LUA_BINDING_END( "integer", "Primary attack activity ID." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetPrintName, "class", "Get the print name." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushstring( L, pWeapon->GetPrintName() );
    return 1;
}
LUA_BINDING_END( "string", "Print name." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetRandomBurst, "class", "Get a random burst." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetRandomBurst() );
    return 1;
}
LUA_BINDING_END( "integer", "Random burst." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetRumbleEffect, "class", "Get the rumble effect." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetRumbleEffect() );
    return 1;
}
LUA_BINDING_END( "integer", "Rumble effect." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetSecondaryAmmoCount, "class", "Get the amount of secondary ammo." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetSecondaryAmmoCount() );
    return 1;
}
LUA_BINDING_END( "integer", "Amount of secondary ammo." )
//}
LUA_BINDING_BEGIN( CBaseCombatWeapon, GetSecondaryAmmoType, "class", "Get the secondary ammo type." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetSecondaryAmmoType() );
    return 1;
}
LUA_BINDING_END( "integer", "Secondary ammo type." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetSecondaryAttackActivity, "class", "Get the secondary attack activity." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetSecondaryAttackActivity() );
    return 1;
}
LUA_BINDING_END( "integer", "Secondary attack activity ID." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetShootSound, "class", "Get the shoot sound." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" );
    lua_pushstring( L, pWeapon->GetShootSound( iIndex ) );
    return 1;
}
LUA_BINDING_END( "string", "Shoot sound." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetSlot, "class", "Get the slot." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetSlot() );
    return 1;
}
LUA_BINDING_END( "integer", "Slot." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetSubType, "class", "Get the sub type." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetSubType() );
    return 1;
}
LUA_BINDING_END( "integer", "Sub type." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetViewModel, "class", "Get the view model." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iViewModelIndex = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0, "index" );
    lua_pushstring( L, pWeapon->GetViewModel( iViewModelIndex ) );
    return 1;
}
LUA_BINDING_END( "string", "View model." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetViewModelSequenceDuration, "class", "Get the view model sequence duration." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushnumber( L, pWeapon->GetViewModelSequenceDuration() );
    return 1;
}
LUA_BINDING_END( "number", "View model sequence duration." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetWeaponFlags, "class", "Get the weapon flags." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetWeaponFlags() );
    return 1;
}
LUA_BINDING_END( "integer", "Weapon flags." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetWeaponIdleTime, "class", "Get the weapon idle time." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushnumber( L, pWeapon->GetWeaponIdleTime() );
    return 1;
}
LUA_BINDING_END( "number", "Weapon idle time." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetWeight, "class", "Get the weight." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->GetWeight() );
    return 1;
}
LUA_BINDING_END( "integer", "Weight." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetWorldModel, "class", "Get the world model." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushstring( L, pWeapon->GetWorldModel() );
    return 1;
}
LUA_BINDING_END( "string", "World model." )

extern const char *pWeaponSoundCategories[NUM_SHOOT_SOUND_TYPES];

LUA_BINDING_BEGIN( CBaseCombatWeapon, GetWeaponData, "class", "Get weapon data" )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    const FileWeaponInfo_t &weaponInfo = pWeapon->GetWpnData();

    lua_newtable( L );
    lua_pushstring( L, "aShootSounds" );
    lua_newtable( L );

    for ( int i = EMPTY; i < NUM_SHOOT_SOUND_TYPES; i++ )
    {
        lua_pushstring( L, pWeaponSoundCategories[i] );
        const char *soundname = weaponInfo.aShootSounds[i];
        if ( soundname && soundname[0] )
        {
            lua_pushstring( L, soundname );
            lua_settable( L, -3 );
        }
        else
        {
            lua_pop( L, 1 );
        }
    }

    lua_settable( L, -3 );
    lua_pushstring( L, "bAutoSwitchFrom" );
    lua_pushboolean( L, weaponInfo.bAutoSwitchFrom );
    lua_settable( L, -3 );
    lua_pushstring( L, "bAutoSwitchTo" );
    lua_pushboolean( L, weaponInfo.bAutoSwitchTo );
    lua_settable( L, -3 );
    lua_pushstring( L, "bLoadedHudElements" );
    lua_pushboolean( L, weaponInfo.bLoadedHudElements );
    lua_settable( L, -3 );
    lua_pushstring( L, "bParsedScript" );
    lua_pushboolean( L, weaponInfo.bParsedScript );
    lua_settable( L, -3 );
    lua_pushstring( L, "bShowUsageHint" );
    lua_pushboolean( L, weaponInfo.bShowUsageHint );
    lua_settable( L, -3 );
    lua_pushstring( L, "iAmmo2Type" );
    lua_pushinteger( L, weaponInfo.iAmmo2Type );
    lua_settable( L, -3 );
    lua_pushstring( L, "iAmmoType" );
    lua_pushinteger( L, weaponInfo.iAmmoType );
    lua_settable( L, -3 );
    lua_pushstring( L, "iDefaultClip1" );
    lua_pushinteger( L, weaponInfo.iDefaultClip1 );
    lua_settable( L, -3 );
    lua_pushstring( L, "iDefaultClip2" );
    lua_pushinteger( L, weaponInfo.iDefaultClip2 );
    lua_settable( L, -3 );
    lua_pushstring( L, "iFlags" );
    lua_pushinteger( L, weaponInfo.iFlags );
    lua_settable( L, -3 );
    lua_pushstring( L, "iMaxClip1" );
    lua_pushinteger( L, weaponInfo.iMaxClip1 );
    lua_settable( L, -3 );
    lua_pushstring( L, "iMaxClip2" );
    lua_pushinteger( L, weaponInfo.iMaxClip2 );
    lua_settable( L, -3 );
    lua_pushstring( L, "iPosition" );
    lua_pushinteger( L, weaponInfo.iPosition );
    lua_settable( L, -3 );
    lua_pushstring( L, "iRumbleEffect" );
    lua_pushinteger( L, weaponInfo.iRumbleEffect );
    lua_settable( L, -3 );
    lua_pushstring( L, "iSlot" );
    lua_pushinteger( L, weaponInfo.iSlot );
    lua_settable( L, -3 );
    lua_pushstring( L, "iSpriteCount" );
    lua_pushinteger( L, weaponInfo.iSpriteCount );
    lua_settable( L, -3 );
    lua_pushstring( L, "iWeight" );
    lua_pushinteger( L, weaponInfo.iWeight );
    lua_settable( L, -3 );
    lua_pushstring( L, "m_bAllowFlipping" );
    lua_pushboolean( L, weaponInfo.m_bAllowFlipping );
    lua_settable( L, -3 );
    lua_pushstring( L, "m_bBuiltRightHanded" );
    lua_pushboolean( L, weaponInfo.m_bBuiltRightHanded );
    lua_settable( L, -3 );
    lua_pushstring( L, "m_bMeleeWeapon" );
    lua_pushboolean( L, weaponInfo.m_bMeleeWeapon );
    lua_settable( L, -3 );
    lua_pushstring( L, "szAmmo1" );
    lua_pushstring( L, weaponInfo.szAmmo1 );
    lua_settable( L, -3 );
    lua_pushstring( L, "szAmmo2" );
    lua_pushstring( L, weaponInfo.szAmmo2 );
    lua_settable( L, -3 );
    lua_pushstring( L, "szAnimationPrefix" );
    lua_pushstring( L, weaponInfo.szAnimationPrefix );
    lua_settable( L, -3 );
    lua_pushstring( L, "szClassName" );
    lua_pushstring( L, weaponInfo.szClassName );
    lua_settable( L, -3 );
    lua_pushstring( L, "szPrintName" );
    lua_pushstring( L, weaponInfo.szPrintName );
    lua_settable( L, -3 );
    lua_pushstring( L, "szViewModel" );
    lua_pushstring( L, weaponInfo.szViewModel );
    lua_settable( L, -3 );
    lua_pushstring( L, "szWorldModel" );
    lua_pushstring( L, weaponInfo.szWorldModel );
    lua_settable( L, -3 );

    return 1;
}
LUA_BINDING_END( "table", "Weapon data." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, GiveDefaultAmmo, "class", "Give default ammo." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->GiveDefaultAmmo();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, HandleFireOnEmpty, "class", "Handle fire on empty." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->HandleFireOnEmpty();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, HasAmmo, "class", "Check if the weapon has ammo." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->HasAmmo() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon has ammo." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, HasAnyAmmo, "class", "Check if the weapon has any ammo." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->HasAnyAmmo() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon has any ammo." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, HasPrimaryAmmo, "class", "Check if the weapon has primary ammo." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->HasPrimaryAmmo() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon has primary ammo." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, HasSecondaryAmmo, "class", "Check if the weapon has secondary ammo." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->HasSecondaryAmmo() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon has secondary ammo." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, HasWeaponIdleTimeElapsed, "class", "Check if the weapon idle time has elapsed." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->HasWeaponIdleTimeElapsed() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon idle time has elapsed." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, HideThink, "class", "Hide think." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->HideThink();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, IsAllowedToSwitch, "class", "Check if the weapon is allowed to switch." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->IsAllowedToSwitch() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon is allowed to switch." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, IsLocked, "class", "Check if the weapon is locked." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 2, "asker" );
    lua_pushboolean( L, pWeapon->IsLocked( pEntity ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon is locked." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, IsMeleeWeapon, "class", "Check if the weapon is a melee weapon." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->IsMeleeWeapon() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon is a melee weapon." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, IsPredicted, "class", "Check if the weapon is predicted." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->IsPredicted() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon is predicted." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, IsScripted, "class", "Check if the weapon is scripted." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->IsScripted() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon is scripted." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, IsViewModelSequenceFinished, "class", "Check if the view model sequence is finished." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->IsViewModelSequenceFinished() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the view model sequence is finished." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, IsWeaponVisible, "class", "Check if the weapon is visible." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->IsWeaponVisible() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon is visible." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, IsWeaponZoomed, "class", "Check if the weapon is zoomed." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->IsWeaponZoomed() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon is zoomed." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, ItemBusyFrame, "class", "Item busy frame." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->ItemBusyFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, ItemHolsterFrame, "class", "Item holster frame." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->ItemHolsterFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, ItemPostFrame, "class", "Item post frame." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->ItemPostFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, ItemPreFrame, "class", "Item pre frame." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->ItemPreFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, Lock, "class", "Lock the weapon." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "lockTime" );
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 3, "locker" );
    pWeapon->Lock( flTime, pEntity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, Lower, "class", "Lower the weapon." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->Lower() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon was lowered." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, MaintainIdealActivity, "class", "Maintain ideal activity." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->MaintainIdealActivity();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, OnActiveStateChanged, "class", "On active state changed." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iOldState = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "oldState" );
    pWeapon->OnActiveStateChanged( iOldState );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, PrimaryAttack, "class", "Primary attack." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->PrimaryAttack();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, Ready, "class", "Ready." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->Ready() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon is ready." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, Reload, "class", "Reload." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->Reload() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, ReloadOrSwitchWeapons, "class", "Reload or switch weapons." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->ReloadOrSwitchWeapons() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, RescindAltFireHudHint, "class", "Rescind alt fire HUD hint." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->RescindAltFireHudHint();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, RescindReloadHudHint, "class", "Rescind reload HUD hint." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->RescindReloadHudHint();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SecondaryAttack, "class", "Secondary attack." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->SecondaryAttack();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SendViewModelAnim, "class", "Send view model animation." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );
    pWeapon->SendViewModelAnim( iSequence );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SendWeaponAnim, "class", "Send weapon animation." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iActivity = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "activity" );
    lua_pushboolean( L, pWeapon->SendWeaponAnim( iActivity ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetActivity, "class", "Set activity." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iActivity = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "activity" );
    pWeapon->SetActivity( ( Activity )iActivity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetIdealActivity, "class", "Set ideal activity." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iActivity = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "activity" );
    lua_pushboolean( L, pWeapon->SetIdealActivity( ( Activity )iActivity ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetPickupTouch, "class", "Set pickup touch." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->SetPickupTouch();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetPrimaryAmmoCount, "class", "Set primary ammo count." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iCount = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "count" );
    pWeapon->SetPrimaryAmmoCount( iCount );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetSecondaryAmmoCount, "class", "Set secondary ammo count." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iCount = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "count" );
    pWeapon->SetSecondaryAmmoCount( iCount );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetSubType, "class", "Set sub type." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iSubType = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "subType" );
    pWeapon->SetSubType( iSubType );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetViewModel, "class", "Set view model." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->SetViewModel();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetViewModelIndex, "class", "Set view model index." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" );
    pWeapon->SetViewModelIndex( iIndex );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetWeaponIdleTime, "class", "Set weapon idle time." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" );
    pWeapon->SetWeaponIdleTime( flTime );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, SetWeaponVisible, "class", "Set weapon visible." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    bool bVisible = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "visible" );
    pWeapon->SetWeaponVisible( bVisible );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, ShouldDisplayAltFireHUDHint, "class", "Should display alt fire HUD hint." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->ShouldDisplayAltFireHUDHint() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the alt fire HUD hint should be displayed." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, ShouldDisplayReloadHUDHint, "class", "Should display reload HUD hint." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->ShouldDisplayReloadHUDHint() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the reload HUD hint should be displayed." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, ShouldShowControlPanels, "class", "Should show control panels." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->ShouldShowControlPanels() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the control panels should be shown." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, StartSprinting, "class", "Start sprinting." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->StartSprinting() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether sprinting was started." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, StopSprinting, "class", "Stop sprinting." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->StopSprinting() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether sprinting was stopped." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, StopWeaponSound, "class", "Stop weapon sound." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iSound = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "soundType" );
    pWeapon->StopWeaponSound( ( WeaponSound_t )iSound );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, UsesClipsForAmmo1, "class", "Uses clips for ammo 1." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->UsesClipsForAmmo1() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether clips are used for ammo 1." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, UsesClipsForAmmo2, "class", "Uses clips for ammo 2." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->UsesClipsForAmmo2() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether clips are used for ammo 2." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, UsesPrimaryAmmo, "class", "Uses primary ammo." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->UsesPrimaryAmmo() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether primary ammo is used." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, UsesSecondaryAmmo, "class", "Uses secondary ammo." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->UsesSecondaryAmmo() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether secondary ammo is used." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, VisibleInWeaponSelection, "class", "Visible in weapon selection." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushboolean( L, pWeapon->VisibleInWeaponSelection() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the weapon is visible in weapon selection." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, WeaponAutoAimScale, "class", "Weapon auto aim scale." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushnumber( L, pWeapon->WeaponAutoAimScale() );
    return 1;
}
LUA_BINDING_END( "number", "Weapon auto aim scale." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, WeaponIdle, "class", "Weapon idle." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    pWeapon->WeaponIdle();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, WeaponSound, "class", "Weapon sound." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    int iSound = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "soundType" );
    float flSoundTime = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0.0f, "soundTime" );
    pWeapon->WeaponSound( ( WeaponSound_t )iSound, flSoundTime );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseCombatWeapon, WeaponState, "class", "Weapon state." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( luaL_checkweapon, 1, "entity" );
    lua_pushinteger( L, pWeapon->WeaponState() );
    return 1;
}
LUA_BINDING_END( "number", "Weapon state." )

// static int CBaseCombatWeapon___index( lua_State *L )
//{
//     CBaseCombatWeapon *pWeapon = lua_toweapon( L, 1 );
//
//     LUA_METATABLE_INDEX_CHECK_VALID( L, CBaseEntity_IsValid );
//     LUA_METATABLE_INDEX_CHECK( L, pWeapon );
//
//     const char *field = luaL_checkstring( L, 2 );
//     if ( Q_strcmp( field, "m_bAltFiresUnderwater" ) == 0 )
//         lua_pushboolean( L, pWeapon->m_bAltFiresUnderwater );
//     else if ( Q_strcmp( field, "m_bFireOnEmpty" ) == 0 )
//         lua_pushboolean( L, pWeapon->m_bFireOnEmpty );
//     else if ( Q_strcmp( field, "m_bFiresUnderwater" ) == 0 )
//         lua_pushboolean( L, pWeapon->m_bFiresUnderwater );
//     else if ( Q_strcmp( field, "m_bInReload" ) == 0 )
//         lua_pushboolean( L, pWeapon->m_bInReload );
//     else if ( Q_strcmp( field, "m_bReloadsSingly" ) == 0 )
//         lua_pushboolean( L, pWeapon->m_bReloadsSingly );
//     else if ( Q_strcmp( field, "m_fFireDuration" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_fFireDuration );
//     else if ( Q_strcmp( field, "m_flNextEmptySoundTime" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_flNextEmptySoundTime );
//     else if ( Q_strcmp( field, "m_flNextPrimaryAttack" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_flNextPrimaryAttack );
//     else if ( Q_strcmp( field, "m_flNextSecondaryAttack" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_flNextSecondaryAttack );
//     else if ( Q_strcmp( field, "m_flTimeWeaponIdle" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_flTimeWeaponIdle );
//     else if ( Q_strcmp( field, "m_flUnlockTime" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_flUnlockTime );
//     else if ( Q_strcmp( field, "m_fMaxRange1" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_fMaxRange1 );
//     else if ( Q_strcmp( field, "m_fMaxRange2" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_fMaxRange2 );
//     else if ( Q_strcmp( field, "m_fMinRange1" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_fMinRange1 );
//     else if ( Q_strcmp( field, "m_fMinRange2" ) == 0 )
//         lua_pushnumber( L, pWeapon->m_fMinRange2 );
//     else if ( Q_strcmp( field, "m_iClip1" ) == 0 )
//         lua_pushinteger( L, pWeapon->m_iClip1 );
//     else if ( Q_strcmp( field, "m_iClip2" ) == 0 )
//         lua_pushinteger( L, pWeapon->m_iClip2 );
//     else if ( Q_strcmp( field, "m_iPrimaryAmmoType" ) == 0 )
//         lua_pushinteger( L, pWeapon->m_iPrimaryAmmoType );
//     else if ( Q_strcmp( field, "m_iSecondaryAmmoType" ) == 0 )
//         lua_pushinteger( L, pWeapon->m_iSecondaryAmmoType );
//     else if ( Q_strcmp( field, "m_iState" ) == 0 )
//         lua_pushinteger( L, pWeapon->m_iState );
//     else if ( Q_strcmp( field, "m_iSubType" ) == 0 )
//         lua_pushinteger( L, pWeapon->m_iSubType );
//     else if ( Q_strcmp( field, "m_iViewModelIndex" ) == 0 )
//         lua_pushinteger( L, pWeapon->m_iViewModelIndex );
//     else if ( Q_strcmp( field, "m_iWorldModelIndex" ) == 0 )
//         lua_pushinteger( L, pWeapon->m_iWorldModelIndex );
//     else if ( Q_strcmp( field, "m_nViewModelIndex" ) == 0 )
//         lua_pushinteger( L, pWeapon->m_nViewModelIndex );
//     else
//     {
//         LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pWeapon );
//
//         if ( lua_getmetatable( L, 1 ) )
//         {
//             LUA_METATABLE_INDEX_CHECK_TABLE( L );
//         }
//
//         luaL_getmetatable( L, LUA_BASECOMBATWEAPONLIBNAME );
//         LUA_METATABLE_INDEX_CHECK_TABLE( L );
//
//         LUA_METATABLE_INDEX_DERIVE_INDEX( L, LUA_BASEANIMATINGLIBNAME );
//
//         lua_pushnil( L );
//     }
//
//     return 1;
// }

// static int CBaseCombatWeapon___newindex( lua_State *L )
//{
//     CBaseCombatWeapon *pWeapon = lua_toweapon( L, 1 );
//
//     if ( pWeapon == NULL )
//     { /* avoid extra test when d is not 0 */
//         lua_Debug ar1;
//         lua_getstack( L, 1, &ar1 );
//         lua_getinfo( L, "fl", &ar1 );
//         lua_Debug ar2;
//         lua_getinfo( L, ">S", &ar2 );
//         lua_pushfstring( L, "%s:%d: attempt to index a NULL entity", ar2.short_src, ar1.currentline );
//         return lua_error( L );
//     }
//
//     const char *field = luaL_checkstring( L, 2 );
//
//     if ( Q_strcmp( field, "m_bAltFiresUnderwater" ) == 0 )
//         pWeapon->m_bAltFiresUnderwater = luaL_checkboolean( L, 3 );
//     else if ( Q_strcmp( field, "m_bFireOnEmpty" ) == 0 )
//         pWeapon->m_bFireOnEmpty = luaL_checkboolean( L, 3 );
//     else if ( Q_strcmp( field, "m_bFiresUnderwater" ) == 0 )
//         pWeapon->m_bFiresUnderwater = luaL_checkboolean( L, 3 );
//     else if ( Q_strcmp( field, "m_bInReload" ) == 0 )
//         pWeapon->m_bInReload = luaL_checkboolean( L, 3 );
//     else if ( Q_strcmp( field, "m_bReloadsSingly" ) == 0 )
//         pWeapon->m_bReloadsSingly = luaL_checkboolean( L, 3 );
//     else if ( Q_strcmp( field, "m_fFireDuration" ) == 0 )
//         pWeapon->m_fFireDuration = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_flNextEmptySoundTime" ) == 0 )
//         pWeapon->m_flNextEmptySoundTime = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_flNextPrimaryAttack" ) == 0 )
//         pWeapon->m_flNextPrimaryAttack.GetForModify() = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_flNextSecondaryAttack" ) == 0 )
//         pWeapon->m_flNextSecondaryAttack.GetForModify() =
//             luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_flTimeWeaponIdle" ) == 0 )
//         pWeapon->m_flTimeWeaponIdle.GetForModify() = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_flUnlockTime" ) == 0 )
//         pWeapon->m_flUnlockTime = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_fMaxRange1" ) == 0 )
//         pWeapon->m_fMaxRange1 = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_fMaxRange2" ) == 0 )
//         pWeapon->m_fMaxRange2 = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_fMinRange1" ) == 0 )
//         pWeapon->m_fMinRange1 = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_fMinRange2" ) == 0 )
//         pWeapon->m_fMinRange2 = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_iClip1" ) == 0 )
//         pWeapon->m_iClip1.GetForModify() = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_iClip2" ) == 0 )
//         pWeapon->m_iClip2.GetForModify() = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_iPrimaryAmmoType" ) == 0 )
//         pWeapon->m_iPrimaryAmmoType.GetForModify() = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_iSecondaryAmmoType" ) == 0 )
//         pWeapon->m_iSecondaryAmmoType.GetForModify() = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_iState" ) == 0 )
//         pWeapon->m_iState.GetForModify() = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_iSubType" ) == 0 )
//         pWeapon->m_iSubType = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_iViewModelIndex" ) == 0 )
//         pWeapon->m_iViewModelIndex.GetForModify() = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_iWorldModelIndex" ) == 0 )
//         pWeapon->m_iWorldModelIndex.GetForModify() = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_nViewModelIndex" ) == 0 )
//         pWeapon->m_nViewModelIndex.GetForModify() = luaL_checknumber( L, 3 );
//     else
//     {
//         LUA_GET_REF_TABLE( L, pWeapon );
//         lua_pushvalue( L, 3 );
//         lua_setfield( L, -2, field );
//         lua_pop( L, 1 );
//     }
//
//     return 0;
// }

LUA_BINDING_BEGIN( CBaseCombatWeapon, __eq, "class", "Equality comparison." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( lua_toweapon, 1, "entity" );
    lua_CBaseCombatWeapon *pOtherWeapon = LUA_BINDING_ARGUMENT( lua_toweapon, 2, "otherEntity" );
    lua_pushboolean( L, pWeapon == pOtherWeapon );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the two entities are equal." )

LUA_BINDING_BEGIN( CBaseCombatWeapon, __tostring, "class", "String representation." )
{
    lua_CBaseCombatWeapon *pWeapon = LUA_BINDING_ARGUMENT( lua_toweapon, 1, "entity" );
    if ( pWeapon == NULL )
        lua_pushstring( L, "NULL" );
    else
        lua_pushfstring( L, "CBaseCombatWeapon: %d %s", pWeapon->entindex(), pWeapon->GetClassname() );
    return 1;
}
LUA_BINDING_END( "string", "String representation." )

/*
** Open CBaseCombatWeapon object
*/
LUALIB_API int luaopen_CBaseCombatWeapon( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_BASECOMBATWEAPONLIBNAME );

    lua_pushstring( L, "Entity" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */

    LUA_REGISTRATION_COMMIT( CBaseCombatWeapon );

    return 1;
}
