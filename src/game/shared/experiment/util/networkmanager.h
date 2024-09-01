//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $Revision: $
// $NoKeywords: $
//===========================================================================//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "networksystem/inetworksystem.h"

class CNetworkManager : public CBaseGameSystemPerFrame
{
   public:
    virtual char const *Name()
    {
        return "CNetworkManager";
    }

    virtual bool Init();
    virtual void Shutdown();

    // Update functions
    virtual void PerformUpdate();
#ifdef CLIENT_DLL
    virtual void Update( float frametime );
#else
    virtual void FrameUpdatePreEntityThink();
#endif

    // The manager can be a client or a server or both
    bool IsServer();
    bool IsClient();

    bool IsClientConnected();

    bool StartServer( unsigned short nServerListenPort = NETWORKSYSTEM_DEFAULT_SERVER_PORT );
    void ShutdownServer();

    bool StartClient( unsigned short nClientListenPort = NETWORKSYSTEM_DEFAULT_CLIENT_PORT );
    void ShutdownClient();

    bool ConnectClientToServer( const char *serverIp, int serverPort = NETWORKSYSTEM_DEFAULT_SERVER_PORT );
    void DisconnectClientFromServer();

    void PostClientToServerMessage( INetworkMessage *pMessage );
    void BroadcastServerToClientMessage( INetworkMessage *pMessage );

   private:
    bool m_bIsClient;
    bool m_bIsServer;

    INetworkPeerBase *m_pClientPeer;
    INetworkPeerBase *m_pServerPeer;
};

extern CNetworkManager *g_pNetworkManager;

#endif  // NETWORKMANAGER_H
