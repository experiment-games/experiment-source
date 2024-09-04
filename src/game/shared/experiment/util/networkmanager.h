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
#include "GameEventListener.h"

struct ConnectedPlayer_t
{
    IConnectedClient *client;
    CBasePlayer *player;
};

class CNetworkManager : public CBaseGameSystemPerFrame, public CGameEventListener
{
   public:
    virtual char const *Name()
    {
        return "CNetworkManager";
    }
    
    // IGameEventListener interface:
    virtual void FireGameEvent( IGameEvent *event );

    virtual bool Init();
    virtual void Shutdown();

    bool AcceptClient( CBasePlayer *player );

    // Update functions
    virtual void Tick();
#ifdef CLIENT_DLL
    virtual void Update( float frametime );
#else
    virtual void FrameUpdatePreEntityThink();
#endif

    // The manager can be a client or a server or both
    bool IsServer();
    bool IsClient();

    bool IsClientConnected();

    bool StartServer( unsigned short serverPort );
    void ShutdownServer();

    bool StartClient();
    void ShutdownClient();

    bool ConnectClientToServer();
    void DisconnectClientFromServer();

    void SendClientToServerMessage( INetworkMessage *pMessage );
    void BroadcastServerToClientMessage( INetworkMessage *pMessage );
    void SendServerToClientMessage( INetworkMessage *pMessage, CBasePlayer *client );
    IConnectedClient *FindConnectedClient( CBasePlayer *client );
    CBasePlayer *FindConnectedPlayer( IConnectedClient *client );

   private:
    bool m_bIsClient;
    bool m_bIsServer;

    INetworkPeerBase *m_pClientPeer;
    INetworkPeerBase *m_pServerPeer;

    CUtlVector< ConnectedPlayer_t > m_ConnectedPlayers;
};

extern CNetworkManager *g_pNetworkManager;

#endif  // NETWORKMANAGER_H
