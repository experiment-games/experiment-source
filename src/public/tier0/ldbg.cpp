#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "lColor.h"
#include <luamanager.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( debug );

LUA_BINDING_BEGIN( debug, SpewActivate, "library", "Activate spew output." )
{
    const char *pGroupName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "groupName" );
    int iLevel = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "level" );

    SpewActivate( pGroupName, iLevel );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, IsSpewActive, "library", "Check if spew output is active." )
{
    const char *pGroupName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "groupName" );
    int iLevel = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "level" );

    lua_pushboolean( L, IsSpewActive( pGroupName, iLevel ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if spew output is active, false otherwise." )

LUA_BINDING_BEGIN( debug, PrintMessage, "library", "Print a message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );

    Msg( "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintDebugMessage, "library", "Print a debug message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );
    int iLevel = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "level" );
    const char *pGroupName = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "groupName" );

    DMsg( pGroupName, iLevel, "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintWarning, "library", "Print a warning message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );

    Warning( "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintDebugWarning, "library", "Print a debug warning message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );
    int iLevel = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "level" );
    const char *pGroupName = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "groupName" );

    DWarning( pGroupName, iLevel, "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintLog, "library", "Print a log message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );

    Log( "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintDebugLog, "library", "Print a debug log message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );
    int iLevel = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "level" );
    const char *pGroupName = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "groupName" );

    DLog( pGroupName, iLevel, "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

//LUA_BINDING_BEGIN( debug, CrashError, "library", "Crashes the game with an error message." )
//{
//    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );
//
//    Error( "%s", pMessage );
//    return 0;
//}
//LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintDevMessage, "library", "Print a developer message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );

    DevMsg( "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintDevWarning, "library", "Print a developer warning message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );

    DevWarning( "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintDevLog, "library", "Print a developer log message to the console." )
{
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "message" );

    DevLog( "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintColorMessage, "library", "Print a message to the console with a specific color." )
{
    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" );

    ConColorMsg( color, "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintDebugColorMessage, "library", "Print a debug message to the console with a specific color." )
{
    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" );

    ConDColorMsg( color, "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintNetworkMessage, "library", "Print a network message to the console." )
{
    int iLevel = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "level" );
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" );

    NetMsg( iLevel, "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintNetworkWarning, "library", "Print a network warning message to the console." )
{
    int iLevel = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "level" );
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" );

    NetWarning( iLevel, "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, PrintNetworkLog, "library", "Print a network log message to the console." )
{
    int iLevel = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "level" );
    const char *pMessage = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "message" );

    NetLog( iLevel, "%s", pMessage );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( debug, GetRealmColor, "library", "Get the color of the current realm." )
{
    lua_pushcolor( L, REALM_COLOR );
    return 1;
}
LUA_BINDING_END()

LUALIB_API int luaopen_dbg( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( debug );
    return 1;
}
