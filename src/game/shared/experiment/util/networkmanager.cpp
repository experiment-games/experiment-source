#include "cbase.h"
#include "tier1/bitbuf.h"
#include "tier0/icommandline.h"
#include "tier2/tier2.h"

#include "networksystem/isockets.h"
#include "networkmanager.h"
#include "networksystem/inetworkmessage.h"
#include <lnetwork.h>

static CNetworkManager s_NetworkManager;
extern CNetworkManager *g_pNetworkManager = &s_NetworkManager;

/*
** Network Manager
*/
bool CNetworkManager::Init()
{
    m_bIsClient = m_bIsServer = false;

    return true;
}

/// <summary>
/// Completely shuts down the network manager.
/// Should only be called when the game is shutting down.
/// </summary>
void CNetworkManager::Shutdown()
{
    ShutdownClient();
    ShutdownServer();

    g_pNetworkSystem->Shutdown();
}

bool CNetworkManager::StartServer( unsigned short serverListenPort )
{
    ShutdownServer();

    if ( serverListenPort == NETWORKSYSTEM_DEFAULT_SERVER_PORT )
    {
        serverListenPort = CommandLine()->ParmValue( "-serverport", NETWORKSYSTEM_DEFAULT_SERVER_PORT );
    }

    m_bIsServer = g_pNetworkSystem->StartServer( serverListenPort );
    m_pServerPeer = g_pNetworkSystem->GetServerPeer();
    return m_bIsServer;
}

void CNetworkManager::ShutdownServer()
{
    if ( m_bIsServer )
    {
        g_pNetworkSystem->ShutdownServer();
        m_bIsServer = false;
    }
}

bool CNetworkManager::StartClient( unsigned short nClientListenPort )
{
    ShutdownClient();

    if ( nClientListenPort == NETWORKSYSTEM_DEFAULT_CLIENT_PORT )
    {
        nClientListenPort = CommandLine()->ParmValue( "-clientport", NETWORKSYSTEM_DEFAULT_CLIENT_PORT );
    }

    m_bIsClient = g_pNetworkSystem->StartClient( nClientListenPort );
    return m_bIsClient;
}

void CNetworkManager::ShutdownClient()
{
    if ( m_bIsClient )
    {
        DisconnectClientFromServer();
        g_pNetworkSystem->ShutdownClient();
        m_bIsClient = false;
    }
}

bool CNetworkManager::ConnectClientToServer( const char *serverIp, int serverPort )
{
    DisconnectClientFromServer();

    if ( !IsClient() )
        return false;

    if ( serverPort == NETWORKSYSTEM_DEFAULT_SERVER_PORT )
    {
        serverPort = CommandLine()->ParmValue( "-serverport", NETWORKSYSTEM_DEFAULT_SERVER_PORT );
    }

    m_pClientPeer = g_pNetworkSystem->ConnectClientToServer( serverIp, serverPort );
    return ( m_pClientPeer != NULL );
}

void CNetworkManager::DisconnectClientFromServer()
{
    if ( m_pClientPeer )
    {
        g_pNetworkSystem->DisconnectClientFromServer();
        m_pClientPeer = NULL;
    }
}

bool CNetworkManager::IsClient()
{
    return m_bIsClient;
}

bool CNetworkManager::IsServer()
{
    return m_bIsServer;
}

bool CNetworkManager::IsClientConnected()
{
    return m_bIsClient && ( m_pClientPeer != NULL ) && ( m_pClientPeer->ConnectionState == NETWORK_CONNECTION_STATE::CONNECTED );
}

void CNetworkManager::SendClientToServerMessage( INetworkMessage *message )
{
    Assert( IsClientConnected() );
    g_pNetworkSystem->SendClientToServerMessage( message );
}

void CNetworkManager::BroadcastServerToClientMessage( INetworkMessage *message )
{
    Assert( IsServer() );
    g_pNetworkSystem->BroadcastServerToClientMessage( message );
}

void CNetworkManager::SendServerToClientMessage( INetworkMessage *message, const char *clientRemoteAddress )
{
    Assert( IsServer() );
    g_pNetworkSystem->SendServerToClientMessage( message, clientRemoteAddress );
}

void CNetworkManager::PerformUpdate()
{
    g_pNetworkSystem->Tick();
}

#ifdef CLIENT_DLL
void CNetworkManager::Update( float frametime )
{
    PerformUpdate();
}
#else
void CNetworkManager::FrameUpdatePreEntityThink()
{
    PerformUpdate();
}
#endif

#ifdef CLIENT_DLL
CON_COMMAND( test_net_cl_send, "Tests sending a net message ad-hoc on the client" )
{
    CDynamicWriteNetworkMessage *message = new CDynamicWriteNetworkMessage( NETWORK_MESSAGE_GROUP::SCRIPT, 0 );
    // Let's write some test data to the message
    byte sendBuffer[NET_MAX_MESSAGE];
    bf_write buffer( "test_net_cl_send", sendBuffer, sizeof( sendBuffer ) );

    buffer.WriteFloat( 3.14f );

    const char *pszString = "Hello, world!";
    buffer.WriteLong( Q_strlen( pszString ) );
    buffer.WriteString( pszString );

    buffer.WriteLong( 42 );

    message->SetBuffer( ( const char * )buffer.GetBasePointer(), buffer.GetNumBytesWritten() );

    if ( !message )
        return;

    s_NetworkManager.SendClientToServerMessage( message );

    delete message;
}
#else
CON_COMMAND( test_net_sv_send, "Test sending a net message ad-hoc on the server" )
{
    CDynamicWriteNetworkMessage *message = new CDynamicWriteNetworkMessage( NETWORK_MESSAGE_GROUP::SCRIPT, 0 );
    // Let's write some test data to the message
    byte sendBuffer[NET_MAX_MESSAGE];
    bf_write buffer( "test_net_sv_send", sendBuffer, sizeof( sendBuffer ) );

    buffer.WriteFloat( 3.14f );

    const char *pszString = "Hello, world!";
    buffer.WriteLong( Q_strlen( pszString ) );
    buffer.WriteString( pszString );

    buffer.WriteLong( 42 );

    message->SetBuffer( ( const char * )buffer.GetBasePointer(), buffer.GetNumBytesWritten() );

    if ( !message )
        return;

    s_NetworkManager.BroadcastServerToClientMessage( message );

    delete message;
}
#endif
