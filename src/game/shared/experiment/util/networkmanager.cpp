#include "cbase.h"
#include "tier1/bitbuf.h"
#include "tier0/icommandline.h"
#include "tier2/tier2.h"

#include "networkmanager.h"
#include "gameinfostore.h"
#include "lnetwork.h"
#include "networkmessage.h"

static CNetworkManager s_NetworkManager;
CNetworkManager *g_pNetworkManager = &s_NetworkManager;

#ifdef GAME_DLL
bool CNetworkManager::BindClient( int userId, INetChannel *netChannel )
{
    if ( !netChannel )
    {
        return false;
    }

    ConnectedPlayer_t player;
    player.userId = userId;
    player.netChannel = netChannel;
    m_ConnectedPlayers.AddToTail( player );

    // Bind the handler that will get called on incoming messages
    CDynamicWriteNetworkMessage *netMessage = new CDynamicWriteNetworkMessage();
    netMessage->SetSender( userId );
    netChannel->RegisterMessage( netMessage );

    return true;
}

void CNetworkManager::UnbindClient( int userId )
{
    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        if ( m_ConnectedPlayers[i].userId == userId )
        {
            m_ConnectedPlayers.Remove( i );
            break;
        }
    }
}
#else
bool CNetworkManager::BindClient( INetChannel *netChannel )
{
    if ( !netChannel )
    {
        return false;
    }

    ConnectedPlayer_t player;
    player.userId = -1;
    player.netChannel = netChannel;
    m_ConnectedPlayers.AddToTail( player );

    // Bind the handler that will get called on incoming messages
    CDynamicWriteNetworkMessage *netMessage = new CDynamicWriteNetworkMessage();
    netChannel->RegisterMessage( netMessage );

    return true;
}

void CNetworkManager::UnbindClient()
{
    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        if ( m_ConnectedPlayers[i].userId == -1 )
        {
            m_ConnectedPlayers.Remove( i );
            return;
        }
    }
}
#endif

void CNetworkManager::UnbindClients()
{
    m_ConnectedPlayers.Purge();
}

void CNetworkManager::SendClientToServerMessage( INetMessage *pMessage )
{
    if ( !pMessage )
    {
        return;
    }

    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        if ( m_ConnectedPlayers[i].userId == -1 )
        {
            m_ConnectedPlayers[i].netChannel->SendNetMsg( *pMessage, true );
            return;
        }
    }
}

void CNetworkManager::BroadcastServerToClientsMessage( INetMessage *pMessage )
{
    if ( !pMessage )
    {
        return;
    }

    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        m_ConnectedPlayers[i].netChannel->SendNetMsg( *pMessage, true );
    }
}

void CNetworkManager::SendServerToClientMessage( INetMessage *pMessage, int userId )
{
    if ( !pMessage )
    {
        return;
    }

    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        if ( m_ConnectedPlayers[i].userId == userId )
        {
            m_ConnectedPlayers[i].netChannel->SendNetMsg( *pMessage, true );
            return;
        }
    }
}
