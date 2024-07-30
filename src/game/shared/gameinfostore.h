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

    virtual CBasePlayer* GetPlayerByAddress(const char* pszAddress)
    {
        int mapIndex = m_mapPlayerIndexToAddress.Find(pszAddress);

        if ( mapIndex == m_mapPlayerIndexToAddress.InvalidIndex() )
        {
            return NULL;
        }

        return UTIL_PlayerByUserId( m_mapPlayerIndexToAddress[mapIndex] );
    }

   private:
    char m_pszServerAddress[46];  // 39 (ipv6) + 6 (port) + 1 (null terminator) = 46
    char m_pszServerName[512];
    char m_pszMapName[512];

    CUtlStringMap< int > m_mapPlayerIndexToAddress;
};

extern CGameInfoStore *g_pGameInfoStore;

#endif  // CGAMEINFOSTORE_H
