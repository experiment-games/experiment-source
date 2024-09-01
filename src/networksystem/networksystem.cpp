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

#if _DEBUG
void DEBUG_MESSAGE_DATA( const char *prefix, unsigned char *data, int size )
{
    bf_read buffer( data, size );
    buffer.Seek( 0 );

    CUtlString debugString;

    debugString.Format( "%s packet: \t%i bytes\n", prefix, size );
    debugString.Append( "Packet header: \n" );
    debugString.Append( "group: " );
    unsigned int group = buffer.ReadUBitLong( NETWORK_MESSAGE_GROUP_BITS );
    char groupString[256];
    sprintf( groupString, "%d", group );
    debugString.Append( groupString );
    debugString.Append( "\n" );
    debugString.Append( "type: " );
    unsigned int type = buffer.ReadUBitLong( NETWORK_MESSAGE_INDEX_BITS );
    char typeString[256];
    sprintf( typeString, "%d", type );
    debugString.Append( typeString );
    debugString.Append( "\n" );

    int headerBytes = ( NETWORK_MESSAGE_HEADER_BITS / 8 );
    // Pad with zeros to byte align the header
    headerBytes += ( ( headerBytes % 4 ) == 0 ) ? 0 : 4 - ( headerBytes % 4 );

    debugString.Append( "Packet data: \t" );

    for ( int i = 0; i < size - headerBytes; ++i )
    {
        debugString.Append( "\t" );
        int byte = buffer.ReadByte();
        char byteString[256];
        sprintf( byteString, "%d", byte );
        debugString.Append( byteString );
        debugString.Append( "(" );
        debugString.Append( byte );
        debugString.Append( ")" );
    }

    debugString.Append( "\n" );
    Msg( debugString.Get() );
}
#endif

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

    CleanupNetworkHandlers();
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
    if ( m_pServer || m_pClient )
    {
        Warning( "Cannot register messages after connection has started.\n" );
        return;
    }

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

void CNetworkSystem::CleanupNetworkHandlers()
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

bool CNetworkSystem::StartClient( unsigned short nClientListenPort )
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

void CNetworkSystem::DisconnectClientFromServer( INetworkPeerBase *client )
{
    if ( m_pClient && m_pClient == client )
    {
        m_pClient->Disconnect();
    }
}

INetworkPeerBase *CNetworkSystem::GetServerPeer()
{
    return m_pServer;
}

void CNetworkSystem::PostClientToServerMessage( INetworkMessage *pMessage )
{
    Assert( m_pClient );
    m_pClient->SendPacket( pMessage );
}

void CNetworkSystem::BroadcastServerToClientMessage( INetworkMessage *pMessage )
{
    Assert( m_pServer );
    m_pServer->SendPacket( pMessage );
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

    // Skip the rest of the header
    int headerBytes = INetworkMessage::GetHeaderBytes();
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

        Msg( "unknown net message group %i  \n", group );
        Assert( 0 );
        return false;
    }

    INetworkMessage *pNetMessage = pHandler->ReadMessage( messageTypeId, payload );

    if ( !pNetMessage )
    {
        free( payloadBuffer );

        Msg( "unknown net message (%i:%i) \n", group, messageTypeId );
        Assert( 0 );
        return false;
    }

    free( payloadBuffer );

    return true;
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
