#include "tier1/utlbuffer.h"
#include "tier1/strtools.h"

#include <winsock.h>
#include "tcp_sockets.h"
#include "tcp_process.h"
#include "netpacket.h"

CTcpClientSocket::CTcpClientSocket()
{
    m_Socket = INVALID_SOCKET;
    m_CurrentMessageReceiving = nullptr;
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

int CTcpClientSocket::ReceiveMessageLength()
{
    char buffer[sizeof( NETWORK_MESSAGE_LENGTH_DATATYPE )];
    int bytesRead = recv( m_Socket, buffer, sizeof( buffer ), 0 );

    if ( bytesRead == SOCKET_ERROR )
    {
        int nError = WSAGetLastError();

        if ( nError != WSAEWOULDBLOCK )
        {
            Warning( "Socket error '%i'\n", nError );
        }

        return -1;
    }

    if ( bytesRead == 0 )
    {
        return 0;
    }

    if ( bytesRead != sizeof( NETWORK_MESSAGE_LENGTH_DATATYPE ) )
    {
        return -1;
    }

    NETWORK_MESSAGE_LENGTH_DATATYPE length;
    memcpy( &length, buffer, sizeof( length ) );
    return LittleLong( length );
}

bool CTcpClientSocket::Receive( CUtlBuffer &data )
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

    data.SeekPut( CUtlBuffer::SEEK_CURRENT, bytesRead );

    return true;
}

bool CTcpClientSocket::HasDataInSocket()
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
        DevWarning( "CTcpClientSocket::HasDataInSocket:  Invalid socket on read\n" );
        return false;
    }

    return FD_ISSET( m_Socket, &readSet ) == 1;
}

bool CTcpClientSocket::Send( const char *data, size_t dataLength )
{
    // Send the length of the message first
    char lengthBuffer[sizeof( NETWORK_MESSAGE_LENGTH_DATATYPE )];
    NETWORK_MESSAGE_LENGTH_DATATYPE length = LittleLong( ( int )dataLength );
    memcpy( lengthBuffer, &length, sizeof( length ) );

    int bytesSent = send( m_Socket, lengthBuffer, sizeof( lengthBuffer ), 0 );

    if ( SOCKET_ERROR == bytesSent )
    {
        Assert( 0 );
        return false;
    }

    bytesSent = send( m_Socket, data, ( int )dataLength, 0 );

    if ( SOCKET_ERROR == bytesSent )
    {
        Assert( 0 );
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
        return TcpGetOrContinuePackets( this, newPackets );
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

    struct sockaddr clientAddress;
    int len = sizeof( clientAddress );
    SOCKET newSocket = accept( m_Socket, &clientAddress, &len );

    if ( newSocket == INVALID_SOCKET )
    {
        return false;
    }

    struct sockaddr_in localAddress, remoteAddress;
    int localAddressSize = sizeof( localAddress );

    if ( getsockname( newSocket, ( struct sockaddr * )&localAddress, &localAddressSize ) == SOCKET_ERROR )
    {
        closesocket( newSocket );
        return false;
    }

    if ( getpeername( newSocket, ( struct sockaddr * )&remoteAddress, &len ) == SOCKET_ERROR )
    {
        closesocket( newSocket );
        return false;
    }

    // Store the new client
    CTcpClientSocket *client = new CTcpClientSocket;
    client->m_Socket = newSocket;
    client->m_SocketIP.SetFromSockadr( &clientAddress );

    // Check if the connection is a loopback connection so we can store it as such
    if ( ( localAddress.sin_addr.s_addr == htonl( INADDR_LOOPBACK ) ) &&
         ( remoteAddress.sin_addr.s_addr == htonl( INADDR_LOOPBACK ) ) )
    {
        DevWarning( "CTcpServerSocket::AcceptClients:  Loopback connection\n" );

        client->m_SocketIP.SetType( NA_LOOPBACK );
    }

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
            i--;  // Decrement to avoid skipping the next client
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
            return TcpGetOrContinuePackets( client, newPackets );
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
