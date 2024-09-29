#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include <lgameevents.h>
#include <igameevents.h>
#include <lcolor.h>
#include <LKeyValues.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CLuaGameEventListener : public IGameEventListener2
{
   protected:
    lua_State *L;

   public:
    CLuaGameEventListener( lua_State *L )
    {
        this->L = L;
    }

    void AddListener( const char *name )
    {
        gameeventmanager->AddListener( this, name, false );
    }

    // IGameEventListener2 Interface:
   public:
    virtual void FireGameEvent( IGameEvent *event )
    {
        const char *eventName = event->GetName();

        LUA_CALL_HOOK_FOR_STATE_BEGIN( this->L, eventName );
        lua_pushkeyvalues_as_table( this->L, event->m_pDataKeys );
        LUA_CALL_HOOK_FOR_STATE_END( this->L, 1, 0 );
    }
};

static CLuaGameEventListener *luaGameEventListener;

void InitializeLuaGameEventHandler( lua_State *L )
{
    luaGameEventListener = new CLuaGameEventListener( L );
}

void ShutdownLuaGameEventHandler( lua_State *L )
{
    gameeventmanager->RemoveListener( luaGameEventListener );
    delete luaGameEventListener;
}

LUA_REGISTRATION_INIT( GameEvents )

LUA_BINDING_BEGIN( GameEvents, Listen, "library", "Call a hook for this game event name, when the event occurs." )
{
    luaGameEventListener->AddListener( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "eventName" ) );
    return 0;
}
LUA_BINDING_END()

/*
** Open GameEvents library
*/
LUALIB_API int luaopen_GameEvents( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( GameEvents );

    return 1;
}
