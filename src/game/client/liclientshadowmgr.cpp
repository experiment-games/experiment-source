#include "cbase.h"
#include "luamanager.h"
#include "mathlib/lvector.h"
#include "luasrclib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( ClientShadows )

LUA_BINDING_BEGIN( ClientShadows, AdvanceFrame, "library", "Advance shadow frame processing." )
{
    g_pClientShadowMgr->AdvanceFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ClientShadows, GetShadowDirection, "library", "Get the current shadow direction as a vector." )
{
    Vector v = g_pClientShadowMgr->GetShadowDirection();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "Vector", "Current shadow direction." )

LUA_BINDING_BEGIN( ClientShadows, RenderShadowTexture, "library", "Render shadow texture using provided width and height." )
{
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "width" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "height" );
    g_pClientShadowMgr->RenderShadowTexture( width, height );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ClientShadows, SetShadowBlobbyCutoffArea, "library", "Set the blobby cutoff area for shadows." )
{
    double area = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "area" );
    g_pClientShadowMgr->SetShadowBlobbyCutoffArea( area );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ClientShadows, SetShadowColor, "library", "Set shadow color using RGB values." )
{
    int r = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "r" );
    int g = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "g" );
    int b = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "b" );
    g_pClientShadowMgr->SetShadowColor( r, g, b );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ClientShadows, SetShadowDirection, "library", "Set the direction of shadows as a vector." )
{
    Vector dir = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "direction" );
    g_pClientShadowMgr->SetShadowDirection( dir );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ClientShadows, SetShadowDistance, "library", "Set the distance for shadow rendering." )
{
    double distance = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "distance" );
    g_pClientShadowMgr->SetShadowDistance( distance );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ClientShadows, SetShadowsDisabled, "library", "Enable or disable shadow rendering." )
{
    bool disabled = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "disabled" );
    g_pClientShadowMgr->SetShadowsDisabled( disabled );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ClientShadows, UnlockAllShadowDepthTextures, "library", "Unlock all shadow depth textures." )
{
    g_pClientShadowMgr->UnlockAllShadowDepthTextures();
    return 0;
}
LUA_BINDING_END()

/*
** Open g_pClientShadowMgr library
*/
LUALIB_API int luaopen_g_pClientShadowMgr( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( ClientShadows );
    return 1;
}
