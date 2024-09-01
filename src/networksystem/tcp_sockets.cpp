#include "tier1/utlbuffer.h"
#include "tier1/strtools.h"

#include <winsock.h>
#include "tcp_sockets.h"
#include "tcp_process.h"
#include "netpacket.h"

CTcpClientSocket::CTcpClientSocket()
{
    m_Socket = INVALID_SOCKET;
    Q_memset( &m_SocketIP, 0, sizeof( m_SocketIP ) );
}

CTcpClientSocket::~CTcpClientSocket()
{
    Disconnect();
}

bool CTcpClientSocket::Connect( netadr_t remote )
{
    m_Socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( m_Socket == INVALID_SOCKET )
        return false;

    struct sockaddr address;
    remote.ToSockadr( &address );

    if ( SOCKET_ERROR == connect( m_Socket, &address, sizeof( address ) ) )
    {
        return false;
    }

    return true;
}

void CTcpClientSocket::Disconnect()
{
    if ( m_Socket != INVALID_SOCKET )
    {
        closesocket( m_Socket );
    }
}

bool CTcpClientSocket::ReceiveFrom( netadr_t &remote, CUtlBuffer &data )
{
    int maxBytesToRead = data.Size() - data.TellPut();
    char *buffer = ( char * )data.PeekPut();

    int bytesRead = recv( m_Socket, buffer, maxBytesToRead, 0 );

    if ( bytesRead == SOCKET_ERROR )
    {
        int nError = WSAGetLastError();

        if ( nError != WSAEWOULDBLOCK )
        {
            Warning( "Socket error '%i'\n", nError );
        }

        return false;
    }

    remote.SetFromSocket( m_Socket );
    data.SeekPut( CUtlBuffer::SEEK_CURRENT, bytesRead );

    return true;
}

bool CTcpClientSocket::Send( const char *data, size_t dataLength )
{
    int bytesSent = send( m_Socket, data, ( int )dataLength, 0 );

    if ( SOCKET_ERROR == bytesSent )
    {
        return false;
    }

    return true;
}

bool CTcpClientSocket::ReadPackets( CUtlVector< CNetPacket * > &newPackets )
{
    if ( m_Socket == INVALID_SOCKET )
    {
        DevWarning( "CTcpClientSocket::ReadPackets:  Invalid socket\n" );
        return false;
    }

    fd_set readSet;
    FD_ZERO( &readSet );
    FD_SET( m_Socket, &readSet );

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int nRet = select( 0, &readSet, NULL, NULL, &timeout );  // Check for incoming data
    if ( nRet == SOCKET_ERROR )
    {
        DevWarning( "CTcpClientSocket::ReadPackets:  Invalid socket on read\n" );
        return nullptr;
    }

    if ( FD_ISSET( m_Socket, &readSet ) )
    {
        CNetPacket *pPacket = TcpGetPacket( this );

        if ( pPacket )
        {
            pPacket->m_From = m_SocketIP;
            newPackets.AddToTail( pPacket );
            return true; // TODO: Can there be more than one packet in a single read?
        }
    }

    return false;
}

CTcpServerSocket::CTcpServerSocket()
    : m_Socket( INVALID_SOCKET ),
      m_Port( 0 )
{
}

CTcpServerSocket::~CTcpServerSocket()
{
    Shutdown();
}

bool CTcpServerSocket::Init( unsigned short bindToPort )
{
    m_Port = bindToPort;

    return CreateSocket();
}

void CTcpServerSocket::Shutdown()
{
    if ( m_Socket != INVALID_SOCKET )
    {
        closesocket( m_Socket );
    }
}

bool CTcpServerSocket::AcceptClients( CUtlVector< CTcpClientSocket * > &newClients )
{
    fd_set readSet;
    FD_ZERO( &readSet );
    FD_SET( m_Socket, &readSet );

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int nRet = select( 0, &readSet, NULL, NULL, &timeout );

    if ( nRet == SOCKET_ERROR )
    {
        DevWarning( "CTcpServerSocket::ReadPackets:  Invalid socket on read\n" );
        return false;
    }

    if ( !FD_ISSET( m_Socket, &readSet ) )
    {
        return false;
    }

    SOCKET newSocket = accept( m_Socket, NULL, NULL );

    if ( newSocket == INVALID_SOCKET )
    {
        return false;
    }

    CTcpClientSocket *client = new CTcpClientSocket;
    client->m_Socket = newSocket;
    client->m_SocketIP.SetFromSocket( newSocket );
    m_Clients.AddToTail( client );
    newClients.AddToTail( client );
    return true;
}

bool CTcpServerSocket::ReadPackets( CUtlVector< CNetPacket * > &newPackets )
{
    if ( m_Socket == INVALID_SOCKET )
    {
        DevWarning( "CTcpServerSocket::ReadPackets:  Invalid socket\n" );
        return false;
    }

    bool anyPackets = false;

    // Check for incoming data
    for ( int i = 0; i < m_Clients.Count(); i++ )
    {
        CTcpClientSocket *client = m_Clients[i];

        if ( client->m_Socket == INVALID_SOCKET )
        {
            // TODO:Inform the NetworkServer of the disconnect
            delete m_Clients[i];
            m_Clients.Remove( i );
            i--; // Decrement to avoid skipping the next client
            continue;
        }

        fd_set readSet;
        FD_ZERO( &readSet );
        FD_SET( client->m_Socket, &readSet );

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int nRet = select( 0, &readSet, NULL, NULL, &timeout );

        if ( nRet == SOCKET_ERROR )
        {
            DevWarning( "CTcpServerSocket::ReadPackets:  Invalid socket on read\n" );
            continue;
        }

        if ( FD_ISSET( client->m_Socket, &readSet ) )
        {
            CNetPacket *pPacket = TcpGetPacket( client );

            if ( pPacket )
            {
                pPacket->m_From = client->m_SocketIP;
                newPackets.AddToTail( pPacket );
                anyPackets = true;
            }
        }
    }

    return anyPackets;
}

bool CTcpServerSocket::CreateSocket( void )
{
    m_Socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( m_Socket == INVALID_SOCKET )
        return false;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons( m_Port );
    address.sin_addr.S_un.S_addr = INADDR_ANY;

    if ( SOCKET_ERROR == bind( m_Socket, ( struct sockaddr * )&address, sizeof( address ) ) )
    {
        return false;
    }

    if ( SOCKET_ERROR == listen( m_Socket, 5 ) )
    {
        return false;
    }

    return true;
}
