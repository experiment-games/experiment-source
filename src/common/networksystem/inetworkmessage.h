#ifndef INETWORKMESSAGE_H
#define INETWORKMESSAGE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/dbg.h"
#include "tier1/bitbuf.h"
#include "tier1/utlstring.h"

// Forward declarations
class INetMessage;

abstract_class INetworkMessage
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

    virtual void SetReliable( bool state ) = 0;
    virtual bool IsReliable( void ) const = 0;

    virtual const char *GetData() = 0;
    virtual ULONG GetSize() = 0;

    virtual int GetGroup( void ) const = 0;
    virtual int GetMessageTypeId( void ) const = 0;

    virtual const char *ToString( void ) const = 0;

    virtual void Release() = 0;

   protected:
    virtual ~INetworkMessage(){};
};

// Basic empty network message that can be extended
class CNetworkMessage : public INetworkMessage
{
   public:
    CNetworkMessage()
    {
        m_bReliable = true;
    }

    virtual ~CNetworkMessage(){};

    virtual void Release()
    {
        delete this;
    }

    virtual void SetReliable( bool state )
    {
        m_bReliable = state;
    }

    virtual bool IsReliable() const
    {
        return m_bReliable;
    }

   protected:
    bool m_bReliable;
};

// Dynamic message where you can set the group and type id, filling it with
// data from a buffer
class CDynamicWriteNetworkMessage : public CNetworkMessage
{
   public:
    CDynamicWriteNetworkMessage( int group, int typeId )
    {
        m_nGroup = group;
        m_nTypeId = typeId;
        m_pBuffer = NULL;
    }

    virtual ~CDynamicWriteNetworkMessage()
    {
        if ( m_pBuffer )
        {
            free( ( void * )m_pBuffer );
        }
    };

    virtual int GetGroup() const
    {
        return m_nGroup;
    }

    virtual int GetMessageTypeId() const
    {
        return m_nTypeId;
    }

    virtual void SetBuffer( const char *buffer, int bufferLength )
    {
        int headerBytes = GetHeaderBytes();

        m_nBufferLength = bufferLength + headerBytes;
        m_pBuffer = ( const char * )malloc( m_nBufferLength );

        char *headerBuffer = ( char * )malloc( headerBytes );
        bf_write header( "CDynamicWriteNetworkMessage::WriteHeader", headerBuffer, headerBytes );
        header.WriteUBitLong( GetGroup(), NETWORK_MESSAGE_GROUP_BITS );
        header.WriteUBitLong( GetMessageTypeId(), NETWORK_MESSAGE_INDEX_BITS );

        memcpy( ( void * )m_pBuffer, headerBuffer, headerBytes );
        memcpy( ( void * )( m_pBuffer + headerBytes ), buffer, bufferLength );

        free( headerBuffer );
    }

    virtual const char *GetData()
    {
        return m_pBuffer;
    }

    virtual ULONG GetSize()
    {
        return m_nBufferLength;
    }

    virtual const char *ToString() const
    {
        return "CDynamicNetworkMessage";
    }

   protected:
    int m_nGroup;
    int m_nTypeId;
    const char *m_pBuffer;
    int m_nBufferLength;
};

abstract_class IConnectedClient
{
   public:
    virtual void SendNetMessage( INetworkMessage * pMessage ) = 0;

    netadr_t m_RemoteAddress;
};

#if _DEBUG
void DEBUG_MESSAGE_DATA( const char *prefix, unsigned char *data, int size );
#endif

#endif  // INETWORKMESSAGE_H
