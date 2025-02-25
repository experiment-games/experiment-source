#include "cbase.h"
#include "vgui/ischeme.h"
#include "vgui_controls/Controls.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "vgui/lischeme.h"
#include "vgui/lvgui.h"

#include "scripted_controls/lPanel.h"
#include "lColor.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

/*
** access functions (stack -> C)
*/

LUA_API lua_IScheme *lua_toischeme( lua_State *L, int idx )
{
    lua_IScheme **ppScheme = ( lua_IScheme ** )lua_touserdata( L, idx );
    return *ppScheme;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushischeme( lua_State *L, lua_IScheme *pScheme )
{
    lua_IScheme **ppScheme = ( lua_IScheme ** )lua_newuserdata( L, sizeof( lua_IScheme ) );
    *ppScheme = pScheme;
    LUA_SAFE_SET_METATABLE( L, LUA_ISCHEMELIBNAME );
}

LUALIB_API lua_IScheme *luaL_checkischeme( lua_State *L, int narg )
{
    lua_IScheme **d = ( lua_IScheme ** )luaL_checkudata( L, narg, LUA_ISCHEMELIBNAME );
    return *d;
}

// Experiment; Disabled so we don't have to deal with SchemeHandle (HScheme) objects from lua, but only IScheme objects.
// LUA_REGISTRATION_INIT( SchemeHandle );
//
// LUA_BINDING_BEGIN( SchemeHandle, __tostring, "class", "Metamethod to get the string representation of the scheme handle." )
//{
//    HScheme hScheme = LUA_BINDING_ARGUMENT( luaL_checkscheme, 1, "scheme" );
//    lua_pushfstring( L, "SchemeHandle: %d", hScheme );
//    return 1;
//}
// LUA_BINDING_END( "string", "The string representation of the scheme handle." )
//
// LUALIB_API int luaopen_HScheme( lua_State *L )
//{
//    LUA_PUSH_NEW_METATABLE( L, LUA_HSCHEMELIBNAME );
//
//    LUA_REGISTRATION_COMMIT( SchemeHandle );
//
//    lua_pushvalue( L, -1 );
//    lua_setfield( L, -2, "__index" );
//    lua_pushstring( L, LUA_HSCHEMELIBNAME );
//    lua_setfield( L, -2, "__type" );
//    return 1;
//}
// static int scheme_GetIScheme( lua_State *L )
//{
//    lua_pushischeme( L, scheme()->GetIScheme( luaL_checkscheme( L, 1 ) ) );
//    return 1;
//}
// static int scheme_GetProportionalNormalizedValueEx( lua_State *L )
//{
//     lua_pushinteger( L, scheme()->GetProportionalNormalizedValueEx( luaL_checkscheme( L, 1 ), luaL_checknumber( L, 2 ) ) );
//     return 1;
// }
// static int scheme_GetProportionalScaledValueEx( lua_State *L )
//{
//     lua_pushinteger( L, scheme()->GetProportionalScaledValueEx( luaL_checkscheme( L, 1 ), luaL_checknumber( L, 2 ) ) );
//     return 1;
// }

LUA_REGISTRATION_INIT( Scheme );

LUA_BINDING_BEGIN( Scheme, GetColor, "class", "Returns the color for the specified color name." )
{
    lua_IScheme *scheme = LUA_BINDING_ARGUMENT( luaL_checkischeme, 1, "scheme" );
    const char *colorName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "colorName" );
    Color &defaultColor = LUA_BINDING_ARGUMENT( luaL_checkcolor, 3, "defaultColor" );
    Color &color = scheme->GetColor( colorName, defaultColor );
    lua_pushcolor( L, color );
    return 1;
}
LUA_BINDING_END( "color", "The color for the specified color name." )

LUA_BINDING_BEGIN( Scheme, GetFont, "class", "Returns the font for the specified font name." )
{
    lua_IScheme *scheme = LUA_BINDING_ARGUMENT( luaL_checkischeme, 1, "scheme" );
    const char *fontName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "fontName" );
    bool proportional = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "proportional" );
    HFont font = scheme->GetFont( fontName, proportional );
    lua_pushfont( L, font );
    return 1;
}
LUA_BINDING_END( "font", "The font for the specified font name." )

LUA_BINDING_BEGIN( Scheme, GetFontName, "class", "Returns the font name for the specified font handle." )
{
    lua_IScheme *scheme = LUA_BINDING_ARGUMENT( luaL_checkischeme, 1, "scheme" );
    HFont font = LUA_BINDING_ARGUMENT( luaL_checkfont, 2, "font" );
    const char *fontName = scheme->GetFontName( font );
    lua_pushstring( L, fontName );
    return 1;
}
LUA_BINDING_END( "string", "The font name for the specified font handle." )

LUA_BINDING_BEGIN( Scheme, GetResourceString, "class", "Returns the resource string for the specified resource name." )
{
    lua_IScheme *scheme = LUA_BINDING_ARGUMENT( luaL_checkischeme, 1, "scheme" );
    const char *resourceName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "resourceName" );
    const char *resourceString = scheme->GetResourceString( resourceName );
    lua_pushstring( L, resourceString );
    return 1;
}
LUA_BINDING_END( "string", "The resource string for the specified resource name." )

LUA_BINDING_BEGIN( Scheme, __tostring, "class", "Metamethod to get the string representation of the IScheme object." )
{
    lua_IScheme *scheme = LUA_BINDING_ARGUMENT( lua_toischeme, 1, "scheme" );
    lua_pushfstring( L, "Scheme: %p", scheme );
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the IScheme object." )

/*
** Open IScheme object
*/
LUALIB_API int luaopen_IScheme( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_ISCHEMELIBNAME );

    LUA_REGISTRATION_COMMIT( Scheme );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_ISCHEMELIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Scheme" */
    return 1;
}

LUA_REGISTRATION_INIT( Schemes );

LUA_BINDING_BEGIN( Schemes, GetDefaultScheme, "library", "Returns the default scheme." )
{
    lua_pushischeme( L, scheme()->GetIScheme( scheme()->GetDefaultScheme() ) );
    return 1;
}
LUA_BINDING_END( "scheme", "The default scheme." )

LUA_BINDING_BEGIN( Schemes, GetProportionalNormalizedValue, "library", "Returns the proportional normalized value." )
{
    int scaledValue = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "scaledValue" );
    lua_pushinteger( L, scheme()->GetProportionalNormalizedValue( scaledValue ) );
    return 1;
}
LUA_BINDING_END( "integer", "The proportional normalized value." )

LUA_BINDING_BEGIN( Schemes, GetProportionalScaledValue, "library", "Returns the proportional scaled value." )
{
    int normalizedValue = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "normalizedValue" );
    lua_pushinteger( L, scheme()->GetProportionalScaledValue( normalizedValue ) );
    return 1;
}
LUA_BINDING_END( "integer", "The proportional scaled value." )

LUA_BINDING_BEGIN( Schemes, GetScheme, "library", "Returns the scheme for the specified scheme name." )
{
    const char *schemeName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "schemeName" );
    HScheme hScheme = scheme()->GetScheme( schemeName );
    lua_pushischeme( L, scheme()->GetIScheme( hScheme ) );
    return 1;
}
LUA_BINDING_END( "scheme", "The scheme for the specified scheme name." )

LUA_BINDING_BEGIN( Schemes, LoadSchemeFromFile, "library", "Loads the scheme from the specified file." )
{
    const char *fileName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "fileName" );
    const char *tag = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "tag" );
    HScheme hScheme = scheme()->LoadSchemeFromFile( fileName, tag );
    lua_pushischeme( L, scheme()->GetIScheme( hScheme ) );
    return 1;
}
LUA_BINDING_END( "scheme", "The scheme loaded from the specified file." )

LUA_BINDING_BEGIN( Schemes, LoadSchemeFromFileEx, "library", "Loads the scheme from the specified file." )
{
    vgui::Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "sizingPanel" );
    const char *fileName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "fileName" );
    const char *tag = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "tag" );
    HScheme hScheme = scheme()->LoadSchemeFromFileEx( panel->GetVPanel(), fileName, tag );
    lua_pushischeme( L, scheme()->GetIScheme( hScheme ) );
    return 1;
}
LUA_BINDING_END( "scheme", "The scheme loaded from the specified file." )

LUA_BINDING_BEGIN( Schemes, ReloadFonts, "library", "Reloads the fonts." )
{
    scheme()->ReloadFonts();
    return 0;
}
LUA_BINDING_END( NULL, NULL )

LUA_BINDING_BEGIN( Schemes, ReloadSchemes, "library", "Reloads the schemes." )
{
    scheme()->ReloadSchemes();
    return 0;
}
LUA_BINDING_END( NULL, NULL )

/*
** Open scheme library
*/
LUALIB_API int luaopen_Schemes( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Schemes );

    return 1;
}
