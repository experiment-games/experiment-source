#ifndef TCP_SOCKETS_H
#define TCP_SOCKETS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier1/netadr.h"
#include "tier0/basetypes.h"
#include "tier1/utlvector.h"

#include "networksystem/isockets.h"

class CUtlBuffer;
class CNetPacket;

class CTcpClientSocket : public ITcpSocket, public ISendData
{
   public:
    CTcpClientSocket();
    ~CTcpClientSocket();

    bool Connect( netadr_t remote );
    void Disconnect();

    bool ReceiveFrom( netadr_t &remote, CUtlBuffer &data );
    virtual bool Send( const char *data, size_t dataLength );
    bool ReadPackets( CUtlVector< CNetPacket * > &newPackets );

    bool IsServer() const
    {
        return false;
    }

    bool IsValid() const
    {
        return m_Socket != 0;
    }

    unsigned int m_Socket;
    netadr_t m_SocketIP;
};

class CTcpServerSocket : public ITcpSocket
{
   public:
    CTcpServerSocket();
    ~CTcpServerSocket();

    bool Init( unsigned short bindToPort );
    void Shutdown();

    bool AcceptClients( CUtlVector< CTcpClientSocket * > &newClients );
    bool ReadPackets( CUtlVector< CNetPacket * > &newPackets );

    bool IsServer() const
    {
        return true;
    }

    bool IsValid() const
    {
        return m_Socket != 0;
    }

    CUtlVector< CTcpClientSocket * > &GetClients()
    {
        return m_Clients;
    }

    unsigned int m_Socket;

   protected:
    bool CreateSocket( void );

    unsigned short m_Port;
    CUtlVector< CTcpClientSocket * > m_Clients;
};

#endif  // TCP_SOCKETS_H
