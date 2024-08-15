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

LUA_REGISTRATION_INIT( Systems );

LUA_BINDING_BEGIN( Systems, GetSecondsSinceAppActive, "library", "Get the number of seconds since the application was started." )
{
    lua_pushinteger( L, steamapicontext->SteamUtils()->GetSecondsSinceAppActive() );
    return 1;
}
LUA_BINDING_END( "integer", "The number of seconds since the application was started." )

LUA_BINDING_BEGIN( Systems, GetCountry, "library", "Get the country code of the user." )
{
    char country[10];

    if ( GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, country, sizeof( country ) ) )
    {
        lua_pushstring( L, country );
        return 1;
    }

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END( "string", "The country code of the user." )

LUA_BINDING_BEGIN( Systems, HasFocus, "library", "Check if the application has focus." )
{
    lua_pushboolean( L, GetForegroundWindow() == GetActiveWindow() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the application has focus." )

LUA_BINDING_BEGIN( Systems, IsLinux, "library", "Check if the application is running on Linux." )
{
#ifdef __linux__
    lua_pushboolean( L, true );
#else
    lua_pushboolean( L, false );
#endif
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the application is running on Linux." )

LUA_BINDING_BEGIN( Systems, IsOsx, "library", "Check if the application is running on macOS." )
{
#ifdef __APPLE__
    lua_pushboolean( L, true );
#else
    lua_pushboolean( L, false );
#endif
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the application is running on macOS." )

LUA_BINDING_BEGIN( Systems, IsWindows, "library", "Check if the application is running on Windows." )
{
#ifdef _WIN32
    lua_pushboolean( L, true );
#else
    lua_pushboolean( L, false );
#endif

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the application is running on Windows." )

LUA_BINDING_BEGIN( Systems, IsWindowed, "library", "Check if the application is running in windowed mode." )
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
LUA_BINDING_END( "boolean", "Whether the application is running in windowed mode." )

LUA_BINDING_BEGIN( Systems, GetSteamServerRealTime, "library", "Get the server time." )
{
    lua_pushinteger( L, steamapicontext->SteamUtils()->GetServerRealTime() );
    return 1;
}
LUA_BINDING_END( "integer", "The server time." )

LUA_BINDING_BEGIN( Systems, GetSecondsSinceComputerActive, "library", "Get the number of seconds since the computer was started." )
{
    lua_pushinteger( L, steamapicontext->SteamUtils()->GetSecondsSinceComputerActive() );
    return 1;
}
LUA_BINDING_END( "integer", "The number of seconds since the computer was started." )

LUALIB_API int( luaopen_Systems )( lua_State* L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Systems );
    return 1;
}
