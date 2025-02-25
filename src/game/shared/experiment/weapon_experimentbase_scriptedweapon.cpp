//========= Copyright © 1996-2005, Valve Corporation, All rights reserved.
//============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"

#if defined( CLIENT_DLL )
#include "c_experiment_player.h"
#else
#include "experiment_player.h"
#endif

#include "weapon_experimentbase_scriptedweapon.h"
#include "in_buttons.h"
#include "ammodef.h"
#include "luamanager.h"
#include "lbasecombatweapon_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// clang-format off

IMPLEMENT_NETWORKCLASS_ALIASED( ExperimentScriptedWeapon, DT_ExperimentScriptedWeapon )

BEGIN_NETWORK_TABLE( CExperimentScriptedWeapon, DT_ExperimentScriptedWeapon )
#ifdef CLIENT_DLL
    RecvPropString( RECVINFO( m_iScriptedClassname ) ),
#else
    SendPropString( SENDINFO( m_iScriptedClassname ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CExperimentScriptedWeapon )
END_PREDICTION_DATA()

//=========================================================
//	>> CHLSelectFireScriptedWeapon
//=========================================================
BEGIN_DATADESC( CExperimentScriptedWeapon ) END_DATADESC()

// LINK_ENTITY_TO_CLASS( weapon_experimentbase_scriptedweapon, CExperimentScriptedWeapon ); PRECACHE_WEAPON_REGISTER( weapon_experimentbase_scriptedweapon );

static bool WORKAROUND_NASTY_FORMATTING_BUG;  // clang-format on

// These functions replace the macros above for runtime registration of
// scripted weapons.
#ifdef CLIENT_DLL
static C_BaseEntity *CCExperimentScriptedWeaponFactory( void )
{
    return static_cast< C_BaseEntity * >( new CExperimentScriptedWeapon );
};
#endif

#ifndef CLIENT_DLL
static CUtlDict< CEntityFactory< CExperimentScriptedWeapon > *, unsigned short >
    m_WeaponFactoryDatabase;
#endif

// Experiment; Commented to instead go ask Lua when we encounter a non-C-registered weapon
// void RegisterScriptedWeapon( const char *className )
//{
//    if (Q_strcmp(className, "exp_keys") == 0)
//    {
//        Warning("breakpoint!\n");
//    }
//
// #ifdef CLIENT_DLL
//    if ( GetClassMap().FindFactory( className ) )
//    {
//        return;
//    }
//
//    GetClassMap().Add( className, "CExperimentScriptedWeapon", sizeof( CExperimentScriptedWeapon ), &CCExperimentScriptedWeaponFactory, true );
// #else
//    if ( EntityFactoryDictionary()->FindFactory( className ) )
//    {
//        return;
//    }
//
//    unsigned short lookup = m_WeaponFactoryDatabase.Find( className );
//    if ( lookup != m_WeaponFactoryDatabase.InvalidIndex() )
//    {
//        return;
//    }
//
//    // Andrew; This fixes months worth of pain and anguish.
//    CEntityFactory< CExperimentScriptedWeapon > *pFactory =
//        new CEntityFactory< CExperimentScriptedWeapon >( className );
//
//    lookup = m_WeaponFactoryDatabase.Insert( className, pFactory );
//    Assert( lookup != m_WeaponFactoryDatabase.InvalidIndex() );
// #endif
//    // BUGBUG: When attempting to precache weapons registered during runtime,
//    // they don't appear as valid registered entities.
//    // static CPrecacheRegister
//    // precache_weapon_(&CPrecacheRegister::PrecacheFn_Other, className);
//}

void ResetWeaponFactoryDatabase( void )
{
#ifdef CLIENT_DLL
#ifdef LUA_SDK
    GetClassMap().RemoveAllScripted();
#endif
#else
    for ( int i = m_WeaponFactoryDatabase.First();
          i != m_WeaponFactoryDatabase.InvalidIndex();
          i = m_WeaponFactoryDatabase.Next( i ) )
    {
        delete m_WeaponFactoryDatabase[i];
    }
    m_WeaponFactoryDatabase.RemoveAll();
#endif
}

// acttable_t CExperimentScriptedWeapon::m_acttable[] =
// {
// 	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_PISTOL,
// false }, 	{ ACT_MP_CROUCH_IDLE,
// ACT_HL2MP_IDLE_CROUCH_PISTOL,			false },
//
// 	{ ACT_MP_RUN, ACT_HL2MP_RUN_PISTOL,
// false }, 	{ ACT_MP_CROUCHWALK,
// ACT_HL2MP_WALK_CROUCH_PISTOL,			false },
//
// 	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,
// ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false }, 	{
// ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,
// false },
//
// 	{ ACT_MP_RELOAD_STAND,
// ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false }, 	{ ACT_MP_RELOAD_CROUCH,
// ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false },
//
// 	{ ACT_MP_JUMP,
// ACT_HL2MP_JUMP_PISTOL,					false },
// };

// IMPLEMENT_ACTTABLE( CExperimentScriptedWeapon );

// These functions serve as skeletons for the our weapons' actions to be
// implemented in Lua.
acttable_t *CExperimentScriptedWeapon::ActivityList( void )
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "m_acttable" );
    lua_remove( L, -2 );  // Remove the reference table

    if ( lua_istable( L, -1 ) )
    {
        for ( int i = 0; i < LUA_MAX_WEAPON_ACTIVITIES; i++ )
        {
            lua_pushinteger( L, i );
            lua_gettable( L, -2 );
            if ( lua_istable( L, -1 ) )
            {
                m_acttable[i].baseAct = ACT_INVALID;
                lua_pushinteger( L, 1 );
                lua_gettable( L, -2 );
                if ( lua_isnumber( L, -1 ) )
                    m_acttable[i].baseAct = lua_tointeger( L, -1 );
                lua_pop( L, 1 );

                m_acttable[i].weaponAct = ACT_INVALID;
                lua_pushinteger( L, 2 );
                lua_gettable( L, -2 );
                if ( lua_isnumber( L, -1 ) )
                    m_acttable[i].weaponAct = lua_tointeger( L, -1 );
                lua_pop( L, 1 );

                m_acttable[i].required = false;
                lua_pushinteger( L, 3 );
                lua_gettable( L, -2 );
                if ( lua_isboolean( L, -1 ) )
                    m_acttable[i].required = ( bool )lua_toboolean( L, -1 );
                lua_pop( L, 1 );
            }
            lua_pop( L, 1 );
        }
    }
    lua_pop( L, 1 );
#endif
    return m_acttable;
}
int CExperimentScriptedWeapon::ActivityListCount( void )
{
    return LUA_MAX_WEAPON_ACTIVITIES;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CExperimentScriptedWeapon::CExperimentScriptedWeapon( void )
{
    m_pLuaWeaponInfo = dynamic_cast< CExperimentSWeaponInfo * >( CreateWeaponInfo() );

#ifdef LUA_SDK
    // Experiment; Already done in CBaseEntity.
    // m_nTableReference = LUA_NOREF;
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CExperimentScriptedWeapon::~CExperimentScriptedWeapon( void )
{
    delete m_pLuaWeaponInfo;
    // Andrew; This is actually done in CBaseEntity. I'm doing it here because
    // this is the class that initialized the reference.
#ifdef LUA_SDK
    if ( L )
        lua_unref( L, m_nTableReference );
#endif
}

#ifdef LUA_SDK
//-----------------------------------------------------------------------------
// Purpose: Initialize any variables in the reference table
//-----------------------------------------------------------------------------
void CExperimentScriptedWeapon::SetupRefTable( lua_State *L )
{
    lua_newtable( L );
    m_nTableReference = luaL_ref( L, LUA_REGISTRYINDEX );

    // We need the classname, so we can initialize the weapon based on the
    // values given when registering with ScriptedWeapons.Register.
    Assert( m_iScriptedClassname != nullptr );
    Assert( Q_strlen( m_iScriptedClassname ) > 0 );

    lua_getglobal( L, LUA_SCRIPTEDWEAPONSLIBNAME );

    if ( lua_istable( L, -1 ) )
    {
        lua_getfield( L, -1, "InitializeRefTable" );
        if ( lua_isfunction( L, -1 ) )
        {
            // Initialize the ref table with copies of the registered values for this weapon.
            lua_remove( L, -2 );  // Remove the library table
            lua_getref( L, m_nTableReference );
            lua_pushstring( L, m_iScriptedClassname );
            luasrc_pcall( L, 2, 0 );
        }
        else
        {
            lua_pop( L, 2 );  // Remove the library table and the function
            Error( "ScriptedWeapons library does not have InitializeRefTable function!\n" );
        }
    }
    else
    {
        lua_pop( L, 1 );
        Error( "ScriptedWeapons library not found!\n" );
    }
}

// Override the base class, so we call OnRemove on the Lua scripted weapon.
void CExperimentScriptedWeapon::Remove()
{
    bool fullUpdate = false;  // TODO: implement this argument https://wiki.facepunch.com/gmod/Entity:OnRemove
    LUA_CALL_WEAPON_METHOD_BEGIN( "OnRemove" );
    lua_pushboolean( L, fullUpdate );  // doc: fullUpdate (always false, unimplemented currently)
    LUA_CALL_WEAPON_METHOD_END( 1, 0 );

    BaseClass::Remove();
}
#endif

extern const char *pWeaponSoundCategories[NUM_SHOOT_SOUND_TYPES];

#ifdef CLIENT_DLL
extern ConVar hud_fastswitch;
#endif

void CExperimentScriptedWeapon::InitScriptedWeapon( void )
{
#ifdef LUA_SDK
    // #ifndef CLIENT_DLL
    //     // Let the instance reinitialize itself for the client. <- Experiment; TODO: Is this correct?
    //     if ( lua_isrefvalid( L, m_nTableReference ) )
    //         return;
    // #endif

    char className[MAX_WEAPON_STRING];
#if defined( CLIENT_DLL )
    if ( strlen( GetScriptedClassname() ) == 0 )
    {
        // Experiment;
        // Wait for the client to receive the actual classname from the server.
        // The entity will arrive before the networked data.
        // TODO: Can we wait before spawning locally? Because those checks like
        // `if ( lua_isrefvalid( L, m_nTableReference ) )` below may cause
        // unexpected behavior (e.g: baseclass methods deciding differently
        // from the scripted methods).
        // To fix the above I tried to override ::Spawn (see below)
        return;
    }

    Q_strncpy( className, GetScriptedClassname(), sizeof( className ) );
#else
    Q_strncpy( m_iScriptedClassname.GetForModify(), GetClassname(), sizeof( className ) );
    Q_strncpy( className, GetClassname(), sizeof( className ) );
#endif

    Q_strlower( className );
    // Andrew; This redundancy is pretty annoying.
    // Classname
    Q_strncpy( m_pLuaWeaponInfo->szClassName, className, MAX_WEAPON_STRING );
    SetClassname( m_pLuaWeaponInfo->szClassName );

    m_pLuaWeaponInfo->bParsedScript = true;

    // Get (or init) the ref table (popped near the end of this function)
    LUA_GET_REF_TABLE( L, this );

    // Printable name
    lua_getfield( L, -1, "PrintName" );
    if ( lua_isstring( L, -1 ) )
    {
        Q_strncpy( m_pLuaWeaponInfo->szPrintName, lua_tostring( L, -1 ), MAX_WEAPON_STRING );
    }
    else
    {
        Q_strncpy( m_pLuaWeaponInfo->szPrintName, WEAPON_PRINTNAME_MISSING, MAX_WEAPON_STRING );
    }
    lua_pop( L, 1 );  // Pop the PrintName field

    // ViewModel
    lua_getfield( L, -1, "ViewModel" );
    if ( lua_isstring( L, -1 ) )
    {
        Q_strncpy( m_pLuaWeaponInfo->szViewModel, lua_tostring( L, -1 ), MAX_WEAPON_STRING );
    }
    lua_pop( L, 1 );  // Pop the ViewModel field

    // WorldModel
    lua_getfield( L, -1, "WorldModel" );
    if ( lua_isstring( L, -1 ) )
    {
        Q_strncpy( m_pLuaWeaponInfo->szWorldModel, lua_tostring( L, -1 ), MAX_WEAPON_STRING );
    }
    lua_pop( L, 1 );  // Pop the WorldModel field

    // AnimationPrefix
    lua_getfield( L, -1, "AnimationPrefix" );
    if ( lua_isstring( L, -1 ) )
    {
        Q_strncpy( m_pLuaWeaponInfo->szAnimationPrefix, lua_tostring( L, -1 ), MAX_WEAPON_PREFIX );
    }
    lua_pop( L, 1 );  // Pop the AnimationPrefix field

    // InventorySlot
    lua_getfield( L, -1, "InventorySlot" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iSlot = lua_tonumber( L, -1 );
    }
    else
    {
        m_pLuaWeaponInfo->iSlot = 0;
    }
    lua_pop( L, 1 );  // Pop the InventorySlot field

    // InventorySlotPosition
    lua_getfield( L, -1, "InventorySlotPosition" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iPosition = lua_tonumber( L, -1 );
    }
    else
    {
        m_pLuaWeaponInfo->iPosition = 0;
    }
    lua_pop( L, 1 );  // Pop the InventorySlotPosition field

    // Use the console (X360) buckets if hud_fastswitch is set to 2.
#ifdef CLIENT_DLL
    if ( hud_fastswitch.GetInt() == 2 )
#else
    if ( IsX360() )
#endif
    {
        // InventorySlotX360
        lua_getfield( L, -1, "InventorySlotX360" );
        if ( lua_isnumber( L, -1 ) )
        {
            m_pLuaWeaponInfo->iSlot = lua_tonumber( L, -1 );
        }
        lua_pop( L, 1 );  // Pop the InventorySlotX360 field

        // InventorySlotPositionX360
        lua_getfield( L, -1, "InventorySlotPositionX360" );
        if ( lua_isnumber( L, -1 ) )
        {
            m_pLuaWeaponInfo->iPosition = lua_tonumber( L, -1 );
        }
        lua_pop( L, 1 );  // Pop the InventorySlotPositionX360 field
    }

    // MaxClip
    lua_getfield( L, -1, "MaxClip" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iMaxClip1 =
            lua_tonumber( L, -1 );  // Max primary clips gun can hold (assume they
                                    // don't use clips by default)
    }
    else
    {
        m_pLuaWeaponInfo->iMaxClip1 = WEAPON_NOCLIP;
    }
    lua_pop( L, 1 );  // Pop the MaxClip field

    // MaxClip2
    lua_getfield( L, -1, "MaxClip2" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iMaxClip2 =
            lua_tonumber( L, -1 );  // Max secondary clips gun can hold (assume
                                    // they don't use clips by default)
    }
    else
    {
        m_pLuaWeaponInfo->iMaxClip2 = WEAPON_NOCLIP;
    }
    lua_pop( L, 1 );  // Pop the MaxClip2 field

    // DefaultClip
    lua_getfield( L, -1, "DefaultClip" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iDefaultClip1 =
            lua_tonumber( L, -1 );  // amount of primary ammo placed in the
                                    // primary clip when it's picked up
    }
    else
    {
        m_pLuaWeaponInfo->iDefaultClip1 = m_pLuaWeaponInfo->iMaxClip1;
    }
    lua_pop( L, 1 );  // Pop the DefaultClip field

    // DefaultClip2
    lua_getfield( L, -1, "DefaultClip2" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iDefaultClip2 =
            lua_tonumber( L, -1 );  // amount of secondary ammo placed in the
                                    // secondary clip when it's picked up
    }
    else
    {
        m_pLuaWeaponInfo->iDefaultClip2 = m_pLuaWeaponInfo->iMaxClip2;
    }
    lua_pop( L, 1 );  // Pop the DefaultClip2 field

    // Weight
    lua_getfield( L, -1, "Weight" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iWeight = lua_tonumber( L, -1 );
    }
    else
    {
        m_pLuaWeaponInfo->iWeight = 0;
    }
    lua_pop( L, 1 );

    // RumbleEffectX360
    lua_getfield( L, -1, "RumbleEffectX360" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iRumbleEffect = lua_tonumber( L, -1 );
    }
    else
    {
        m_pLuaWeaponInfo->iRumbleEffect = -1;
    }
    lua_pop( L, 1 );

    // TODO: Create a helper function to read these as int / bool to boolean

    // ShowUsageHint
    lua_getfield( L, -1, "ShowUsageHint" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->bShowUsageHint =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->bShowUsageHint = false;
    }
    lua_pop( L, 1 );  // Pop the ShowUsageHint field

    // AutoSwitchTo
    lua_getfield( L, -1, "AutoSwitchTo" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->bAutoSwitchTo =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->bAutoSwitchTo = true;
    }
    lua_pop( L, 1 );  // Pop the AutoSwitchTo field

    // AutoSwitchFrom
    lua_getfield( L, -1, "AutoSwitchFrom" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->bAutoSwitchFrom =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->bAutoSwitchFrom = true;
    }
    lua_pop( L, 1 );  // Pop the AutoSwitchFrom field

    // BuiltRightHanded
    lua_getfield( L, -1, "BuiltRightHanded" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->m_bBuiltRightHanded =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->m_bBuiltRightHanded = true;
    }
    lua_pop( L, 1 );  // Pop the BuiltRightHanded field

    // AllowFlipping
    lua_getfield( L, -1, "AllowFlipping" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->m_bAllowFlipping =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->m_bAllowFlipping = true;
    }
    lua_pop( L, 1 );  // Pop the AllowFlipping field

    // MeleeWeapon
    lua_getfield( L, -1, "MeleeWeapon" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->m_bMeleeWeapon =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->m_bMeleeWeapon = false;
    }
    lua_pop( L, 1 );  // Pop the MeleeWeapon field

    // Primary ammo used
    lua_getfield( L, -1, "PrimaryAmmo" );
    if ( lua_isstring( L, -1 ) )
    {
        const char *pAmmo = lua_tostring( L, -1 );
        if ( strcmp( "None", pAmmo ) == 0 )
            Q_strncpy( m_pLuaWeaponInfo->szAmmo1, "", sizeof( m_pLuaWeaponInfo->szAmmo1 ) );
        else
            Q_strncpy( m_pLuaWeaponInfo->szAmmo1, pAmmo, sizeof( m_pLuaWeaponInfo->szAmmo1 ) );
        m_pLuaWeaponInfo->iAmmoType =
            GetAmmoDef()->Index( m_pLuaWeaponInfo->szAmmo1 );
    }
    lua_pop( L, 1 );  // Pop the PrimaryAmmo field

    // Secondary ammo used
    lua_getfield( L, -1, "SecondaryAmmo" );
    if ( lua_isstring( L, -1 ) )
    {
        const char *pAmmo = lua_tostring( L, -1 );
        if ( strcmp( "None", pAmmo ) == 0 )
            Q_strncpy( m_pLuaWeaponInfo->szAmmo2, "", sizeof( m_pLuaWeaponInfo->szAmmo2 ) );
        else
            Q_strncpy( m_pLuaWeaponInfo->szAmmo2, pAmmo, sizeof( m_pLuaWeaponInfo->szAmmo2 ) );
        m_pLuaWeaponInfo->iAmmo2Type =
            GetAmmoDef()->Index( m_pLuaWeaponInfo->szAmmo2 );
    }
    lua_pop( L, 1 );  // Pop the SecondaryAmmo field

    // Now read the weapon sounds
    memset( m_pLuaWeaponInfo->aShootSounds, 0, sizeof( m_pLuaWeaponInfo->aShootSounds ) );
    lua_getfield( L, -1, "SoundData" );
    if ( lua_istable( L, -1 ) )
    {
        for ( int i = EMPTY; i < NUM_SHOOT_SOUND_TYPES; i++ )
        {
            lua_getfield( L, -1, pWeaponSoundCategories[i] );
            if ( lua_isstring( L, -1 ) )
            {
                const char *soundname = lua_tostring( L, -1 );
                if ( soundname && soundname[0] )
                {
                    Q_strncpy( m_pLuaWeaponInfo->aShootSounds[i], soundname, MAX_WEAPON_STRING );
                }
            }
            lua_pop( L, 1 );
        }
    }
    lua_pop( L, 1 );  // Pop the SoundData table

    // Damage
    lua_getfield( L, -1, "Damage" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->m_iPlayerDamage = ( int )lua_tointeger( L, -1 );
    }
    lua_pop( L, 1 );  // Pop the Damage field

    // Pop the LUA_GET_REF_TABLE
    lua_pop( L, 1 );

    LUA_CALL_HOOK_BEGIN( "PreEntityInitialize" );
    CBaseCombatWeapon::PushLuaInstanceSafe( L, this );
    LUA_CALL_HOOK_END( 1, 0 );

    m_bInitialized = true;

    LUA_CALL_WEAPON_METHOD_BEGIN( "Initialize" );
    LUA_CALL_WEAPON_METHOD_END( 0, 0 );
#endif
}

#ifdef CLIENT_DLL
void CExperimentScriptedWeapon::Spawn( void )
{
    // Experiment; Prevent spawning, until the client has
    // received the classname from the server.
}

void CExperimentScriptedWeapon::OnDataChanged( DataUpdateType_t updateType )
{
    BaseClass::OnDataChanged( updateType );

    if ( updateType == DATA_UPDATE_CREATED )
    {
        if ( m_iScriptedClassname.Get() && !m_pLuaWeaponInfo->bParsedScript )
        {
            SetClassname( m_iScriptedClassname.Get() );
            InitScriptedWeapon();

#ifdef LUA_SDK
            LUA_CALL_WEAPON_METHOD_BEGIN( "Precache" );
            LUA_CALL_WEAPON_METHOD_END( 0, 0 );
#endif

            // Only now do we call the base class's spawn method, after we're
            // sure to have the scripted classname.
            BaseClass::Spawn();
        }
    }
}

const char *CExperimentScriptedWeapon::GetScriptedClassname( void )
{
    if ( m_iScriptedClassname.Get() )
        return m_iScriptedClassname.Get();
    return BaseClass::GetClassname();
}
#endif

void CExperimentScriptedWeapon::Precache( void )
{
    BaseClass::Precache();

    InitScriptedWeapon();

    // Get the ammo indexes for the ammo's specified in the data file
    if ( GetWpnData().szAmmo1[0] )
    {
        m_iPrimaryAmmoType = GetAmmoDef()->Index( GetWpnData().szAmmo1 );
        if ( m_iPrimaryAmmoType == -1 )
        {
            Msg( "ERROR: Weapon (%s) using undefined primary ammo type (%s)\n",
                 GetClassname(),
                 GetWpnData().szAmmo1 );
        }
    }
    if ( GetWpnData().szAmmo2[0] )
    {
        m_iSecondaryAmmoType = GetAmmoDef()->Index( GetWpnData().szAmmo2 );
        if ( m_iSecondaryAmmoType == -1 )
        {
            Msg( "ERROR: Weapon (%s) using undefined secondary ammo type (%s)\n",
                 GetClassname(),
                 GetWpnData().szAmmo2 );
        }
    }

    // Precache models (preload to avoid hitch)
    m_iViewModelIndex = 0;
    m_iWorldModelIndex = 0;

    if ( GetViewModel() && GetViewModel()[0] )
    {
        m_iViewModelIndex = CBaseEntity::PrecacheModel( GetViewModel() );
    }

    if ( GetWorldModel() && GetWorldModel()[0] )
    {
        m_iWorldModelIndex = CBaseEntity::PrecacheModel( GetWorldModel() );
    }

    // Precache sounds, too
    for ( int i = 0; i < NUM_SHOOT_SOUND_TYPES; ++i )
    {
        const char *shootsound = GetShootSound( i );
        if ( shootsound && shootsound[0] )
        {
            CBaseEntity::PrecacheScriptSound( shootsound );
        }
    }

#if defined( LUA_SDK ) && !defined( CLIENT_DLL )
    LUA_CALL_WEAPON_METHOD_BEGIN( "Precache" );
    LUA_CALL_WEAPON_METHOD_END( 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Get my data in the file weapon info array
//-----------------------------------------------------------------------------
const FileWeaponInfo_t &CExperimentScriptedWeapon::GetWpnData( void ) const
{
    return *m_pLuaWeaponInfo;
}

const char *CExperimentScriptedWeapon::GetViewModel( int ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "ViewModel" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_STRING();
    }
#endif

    return BaseClass::GetViewModel();
}

const char *CExperimentScriptedWeapon::GetWorldModel( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "WorldModel" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_STRING();
    }
#endif

    return BaseClass::GetWorldModel();
}

const char *CExperimentScriptedWeapon::GetAnimPrefix( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "AnimationPrefix" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_STRING();
    }
#endif

    return BaseClass::GetAnimPrefix();
}

const char *CExperimentScriptedWeapon::GetPrintName( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "PrintName" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_STRING();
    }
#endif

    return BaseClass::GetPrintName();
}

int CExperimentScriptedWeapon::GetMaxClip1( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "MaxClip" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_INTEGER();
    }
#endif

    return BaseClass::GetMaxClip1();
}

int CExperimentScriptedWeapon::GetMaxClip2( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "MaxClip2" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_INTEGER();
    }
#endif

    return BaseClass::GetMaxClip2();
}

int CExperimentScriptedWeapon::GetDefaultClip1( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "DefaultClip" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_INTEGER();
    }
#endif

    return BaseClass::GetDefaultClip1();
}

int CExperimentScriptedWeapon::GetDefaultClip2( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "DefaultClip2" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_INTEGER();
    }
#endif

    return BaseClass::GetDefaultClip2();
}

bool CExperimentScriptedWeapon::IsMeleeWeapon() const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "MeleeWeapon" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_BOOLEAN_FROM_INTEGER();
    }
#endif

    return BaseClass::IsMeleeWeapon();
}

int CExperimentScriptedWeapon::GetWeight( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "Weight" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_INTEGER();
    }
#endif

    return BaseClass::GetWeight();
}

bool CExperimentScriptedWeapon::AllowsAutoSwitchTo( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "AutoSwitchTo" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_BOOLEAN_FROM_INTEGER();
    }
#endif

    return BaseClass::AllowsAutoSwitchTo();
}

bool CExperimentScriptedWeapon::AllowsAutoSwitchFrom( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "AutoSwitchFrom" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_BOOLEAN_FROM_INTEGER();
    }
#endif

    return BaseClass::AllowsAutoSwitchFrom();
}

int CExperimentScriptedWeapon::GetWeaponFlags( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "WeaponFlags" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_INTEGER();
    }
#endif

    return BaseClass::GetWeaponFlags();
}

int CExperimentScriptedWeapon::GetSlot( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "InventorySlot" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_INTEGER();
    }
#endif

    return BaseClass::GetSlot();
}

int CExperimentScriptedWeapon::GetPosition( void ) const
{
#ifdef LUA_SDK
    if ( lua_isrefvalid( L, m_nTableReference ) )
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "InventorySlotPosition" );
        lua_remove( L, -2 );  // Remove the reference table

        LUA_RETURN_INTEGER();
    }
#endif

    return BaseClass::GetPosition();
}

const Vector &CExperimentScriptedWeapon::GetBulletSpread( void )
{
    static Vector cone = VECTOR_CONE_3DEGREES;
    return cone;
}

//-----------------------------------------------------------------------------
// Purpose:
//
//
//-----------------------------------------------------------------------------
void CExperimentScriptedWeapon::PrimaryAttack( void )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "PrimaryAttack" );
    LUA_CALL_WEAPON_METHOD_END( 0, 0 );
#endif
}

void CExperimentScriptedWeapon::SecondaryAttack( void )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "SecondaryAttack" );
    LUA_CALL_WEAPON_METHOD_END( 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : &info -
//-----------------------------------------------------------------------------
void CExperimentScriptedWeapon::FireBullets( const FireBulletsInfo_t &info )
{
    if ( CBasePlayer *pPlayer = ToBasePlayer( GetOwner() ) )
    {
        pPlayer->FireBullets( info );
    }
}

bool CExperimentScriptedWeapon::Reload( void )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "Reload" );
    LUA_CALL_WEAPON_METHOD_END( 0, 1 );

    LUA_RETURN_BOOLEAN();
#endif
    return BaseClass::Reload();
}

//-----------------------------------------------------------------------------
// Purpose:
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CExperimentScriptedWeapon::Deploy( void )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "Deploy" );
    LUA_CALL_WEAPON_METHOD_END( 0, 1 );

    LUA_RETURN_VALUE_IF_TRUE( false );
#endif

    return BaseClass::Deploy();
}

Activity CExperimentScriptedWeapon::GetDrawActivity( void )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "GetDrawActivity" );
    LUA_CALL_WEAPON_METHOD_END( 0, 1 );

    // Kind of lame, but we're required to explicitly cast
    LUA_RETURN_ACTIVITY();
#endif

    return BaseClass::GetDrawActivity();
}

//-----------------------------------------------------------------------------
// Purpose:
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CExperimentScriptedWeapon::Holster( CBaseCombatWeapon *pSwitchingTo )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "Holster" );
    CBaseCombatWeapon::PushLuaInstanceSafe( L, pSwitchingTo );
    LUA_CALL_WEAPON_METHOD_END( 1, 1 );

    LUA_RETURN_BOOLEAN();
#endif

    return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CExperimentScriptedWeapon::ItemPostFrame( void )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "ItemPostFrame" );
    LUA_CALL_WEAPON_METHOD_END( 0, 1 );

    LUA_RETURN_NONE_IF_FALSE();
#endif

    BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Called each frame by the player PostThink, if the player's not ready
// to attack yet
//-----------------------------------------------------------------------------
void CExperimentScriptedWeapon::ItemBusyFrame( void )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "ItemBusyFrame" );
    LUA_CALL_WEAPON_METHOD_END( 0, 1 );

    LUA_RETURN_NONE_IF_FALSE();
#endif

    BaseClass::ItemBusyFrame();
}

#ifndef CLIENT_DLL
int CExperimentScriptedWeapon::CapabilitiesGet( void )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "CapabilitiesGet" );
    LUA_CALL_WEAPON_METHOD_END( 0, 1 );

    LUA_RETURN_INTEGER();
#endif

    return BaseClass::CapabilitiesGet();
}
#else
int CExperimentScriptedWeapon::DrawModel( int flags )
{
#ifdef LUA_SDK
    LUA_CALL_WEAPON_METHOD_BEGIN( "DrawModel" );
    lua_pushinteger( L, flags );
    LUA_CALL_WEAPON_METHOD_END( 1, 1 );

    LUA_RETURN_INTEGER();
#endif

    return BaseClass::DrawModel( flags );
}
#endif
