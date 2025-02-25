#ifndef LSOUNDS_H
#define LSOUNDS_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include <util/bassmanager.h>

/* type for IAudioChannel functions */
typedef IAudioChannel lua_IAudioChannel;

/*
** access functions (stack -> C)
*/

LUA_API lua_IAudioChannel *( lua_toaudiochannel )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushaudiochannel )( lua_State *L, lua_IAudioChannel *pData );

LUALIB_API lua_IAudioChannel *( luaL_checkaudiochannel )( lua_State *L, int narg );

class CPlayUrlCallbackData : public IBassManagerCallbackData
{
   public:
    virtual void Release() OVERRIDE;

    lua_State *L;
    int callbackRef;
};
#endif

#endif  // LSOUNDS_H
