#ifndef INETWORKMESSAGE_H
#define INETWORKMESSAGE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/dbg.h"
#include "tier1/bitbuf.h"
#include "tier1/utlstring.h"
#include "tier1/netadr.h"

// Forward declarations
class INetMessage;

abstract_class INetworkMessage
{
   public:
    virtual void SetReliable( bool state ) = 0;
    virtual bool IsReliable( void ) const = 0;

    virtual const char *GetData() = 0;
    virtual unsigned long GetSize() = 0;

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
        m_nBufferLength = bufferLength;
        m_pBuffer = ( const char * )malloc( bufferLength );
        Q_memcpy( ( void * )m_pBuffer, buffer, bufferLength );
    }

    virtual const char *GetData()
    {
        return m_pBuffer;
    }

    virtual unsigned long GetSize()
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
    virtual bool SendNetMessage( INetworkMessage * pMessage ) = 0;

    const char *GetRemoteAddress() const
    {
        return m_RemoteAddress.ToString();
    }

    bool IsLoopback() const
    {
        return m_RemoteAddress.IsLoopback();
    }

    netadr_t m_RemoteAddress;
};

#if _DEBUG
void DEBUG_MESSAGE_DATA( const char *prefix, unsigned char *data, int size );
#endif

#endif  // INETWORKMESSAGE_H
