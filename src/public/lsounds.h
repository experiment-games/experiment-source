#ifndef LSOUNDS_H
#define LSOUNDS_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include <util/bassmanager.h>

class CPlayUrlCallbackData : public IBassManagerCallbackData
{
   public:
    virtual void Release() OVERRIDE;

    lua_State *L;
    int callbackRef;
};
#endif

#endif  // LSOUNDS_H
