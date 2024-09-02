#ifndef CGAMEINFOSTORE_H
#define CGAMEINFOSTORE_H
#pragma once

#include "GameEventListener.h"
#include "utlstringmap.h"

class CGameInfoStore : public CGameEventListener
{
   public:
    CGameInfoStore();
    ~CGameInfoStore();

    // IGameEventListener interface:
    virtual void FireGameEvent( IGameEvent *event );

    virtual char *GetServerAddress()
    {
        return m_pszServerAddress;
    }

    virtual char *GetServerName()
    {
        return m_pszServerName;
    }

    virtual char *GetMapName()
    {
        return m_pszMapName;
    }

    bool IsDedicatedServer()
    {
        return m_bIsDedicatedServer;
    }

    virtual CBasePlayer *GetPlayerByAddress( const char *pszAddress );
    virtual bool GetPlayerAddress( CBasePlayer *pPlayer, char *pszAddress, int nMaxLen );

   private:
    char m_pszServerAddress[46];  // 39 (ipv6) + 6 (port) + 1 (null terminator) = 46
    char m_pszServerName[512];
    char m_pszMapName[512];
    bool m_bIsDedicatedServer;

    CUtlStringMap< int > m_mapPlayerIndexToAddress;
};

extern CGameInfoStore *g_pGameInfoStore;

#endif  // CGAMEINFOSTORE_H
