#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include "tcp_sockets.h"
#include "networksystem/inetworksystem.h"
#include "networksystem/inetworkpeerbase.h"

// Forward declarations
class CConnectedClient;

class CNetworkServer : public INetworkPeerBase
{
   public:
    CNetworkServer( INetworkSystem *networkSystem );
    virtual ~CNetworkServer();

    bool Init( int nServerPort );

    NETWORK_PEER_DECLARATION( CTcpServerSocket );

    bool AcceptClients( CUtlVector< IConnectedClient * > &newClients );
    IConnectedClient *FindClientByAddress( const netadr_t &adr );

    CUtlVector< CConnectedClient * > m_Clients;
    INetworkSystem *m_pNetworkSystem;
};

class CConnectedClient : public IConnectedClient
{
   public:
    CConnectedClient( CNetworkServer *server, netadr_t &remote, CTcpClientSocket *pSocket );
    virtual ~CConnectedClient();

    char const *GetName()
    {
        return "Unknown";
    }

    virtual bool SendNetMessage( INetworkMessage *pMessage ) override;
    void Shutdown();

    bool m_bMarkedForDeletion;
    CTcpClientSocket *m_pStreamSocket;
};

#endif  // NETWORKSERVER_H
