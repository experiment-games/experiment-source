#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "lvgui.h"
#include <luamanager.h>  // TODO: Remove, devbugging only

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

/*
** access functions (stack -> C)
*/

LUA_API lua_HFont lua_tofont( lua_State *L, int idx )
{
    lua_HFont hFont = *( lua_HFont * )lua_touserdata( L, idx );
    return hFont;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushfont( lua_State *L, lua_HFont hFont )
{
    lua_HFont *phFont = ( lua_HFont * )lua_newuserdata( L, sizeof( lua_HFont ) );
    *phFont = hFont;
    LUA_SAFE_SET_METATABLE( L, LUA_FONTLIBNAME );
}

LUALIB_API lua_HFont luaL_checkfont( lua_State *L, int narg )
{
    lua_HFont *d = ( lua_HFont * )luaL_checkudata( L, narg, LUA_FONTLIBNAME );
    return *d;
}

// Experiment;  Disabled so we can focus on pushing Scheme (IScheme) objects
//              which are more useful.
//LUA_REGISTRATION_INIT( SchemeHandle );
//
//LUA_BINDING_BEGIN( SchemeHandle, __tostring, "class", "Metamethod to get the string representation of the scheme handle." )
//{
//    HScheme hScheme = LUA_BINDING_ARGUMENT( luaL_checkscheme, 1, "scheme" );
//    lua_pushfstring( L, "SchemeHandle: %d", hScheme );
//    return 1;
//}
//LUA_BINDING_END( "string", "The string representation of the scheme handle." )
//
///*
//** Open HScheme object
//*/
//LUALIB_API int luaopen_HScheme( lua_State *L )
//{
//    LUA_PUSH_NEW_METATABLE( L, LUA_HSCHEMELIBNAME );
//
//    LUA_REGISTRATION_COMMIT( SchemeHandle );
//
//    lua_pushvalue( L, -1 );           /* push metatable */
//    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
//    lua_pushstring( L, LUA_HSCHEMELIBNAME );
//    lua_setfield( L, -2, "__type" ); /* metatable.__type = "SchemeHandle" */
//    return 1;
//}

LUA_REGISTRATION_INIT( FontHandle );

LUA_BINDING_BEGIN( FontHandle, __tostring, "class", "Metamethod to get the string representation of the font handle." )
{
    HFont hFont = LUA_BINDING_ARGUMENT( lua_tofont, 1, "font" );

    if ( hFont == INVALID_FONT )
        lua_pushstring( L, "INVALID_FONT" );
    else
        lua_pushfstring( L, "FontHandle: %d", hFont );

    return 1;
}
LUA_BINDING_END( "string", "The string representation of the font handle." )

/*
** Open HFont object
*/
LUALIB_API int luaopen_HFont( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_FONTLIBNAME );

    LUA_REGISTRATION_COMMIT( FontHandle );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_FONTLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "FontHandle" */

    lua_pushfont( L, INVALID_FONT );
    lua_setglobal( L, "INVALID_FONT" ); /* set global INVALID_FONT */

    return 1;
}
