//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "weapon_experimentbasehlmpcombatweapon.h"
#include "luamanager.h"

#ifndef BASEHLCOMBATWEAPON_H
#define BASEHLCOMBATWEAPON_H
#ifdef _WIN32
#pragma once
#endif

#if defined( CLIENT_DLL )
#define CExperimentScriptedWeapon C_ExperimentScriptedWeapon
#endif

//=========================================================
// Scripted weapon base class
//=========================================================
class CExperimentScriptedWeapon : public CBaseExperimentCombatWeapon
{
   public:
    DECLARE_CLASS( CExperimentScriptedWeapon, CWeaponExperimentBase );
    DECLARE_DATADESC();

    CExperimentScriptedWeapon();
    ~CExperimentScriptedWeapon();

#ifdef LUA_SDK
    virtual void SetupRefTable( lua_State *L );
    bool m_bInitialized;

    bool IsValid( void ) const
    {
        return m_bInitialized;
    }

    /// <summary>
    /// We usually want the scripted weapon to be fully initialized before we can call any of its methods.
    /// Especially NetworkVar methods need to have been installed before they're used.
    /// </summary>
    /// <param name="pWeapon"></param>
    /// <returns></returns>
    static bool IsValidWeapon( CBaseCombatWeapon *pWeapon )
    {
        CExperimentScriptedWeapon *pScriptedWeapon = dynamic_cast< CExperimentScriptedWeapon * >( pWeapon );

        if ( pScriptedWeapon )
        {
            return pScriptedWeapon->IsValid();
        }

        return pWeapon != NULL;
    }

    virtual void Remove() OVERRIDE;
#endif

    bool IsScripted( void ) const
    {
        return true;
    }

    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();
    // DECLARE_ACTTABLE();

    acttable_t m_acttable[LUA_MAX_WEAPON_ACTIVITIES];
    acttable_t *ActivityList( void );
    int ActivityListCount( void );

    void Precache( void );
    void InitScriptedWeapon( void );

    void PrimaryAttack( void );
    void SecondaryAttack( void );

    // Firing animations
    virtual Activity GetDrawActivity( void );

    // Default calls through to m_hOwner, but plasma weapons can override and shoot projectiles here.
    virtual void ItemPostFrame( void );
    virtual void ItemBusyFrame( void );
    virtual void FireBullets( const FireBulletsInfo_t &info );
    virtual bool Reload();

    virtual bool Deploy( void );
    virtual bool Holster( CBaseCombatWeapon *pSwitchingTo );

#ifdef CLIENT_DLL
    virtual void Spawn( void );
    virtual void OnDataChanged( DataUpdateType_t updateType );
    virtual const char *GetScriptedClassname( void );
#endif

    virtual const Vector &GetBulletSpread( void );

   public:
    // Weapon info accessors for data in the weapon's data file
    CExperimentSWeaponInfo *m_pLuaWeaponInfo;
    virtual const FileWeaponInfo_t &GetWpnData( void ) const;
    virtual const char *GetViewModel( int viewmodelindex = 0 ) const;
    virtual const char *GetWorldModel( void ) const;
    virtual const char *GetAnimPrefix( void ) const;
    virtual int GetMaxClip1( void ) const;
    virtual int GetMaxClip2( void ) const;
    virtual int GetDefaultClip1( void ) const;
    virtual int GetDefaultClip2( void ) const;
    virtual int GetWeight( void ) const;
    virtual bool AllowsAutoSwitchTo( void ) const;
    virtual bool AllowsAutoSwitchFrom( void ) const;
    virtual int GetWeaponFlags( void ) const;
    virtual int GetSlot( void ) const;
    virtual int GetPosition( void ) const;
    virtual char const *GetPrintName( void ) const;
    bool IsMeleeWeapon() const;

   public:
// Server Only Methods
#if !defined( CLIENT_DLL )

    virtual int CapabilitiesGet( void );

// Client only methods
#else

    // Returns the aiment render origin + angles
    virtual int DrawModel( int flags );

#endif  // End client-only methods

   private:
    CExperimentScriptedWeapon( const CExperimentScriptedWeapon & );

    CNetworkString( m_iScriptedClassname, MAX_WEAPON_STRING );
};

void RegisterScriptedWeapon( const char *szClassname );
void ResetWeaponFactoryDatabase( void );

#endif  // BASEHLCOMBATWEAPON_H
