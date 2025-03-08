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
bool CNetworkManager::BindClientServer( int playerIndex, INetChannel *netChannel )
{
    if ( !netChannel )
    {
        return false;
    }

    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        if ( m_ConnectedPlayers[i].playerIndex == playerIndex )
        {
            m_ConnectedPlayers[i].netChannel = netChannel;
            return true;
        }
    }

    ConnectedPlayer_t player;
    player.playerIndex = playerIndex;
    player.netChannel = netChannel;
    m_ConnectedPlayers.AddToTail( player );

    // Bind the handler that will get called on incoming messages
    CDynamicWriteNetworkMessage *netMessage = new CDynamicWriteNetworkMessage();
    netMessage->SetSender( playerIndex );
    netChannel->RegisterMessage( netMessage );

    return true;
}

void CNetworkManager::UnbindClientServer( int playerIndex )
{
    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        if ( m_ConnectedPlayers[i].playerIndex == playerIndex )
        {
            m_ConnectedPlayers.Remove( i );
            return;
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

    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        if ( m_ConnectedPlayers[i].playerIndex == -1 )
        {
            m_ConnectedPlayers[i].netChannel = netChannel;
            return true;
        }
    }

    ConnectedPlayer_t player;
    player.playerIndex = -1;
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
        if ( m_ConnectedPlayers[i].playerIndex == -1 )
        {
            m_ConnectedPlayers.Remove( i );
            return;
        }
    }
}
#endif

void CNetworkManager::SendClientToServerMessage( INetMessage *pMessage )
{
    if ( !pMessage )
    {
        return;
    }

    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        if ( m_ConnectedPlayers[i].playerIndex == -1 )
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
        if ( m_ConnectedPlayers[i].playerIndex != -1 )
        {
            m_ConnectedPlayers[i].netChannel->SendNetMsg( *pMessage, true );
        }
    }
}

void CNetworkManager::SendServerToClientMessage( INetMessage *pMessage, int playerIndex )
{
    if ( !pMessage )
    {
        return;
    }

    for ( int i = 0; i < m_ConnectedPlayers.Count(); i++ )
    {
        if ( m_ConnectedPlayers[i].playerIndex == playerIndex )
        {
            m_ConnectedPlayers[i].netChannel->SendNetMsg( *pMessage, true );
            return;
        }
    }
}
