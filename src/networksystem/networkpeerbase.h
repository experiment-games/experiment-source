#ifndef NETWORKPEERBASE_H
#define NETWORKPEERBASE_H

#include "networksystem/isockets.h"
#include "netchannel.h"
#include "networksystem/inetworksystem.h"

#include "tcp_sockets.h"

abstract_class CNetworkPeerBase : public IConnectionlessPacketHandler, public INetworkMessageHandler, public ILookupChannel
{
   public:
    virtual bool Init( int nServerPort ) = 0;
    virtual void Shutdown() = 0;

    // inherited from IConnectionlessPacketHandler interface
    virtual bool ProcessHandshakePacket( CNetPacket * packet ) = 0;

    // inherited from INetworkMessageHandler interface
    virtual void OnConnectionClosing( INetServerChannel * channel, char const *reason ) = 0;
    virtual void OnConnectionStarted( INetServerChannel * channel ) = 0;

    virtual void OnPacketStarted( int inseq, int outseq ) = 0;
    virtual void OnPacketFinished() = 0;

    // inherited from ILookupChannel interface
    virtual INetServerChannel *FindNetChannel( const netadr_t &from ) = 0;

    virtual void ReadPackets() = 0;

    CUdpSocket *m_pSocket;
    ITcpSocket *m_pStreamSocket;
};

#define NETWORK_PEER_DECLARATION( TcpImplementationClass )                               \
   public:                                                                               \
    bool Init( int nServerPort ) override;                                               \
    void Shutdown() override;                                                            \
                                                                                         \
    bool ProcessHandshakePacket( CNetPacket *packet ) override;                          \
                                                                                         \
    void OnConnectionClosing( INetServerChannel *channel, char const *reason ) override; \
    void OnConnectionStarted( INetServerChannel *channel ) override;                     \
                                                                                         \
    void OnPacketStarted( int inseq, int outseq ) override;                              \
    void OnPacketFinished() override;                                                    \
                                                                                         \
    INetServerChannel *FindNetChannel( const netadr_t &from ) override;                  \
                                                                                         \
    void ReadPackets() override;                                                         \
                                                                                         \
    TcpImplementationClass *GetStreamSocket() const                                      \
    {                                                                                    \
        return dynamic_cast< TcpImplementationClass * >( m_pStreamSocket );              \
    }

#endif  // NETWORKPEERBASE_H
