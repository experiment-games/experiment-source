#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lsystem.h"
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>
#include <vgui_controls/Controls.h>
#include <steam/steam_api.h>

#ifdef INVALID_HANDLE_VALUE
#undef INVALID_HANDLE_VALUE
#endif

#ifdef GetCommandLine
#undef GetCommandLine
#endif

#ifdef ReadConsoleInput
#undef ReadConsoleInput
#endif

#ifdef RegCreateKey
#undef RegCreateKey
#endif

#ifdef RegCreateKeyEx
#undef RegCreateKeyEx
#endif

#ifdef RegOpenKey
#undef RegOpenKey
#endif

#ifdef RegOpenKeyEx
#undef RegOpenKeyEx
#endif

#ifdef RegQueryValue
#undef RegQueryValue
#endif

#ifdef RegQueryValueEx
#undef RegQueryValueEx
#endif

#ifdef RegSetValue
#undef RegSetValue
#endif

#ifdef RegSetValueEx
#undef RegSetValueEx
#endif

#include <winlite.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int system_AppTime( lua_State *L )
{
    lua_pushnumber( L, steamapicontext->SteamUtils()->GetSecondsSinceAppActive() );
    return 1;
}

static int system_GetCountry( lua_State *L )
{
#ifdef _WIN32
    char country[10];

    if ( GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, country, sizeof( country ) ) )
    {
        lua_pushstring( L, country );
        return 1;
    }
#elif __linux__
    // TODO
#endif

    lua_pushnil( L );
    return 1;
}

static int system_HasFocus( lua_State *L )
{
#ifdef _WIN32
    lua_pushboolean( L, GetForegroundWindow() == GetActiveWindow() );
    return 1;
#elif __linux__
    // TODO
#endif

    lua_pushnil( L );
    return 1;
}

static int system_IsLinux( lua_State *L )
{
#ifdef __linux__
    lua_pushboolean( L, true );
#else
    lua_pushboolean( L, false );
#endif
    return 1;
}

static int system_IsOSX( lua_State *L )
{
#ifdef __APPLE__
    lua_pushboolean( L, true );
#else
    lua_pushboolean( L, false );
#endif
    return 1;
}

static int system_IsWindows( lua_State *L )
{
#ifdef _WIN32
    lua_pushboolean( L, true );
#else
    lua_pushboolean( L, false );
#endif
    return 1;
}

static int system_IsWindowed( lua_State *L )
{
#ifdef _WIN32
    HWND hWnd = GetForegroundWindow();

    if ( hWnd )
    {
        DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
        lua_pushboolean( L, dwStyle & WS_OVERLAPPEDWINDOW );
        return 1;
    }
#elif __linux__
    // TODO
#endif

    lua_pushboolean( L, false );
    return 1;
}

static int system_SteamTime( lua_State *L )
{
    lua_pushnumber( L, steamapicontext->SteamUtils()->GetServerRealTime() );
    return 1;
}

static int system_UpTime( lua_State *L )
{
    lua_pushnumber( L, steamapicontext->SteamUtils()->GetSecondsSinceComputerActive() );
    return 1;
}

static const luaL_Reg systemLib[] = {
    { "AppTime", system_AppTime },
    { "GetCountry", system_GetCountry },
    { "HasFocus", system_HasFocus },
    { "IsLinux", system_IsLinux },
    { "IsOSX", system_IsOSX },
    { "IsWindows", system_IsWindows },
    { "IsWindowed", system_IsWindowed },
    { "SteamTime", system_SteamTime },
    { "UpTime", system_UpTime },
    { NULL, NULL } };

LUALIB_API int( luaopen_system )( lua_State * L )
{
    luaL_register( L, LUA_SYSTEMLIBNAME, systemLib );
    return 1;
}
