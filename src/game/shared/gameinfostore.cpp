//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tier3/tier3.h"
#include <vgui/ILocalize.h>
#include "gameinfostore.h"

CGameInfoStore *g_pGameInfoStore;

CGameInfoStore::CGameInfoStore()
{
    ListenForGameEvent( "game_newmap" );
#ifdef CLIENT_DLL
    ListenForGameEvent( "client_beginconnect" );
#else
    ListenForGameEvent( "server_spawn" );
    ListenForGameEvent( "player_connect" );
#endif
}

CGameInfoStore::~CGameInfoStore()
{
    m_mapPlayerIndexToAddress.Purge();
}

void CGameInfoStore::FireGameEvent( IGameEvent *event )
{
    const char *eventType = event->GetName();

    if ( Q_strcmp( eventType, "client_beginconnect" ) == 0 )
    {
        // client_beginconnect gives us the real ip and port on the client (server_spawn wont)
        const char *serverAddress = event->GetString( "address" );

        // Get the ip form before serverAddress colon
        char serverIpAsString[32];
        Q_strncpy( serverIpAsString, serverAddress, sizeof( serverIpAsString ) );
        char *colon = Q_strstr( serverIpAsString, ":" );
        if ( colon )
        {
            *colon = '\0';
        }

        m_iServerIp = event->GetInt( "ip" );
        m_iServerPort = event->GetInt( "port" );

        Q_strncpy( m_pszServerIpAsString, serverIpAsString, sizeof( m_pszServerIpAsString ) );
        Q_strncpy( m_pszServerAddress, serverAddress, sizeof( m_pszServerAddress ) );

        return;
    }

    if ( Q_strcmp( eventType, "server_spawn" ) == 0 )
    {
        const char *serverName = event->GetString( "hostname" );
        const char *serverAddress = event->GetString( "address" );

        // Get the ip form before serverAddress colon
        char serverIpAsString[32];
        Q_strncpy( serverIpAsString, serverAddress, sizeof( serverIpAsString ) );
        char *colon = Q_strstr( serverIpAsString, ":" );
        if ( colon )
        {
            *colon = '\0';
        }

        m_iServerIp = event->GetInt( "ip" );
        m_iServerPort = event->GetInt( "port" );
        m_bIsDedicatedServer = event->GetBool( "dedicated" );

        Q_strncpy( m_pszServerIpAsString, serverIpAsString, sizeof( m_pszServerIpAsString ) );
        Q_strncpy( m_pszServerAddress, serverAddress, sizeof( m_pszServerAddress ) );
        Q_strncpy( m_pszServerName, serverName, sizeof( m_pszServerName ) );

        return;
    }

    if ( Q_strcmp( eventType, "game_newmap" ) == 0 )
    {
        const char *pzMapName = event->GetString( "mapname" );

        Q_strncpy( m_pszMapName, pzMapName, sizeof( m_pszMapName ) );

        return;
    }

    if ( Q_strcmp( eventType, "player_connect" ) == 0 )
    {
        int playerIndex = event->GetInt( "userid" );
        const char *pszPlayerAddress = event->GetString( "address" );

        // copy the player's address to the map (in a buffer that will persist)
        char *pszPlayerAddressCopy = new char[strlen( pszPlayerAddress ) + 1];
        Q_strncpy( pszPlayerAddressCopy, pszPlayerAddress, strlen( pszPlayerAddress ) + 1 );

        // TODO: Clean this up on disconnect
        m_mapPlayerIndexToAddress[pszPlayerAddressCopy] = playerIndex;

        return;
    }
}

CBasePlayer *CGameInfoStore::GetPlayerByAddress( const char *pszAddress )
{
    int mapIndex = m_mapPlayerIndexToAddress.Find( pszAddress );

    if ( mapIndex == m_mapPlayerIndexToAddress.InvalidIndex() )
    {
        return NULL;
    }

    return UTIL_PlayerByUserId( m_mapPlayerIndexToAddress[mapIndex] );
}

bool CGameInfoStore::GetPlayerAddress( CBasePlayer *pPlayer, char *pszAddress, int nMaxLen )
{
    int nCount = m_mapPlayerIndexToAddress.GetNumStrings();

    for ( int i = 0; i < nCount; ++i )
    {
        if ( m_mapPlayerIndexToAddress[i] == pPlayer->GetUserID() )
        {
            Q_strncpy( pszAddress, m_mapPlayerIndexToAddress.String( i ), nMaxLen );
            return true;
        }
    }

    return false;
}
