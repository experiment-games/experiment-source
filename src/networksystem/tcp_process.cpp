#include "tier1/utlbuffer.h"

#include "tcp_sockets.h"
#include "netpacket.h"
#include "networksystem.h"

bool TcpReceiveNew( CTcpClientSocket *pSocket, CNetPacket *pPacket )
{
    Assert( pPacket );

    int messageLength = pSocket->ReceiveMessageLength();
    pPacket->m_nSizeInBytes = messageLength;

    CUtlBuffer buffer( pPacket->m_pData, messageLength );

    if ( !pSocket->Receive( buffer ) )
    {
        return false;
    }

    pPacket->m_nCurrentSizeInBytes = buffer.TellMaxPut();
    return true;
}

bool TcpReceiveContinue( CTcpClientSocket *pSocket, CNetPacket *pPacket )
{
    Assert( pPacket );

    const void *bufferPastAlreadyRead = pPacket->m_pData + pPacket->m_nCurrentSizeInBytes;
    int remainingBytes = pPacket->m_nSizeInBytes - pPacket->m_nCurrentSizeInBytes;

    CUtlBuffer buffer( bufferPastAlreadyRead, remainingBytes );

    if ( !pSocket->Receive( buffer ) )
    {
        return false;
    }

    pPacket->m_nCurrentSizeInBytes += buffer.TellMaxPut();
    return true;
}

bool TcpGetOrContinuePackets( CTcpClientSocket *socket, CUtlVector< CNetPacket * > &newPackets )
{
    int packetsCount = 0;

    while ( socket->HasDataInSocket() )
    {
        CNetPacket *pPacket;

        if ( socket->m_CurrentMessageReceiving )
        {
            pPacket = socket->m_CurrentMessageReceiving;

            if ( !TcpReceiveContinue( socket, pPacket ) )
            {
                Assert( 0 );
                delete pPacket;
                return NULL;
            }
        }
        else
        {
            pPacket = new CNetPacket;
            pPacket->m_From = socket->m_SocketIP;
            pPacket->m_flReceivedTime = Plat_FloatTime();
            pPacket->m_pSource = socket;
            pPacket->m_nSizeInBytes = 0;
            pPacket->m_Message.SetDebugName( "inpacket.message" );

            if ( !TcpReceiveNew( socket, pPacket ) )
            {
                Assert( 0 );
                delete pPacket;
                return NULL;
            }

            Assert( pPacket->m_nSizeInBytes );

            pPacket->m_Message.StartReading( pPacket->m_pData, pPacket->m_nSizeInBytes );
        }

        if ( pPacket->m_Message.IsOverflowed() )
        {
            Assert( 0 );
            delete pPacket;
            return false;
        }

        if ( pPacket->m_nSizeInBytes != pPacket->m_nCurrentSizeInBytes )
        {
            socket->m_CurrentMessageReceiving = pPacket;

            // We're not done yet, wait for more data to come in and finish the packet
            return false;
        }

        // We've got a full packet now, let's append it
        newPackets.AddToTail( pPacket );
        packetsCount++;
    }

    if ( packetsCount == 0 )
    {
        return false;
    }

    return true;
}
