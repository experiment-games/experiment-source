#include "tier1/utlbuffer.h"

#include "networksystem/isockets.h"
#include "netpacket.h"
#include "networksystem.h"

static CUtlMemoryPool s_PacketBufferAlloc( NET_MAX_MESSAGE, 8, CUtlMemoryPool::GROW_SLOW );
DEFINE_FIXEDSIZE_ALLOCATOR( CNetPacket, 32, CUtlMemoryPool::GROW_SLOW );

CNetPacket::CNetPacket()
{
    m_pData = reinterpret_cast< byte* >( s_PacketBufferAlloc.Alloc() );
    m_nRefCount = 1;

    m_nCurrentSizeInBytes = 0;
    m_nSizeInBytes = 0;
}

CNetPacket::~CNetPacket()
{
    s_PacketBufferAlloc.Free( m_pData );
}

void CNetPacket::AddRef()
{
    ++m_nRefCount;
}

void CNetPacket::Release()
{
    if ( --m_nRefCount <= 0 )
    {
        delete this;
    }
}
