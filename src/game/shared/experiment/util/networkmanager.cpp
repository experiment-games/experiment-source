#include "cbase.h"
#include "tier1/bitbuf.h"
#include "tier0/icommandline.h"
#include "tier2/tier2.h"

#include "networksystem/isockets.h"
#include "networkmanager.h"
#include "networksystem/inetworkmessage.h"
#include "gameinfostore.h"
#include "lnetwork.h"

static CNetworkManager s_NetworkManager;
extern CNetworkManager *g_pNetworkManager = &s_NetworkManager;

/*
** Network Manager
*/
bool CNetworkManager::Init()
{
    m_bIsClient = m_bIsServer = false;

#ifdef GAME_DLL
    ListenForGameEvent( "server_spawn" );
#endif

    return true;
}

void CNetworkManager::FireGameEvent( IGameEvent *event )
{
#ifdef GAME_DLL
    const char *eventType = event->GetName();
    if ( Q_strcmp( eventType, "server_spawn" ) == 0 )
    {
        int serverPort = event->GetInt( "port" );

        // Experiment; TODO:    Since we're using the same port as the game server, we're causing a bug
        //                      where the game server things its out of date and says that in the console...
        //                      We should really figure out how to use the SDK integrated messaging over UDP
        //                      with the datagrams.
        if ( !g_pNetworkManager->StartServer( serverPort ) )
        {
            Assert( 0 );  // Does this ever happen?
            return;
        }
    }
#endif
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

bool CNetworkManager::StartServer( unsigned short serverPort )
{
    ShutdownServer();

    m_bIsServer = g_pNetworkSystem->StartServer( serverPort );
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

bool CNetworkManager::StartClient()
{
    ShutdownClient();

    m_bIsClient = g_pNetworkSystem->StartClient();
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

bool CNetworkManager::ConnectClientToServer()
{
    DisconnectClientFromServer();

    if ( !IsClient() )
        return false;

    const char *serverIp = g_pGameInfoStore->GetServerIpAsString();
    unsigned short serverPort = g_pGameInfoStore->GetServerPort();

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

void CNetworkManager::SendServerToClientMessage( INetworkMessage *message, CBasePlayer *player )
{
    Assert( IsServer() );

    if ( player->IsBot() )
    {
        // Bots don't have a network connection
        return;
    }

    IConnectedClient *connectedClient = FindConnectedClient( player );

    if ( !connectedClient )
    {
        Assert( 0 );  // TODO: what should we do if we can't find the player? Message? Error?
        return;
    }

    g_pNetworkSystem->SendServerToClientMessage( message, connectedClient );
}

IConnectedClient *CNetworkManager::FindConnectedClient( CBasePlayer *player )
{
    Assert( IsServer() );

    for ( int i = 0; i < m_ConnectedPlayers.Count(); ++i )
    {
        ConnectedPlayer_t &connectedPlayer = m_ConnectedPlayers[i];

        if ( connectedPlayer.player == player )
        {
            return connectedPlayer.client;
        }
    }

    return nullptr;
}

CBasePlayer *CNetworkManager::FindConnectedPlayer( IConnectedClient *client )
{
    Assert( IsServer() );

    for ( int i = 0; i < m_ConnectedPlayers.Count(); ++i )
    {
        ConnectedPlayer_t &connectedPlayer = m_ConnectedPlayers[i];

        if ( connectedPlayer.client == client )
        {
            return connectedPlayer.player;
        }
    }

    return nullptr;
}

/// <summary>
/// Tries to find a connected socket and tie it to the player.
/// If it fails, the player will not be able to communicate with the server so
/// we return false to have the player kicked.
/// </summary>
/// <param name="client"></param>
/// <returns></returns>
bool CNetworkManager::AcceptClient( CBasePlayer *client )
{
    Assert( IsServer() );

    CUtlVector< IConnectedClient * > newClients;

    if ( !g_pNetworkSystem->AcceptClients( newClients ) )
    {
        // No new clients, we can't tie this player to a socket
        // This happens for bots.
        return false;
    }

    char clientAddress[32];

    if ( !g_pGameInfoStore->GetPlayerAddress( client, clientAddress, sizeof( clientAddress ) ) )
    {
        // We couldn't determine the player's address, so it's impossible to tie them to a socket
        return false;
    }

    int numMatches = 0;
    IConnectedClient *matchingClient = NULL;

    for ( int i = 0; i < newClients.Count(); ++i )
    {
        IConnectedClient *newClient = newClients[i];
        // TODO: bool isLoopback = newClient->IsLoopback();
        const char *newClientAddress = newClient->GetRemoteAddress();

        // First check if the remoteAddress is already in the list of connected players
        for ( int j = 0; j < m_ConnectedPlayers.Count(); ++j )
        {
            ConnectedPlayer_t &connectedPlayer = m_ConnectedPlayers[j];

            if ( connectedPlayer.client == newClient )
            {
                // This player is already connected. Don't allow anyone to
                // hijack their connection.
                return false;
            }
        }

        // If the remoteAddress doesn't contain a : then we are likely connecting as localhost
        // In that situation we only compare the IP. Later we check if multiple ip matches
        // happened and fail by kicking all those matching players.
        char *found = Q_strstr( clientAddress, ":" );

        if ( !found && Q_strcmp( clientAddress, "loopback" ) != 0 )
        {
            int positionOfColonInNew = Q_strstr( newClientAddress, ":" ) - newClientAddress;

            if ( Q_strncmp( newClientAddress, clientAddress, positionOfColonInNew ) == 0 )
            {
                numMatches++;
                matchingClient = newClient;
            }
        }
        else if ( Q_strcmp( newClientAddress, clientAddress ) == 0 )
        {
            numMatches++;
            matchingClient = newClient;
        }
    }

    // Only allow 1 match, so that if multiple players connect at the same time
    // from the same IP, we don't confuse their sockets.
    // TODO: Find a way to get their connecting port, so we don't have to be this
    // janky.
    if ( numMatches != 1 )
    {
        return false;
    }

    ConnectedPlayer_t connectedPlayer;
    connectedPlayer.client = matchingClient;
    connectedPlayer.player = client;
    m_ConnectedPlayers.AddToTail( connectedPlayer );

    return true;
}

void CNetworkManager::Tick()
{
    g_pNetworkSystem->Tick();
}

#ifdef CLIENT_DLL
void CNetworkManager::Update( float frametime )
{
    Tick();
}
#else
void CNetworkManager::FrameUpdatePreEntityThink()
{
    Tick();
}
#endif
