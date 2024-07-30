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
}

// Copies the hostname and mapname strings to buffers that will persist
void CGameInfoStore::FireGameEvent( IGameEvent *event )
{
    const char *eventType = event->GetName();

    if ( Q_strcmp( eventType, "server_spawn" ) == 0 )
    {
        const char *pszServerName = event->GetString( "hostname" );

        Q_strncpy( m_pszServerName, pszServerName, sizeof( m_pszServerName ) );
    }
    else if ( Q_strcmp( eventType, "game_newmap" ) == 0 )
    {
        const char *pzMapName = event->GetString( "mapname" );

        Q_strncpy( m_pszMapName, pzMapName, sizeof( m_pszMapName ) );
    }
}
