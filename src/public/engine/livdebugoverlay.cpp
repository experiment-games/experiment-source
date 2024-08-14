#include "cbase.h"
#include "engine/ivdebugoverlay.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "mathlib/lvector.h"
#include "lColor.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( DebugOverlays );

LUA_BINDING_BEGIN( DebugOverlays, AddBoxOverlay, "library", "Add a box overlay to the screen." )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector mins = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "mins" );
    Vector maxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "maxs" );
    QAngle angles = LUA_BINDING_ARGUMENT( luaL_checkangle, 4, "angles" );
    int r = LUA_BINDING_ARGUMENT( luaL_checkinteger, 5, "r" );
    int g = LUA_BINDING_ARGUMENT( luaL_checkinteger, 6, "g" );
    int b = LUA_BINDING_ARGUMENT( luaL_checkinteger, 7, "b" );
    int a = LUA_BINDING_ARGUMENT( luaL_checkinteger, 8, "a" );
    float duration = LUA_BINDING_ARGUMENT( luaL_checknumber, 9, "duration" );

    debugoverlay->AddBoxOverlay( start, mins, maxs, angles, r, g, b, a, duration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddBoxOverlay2, "library", "Add a box overlay to the screen with specific colors." )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector mins = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "mins" );
    Vector maxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "maxs" );
    QAngle angles = LUA_BINDING_ARGUMENT( luaL_checkangle, 4, "angles" );
    lua_Color color1 = LUA_BINDING_ARGUMENT( luaL_checkcolor, 5, "color1" );
    lua_Color color2 = LUA_BINDING_ARGUMENT( luaL_checkcolor, 6, "color2" );
    float duration = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "duration" );

    debugoverlay->AddBoxOverlay2( start, mins, maxs, angles, color1, color2, duration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddGridOverlay, "library", "Add a grid overlay to the screen." )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );

    debugoverlay->AddGridOverlay( origin );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddLineOverlay, "library", "Add a line overlay to the screen." )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector end = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    int r = LUA_BINDING_ARGUMENT( luaL_checkinteger, 3, "r" );
    int g = LUA_BINDING_ARGUMENT( luaL_checkinteger, 4, "g" );
    int b = LUA_BINDING_ARGUMENT( luaL_checkinteger, 5, "b" );
    bool noDepthTest = LUA_BINDING_ARGUMENT( luaL_checkboolean, 6, "noDepthTest" );
    float duration = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "duration" );

    debugoverlay->AddLineOverlay( start, end, r, g, b, noDepthTest, duration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddLineOverlayAlpha, "library", "Add a line overlay to the screen with alpha." )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector end = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    int r = LUA_BINDING_ARGUMENT( luaL_checkinteger, 3, "r" );
    int g = LUA_BINDING_ARGUMENT( luaL_checkinteger, 4, "g" );
    int b = LUA_BINDING_ARGUMENT( luaL_checkinteger, 5, "b" );
    int a = LUA_BINDING_ARGUMENT( luaL_checkinteger, 6, "a" );
    bool noDepthTest = LUA_BINDING_ARGUMENT( luaL_checkboolean, 7, "noDepthTest" );
    float duration = LUA_BINDING_ARGUMENT( luaL_checknumber, 8, "duration" );

    debugoverlay->AddLineOverlayAlpha( start, end, r, g, b, a, noDepthTest, duration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddScreenTextOverlay, "library", "Add a screen text overlay." )
{
    float x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    float y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    float scale = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "scale" );
    int r = LUA_BINDING_ARGUMENT( luaL_checkinteger, 4, "r" );
    int g = LUA_BINDING_ARGUMENT( luaL_checkinteger, 5, "g" );
    int b = LUA_BINDING_ARGUMENT( luaL_checkinteger, 6, "b" );
    int a = LUA_BINDING_ARGUMENT( luaL_checkinteger, 7, "a" );
    const char *text = LUA_BINDING_ARGUMENT( luaL_checkstring, 8, "text" );

    debugoverlay->AddScreenTextOverlay( x, y, scale, r, g, b, a, text );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddSweptBoxOverlay, "library", "Add a swept box overlay to the screen." )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector end = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    Vector mins = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "mins" );
    Vector maxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "maxs" );
    QAngle angles = LUA_BINDING_ARGUMENT( luaL_checkangle, 5, "angles" );
    int r = LUA_BINDING_ARGUMENT( luaL_checkinteger, 6, "r" );
    int g = LUA_BINDING_ARGUMENT( luaL_checkinteger, 7, "g" );
    int b = LUA_BINDING_ARGUMENT( luaL_checkinteger, 8, "b" );
    int a = LUA_BINDING_ARGUMENT( luaL_checkinteger, 9, "a" );
    float duration = LUA_BINDING_ARGUMENT( luaL_checknumber, 10, "duration" );

    debugoverlay->AddSweptBoxOverlay( start, end, mins, maxs, angles, r, g, b, a, duration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddTriangleOverlay, "library", "Add a triangle overlay to the screen." )
{
    Vector p1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "p1" );
    Vector p2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "p2" );
    Vector p3 = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "p3" );
    int r = LUA_BINDING_ARGUMENT( luaL_checkinteger, 4, "r" );
    int g = LUA_BINDING_ARGUMENT( luaL_checkinteger, 5, "g" );
    int b = LUA_BINDING_ARGUMENT( luaL_checkinteger, 6, "b" );
    int a = LUA_BINDING_ARGUMENT( luaL_checkinteger, 7, "a" );
    bool noDepthTest = LUA_BINDING_ARGUMENT( luaL_checkboolean, 8, "noDepthTest" );
    float duration = LUA_BINDING_ARGUMENT( luaL_checknumber, 9, "duration" );

    debugoverlay->AddTriangleOverlay( p1, p2, p3, r, g, b, a, noDepthTest, duration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, ClearAllOverlays, "library", "Clear all overlays." )
{
    debugoverlay->ClearAllOverlays();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, ClearDeadOverlays, "library", "Clear dead overlays." )
{
    debugoverlay->ClearDeadOverlays();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, ScreenPosition, "library", "Get the screen position of a vector." )
{
    Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vec" );
    Vector screen;

    if ( debugoverlay->ScreenPosition( vec, screen ) )
    {
        lua_pushvector( L, screen );
    }
    else
    {
        lua_pushnil( L );
    }

    return 1;
}
LUA_BINDING_END( "Vector", "The screen position of the vector, or nil if the vector is not on the screen." )

LUA_BINDING_BEGIN( DebugOverlays, AddTextOverlay, "library", "Add a text overlay to the screen." )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );
    int lineOffset = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "lineOffset" );
    float duration = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "duration" );
    const char *text = LUA_BINDING_ARGUMENT( luaL_checkstring, 4, "text" );

    debugoverlay->AddTextOverlay( origin, lineOffset, duration, text );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddTextOverlayRGB, "library", "Add a text overlay to the screen with specific colors." )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );
    int lineOffset = LUA_BINDING_ARGUMENT( luaL_checkinteger, 2, "lineOffset" );
    float duration = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "duration" );
    int r = LUA_BINDING_ARGUMENT( luaL_checkinteger, 4, "r" );
    int g = LUA_BINDING_ARGUMENT( luaL_checkinteger, 5, "g" );
    int b = LUA_BINDING_ARGUMENT( luaL_checkinteger, 6, "b" );
    int a = LUA_BINDING_ARGUMENT( luaL_checkinteger, 7, "a" );
    const char *text = LUA_BINDING_ARGUMENT( luaL_checkstring, 8, "text" );

    debugoverlay->AddTextOverlayRGB( origin, lineOffset, duration, r, g, b, a, text );
    return 0;
}
LUA_BINDING_END()

/*
** Open debugoverlay library
*/
LUALIB_API int luaopen_debugoverlay( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( DebugOverlays );
    return 1;
}
