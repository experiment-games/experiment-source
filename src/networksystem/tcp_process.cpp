#include "tier1/utlbuffer.h"

#include "tcp_sockets.h"
#include "netpacket.h"
#include "networksystem.h"

bool TcpReceive( CTcpClientSocket *pSocket, CNetPacket *pPacket )
{
    Assert( pPacket );

    netadr_t packetFrom;
    CUtlBuffer buffer( pPacket->m_pData, NET_MAX_MESSAGE );

    if ( pSocket->ReceiveFrom( packetFrom, buffer ) )
    {
        pPacket->m_From = packetFrom;
        pPacket->m_nSizeInBytes = buffer.TellPut();
        return true;
    }

    return false;
}

CNetPacket *TcpGetPacket( CTcpClientSocket *pSocket )
{
    CNetPacket *pPacket = new CNetPacket;

    pPacket->m_From.SetType( NA_IP );
    pPacket->m_From.Clear();
    pPacket->m_flReceivedTime = Plat_FloatTime();
    pPacket->m_pSource = pSocket;
    pPacket->m_nSizeInBytes = 0;
    pPacket->m_Message.SetDebugName( "inpacket.message" );

    if ( !TcpReceive( pSocket, pPacket ) )
    {
        delete pPacket;
        return NULL;
    }

    Assert( pPacket->m_nSizeInBytes );

    pPacket->m_Message.StartReading( pPacket->m_pData, pPacket->m_nSizeInBytes );
    return pPacket;
}
