#ifndef NETWORKSYSTEM_H
#define NETWORKSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "tier1/utlvector.h"
#include "tier1/utlstring.h"
#include "tier1/utlmap.h"
#include "tier1/bitbuf.h"
#include "tier1/netadr.h"
#include "tier2/tier2.h"

#include "networkserver.h"
#include "networksystem.h"
#include "networksystem/isockets.h"
#include "networksystem/inetworksystem.h"
#include "networksystem/inetworkmessage.h"
#include "networksystem/inetworkpeerbase.h"

// Forward declarations
class CNetworkServer;
class CNetworkClient;
class IConnectionlessPacketHandler;

class CNetworkSystem : public CTier2AppSystem< INetworkSystem >
{
    typedef CTier2AppSystem< INetworkSystem > BaseClass;

   public:
    CNetworkSystem();
    virtual ~CNetworkSystem();

    // Inherited from IAppSystem
    virtual bool Connect( CreateInterfaceFn factory );
    virtual InitReturnVal_t Init();
    virtual void Shutdown();

    // Inherited from INetworkSystem
    virtual void RegisterGroupHandler( NETWORK_MESSAGE_GROUP::TYPE group, INetworkGroupHandler *pHandler );

    virtual bool StartServer( unsigned short serverPort );
    virtual void ShutdownServer();

    virtual bool StartClient( unsigned short nClientListenPort );
    virtual void ShutdownClient();

    virtual void Tick( void );
    virtual bool ProcessMessage( IConnectedClient *client, CNetPacket *packet );
    virtual bool ProcessControlMessage( int messageCommand, bf_read &buffer );

    virtual INetworkPeerBase *ConnectClientToServer( const char *serverIp, int serverPort );
    virtual void DisconnectClientFromServer( INetworkPeerBase *peer );
    virtual INetworkPeerBase *GetServerPeer( void );

    virtual void PostClientToServerMessage( INetworkMessage *message );
    virtual void BroadcastServerToClientMessage( INetworkMessage *message );

    INetworkGroupHandler *FindNetworkGroupHandler( NETWORK_MESSAGE_GROUP::TYPE group );

    static bool StringToSockaddr( const char *s, struct sockaddr *sadr );

   private:
    void CleanupNetworkHandlers();

    bool m_bWinSockInitialized : 1;

    CNetworkServer *m_pServer;
    CNetworkClient *m_pClient;

    CUtlMap< NETWORK_MESSAGE_GROUP::TYPE, INetworkGroupHandler * > m_MessageHandlers;
};

static CSysModule *g_hNetworkSystemDll = NULL;

inline INetworkSystem *LoadNetworkSystem( CreateInterfaceFn appSystemFactory )
{
    INetworkSystem *networkSystemOut;

    if ( !Sys_LoadInterface( "networksystem", NETWORKSYSTEM_INTERFACE_VERSION, &g_hNetworkSystemDll, ( void ** )&networkSystemOut ) )
        return NULL;

    if ( !networkSystemOut->Connect( appSystemFactory ) )
        return NULL;

    if ( networkSystemOut->Init() != INIT_OK )
        return NULL;

    return networkSystemOut;
}

inline void UnloadNetworkSystem( INetworkSystem *networkSystem )
{
    networkSystem->Disconnect();
    networkSystem = NULL;
    Sys_UnloadModule( g_hNetworkSystemDll );
}

#endif  // NETWORKSYSTEM_H
