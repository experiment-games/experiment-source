//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $Revision: $
// $NoKeywords: $
//===========================================================================//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "GameEventListener.h"
#include <inetchannel.h>

struct ConnectedPlayer_t
{
    int playerIndex;
    INetChannel *netChannel;
};

/// <summary>
/// Main interface for using NetChannel to send and receive messages.
/// </summary>
class CNetworkManager
{
   public:
#ifdef GAME_DLL
    virtual bool BindClientServer( int playerIndex, INetChannel *netChannel );
#else
    virtual bool BindClient( INetChannel *netChannel );
    virtual void UnbindClient();
#endif

    virtual void UnbindClients();

    virtual void SendClientToServerMessage( INetMessage *pMessage );
    virtual void BroadcastServerToClientsMessage( INetMessage *pMessage );
    virtual void SendServerToClientMessage( INetMessage *pMessage, int playerIndex );

   private:
    CUtlVector< ConnectedPlayer_t > m_ConnectedPlayers;
};

extern CNetworkManager *g_pNetworkManager;

#endif  // NETWORKMANAGER_H
