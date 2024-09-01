#ifndef INETWORKPEERBASE_H
#define INETWORKPEERBASE_H

#include "networksystem/isockets.h"

abstract_class INetworkPeerBase
{
   public:
    virtual void Shutdown() = 0;

    virtual void Update() = 0;
    virtual void SendPacket( INetworkMessage * packet ) = 0;

    NETWORK_CONNECTION_STATE::TYPE ConnectionState;

    ITcpSocket *m_pStreamSocket;
};

#define NETWORK_PEER_DECLARATION( TcpImplementationClass )                  \
   public:                                                                  \
    void Shutdown() override;                                               \
                                                                            \
    void Update() override;                                                 \
    void SendPacket( INetworkMessage *packet ) override;                    \
                                                                            \
    TcpImplementationClass *GetStreamSocket() const                         \
    {                                                                       \
        return dynamic_cast< TcpImplementationClass * >( m_pStreamSocket ); \
    }

#endif  // INETWORKPEERBASE_H
