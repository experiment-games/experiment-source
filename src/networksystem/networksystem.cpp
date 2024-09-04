#include <winsock.h>
#include "networksystem/isockets.h"
#include "networksystem.h"
#include "networksystem/inetworkmessage.h"
#include "netpacket.h"
#include "networkclient.h"
#include "networkserver.h"

#include "tcp_sockets.h"
#include "tcp_process.h"

EXPOSE_INTERFACE( CNetworkSystem, INetworkSystem, NETWORKSYSTEM_INTERFACE_VERSION );

CNetworkSystem::CNetworkSystem()
{
    m_bWinSockInitialized = false;
    m_pServer = NULL;
    m_pClient = NULL;

    m_MessageHandlers.SetLessFunc( DefLessFunc( NETWORK_MESSAGE_GROUP::TYPE ) );
}

CNetworkSystem::~CNetworkSystem()
{
}

InitReturnVal_t CNetworkSystem::Init()
{
    InitReturnVal_t nRetVal = BaseClass::Init();
    if ( nRetVal != INIT_OK )
        return nRetVal;

    WSAData wsaData;

    if ( WSAStartup( MAKEWORD( 2, 0 ), &wsaData ) != 0 )
    {
        Warning( "Error! Failed to load network socket library.\n" );
        return INIT_OK;
    }
    else
    {
        m_bWinSockInitialized = true;
    }

    return INIT_OK;
}

void CNetworkSystem::Shutdown()
{
    if ( m_bWinSockInitialized )
    {
        WSACleanup();
    }

    BaseClass::Shutdown();
}

bool CNetworkSystem::Connect( CreateInterfaceFn factory )
{
    if ( !BaseClass::Connect( factory ) )
        return false;

    return INIT_OK;
}

void CNetworkSystem::RegisterGroupHandler( NETWORK_MESSAGE_GROUP::TYPE group, INetworkGroupHandler *pHandler )
{
    if ( group == NETWORK_MESSAGE_GROUP::INTERNAL )
    {
        Warning( "Network message group %d is reserved by the network system.\n", group );
        return;
    }

    // Look for already registered message handlers for this group
    if ( m_MessageHandlers.Find( group ) != m_MessageHandlers.InvalidIndex() )
    {
        Warning( "Network message group %d already has a handler registered.\n", group );
        Assert( 0 );
        return;
    }

    m_MessageHandlers.Insert( group, pHandler );
}

void CNetworkSystem::UnregisterNetworkHandler( NETWORK_MESSAGE_GROUP::TYPE group, INetworkGroupHandler *pHandler )
{
    if ( group == NETWORK_MESSAGE_GROUP::INTERNAL )
    {
        Warning( "Network message group %d is reserved by the network system.\n", group );
        return;
    }

    unsigned short key = m_MessageHandlers.Find( group );

    if ( key == m_MessageHandlers.InvalidIndex() )
    {
        Warning( "Network message group %d has no handler registered.\n", group );
        return;
    }

    if ( m_MessageHandlers[key] != pHandler )
    {
        Warning( "Network message group %d has a different handler registered.\n", group );
        return;
    }

    m_MessageHandlers.RemoveAt( key );
}

void CNetworkSystem::UnregisterAllNetworkHandlers()
{
    m_MessageHandlers.PurgeAndDeleteElements();
}

INetworkGroupHandler *CNetworkSystem::FindNetworkGroupHandler( NETWORK_MESSAGE_GROUP::TYPE group )
{
    unsigned short key = m_MessageHandlers.Find( group );

    if ( key == m_MessageHandlers.InvalidIndex() )
        return NULL;

    return m_MessageHandlers[key];
}

bool CNetworkSystem::StringToSockaddr( const char *s, struct sockaddr *sadr )
{
    struct hostent *h;
    char *colon;
    char copy[128];

    Q_memset( sadr, 0, sizeof( *sadr ) );
    ( ( struct sockaddr_in * )sadr )->sin_family = AF_INET;
    ( ( struct sockaddr_in * )sadr )->sin_port = 0;

    Q_strncpy( copy, s, sizeof( copy ) );
    // strip off a trailing :port if present
    for ( colon = copy; *colon; colon++ )
        if ( *colon == ':' )
        {
            *colon = 0;
            ( ( struct sockaddr_in * )sadr )->sin_port = htons( ( short )atoi( colon + 1 ) );
        }

    if ( copy[0] >= '0' && copy[0] <= '9' && Q_strstr( copy, "." ) )
    {
        *( int * )&( ( struct sockaddr_in * )sadr )->sin_addr = inet_addr( copy );
    }
    else
    {
        if ( ( h = gethostbyname( copy ) ) == NULL )
            return false;

        *( int * )&( ( struct sockaddr_in * )sadr )->sin_addr = *( int * )h->h_addr_list[0];
    }

    return true;
}

bool CNetworkSystem::StartServer( unsigned short serverPort )
{
    if ( !m_bWinSockInitialized )
        return false;

    Assert( !m_pServer );
    m_pServer = new CNetworkServer( this );
    return m_pServer->Init( serverPort );
}

void CNetworkSystem::ShutdownServer()
{
    if ( m_pServer )
    {
        m_pServer->Shutdown();
        delete m_pServer;
        m_pServer = NULL;
    }
}

bool CNetworkSystem::StartClient()
{
    if ( !m_bWinSockInitialized )
        return false;

    Assert( !m_pClient );
    m_pClient = new CNetworkClient( this );
    return true;
}

void CNetworkSystem::ShutdownClient()
{
    if ( m_pClient )
    {
        DisconnectClientFromServer();
        m_pClient->Shutdown();
        delete m_pClient;
        m_pClient = NULL;
    }
}

INetworkPeerBase *CNetworkSystem::ConnectClientToServer( const char *serverIp, int serverPort )
{
    if ( m_pClient )
    {
        if ( m_pClient->Connect( serverIp, serverPort ) )
            return m_pClient;
    }

    return NULL;
}

void CNetworkSystem::DisconnectClientFromServer()
{
    if ( m_pClient )
    {
        m_pClient->Disconnect();
    }
}

INetworkPeerBase *CNetworkSystem::GetServerPeer()
{
    return m_pServer;
}

void CNetworkSystem::SendClientToServerMessage( INetworkMessage *message )
{
    Assert( m_pClient );
    m_pClient->DispatchSocketMessage( message );
}

void CNetworkSystem::BroadcastServerToClientMessage( INetworkMessage *message )
{
    Assert( m_pServer );
    m_pServer->DispatchSocketMessage( message );
}

void CNetworkSystem::SendServerToClientMessage( INetworkMessage *message, IConnectedClient *client )
{
    Assert( m_pServer );
    Assert( client );

    client->SendNetMessage( message );
}

bool CNetworkSystem::ProcessControlMessage( int messageCommand, bf_read &buffer )
{
    switch ( messageCommand )
    {
        case NETWORK_MESSAGE_INTERNAL::NOP:
            return true;

        case NETWORK_MESSAGE_INTERNAL::DISCONNECTED:
            Shutdown();
            return false;

        default:
            Msg( "received bad internal message command %i.\n", messageCommand );
            return false;
    }
}

/// <summary>
/// Reads the packet, extracts the group and message type id, and forwards the message to the appropriate handler
/// </summary>
/// <param name="client">The client that sent the message (nullptr if its from the server)</param>
/// <param name="packet"></param>
/// <returns></returns>
bool CNetworkSystem::ProcessMessage( IConnectedClient *client, CNetPacket *packet )
{
    // DEBUG_MESSAGE_DATA( "receiving", packet->m_pData, packet->m_nSizeInBytes );

    bf_read buffer( packet->m_pData, packet->m_nSizeInBytes );

    if ( buffer.IsOverflowed() )
        return false;

    if ( buffer.GetNumBitsLeft() < NETWORK_MESSAGE_HEADER_BITS )
        return false;

    unsigned int group = buffer.ReadUBitLong( NETWORK_MESSAGE_GROUP_BITS );
    unsigned int messageTypeId = buffer.ReadUBitLong( NETWORK_MESSAGE_INDEX_BITS );

    // Skip the rest of the header which is padding
    int headerBytes = INetworkSystem::GetHeaderBytes();
    buffer.SeekRelative( ( headerBytes * 8 ) - NETWORK_MESSAGE_HEADER_BITS );

    // Create a payload buffer existing only of the message data
    int payloadBytes = buffer.GetNumBytesLeft();
    char *payloadBuffer = ( char * )malloc( payloadBytes );
    buffer.ReadBytes( payloadBuffer, payloadBytes );

    bf_read payload( payloadBuffer, payloadBytes );

    if ( group == NETWORK_MESSAGE_GROUP::INTERNAL )
    {
        if ( !ProcessControlMessage( messageTypeId, payload ) )
        {
            free( payloadBuffer );
            Assert( 0 );  // unhandled control message
            return false;
        }

        free( payloadBuffer );
        return true;
    }

    INetworkGroupHandler *pHandler = FindNetworkGroupHandler( ( NETWORK_MESSAGE_GROUP::TYPE )group );

    if ( !pHandler )
    {
        free( payloadBuffer );

        Msg( "unknown net message group handler %i  \n", group );
        Assert( 0 );
        return false;
    }

    pHandler->HandleReadingMessage( messageTypeId, payload, client );

    free( payloadBuffer );

    return true;
}

/// <summary>
/// Sends the message, prefix the group and type id to the message
/// </summary>
/// <param name="socket"></param>
/// <param name="message"></param>
/// <returns></returns>
bool CNetworkSystem::SendSocketMessage( ISendData *socket, INetworkMessage *message )
{
    Assert( socket );

    const char *payload = message->GetData();
    int payloadBufferLength = message->GetSize();

    int headerBytes = INetworkSystem::GetHeaderBytes();
    int fullBufferLength = payloadBufferLength + headerBytes;

    const char *fullBuffer = ( const char * )malloc( fullBufferLength );

    char *headerBuffer = ( char * )malloc( headerBytes );
    bf_write header( "CDynamicWriteNetworkMessage::WriteHeader", headerBuffer, headerBytes );
    header.WriteUBitLong( message->GetGroup(), NETWORK_MESSAGE_GROUP_BITS );
    header.WriteUBitLong( message->GetMessageTypeId(), NETWORK_MESSAGE_INDEX_BITS );

    memcpy( ( void * )fullBuffer, headerBuffer, headerBytes );
    memcpy( ( void * )( fullBuffer + headerBytes ), payload, payloadBufferLength );
    free( headerBuffer );

    return socket->Send( fullBuffer, fullBufferLength );
}

bool CNetworkSystem::AcceptClients( CUtlVector< IConnectedClient * > &newClients )
{
    Assert( m_pServer );

    return m_pServer->AcceptClients( newClients );
}

void CNetworkSystem::Tick( void )
{
    if ( m_pServer )
    {
        m_pServer->Update();
    }

    if ( m_pClient )
    {
        m_pClient->Update();
    }
}
