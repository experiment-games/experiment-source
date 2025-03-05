#ifndef BASESCRIPTED_H
#define BASESCRIPTED_H

#include "predictable_entity.h"
#include "baseentity_shared.h"

#if defined( CLIENT_DLL )
#define CBaseScripted C_BaseScripted

#endif

class CBaseScripted : /* public CBaseEntity */ public CBaseAnimating
{
    public:
    // DECLARE_CLASS( CBaseScripted, CBaseEntity );
    DECLARE_CLASS( CBaseScripted, CBaseAnimating );
    DECLARE_PREDICTABLE();
    DECLARE_NETWORKCLASS();

    CBaseScripted();
    ~CBaseScripted();

    virtual bool IsScripted( void ) const OVERRIDE
    {
        return true;
    }

    // CBaseEntity overrides.
    public:
    void Think();

    void Spawn( void );
    void Precache( void );
    virtual void Remove( void ) OVERRIDE;
    void LoadScriptedEntity( void );
    void InitScriptedEntity( void );

    void StartTouch( CBaseEntity *pOther );
    void Touch( CBaseEntity *pOther );
    void EndTouch( CBaseEntity *pOther );

#ifdef CLIENT_DLL
    virtual RenderGroup_t GetRenderGroup() OVERRIDE;

    // model specific
    virtual int DrawModel( int flags );
#endif

    virtual void VPhysicsUpdate( IPhysicsObject *pPhysics );

#ifndef CLIENT_DLL
    virtual int UpdateTransmitState();
#endif

#ifdef CLIENT_DLL
    // IClientThinkable.
    public:
    // Called whenever you registered for a think message (with SetNextClientThink).
    virtual void ClientThink();

    virtual void OnDataChanged( DataUpdateType_t updateType );
    virtual const char *GetScriptedClassname( void );
#endif

    private:
    CBaseScripted( const CBaseScripted & );  // not defined, not accessible

    CNetworkString( m_iScriptedClassname, 255 );

#ifdef CLIENT_DLL
    int m_iRenderGroupOverride = -1;
#endif
};

// void RegisterScriptedEntity( const char *szClassname );
void ResetEntityFactoryDatabase( void );

#endif
