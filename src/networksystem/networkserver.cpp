#include <winsock.h>

#include "netpacket.h"
#include "networkserver.h"
#include "networksystem.h"
#include "networksystem/isockets.h"

#include "tcp_sockets.h"
#include "tcp_process.h"

#include "networkclient.h"

CNetworkServer::CNetworkServer( INetworkSystem *networkSystem )
{
    m_pStreamSocket = new CTcpServerSocket;
    m_pNetworkSystem = networkSystem;
}

CNetworkServer::~CNetworkServer()
{
    delete m_pStreamSocket;
}

bool CNetworkServer::Init( int nServerPort )
{
    if ( !GetStreamSocket()->Init( nServerPort ) )
    {
        Warning( "CNetworkServer:  Unable to create TCP socket!!!\n" );
        ConnectionState = NETWORK_CONNECTION_STATE::CONNECTION_FAILED;
        return false;
    }

    ConnectionState = NETWORK_CONNECTION_STATE::CONNECTED;
    return true;
}

void CNetworkServer::Shutdown()
{
    GetStreamSocket()->Shutdown();
}

bool CNetworkServer::AcceptClients( CUtlVector< IConnectedClient * > &newClients )
{
    CUtlVector< CTcpClientSocket * > clients;
    bool anyNewClients = false;

    if ( GetStreamSocket()->AcceptClients( clients ) )
    {
        for ( int i = 0; i < clients.Count(); ++i )
        {
            CTcpClientSocket *pClient = clients[i];
            CConnectedClient *client = new CConnectedClient( this, pClient->m_SocketIP, pClient );

            m_Clients.AddToTail( client );
            newClients.AddToHead( client );

            anyNewClients = true;
        }
    }

    return anyNewClients;
}

/// <summary>
/// Reads packages from the socket and processes them
/// </summary>
/// <param name=""></param>
void CNetworkServer::Update( void )
{
    CUtlVector< CNetPacket * > newPackets;
    if ( GetStreamSocket()->ReadPackets( newPackets ) )
    {
        for ( int i = 0; i < newPackets.Count(); ++i )
        {
            CNetPacket *pPacket = newPackets[i];
            IConnectedClient *client = FindClientByAddress( pPacket->m_From );

            if ( client )
            {
                m_pNetworkSystem->ProcessMessage( client, pPacket );
            }
            else
            {
                Warning( "Received packet from unknown player %s\n", pPacket->m_From.ToString() );
            }
        }
    }
}

/// <summary>
/// Sends a packet to all clients
/// </summary>
/// <param name="pMessage"></param>
void CNetworkServer::DispatchSocketMessage( INetworkMessage *pMessage )
{
    int c = m_Clients.Count();

    for ( int i = 0; i < c; ++i )
    {
        // If the message fails, we need to remove the client
        if ( !m_Clients[i]->SendNetMessage( pMessage ) )
        {
            m_Clients[i]->Shutdown();
        }
    }

    // Remove any clients that are marked for deletion
    for ( int i = c - 1; i >= 0; --i )
    {
        if ( m_Clients[i]->m_bMarkedForDeletion )
        {
            m_Clients.Remove( i );
        }
    }
}

IConnectedClient *CNetworkServer::FindClientByAddress( const netadr_t &adr )
{
    int c = m_Clients.Count();

    for ( int i = 0; i < c; ++i )
    {
        IConnectedClient *player = m_Clients[i];

        // In case of loopback return the host player (index 0)
        if ( i == 0 && adr.GetType() == NA_LOOPBACK )
            return player;

        if ( player->m_RemoteAddress.CompareAdr( adr ) )
            return player;
    }

    return NULL;
}

CConnectedClient::CConnectedClient( CNetworkServer *server, netadr_t &remote, CTcpClientSocket *pSocket )
    : m_bMarkedForDeletion( false ),
    m_pStreamSocket( pSocket )
{
    m_RemoteAddress = remote;
}

CConnectedClient::~CConnectedClient()
{
    Destruct( m_pStreamSocket );
    delete m_pStreamSocket;
    delete this;
}

void CConnectedClient::Shutdown()
{
    m_bMarkedForDeletion = true;
}

bool CConnectedClient::SendNetMessage( INetworkMessage *message )
{
    if ( m_bMarkedForDeletion )
        return false;

    return CNetworkSystem::SendSocketMessage( m_pStreamSocket, message );
}
