#include "cbase.h"
#include "engine/ivdebugoverlay.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "mathlib/lvector.h"
#include "lColor.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#include <lbaseentity_shared.h>

LUA_REGISTRATION_INIT( DebugOverlays );

#define color_white lua_Color( 255, 255, 255, 255 )

LUA_BINDING_BEGIN( DebugOverlays, AddBox, "library", "Add a box overlay to the screen with specific colors." )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector mins = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "mins" );
    Vector maxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "maxs" );
    QAngle angles = LUA_BINDING_ARGUMENT( luaL_checkangle, 4, "angles" );
    float duration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 1, "duration" );
    lua_Color faceColor = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 6, color_white, "faceColor" );
    lua_Color edgeColor = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 7, faceColor, "edgeColor" );

    debugoverlay->AddBoxOverlay2( start, mins, maxs, angles, faceColor, edgeColor, duration );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddGrid, "library", "Add a grid overlay to the screen." )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );

    debugoverlay->AddGridOverlay( origin );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddLine, "library", "Add a line overlay to the screen with alpha." )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector end = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    float duration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 1, "duration" );
    lua_Color color = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 4, color_white, "color" );
    bool noDepthTest = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 5, false, "noDepthTest" );

    debugoverlay->AddLineOverlayAlpha( start, end, color.r(), color.g(), color.b(), color.a(), noDepthTest, duration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddText, "library", "Add a text overlay to the screen with specific colors." )
{
    Vector origin = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "origin" );
    const char *text = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "text" );
    float duration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 1, "duration" );
    bool viewCheck = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "viewCheck" );
    int lineOffset = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 64, "lineOffset" );
    lua_Color color = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 6, color_white, "color" );

    if ( viewCheck )
        DevWarning( "DebugOverlays.AddText: viewCheck argument (4) is not yet implemented.\n" );

    debugoverlay->AddTextOverlayRGB( origin, lineOffset, duration, color.r(), color.g(), color.b(), color.a(), "%s", text );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddScreenText, "library", "Add a screen text overlay." )
{
    float x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    float y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    const char *text = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "text" );
    float duration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 1, "duration" );
    lua_Color color = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 5, color_white, "color" );

    debugoverlay->AddScreenTextOverlay( x, y, duration, color.r(), color.g(), color.b(), color.a(), text );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddEntityTextOverlay, "library", "Displays 2D text at the specified coordinates." )
{
    lua_CBaseEntity *entity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int lineOffset = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "lineOffset" );
    const char *text = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "text" );
    float duration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 1, "duration" );
    lua_Color color = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 5, color_white, "color" );

    debugoverlay->AddEntityTextOverlay( entity->entindex(), lineOffset, duration, color.r(), color.g(), color.b(), color.a(), "%s", text );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddSweptBox, "library", "Add a swept box overlay to the screen." )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector end = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    Vector mins = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "mins" );
    Vector maxs = LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "maxs" );
    QAngle angles = LUA_BINDING_ARGUMENT( luaL_checkangle, 5, "angles" );
    float duration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 6, 1, "duration" );
    lua_Color color = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 7, color_white, "color" );

    debugoverlay->AddSweptBoxOverlay( start, end, mins, maxs, angles, color.r(), color.g(), color.b(), color.a(), duration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, AddTriangle, "library", "Add a triangle overlay to the screen." )
{
    Vector p1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "p1" );
    Vector p2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "p2" );
    Vector p3 = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "p3" );
    float duration = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 1, "duration" );
    lua_Color color = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 5, color_white, "color" );
    bool noDepthTest = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 6, false, "noDepthTest" );

    debugoverlay->AddTriangleOverlay( p1, p2, p3, color.r(), color.g(), color.b(), color.a(), noDepthTest, duration );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, ClearAll, "library", "Clear all overlays." )
{
    debugoverlay->ClearAllOverlays();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( DebugOverlays, ClearDead, "library", "Clear dead overlays." )
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

/*
** Open debugoverlay library
*/
LUALIB_API int luaopen_debugoverlay( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( DebugOverlays );
    return 1;
}
