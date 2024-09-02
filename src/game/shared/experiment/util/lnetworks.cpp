#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lnetwork.h"
#include "networkmanager.h"
#include "lbaseplayer_shared.h"
#include "gameinfostore.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Networks )

#ifdef GAME_DLL
LUA_BINDING_BEGIN( Networks, InternalSendToClients, "library", "Send a message to the specified clients, or all", "server" )
{
    int messageTypeId = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "messageTypeId" );
    bf_write &messageWriter = LUA_BINDING_ARGUMENT( lua_tobf_write, 2, "messageWriter" );

    // Build the message
    CDynamicWriteNetworkMessage *message = new CDynamicWriteNetworkMessage( NETWORK_MESSAGE_GROUP::SCRIPT, messageTypeId );

    message->SetBuffer( ( const char * )messageWriter.GetBasePointer(), messageWriter.GetNumBytesWritten() );

    // If no clients are probided, broadcast the message
    if ( lua_isnil( L, 3 ) )
    {
        g_pNetworkManager->BroadcastServerToClientMessage( message );
    }
    else if ( LUA_BINDING_ARGUMENT_NILLABLE( lua_istable, 3, "clientOrClients" ) )
    {
        lua_pushnil( L );

        while ( lua_next( L, 3 ) != 0 )
        {
            CBasePlayer *client = lua_toplayer( L, -1 );

            if ( client )
            {
                char pszAddress[32];

                if ( g_pGameInfoStore->GetPlayerAddress( client, pszAddress, sizeof( pszAddress ) ) )
                {
                    g_pNetworkManager->SendServerToClientMessage( message, pszAddress );
                }
                else
                {
                    Assert( 0 );  // TODO: warn? should this be allowed to happen?
                }
            }
            else
            {
                Assert( 0 );  // NULL player TODO: warn? should this be allowed to happen?
            }

            lua_pop( L, 1 );  // Remove the value, keep the key
        }
    }
    else if ( LUA_BINDING_ARGUMENT_NILLABLE( lua_toplayer, 3, "clientOrClients" ) )
    {
        CBasePlayer *client = lua_toplayer( L, 3 );

        if ( client )
        {
            char pszAddress[32];

            if ( g_pGameInfoStore->GetPlayerAddress( client, pszAddress, sizeof( pszAddress ) ) )
            {
                g_pNetworkManager->SendServerToClientMessage( message, pszAddress );
            }
            else
            {
                Assert( 0 );  // TODO: warn? should this be allowed to happen?
            }
        }
        else
        {
            Assert( 0 );  // NULL player TODO: warn? should this be allowed to happen?
        }
    }
    else
    {
        luaL_typeerror( L, 3, "nil, player or table of players" );
    }

    // Clean up
    delete message;

    return 0;
}
LUA_BINDING_END()
#endif

#ifdef CLIENT_DLL
LUA_BINDING_BEGIN( Networks, InternalSendToServer, "library", "Send a message to the server", "client" )
{
    int messageTypeId = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "messageTypeId" );
    bf_write &messageWriter = LUA_BINDING_ARGUMENT( lua_tobf_write, 2, "messageWriter" );

    // Build the message
    CDynamicWriteNetworkMessage *message = new CDynamicWriteNetworkMessage( NETWORK_MESSAGE_GROUP::SCRIPT, messageTypeId );

    message->SetBuffer( ( const char * )messageWriter.GetBasePointer(), messageWriter.GetNumBytesWritten() );

    g_pNetworkManager->SendClientToServerMessage( message );

    // Clean up
    delete message;

    return 0;
}
LUA_BINDING_END()
#endif


/*
** Open Networks library
*/
LUALIB_API int luaopen_Networks( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Networks );
    return 1;
}
