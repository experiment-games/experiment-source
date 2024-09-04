#ifndef NETPACKET_H
#define NETPACKET_H
#ifdef _WIN32
#pragma once
#endif

#include "tier1/bitbuf.h"
#include "tier1/netadr.h"
#include "tier1/utlvector.h"
#include "tier1/mempool.h"

#include "networksystem/isockets.h"
#include "networksystem/inetworksystem.h"

class CUtlBuffer;
class CNetPacket;

class CNetPacket
{
    DECLARE_FIXEDSIZE_ALLOCATOR( CNetPacket );

   public:
    CNetPacket();
    ~CNetPacket();

    void AddRef();
    void Release();

   public:
    netadr_t m_From;
    ISocket *m_pSource;
    float m_flReceivedTime;
    unsigned char *m_pData;
    bf_read m_Message;

    int m_nSizeInBytes;
    int m_nCurrentSizeInBytes;

   private:
    int m_nRefCount;
};

#endif  // NETPACKET_H
