#include "tier1/bitbuf.h"

#include <winsock.h>
#include "networksystem/isockets.h"
#include "netpacket.h"
#include "networksystem.h"
#include "networkclient.h"

#include "tcp_sockets.h"
#include "tcp_process.h"

CNetworkClient::CNetworkClient( INetworkSystem *networkSystem )
    : m_bConnected( false )
{
    m_pStreamSocket = new CTcpClientSocket;
    m_pNetworkSystem = networkSystem;
}

CNetworkClient::~CNetworkClient()
{
    delete m_pStreamSocket;
}

void CNetworkClient::Shutdown()
{
    Disconnect();
}

bool CNetworkClient::Connect( char const *server, int port /*=SM_SERVER_PORT*/ )
{
    ConnectionState = NETWORK_CONNECTION_STATE::CONNECTING;

    netadr_t remote;
    remote.type = NA_IP;
    remote.port = htons( ( unsigned short )port );

    // Resolve remote name
    sockaddr sa;

    if ( Q_stricmp( server, "localhost" ) == 0 || Q_stricmp( server, "loopback" ) == 0 )
    {
        remote.ip[0] = 127;
        remote.ip[1] = 0;
        remote.ip[2] = 0;
        remote.ip[3] = 1;
    }
    else
    {
        if ( !CNetworkSystem::StringToSockaddr( server, &sa ) )
        {
            Msg( "Unable to resolve '%s'\n", server );
            ConnectionState = NETWORK_CONNECTION_STATE::CONNECTION_FAILED;
            return false;
        }

        remote.SetFromSockadr( &sa );
    }

    if ( !GetStreamSocket()->Connect( remote ) )
    {
        Warning( "CNetworkClient:  Unable to connect to TCP server!!!\n" );
        ConnectionState = NETWORK_CONNECTION_STATE::CONNECTION_FAILED;
        return false;
    }

    ConnectionState = NETWORK_CONNECTION_STATE::CONNECTED;
    return true;
}

void CNetworkClient::Disconnect()
{
    ConnectionState = NETWORK_CONNECTION_STATE::DISCONNECTED;

    GetStreamSocket()->Disconnect();
}

void CNetworkClient::Update()
{
    CUtlVector< CNetPacket * > newPackets;
    GetStreamSocket()->ReadPackets( newPackets );

    for ( int i = 0; i < newPackets.Count(); ++i )
    {
        CNetPacket *pPacket = newPackets[i];
        m_pNetworkSystem->ProcessMessage( nullptr, pPacket );
    }
}

void CNetworkClient::SendPacket( INetworkMessage *packet )
{
    //DEBUG_MESSAGE_DATA( "sending", ( unsigned char * )packet->GetData(), packet->GetSize() );

    if ( !GetStreamSocket()->Send( packet->GetData(), packet->GetSize() ) )
    {
        Warning( "CNetworkClient::SendPacket:  Unable to send packet\n" );
    }
}
