#ifndef ISOCKETS_H
#define ISOCKETS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier1/netadr.h"
#include "tier0/basetypes.h"
#include "tier1/utlvector.h"

class CUtlBuffer;
class CNetPacket;

#define NETWORK_MESSAGE_GROUP_BITS 4  // Max 16 groups
#define NETWORK_MESSAGE_INDEX_BITS 12 // Max 4096 messages per group

// Ensure the header bits are divisible by 8
#define NETWORK_MESSAGE_HEADER_BITS ( NETWORK_MESSAGE_GROUP_BITS + NETWORK_MESSAGE_INDEX_BITS )

// The size of messages is sent first, so we know when a 'packet' is complete
#define NETWORK_MESSAGE_LENGTH_DATATYPE short  // Max 64KB
#define NET_MAX_PAYLOAD 4000
#define NET_MAX_MESSAGE 4096

enum
{
    NETWORKSYSTEM_DEFAULT_SERVER_PORT = 27001,
};

typedef int ConnectionHandle_t;

namespace NETWORK_CONNECTION_STATE
{
enum TYPE
{
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTION_FAILED,
    CONNECTED,
};
};

abstract_class ISocket
{
   public:
    bool IsValid() const;
};

abstract_class ISendData
{
   public:
    virtual bool Send( const char *data, size_t dataLength ) = 0;
};

abstract_class ITcpSocket : public ISocket
{
   public:
    virtual bool IsServer() const = 0;
};

namespace NETWORK_DLL
{
enum TYPE
{
    CLIENT = 1,
    GAME = 2,
};
};

namespace NETWORK_MESSAGE_GROUP
{
enum TYPE
{
    INTERNAL = 0,

    CLIENT = 1,
    SERVER = 2,
    GAME = 3,

    PLAYER = 4,
    ENTITY = 5,
    WEAPON = 6,
    NPC = 7,

    PHYSICS = 8,
    LOGIC = 9,
    MECHANICS = 10,
    META = 11,
    SCRIPT = 12,

    COUNT
};
};

namespace NETWORK_MESSAGE_INTERNAL
{
enum TYPE
{
    NOP = 0,
    DISCONNECTED = 1,

    COUNT
};
};

#endif  // ISOCKETS_H
