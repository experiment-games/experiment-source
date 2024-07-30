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

IMPLEMENT_NETWORKCLASS_ALIASED( ExperimentScriptedWeapon, DT_ExperimentScriptedWeapon )

BEGIN_NETWORK_TABLE( CExperimentScriptedWeapon, DT_ExperimentScriptedWeapon )
#ifdef CLIENT_DLL
RecvPropString( RECVINFO( m_iScriptedClassname ) ),
#else
SendPropString( SENDINFO( m_iScriptedClassname ) ),
#endif
    END_NETWORK_TABLE()

        BEGIN_PREDICTION_DATA( CExperimentScriptedWeapon ) END_PREDICTION_DATA()

    //=========================================================
    //	>> CHLSelectFireScriptedWeapon
    //=========================================================
    BEGIN_DATADESC( CExperimentScriptedWeapon ) END_DATADESC()

// LINK_ENTITY_TO_CLASS( weapon_experimentbase_scriptedweapon, CExperimentScriptedWeapon
// ); PRECACHE_WEAPON_REGISTER( weapon_experimentbase_scriptedweapon );

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

void RegisterScriptedWeapon( const char *className )
{
#ifdef CLIENT_DLL
    if ( GetClassMap().FindFactory( className ) )
    {
        return;
    }

    GetClassMap().Add( className, "CExperimentScriptedWeapon", sizeof( CExperimentScriptedWeapon ), &CCExperimentScriptedWeaponFactory, true );
#else
    if ( EntityFactoryDictionary()->FindFactory( className ) )
    {
        return;
    }

    unsigned short lookup = m_WeaponFactoryDatabase.Find( className );
    if ( lookup != m_WeaponFactoryDatabase.InvalidIndex() )
    {
        return;
    }

    // Andrew; This fixes months worth of pain and anguish.
    CEntityFactory< CExperimentScriptedWeapon > *pFactory =
        new CEntityFactory< CExperimentScriptedWeapon >( className );

    lookup = m_WeaponFactoryDatabase.Insert( className, pFactory );
    Assert( lookup != m_WeaponFactoryDatabase.InvalidIndex() );
#endif
    // BUGBUG: When attempting to precache weapons registered during runtime,
    // they don't appear as valid registered entities.
    // static CPrecacheRegister
    // precache_weapon_(&CPrecacheRegister::PrecacheFn_Other, className);
}

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
    lua_remove( L, -2 );
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

extern const char *pWeaponSoundCategories[NUM_SHOOT_SOUND_TYPES];

#ifdef CLIENT_DLL
extern ConVar hud_fastswitch;
#endif

void CExperimentScriptedWeapon::InitScriptedWeapon( void )
{
#ifdef LUA_SDK
#ifndef CLIENT_DLL
    // Let the instance reinitialize itself for the client.
    if ( lua_isrefvalid( L, m_nTableReference ) )
        return;
#endif

    char className[MAX_WEAPON_STRING];
#if defined( CLIENT_DLL )
    if ( strlen( GetScriptedClassname() ) > 0 )
        Q_strncpy( className, GetScriptedClassname(), sizeof( className ) );
    else
        Q_strncpy( className, GetClassname(), sizeof( className ) );
#else
    Q_strncpy( m_iScriptedClassname.GetForModify(), GetClassname(), sizeof( className ) );
    Q_strncpy( className, GetClassname(), sizeof( className ) );
#endif
    Q_strlower( className );
    // Andrew; This redundancy is pretty annoying.
    // Classname
    Q_strncpy( m_pLuaWeaponInfo->szClassName, className, MAX_WEAPON_STRING );
    SetClassname( className );

    lua_getglobal( L, LUA_WEAPONSLIBNAME );
    if ( lua_istable( L, -1 ) )
    {
        lua_getfield( L, -1, "Get" );
        if ( lua_isfunction( L, -1 ) )
        {
            lua_remove( L, -2 );
            lua_pushstring( L, className );
            luasrc_pcall( L, 1, 1 );
        }
        else
        {
            lua_pop( L, 2 );
        }
    }
    else
    {
        lua_pop( L, 1 );
    }

#ifndef CLIENT_DLL
    m_pLuaWeaponInfo->bParsedScript = true;
#endif

    LUA_GET_REF_TABLE( L, this );
    // Printable name
    lua_getfield( L, -1, "PrintName" );
    lua_remove( L, -2 );
    if ( lua_isstring( L, -1 ) )
    {
        Q_strncpy( m_pLuaWeaponInfo->szPrintName, lua_tostring( L, -1 ), MAX_WEAPON_STRING );
    }
    else
    {
        Q_strncpy( m_pLuaWeaponInfo->szPrintName, WEAPON_PRINTNAME_MISSING, MAX_WEAPON_STRING );
    }
    lua_pop( L, 1 );
    // View model & world model
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "ViewModel" );
    lua_remove( L, -2 );
    if ( lua_isstring( L, -1 ) )
    {
        Q_strncpy( m_pLuaWeaponInfo->szViewModel, lua_tostring( L, -1 ), MAX_WEAPON_STRING );
    }
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "WorldModel" );
    lua_remove( L, -2 );
    if ( lua_isstring( L, -1 ) )
    {
        Q_strncpy( m_pLuaWeaponInfo->szWorldModel, lua_tostring( L, -1 ), MAX_WEAPON_STRING );
    }
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "AnimationPrefix" );
    lua_remove( L, -2 );
    if ( lua_isstring( L, -1 ) )
    {
        Q_strncpy( m_pLuaWeaponInfo->szAnimationPrefix, lua_tostring( L, -1 ), MAX_WEAPON_PREFIX );
    }
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "InventorySlot" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iSlot = lua_tonumber( L, -1 );
    }
    else
    {
        m_pLuaWeaponInfo->iSlot = 0;
    }
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "InventorySlotPosition" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iPosition = lua_tonumber( L, -1 );
    }
    else
    {
        m_pLuaWeaponInfo->iPosition = 0;
    }
    lua_pop( L, 1 );

    // Use the console (X360) buckets if hud_fastswitch is set to 2.
#ifdef CLIENT_DLL
    if ( hud_fastswitch.GetInt() == 2 )
#else
    if ( IsX360() )
#endif
    {
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "InventorySlotX360" );
        lua_remove( L, -2 );
        if ( lua_isnumber( L, -1 ) )
        {
            m_pLuaWeaponInfo->iSlot = lua_tonumber( L, -1 );
        }
        lua_pop( L, 1 );
        lua_getref( L, m_nTableReference );
        lua_getfield( L, -1, "InventorySlotPositionX360" );
        lua_remove( L, -2 );
        if ( lua_isnumber( L, -1 ) )
        {
            m_pLuaWeaponInfo->iPosition = lua_tonumber( L, -1 );
        }
        lua_pop( L, 1 );
    }
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "MaxClip" );
    lua_remove( L, -2 );
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
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "MaxClip2" );
    lua_remove( L, -2 );
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
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "DefaultClip" );
    lua_remove( L, -2 );
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
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "DefaultClip2" );
    lua_remove( L, -2 );
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
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "Weight" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->iWeight = lua_tonumber( L, -1 );
    }
    else
    {
        m_pLuaWeaponInfo->iWeight = 0;
    }
    lua_pop( L, 1 );

    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "RumbleEffectX360" );
    lua_remove( L, -2 );
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
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "ShowUsageHint" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->bShowUsageHint =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->bShowUsageHint = false;
    }
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "AutoSwitchTo" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->bAutoSwitchTo =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->bAutoSwitchTo = true;
    }
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "AutoSwitchFrom" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->bAutoSwitchFrom =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->bAutoSwitchFrom = true;
    }
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "BuiltRightHanded" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->m_bBuiltRightHanded =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->m_bBuiltRightHanded = true;
    }
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "AllowFlipping" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->m_bAllowFlipping =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->m_bAllowFlipping = true;
    }
    lua_pop( L, 1 );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "MeleeWeapon" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->m_bMeleeWeapon =
            ( int )lua_tointeger( L, -1 ) != 0 ? true : false;
    }
    else
    {
        m_pLuaWeaponInfo->m_bMeleeWeapon = false;
    }
    lua_pop( L, 1 );

    // Primary ammo used
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "PrimaryAmmo" );
    lua_remove( L, -2 );
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
    lua_pop( L, 1 );

    // Secondary ammo used
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "SecondaryAmmo" );
    lua_remove( L, -2 );
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
    lua_pop( L, 1 );

    // Now read the weapon sounds
    memset( m_pLuaWeaponInfo->aShootSounds, 0, sizeof( m_pLuaWeaponInfo->aShootSounds ) );
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "SoundData" );
    lua_remove( L, -2 );
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
    lua_pop( L, 1 );

    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "Damage" );
    lua_remove( L, -2 );
    if ( lua_isnumber( L, -1 ) )
    {
        m_pLuaWeaponInfo->m_iPlayerDamage = ( int )lua_tointeger( L, -1 );
    }
    lua_pop( L, 1 );

    BEGIN_LUA_CALL_WEAPON_METHOD( "Initialize" );
    END_LUA_CALL_WEAPON_METHOD( 0, 0 );
#endif
}

#ifdef CLIENT_DLL
void CExperimentScriptedWeapon::OnDataChanged( DataUpdateType_t updateType )
{
    BaseClass::OnDataChanged( updateType );

    if ( updateType == DATA_UPDATE_CREATED )
    {
        if ( m_iScriptedClassname.Get() && !m_pLuaWeaponInfo->bParsedScript )
        {
            m_pLuaWeaponInfo->bParsedScript = true;
            SetClassname( m_iScriptedClassname.Get() );
            InitScriptedWeapon();

#ifdef LUA_SDK
            BEGIN_LUA_CALL_WEAPON_METHOD( "Precache" );
            END_LUA_CALL_WEAPON_METHOD( 0, 0 );
#endif
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
    BEGIN_LUA_CALL_WEAPON_METHOD( "Precache" );
    END_LUA_CALL_WEAPON_METHOD( 0, 0 );
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
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "ViewModel" );
    lua_remove( L, -2 );

    RETURN_LUA_STRING();
#endif

    return BaseClass::GetViewModel();
}

const char *CExperimentScriptedWeapon::GetWorldModel( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "WorldModel" );
    lua_remove( L, -2 );

    RETURN_LUA_STRING();
#endif

    return BaseClass::GetWorldModel();
}

const char *CExperimentScriptedWeapon::GetAnimPrefix( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "AnimationPrefix" );
    lua_remove( L, -2 );

    RETURN_LUA_STRING();
#endif

    return BaseClass::GetAnimPrefix();
}

const char *CExperimentScriptedWeapon::GetPrintName( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "PrintName" );
    lua_remove( L, -2 );

    RETURN_LUA_STRING();
#endif

    return BaseClass::GetPrintName();
}

int CExperimentScriptedWeapon::GetMaxClip1( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "MaxClip" );
    lua_remove( L, -2 );

    RETURN_LUA_INTEGER();
#endif

    return BaseClass::GetMaxClip1();
}

int CExperimentScriptedWeapon::GetMaxClip2( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "MaxClip2" );
    lua_remove( L, -2 );

    RETURN_LUA_INTEGER();
#endif

    return BaseClass::GetMaxClip2();
}

int CExperimentScriptedWeapon::GetDefaultClip1( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "DefaultClip" );
    lua_remove( L, -2 );

    RETURN_LUA_INTEGER();
#endif

    return BaseClass::GetDefaultClip1();
}

int CExperimentScriptedWeapon::GetDefaultClip2( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "DefaultClip2" );
    lua_remove( L, -2 );

    RETURN_LUA_INTEGER();
#endif

    return BaseClass::GetDefaultClip2();
}

bool CExperimentScriptedWeapon::IsMeleeWeapon() const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "MeleeWeapon" );
    lua_remove( L, -2 );

    if ( lua_gettop( L ) == 1 )
    {
        if ( lua_isnumber( L, -1 ) )
        {
            int res = ( ( int )lua_tointeger( L, -1 ) != 0 ) ? true : false;
            lua_pop( L, 1 );
            return res;
        }
        else
            lua_pop( L, 1 );
    }
#endif

    return BaseClass::IsMeleeWeapon();
}

int CExperimentScriptedWeapon::GetWeight( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "Weight" );
    lua_remove( L, -2 );

    RETURN_LUA_INTEGER();
#endif

    return BaseClass::GetWeight();
}

bool CExperimentScriptedWeapon::AllowsAutoSwitchTo( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "AutoSwitchTo" );
    lua_remove( L, -2 );

    if ( lua_gettop( L ) == 1 )
    {
        if ( lua_isnumber( L, -1 ) )
        {
            int res = ( ( int )lua_tointeger( L, -1 ) != 0 ) ? true : false;
            lua_pop( L, 1 );
            return res;
        }
        else
            lua_pop( L, 1 );
    }
#endif

    return BaseClass::AllowsAutoSwitchTo();
}

bool CExperimentScriptedWeapon::AllowsAutoSwitchFrom( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "AutoSwitchFrom" );
    lua_remove( L, -2 );

    if ( lua_gettop( L ) == 1 )
    {
        if ( lua_isnumber( L, -1 ) )
        {
            int res = ( ( int )lua_tointeger( L, -1 ) != 0 ) ? true : false;
            lua_pop( L, 1 );
            return res;
        }
        else
            lua_pop( L, 1 );
    }
#endif

    return BaseClass::AllowsAutoSwitchFrom();
}

int CExperimentScriptedWeapon::GetWeaponFlags( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "WeaponFlags" );
    lua_remove( L, -2 );

    RETURN_LUA_INTEGER();
#endif

    return BaseClass::GetWeaponFlags();
}

int CExperimentScriptedWeapon::GetSlot( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "InventorySlot" );
    lua_remove( L, -2 );

    RETURN_LUA_INTEGER();
#endif

    return BaseClass::GetSlot();
}

int CExperimentScriptedWeapon::GetPosition( void ) const
{
#ifdef LUA_SDK
    lua_getref( L, m_nTableReference );
    lua_getfield( L, -1, "InventorySlotPosition" );
    lua_remove( L, -2 );

    RETURN_LUA_INTEGER();
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
    BEGIN_LUA_CALL_WEAPON_METHOD( "PrimaryAttack" );
    END_LUA_CALL_WEAPON_METHOD( 0, 0 );
#endif
}

void CExperimentScriptedWeapon::SecondaryAttack( void )
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_WEAPON_METHOD( "SecondaryAttack" );
    END_LUA_CALL_WEAPON_METHOD( 0, 0 );
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
    BEGIN_LUA_CALL_WEAPON_METHOD( "Reload" );
    END_LUA_CALL_WEAPON_METHOD( 0, 1 );

    RETURN_LUA_BOOLEAN();
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
    BEGIN_LUA_CALL_WEAPON_METHOD( "Deploy" );
    END_LUA_CALL_WEAPON_METHOD( 0, 1 );

    RETURN_LUA_BOOLEAN();
#endif

    return BaseClass::Deploy();
}

Activity CExperimentScriptedWeapon::GetDrawActivity( void )
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_WEAPON_METHOD( "GetDrawActivity" );
    END_LUA_CALL_WEAPON_METHOD( 0, 1 );

    // Kind of lame, but we're required to explicitly cast
    RETURN_LUA_ACTIVITY();
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
    BEGIN_LUA_CALL_WEAPON_METHOD( "Holster" );
    CBaseEntity::PushLuaInstanceSafe( L, pSwitchingTo );
    END_LUA_CALL_WEAPON_METHOD( 1, 1 );

    RETURN_LUA_BOOLEAN();
#endif

    return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CExperimentScriptedWeapon::ItemPostFrame( void )
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_WEAPON_METHOD( "ItemPostFrame" );
    END_LUA_CALL_WEAPON_METHOD( 0, 1 );

    RETURN_LUA_NONE_IF_FALSE();
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
    BEGIN_LUA_CALL_WEAPON_METHOD( "ItemBusyFrame" );
    END_LUA_CALL_WEAPON_METHOD( 0, 1 );

    RETURN_LUA_NONE_IF_FALSE();
#endif

    BaseClass::ItemBusyFrame();
}

#ifndef CLIENT_DLL
int CExperimentScriptedWeapon::CapabilitiesGet( void )
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_WEAPON_METHOD( "CapabilitiesGet" );
    END_LUA_CALL_WEAPON_METHOD( 0, 1 );

    RETURN_LUA_INTEGER();
#endif

    return BaseClass::CapabilitiesGet();
}
#else
int CExperimentScriptedWeapon::DrawModel( int flags )
{
#ifdef LUA_SDK
    BEGIN_LUA_CALL_WEAPON_METHOD( "DrawModel" );
    lua_pushinteger( L, flags );
    END_LUA_CALL_WEAPON_METHOD( 1, 1 );

    RETURN_LUA_INTEGER();
#endif

    return BaseClass::DrawModel( flags );
}
#endif
