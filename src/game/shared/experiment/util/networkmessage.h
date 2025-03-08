#ifndef INETWORKMESSAGE_H
#define INETWORKMESSAGE_H

#ifdef _WIN32
#pragma once
#endif

#include <inetchannelinfo.h>
#include <inetmessage.h>
#include <strtools.h>

#ifndef WITH_ENGINE_PATCHES
static_assert( false, "This file should only be included when WITH_ENGINE_PATCHES is defined" );
#endif

// Should match engine since that reads it, so we can't increase it sadly :/
#define NETWORK_MESSAGE_TYPE_BITS 6

// TODO: What's a good value for this?
#define NETWORK_MAX_MESSAGE_LENGTH 64000

class CBaseNetMessage : public INetMessage
{
   public:
    CBaseNetMessage();

    virtual ~CBaseNetMessage(){};

    virtual void SetNetChannel( INetChannel* netchan );
    virtual INetChannel* GetNetChannel( void ) const;
    virtual void SetReliable( bool state );
    virtual bool IsReliable( void ) const;

    // TODO: Implement these in the derived classes
    // virtual bool Process( void ) = 0;   // should process this message, return true if it succeeded
    // virtual bool ReadFromBuffer( bf_read &buffer ) = 0;  // returns true if parsing was OK
    // virtual bool WriteToBuffer( bf_write &buffer ) = 0;  // returns true if writing was OK

    // These identify the net message, so the correct 'Process' function can be called on arrival
    // Use the IMPLEMENT_NET_MESSAGE macro to implement these
    // virtual int GetType( void ) const = 0;          // returns module specific header tag eg svc_serverinfo
    // virtual int GetGroup( void ) const = 0;         // returns net message group of this message
    // virtual const char *GetName( void ) const = 0;  // returns network message name, eg "svc_serverinfo"

    virtual const char* ToString( void ) const;  // returns a human readable string about message content

    virtual size_t GetSize() const;

   protected:
    INetChannel* m_pNetChannel;
    bool m_bReliable;
    size_t m_ExpectedSize;
};

// Macro to ensure a NetMessage class has the correct type, group, and name
#define IMPLEMENT_NET_MESSAGE( group, type, name ) \
    virtual int GetGroup( void ) const             \
    {                                              \
        return group;                              \
    }                                              \
    virtual int GetType( void ) const              \
    {                                              \
        return type;                               \
    }                                              \
    virtual const char* GetName( void ) const      \
    {                                              \
        return name;                               \
    }

// Dynamic message where you can set the type id, filling it with
// data from a buffer
class CDynamicWriteNetworkMessage : public CBaseNetMessage
{
    // I believe 33 to be the highest type used in the engine
    IMPLEMENT_NET_MESSAGE( INetChannelInfo::GENERIC, 33 + 1, "LuaDynamic" );

   public:
    CDynamicWriteNetworkMessage( int messageTypeId = -1 );
    virtual ~CDynamicWriteNetworkMessage();

    virtual void SetBuffer( const char* buffer, int bufferLength );
    virtual const char* GetData() const;

    virtual bool Process( void ) OVERRIDE;
    virtual bool ReadFromBuffer( bf_read& buffer ) OVERRIDE;
    virtual bool WriteToBuffer( bf_write& buffer ) OVERRIDE;

    void SetSender( int playerIndex );

   protected:
    const char* m_pBuffer;
    int m_nBufferLength;
    int m_iTypeId;
    int m_iSender;
};

#endif  // INETWORKMESSAGE_H
