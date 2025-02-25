#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H
#ifdef _WIN32
#pragma once
#endif

#include "networksystem/inetworkpeerbase.h"

class CNetworkClient : public INetworkPeerBase
{
   public:
    CNetworkClient( INetworkSystem *networkSystem );
    virtual ~CNetworkClient();

    NETWORK_PEER_DECLARATION( CTcpClientSocket );

    bool Connect( const char *server, int port );
    void Disconnect();

    bool m_bConnected;
    INetworkSystem *m_pNetworkSystem;
};

#endif  // NETWORKCLIENT_H
