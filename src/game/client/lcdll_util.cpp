#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseplayer_shared.h"
#include "lbaseentity_shared.h"
#include "lgametrace.h"
#include "mathlib/lvector.h"
#include "vgui/lvgui.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Utilities )

LUA_BINDING_BEGIN( Utilities, NormalizeAngles, "library", "Normalize angles provided" )
{
    lua_QAngle angles = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angles" );
    NormalizeAngles( angles );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, InterpolateAngles, "library", "Interpolate between angles based on the provided fraction" )
{
    lua_QAngle startAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "startAngles" );
    lua_QAngle endAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "endAngles" );
    lua_QAngle resultAngles = LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "resultAngles" );
    float fraction = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "fraction" );
    InterpolateAngles( startAngles, endAngles, resultAngles, fraction );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Utilities, GetScreenHeight, "library", "Get screen height" )
{
    lua_pushinteger( L, ScreenHeight() );
    return 1;
}
LUA_BINDING_END( "integer", "Screen height." )

LUA_BINDING_BEGIN( Utilities, GetScreenWidth, "library", "Get screen width" )
{
    lua_pushinteger( L, ScreenWidth() );
    return 1;
}
LUA_BINDING_END( "integer", "Screen width." )

LUA_BINDING_BEGIN( Utilities, SafeName, "library", "Get a safe name for the provided string" )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    lua_pushstring( L, UTIL_SafeName( name ) );
    return 1;
}
LUA_BINDING_END( "string", "Safe name." )

LUA_BINDING_BEGIN( Utilities, ComputeStringWidth, "library", "Compute width of the string with the given font" )
{
    vgui::HFont font = *( vgui::HFont * )LUA_BINDING_ARGUMENT( luaL_checkfont, 1, "font" );
    const char *text = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "text" );
    lua_pushinteger( L, UTIL_ComputeStringWidth( font, text ) );
    return 1;
}
LUA_BINDING_END( "integer", "Width of the string." )

LUA_BINDING_BEGIN( Utilities, GetLocalPlayer, "library", "Get the local player entity" )
{
#ifdef CLIENT_DLL
    CBaseEntity::PushLuaInstanceSafe( L, C_BasePlayer::GetLocalPlayer() );
#else
    CBaseEntity::PushLuaInstanceSafe( L, UTIL_GetLocalPlayer() );
#endif

    return 1;
}
LUA_BINDING_END( "Entity", "Local player entity." )

LUALIB_API int luaopen_Util( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Utilities );

    return 1;
}
