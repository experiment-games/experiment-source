#ifndef INETWORKGROUPHANDLER_H
#define INETWORKGROUPHANDLER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/dbg.h"

#include "inetworkmessage.h"

// Forward declarations
class bf_read;
class bf_write;
class INetMessage;

abstract_class INetworkGroupHandler
{
   public:
    virtual void HandleReadingMessage( unsigned int messageTypeId, bf_read &buffer, IConnectedClient *client ) = 0;
};

#endif  // INETWORKGROUPHANDLER_H
