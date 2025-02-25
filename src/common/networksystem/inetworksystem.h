//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//===========================================================================//

#ifndef INETWORKSYSTEM_H
#define INETWORKSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/IAppSystem.h"

#include "networksystem/isockets.h"
#include "networksystem/inetworkmessage.h"
#include "networksystem/inetworkgrouphandler.h"
#include "networksystem/inetworkpeerbase.h"

#define NETWORKSYSTEM_INTERFACE_VERSION "NetworkSystemVersion001"

/// <summary>
/// Main interface for low-level networking (packet sending).
/// </summary>
abstract_class INetworkSystem : public IAppSystem
{
   public:
    static uint GetHeaderBytes()
    {
        Assert( ( NETWORK_MESSAGE_HEADER_BITS % 8 ) == 0 );

        // Pad with zeros to byte align the header
        int headerBytes = ( NETWORK_MESSAGE_HEADER_BITS / 8 );
        headerBytes += ( ( headerBytes % 4 ) == 0 ) ? 0 : 4 - ( headerBytes % 4 );

        return headerBytes;
    }

    virtual void RegisterGroupHandler( NETWORK_MESSAGE_GROUP::TYPE group, INetworkGroupHandler * pHandler ) = 0;
    virtual void UnregisterNetworkHandler( NETWORK_MESSAGE_GROUP::TYPE group, INetworkGroupHandler * pHandler ) = 0;
    virtual void UnregisterAllNetworkHandlers() = 0;

    virtual bool StartServer( unsigned short serverPort = NETWORKSYSTEM_DEFAULT_SERVER_PORT ) = 0;
    virtual void ShutdownServer() = 0;

    virtual bool StartClient() = 0;
    virtual void ShutdownClient() = 0;

    virtual bool AcceptClients( CUtlVector< IConnectedClient* > & newClients ) = 0;
    virtual void Tick( void ) = 0;
    virtual bool ProcessMessage( IConnectedClient * client, CNetPacket * packet ) = 0;
    virtual bool ProcessControlMessage( int messageCommand, bf_read& buffer ) = 0;

    virtual INetworkPeerBase* ConnectClientToServer( const char* serverIp, int serverPort = NETWORKSYSTEM_DEFAULT_SERVER_PORT ) = 0;
    virtual void DisconnectClientFromServer() = 0;
    virtual INetworkPeerBase* GetServerPeer( void ) = 0;

    virtual void SendClientToServerMessage( INetworkMessage * pMessage ) = 0;
    virtual void BroadcastServerToClientMessage( INetworkMessage * pMessage ) = 0;
    virtual void SendServerToClientMessage( INetworkMessage * pMessage, IConnectedClient * client ) = 0;

    virtual INetworkGroupHandler* FindNetworkGroupHandler( NETWORK_MESSAGE_GROUP::TYPE group ) = 0;
};

#endif  // INETWORKSYSTEM_H
