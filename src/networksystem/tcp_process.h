#ifndef TCP_PROCESS_H
#define TCP_PROCESS_H
#ifdef _WIN32
#pragma once
#endif

class CNetPacket;

bool TcpReceiveNew( CTcpClientSocket *pSocket, CNetPacket *pPacket );
bool TcpReceiveContinue( CTcpClientSocket *pSocket, CNetPacket *pPacket );
bool TcpGetOrContinuePackets( CTcpClientSocket *pSocket, CUtlVector< CNetPacket * > &newPackets );

#endif  // TCP_PROCESS_H
