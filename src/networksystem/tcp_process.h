#ifndef TCP_PROCESS_H
#define TCP_PROCESS_H
#ifdef _WIN32
#pragma once
#endif

class CNetPacket;

bool TcpReceive( CTcpClientSocket *pSocket, CNetPacket *pPacket );
CNetPacket *TcpGetPacket( CTcpClientSocket *pSocket );

#endif  // TCP_PROCESS_H
