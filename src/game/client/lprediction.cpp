#include "cbase.h"
#include "prediction.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Predictions )

LUA_BINDING_BEGIN( Predictions, GetIdealPitch, "library", "Get the ideal pitch." )
{
    lua_pushnumber( L, prediction->GetIdealPitch() );
    return 1;
}
LUA_BINDING_END( "number", "The ideal pitch." )

LUA_BINDING_BEGIN( Predictions, GetIncomingPacketNumber, "library", "Get the incoming packet number." )
{
    lua_pushinteger( L, prediction->GetIncomingPacketNumber() );
    return 1;
}
LUA_BINDING_END( "integer", "The incoming packet number." )

LUA_BINDING_BEGIN( Predictions, GetLocalViewAngles, "library", "Get the local view angles." )
{
    QAngle ang;
    prediction->GetLocalViewAngles( ang );
    lua_pushangle( L, ang );
    return 1;
}
LUA_BINDING_END( "Angle", "The local view angles." )

LUA_BINDING_BEGIN( Predictions, GetViewAngles, "library", "Get the view angles." )
{
    QAngle ang;
    prediction->GetViewAngles( ang );
    lua_pushangle( L, ang );
    return 1;
}
LUA_BINDING_END( "Angle", "The view angles." )

LUA_BINDING_BEGIN( Predictions, GetViewOrigin, "library", "Get the view origin." )
{
    Vector org;
    prediction->GetViewOrigin( org );
    lua_pushvector( L, org );
    return 1;
}
LUA_BINDING_END( "Vector", "The view origin." )

LUA_BINDING_BEGIN( Predictions, InPrediction, "library", "Check if the game is in prediction." )
{
    lua_pushboolean( L, prediction->InPrediction() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the game is in prediction." )

LUA_BINDING_BEGIN( Predictions, IsFirstTimePredicted, "library", "Check if the game is in the first time predicted." )
{
    lua_pushboolean( L, prediction->IsFirstTimePredicted() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the game is in the first time predicted." )

LUA_BINDING_BEGIN( Predictions, OnReceivedUncompressedPacket, "library", "Call when the game receives an uncompressed packet." )
{
    prediction->OnReceivedUncompressedPacket();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Predictions, PostEntityPacketReceived, "library", "Call after the game receives an entity packet." )
{
    prediction->PostEntityPacketReceived();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Predictions, PostNetworkDataReceived, "library", "Call after the game receives network data." )
{
    int commandsAcknowledged = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "commandsAcknowledged" );
    prediction->PostNetworkDataReceived( commandsAcknowledged );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Predictions, PreEntityPacketReceived, "library", "Call before the game receives an entity packet." )
{
    int commandsAcknowledged = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "commandsAcknowledged" );
    int currentWorldUpdate = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "currentWorldUpdate" );
    prediction->PreEntityPacketReceived( commandsAcknowledged, currentWorldUpdate );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Predictions, SetLocalViewAngles, "library", "Set the local view angles." )
{
    QAngle ang = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angles" );
    prediction->SetLocalViewAngles( ang );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Predictions, SetViewAngles, "library", "Set the view angles." )
{
    QAngle ang = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angles" );
    prediction->SetViewAngles( ang );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Predictions, SetViewOrigin, "library", "Set the view origin." )
{
    Vector org = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );
    prediction->SetViewOrigin( org );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Predictions, Update, "library", "Update the prediction." )
{
    int startFrame = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "startFrame" );
    bool validFrame = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "validFrame" );
    int incomingAcknowledged = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "incomingAcknowledged" );
    int outgoingCommand = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "outgoingCommand" );
    prediction->Update( startFrame, validFrame, incomingAcknowledged, outgoingCommand );
    return 0;
}
LUA_BINDING_END()

/*
** Open prediction library
*/
LUALIB_API int luaopen_prediction( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Predictions );
    return 1;
}
