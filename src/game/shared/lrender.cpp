#include "cbase.h"
#include "materialsystem/itexture.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lrender.h"
#ifdef CLIENT_DLL
#include "rendertexture.h"
#include "view_scene.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/
LUA_API lua_ITexture *lua_toitexture( lua_State *L, int idx )
{
    lua_ITexture **ppTexture = ( lua_ITexture ** )lua_touserdata( L, idx );
    return *ppTexture;
}

/*
** push functions (C -> stack)
*/
LUA_API void lua_pushitexture( lua_State *L, lua_ITexture *pTexture )
{
    if ( pTexture == NULL )
        lua_pushnil( L );
    else
    {
        lua_ITexture **ppTexture = ( lua_ITexture ** )lua_newuserdata( L, sizeof( pTexture ) );
        *ppTexture = pTexture;
        luaL_getmetatable( L, "ITexture" );
        lua_setmetatable( L, -2 );
    }
}

LUALIB_API lua_ITexture *luaL_checkitexture( lua_State *L, int narg )
{
    lua_ITexture **ppData = ( lua_ITexture ** )luaL_checkudata( L, narg, "ITexture" );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ITexture expected, got NULL" );

    return *ppData;
}

#ifdef CLIENT_DLL
static int render_GetScreenEffectTexture( lua_State *L )
{
    lua_ITexture *pTexture = GetFullFrameFrameBufferTexture( luaL_checkint( L, 1 ) );
    lua_pushlightuserdata( L, pTexture );
    return 1;
}

static int render_UpdateScreenEffectTexture( lua_State *L )
{
    const CViewSetup *pViewSetup = view->GetViewSetup();
    UpdateScreenEffectTexture( luaL_checkinteger( L, 1 ), pViewSetup->x, pViewSetup->y, pViewSetup->width, pViewSetup->height );
    return 0;
}
#endif

static const luaL_Reg renderLib[] = {
#ifdef CLIENT_DLL
    { "GetScreenEffectTexture", render_GetScreenEffectTexture },
    { "UpdateScreenEffectTexture", render_UpdateScreenEffectTexture },
#endif
    { NULL, NULL } };

/*
** ITexture Meta
*/
static int ITexture_Download( lua_State *L )
{
    luaL_checkitexture( L, 1 )->Download();
    return 0;
}

static int ITexture_GetName( lua_State *L )
{
    lua_pushstring( L, luaL_checkitexture( L, 1 )->GetName() );
    return 1;
}

static int ITexture_GetActualWidth( lua_State *L )
{
    lua_pushinteger( L, luaL_checkitexture( L, 1 )->GetActualWidth() );
    return 1;
}

static int ITexture_GetActualHeight( lua_State *L )
{
    lua_pushinteger( L, luaL_checkitexture( L, 1 )->GetActualHeight() );
    return 1;
}

static int ITexture_GetMappingWidth( lua_State *L )
{
    lua_pushinteger( L, luaL_checkitexture( L, 1 )->GetMappingWidth() );
    return 1;
}

static int ITexture_GetMappingHeight( lua_State *L )
{
    lua_pushinteger( L, luaL_checkitexture( L, 1 )->GetMappingHeight() );
    return 1;
}

static int ITexture_GetNumAnimationFrames( lua_State *L )
{
    lua_pushinteger( L, luaL_checkitexture( L, 1 )->GetNumAnimationFrames() );
    return 1;
}

static int ITexture_IsTranslucent( lua_State *L )
{
    lua_pushboolean( L, luaL_checkitexture( L, 1 )->IsTranslucent() );
    return 1;
}

static int ITexture_IsMipmapped( lua_State *L )
{
    lua_pushboolean( L, luaL_checkitexture( L, 1 )->IsMipmapped() );
    return 1;
}

static int ITexture_IsError( lua_State *L )
{
    lua_pushboolean( L, luaL_checkitexture( L, 1 )->IsError() );
    return 1;
}

static int ITexture_IsErrorTexture( lua_State *L )
{
    lua_pushboolean( L, IsErrorTexture( luaL_checkitexture( L, 1 ) ) );
    return 1;
}

static int ITexture_Release( lua_State *L )
{
    luaL_checkitexture( L, 1 )->Release();
    return 0;
}

static int ITexture___tostring( lua_State *L )
{
    lua_pushfstring( L, "ITexture: %s", luaL_checkitexture( L, 1 )->GetName() );
    return 1;
}

static const luaL_Reg ITextureMetaLib[] = {
    { "Download", ITexture_Download },
    { "GetName", ITexture_GetName },
    { "GetActualWidth", ITexture_GetActualWidth },
    { "GetActualHeight", ITexture_GetActualHeight },
    { "GetMappingWidth", ITexture_GetMappingWidth },
    { "GetMappingHeight", ITexture_GetMappingHeight },
    { "GetNumAnimationFrames", ITexture_GetNumAnimationFrames },
    { "IsTranslucent", ITexture_IsTranslucent },
    { "IsMipmapped", ITexture_IsMipmapped },
    { "IsError", ITexture_IsError },
    { "IsErrorTexture", ITexture_IsErrorTexture },
    { "Release", ITexture_Release },
    { "__tostring", ITexture___tostring },
    { NULL, NULL } };

/*
** Open render library and ITexture metatable
*/
LUALIB_API int luaopen_render( lua_State *L )
{
    luaL_register( L, LUA_RENDERLIBNAME, renderLib );
    return 1;
}

LUALIB_API int luaopen_ITexture( lua_State *L )
{
    luaL_newmetatable( L, LUA_ITEXTURELIBNAME );
    luaL_register( L, NULL, ITextureMetaLib );
    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_ITEXTURELIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "ITexture" */
    return 1;
}
