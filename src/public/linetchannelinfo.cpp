#include "cbase.h"
#include "inetchannelinfo.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "linetchannelinfo.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_INetChannelInfo *lua_tonetchannel( lua_State *L, int idx )
{
    lua_INetChannelInfo *pNetChannel = *( lua_INetChannelInfo ** )luaL_checkudata( L, idx, LUA_NETCHANNELINFOMETANAME );
    return pNetChannel;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushnetchannel( lua_State *L, INetChannelInfo *netchannel )
{
    lua_INetChannelInfo **ppNetChannel = ( lua_INetChannelInfo ** )lua_newuserdata( L, sizeof( netchannel ) );
    *ppNetChannel = netchannel;
    LUA_SAFE_SET_METATABLE( L, LUA_NETCHANNELINFOMETANAME );
}

LUALIB_API lua_INetChannelInfo *luaL_checknetchannel( lua_State *L, int narg )
{
    lua_INetChannelInfo *d = *( lua_INetChannelInfo ** )luaL_checkudata( L, narg, LUA_NETCHANNELINFOMETANAME );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "INetChannelInfo expected, got NULL" );
    return d;
}

LUA_REGISTRATION_INIT( NetChannelInfo );

LUA_BINDING_BEGIN( NetChannelInfo, GetAddress, "class", "Returns the address of the remote connection." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushstring( L, netChannel->GetAddress() );
    return 1;
}
LUA_BINDING_END( "string", "The address of the remote connection." )

LUA_BINDING_BEGIN( NetChannelInfo, GetAverageChoke, "class", "Returns the average choke for the specified number of packets." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    lua_pushnumber( L, netChannel->GetAvgChoke( flow ) );
    return 1;
}
LUA_BINDING_END( "number", "The average choke for the specified number of packets." )

LUA_BINDING_BEGIN( NetChannelInfo, GetAverageData, "class", "Returns the average data for the specified number of packets." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    lua_pushnumber( L, netChannel->GetAvgData( flow ) );
    return 1;
}
LUA_BINDING_END( "number", "The average data for the specified number of packets." )

LUA_BINDING_BEGIN( NetChannelInfo, GetAverageLatency, "class", "Returns the average latency for the specified number of packets." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    lua_pushnumber( L, netChannel->GetAvgLatency( flow ) );
    return 1;
}
LUA_BINDING_END( "number", "The average latency for the specified number of packets." )

LUA_BINDING_BEGIN( NetChannelInfo, GetAverageLoss, "class", "Returns the average loss for the specified number of packets." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    lua_pushnumber( L, netChannel->GetAvgLoss( flow ) );
    return 1;
}
LUA_BINDING_END( "number", "The average loss for the specified number of packets." )

LUA_BINDING_BEGIN( NetChannelInfo, GetAveragePackets, "class", "Returns the average packets for the specified number of packets." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    lua_pushnumber( L, netChannel->GetAvgPackets( flow ) );
    return 1;
}
LUA_BINDING_END( "number", "The average packets for the specified number of packets." )

LUA_BINDING_BEGIN( NetChannelInfo, GetBufferSize, "class", "Returns the size of the buffer." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushinteger( L, netChannel->GetBufferSize() );
    return 1;
}
LUA_BINDING_END( "integer", "The size of the buffer." )

LUA_BINDING_BEGIN( NetChannelInfo, GetCommandInterpolationAmount, "class", "Returns the command interpolation amount for the specified command number." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int commandNumber = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "commandNumber" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "flow" );
    lua_pushnumber( L, netChannel->GetCommandInterpolationAmount( commandNumber, flow ) );
    return 1;
}
LUA_BINDING_END( "number", "The command interpolation amount for the specified command number." )

LUA_BINDING_BEGIN( NetChannelInfo, GetDataRate, "class", "Returns the data rate." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushinteger( L, netChannel->GetDataRate() );
    return 1;
}
LUA_BINDING_END( "integer", "The data rate." )

LUA_BINDING_BEGIN( NetChannelInfo, GetLatency, "class", "Returns the latency for the specified flow." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    lua_pushnumber( L, netChannel->GetLatency( flow ) );
    return 1;
}
LUA_BINDING_END( "number", "The latency for the specified flow." )

LUA_BINDING_BEGIN( NetChannelInfo, GetName, "class", "Returns the name of the remote connection." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushstring( L, netChannel->GetName() );
    return 1;
}
LUA_BINDING_END( "string", "The name of the remote connection." )

LUA_BINDING_BEGIN( NetChannelInfo, GetPacketBytes, "class", "Returns the number of bytes in the specified packet." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    int frameNumber = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "frameNumber" );
    int group = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "group" );
    lua_pushinteger( L, netChannel->GetPacketBytes( flow, frameNumber, group ) );
    return 1;
}
LUA_BINDING_END( "integer", "The number of bytes in the specified packet." )

LUA_BINDING_BEGIN( NetChannelInfo, GetPacketResponseLatency, "class", "Returns the packet response latency for the specified packet and flow." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    int frameNumber = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "frameNumber" );
    int latencyMsecs, choke;
    netChannel->GetPacketResponseLatency( flow, frameNumber, &latencyMsecs, &choke );
    lua_pushinteger( L, latencyMsecs );
    lua_pushinteger( L, choke );
    return 2;
}
LUA_BINDING_END( "integer", "The packet response latency", "integer", "The choke." )

LUA_BINDING_BEGIN( NetChannelInfo, GetPacketTime, "class", "Returns the time of the specified packet and flow." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    int frameNumber = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "frameNumber" );
    lua_pushnumber( L, netChannel->GetPacketTime( flow, frameNumber ) );
    return 1;
}
LUA_BINDING_END( "number", "The time of the specified packet and flow." )

LUA_BINDING_BEGIN( NetChannelInfo, GetRemoteFramerate, "class", "Returns the remote frame rate." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    float frameTime, frameTimeStdDeviation;
    netChannel->GetRemoteFramerate( &frameTime, &frameTimeStdDeviation );
    lua_pushnumber( L, frameTime );
    lua_pushnumber( L, frameTimeStdDeviation );
    return 2;
}
LUA_BINDING_END( "number", "The remote frame rate.", "number", "The standard deviation of the remote frame rate." )

LUA_BINDING_BEGIN( NetChannelInfo, GetSequenceNumber, "class", "Returns the sequence number for the specified flow." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    lua_pushinteger( L, netChannel->GetSequenceNr( flow ) );
    return 1;
}
LUA_BINDING_END( "integer", "The sequence number for the specified flow." )

LUA_BINDING_BEGIN( NetChannelInfo, GetStreamProgress, "class", "Returns the stream progress for the specified flow." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    int received, total;
    lua_pushboolean( L, netChannel->GetStreamProgress( flow, &received, &total ) );
    lua_pushinteger( L, received );
    lua_pushinteger( L, total );
    return 3;
}
LUA_BINDING_END( "boolean", "If the stream progress was retrieved.", "integer", "The number of bytes received.", "integer", "The total number of bytes." )

LUA_BINDING_BEGIN( NetChannelInfo, GetTime, "class", "Returns the time." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushnumber( L, netChannel->GetTime() );
    return 1;
}
LUA_BINDING_END( "number", "The time." )

LUA_BINDING_BEGIN( NetChannelInfo, GetTimeConnected, "class", "Returns the time connected." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushnumber( L, netChannel->GetTimeConnected() );
    return 1;
}
LUA_BINDING_END( "number", "The time connected." )

LUA_BINDING_BEGIN( NetChannelInfo, GetTimeoutSeconds, "class", "Returns the timeout seconds." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushnumber( L, netChannel->GetTimeoutSeconds() );
    return 1;
}
LUA_BINDING_END( "number", "The timeout seconds." )

LUA_BINDING_BEGIN( NetChannelInfo, GetTimeSinceLastReceived, "class", "Returns the time since the last received packet." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushnumber( L, netChannel->GetTimeSinceLastReceived() );
    return 1;
}
LUA_BINDING_END( "number", "The time since the last received packet." )

LUA_BINDING_BEGIN( NetChannelInfo, GetTotalData, "class", "Returns the total data for the specified flow." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    lua_pushinteger( L, netChannel->GetTotalData( flow ) );
    return 1;
}
LUA_BINDING_END( "integer", "The total data for the specified flow." )

LUA_BINDING_BEGIN( NetChannelInfo, IsLoopback, "class", "Returns if the connection is loopback." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushboolean( L, netChannel->IsLoopback() );
    return 1;
}
LUA_BINDING_END( "boolean", "If the connection is loopback." )

LUA_BINDING_BEGIN( NetChannelInfo, IsPlayback, "class", "Returns if the connection is playback." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushboolean( L, netChannel->IsPlayback() );
    return 1;
}
LUA_BINDING_END( "boolean", "If the connection is playback." )

LUA_BINDING_BEGIN( NetChannelInfo, IsTimingOut, "class", "Returns if the connection is timing out." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    lua_pushboolean( L, netChannel->IsTimingOut() );
    return 1;
}
LUA_BINDING_END( "boolean", "If the connection is timing out." )

LUA_BINDING_BEGIN( NetChannelInfo, IsValidPacket, "class", "Returns if the packet is valid for the specified flow." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( luaL_checknetchannel, 1, "netChannel" );
    int flow = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flow" );
    int frameNumber = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "frameNumber" );
    lua_pushboolean( L, netChannel->IsValidPacket( flow, frameNumber ) );
    return 1;
}
LUA_BINDING_END( "boolean", "If the packet is valid for the specified flow." )

LUA_BINDING_BEGIN( NetChannelInfo, __tostring, "class", "Returns the string representation of the INetChannelInfo object." )
{
    INetChannelInfo *netChannel = LUA_BINDING_ARGUMENT( lua_tonetchannel, 1, "netChannel" );
    lua_pushfstring( L, "INetChannelInfo: %p", netChannel );
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the INetChannelInfo object." )

/*
** Open INetChannelInfo object
*/
LUALIB_API int luaopen_INetChannelInfo( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_NETCHANNELINFOMETANAME );

    LUA_REGISTRATION_COMMIT( NetChannelInfo );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_NETCHANNELINFOMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "NetChannelInfo" */
    return 1;
}
