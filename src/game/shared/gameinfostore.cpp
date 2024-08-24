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
    ListenForGameEvent( "server_spawn" );
    ListenForGameEvent( "game_newmap" );
    ListenForGameEvent( "player_connect" );
}

CGameInfoStore::~CGameInfoStore()
{
    m_mapPlayerIndexToAddress.Purge();
}

// Copies the hostname and mapname strings to buffers that will persist
void CGameInfoStore::FireGameEvent( IGameEvent *event )
{
    const char *eventType = event->GetName();

    if ( Q_strcmp( eventType, "server_spawn" ) == 0 )
    {
        const char *pszServerName = event->GetString( "hostname" );
        const char *pszServerAddress = event->GetString( "address" );

        Q_strncpy( m_pszServerAddress, pszServerAddress, sizeof( m_pszServerAddress ) );
        Q_strncpy( m_pszServerName, pszServerName, sizeof( m_pszServerName ) );
        m_bIsDedicatedServer = event->GetBool( "dedicated" );
    }
    else if ( Q_strcmp( eventType, "game_newmap" ) == 0 )
    {
        const char *pzMapName = event->GetString( "mapname" );

        Q_strncpy( m_pszMapName, pzMapName, sizeof( m_pszMapName ) );
    }
    else if ( Q_strcmp( eventType, "player_connect" ) == 0 )
    {
        int playerIndex = event->GetInt( "userid" );
        const char *pszPlayerAddress = event->GetString( "address" );

        // copy the player's address to the map (in a buffer that will persist)
        char *pszPlayerAddressCopy = new char[strlen( pszPlayerAddress ) + 1];
        Q_strncpy( pszPlayerAddressCopy, pszPlayerAddress, strlen( pszPlayerAddress ) + 1 );

        m_mapPlayerIndexToAddress[ pszPlayerAddressCopy ] = playerIndex;
    }
}
