#ifndef CGAMEINFOSTORE_H
#define CGAMEINFOSTORE_H
#pragma once

#include "GameEventListener.h"

class CGameInfoStore : public CGameEventListener
{
   public:
    CGameInfoStore();

    // IGameEventListener interface:
    virtual void FireGameEvent( IGameEvent *event );

    virtual char *GetServerName()
    {
        return m_pszServerName;
    }

    virtual char *GetMapName()
    {
        return m_pszMapName;
    }

   private:
    char m_pszServerName[512];
    char m_pszMapName[512];
};

extern CGameInfoStore *g_pGameInfoStore;

#endif  // CGAMEINFOSTORE_H
