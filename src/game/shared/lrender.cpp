#include "cbase.h"
#include "materialsystem/itexture.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lrender.h"
#ifdef CLIENT_DLL
#include "rendertexture.h"
#include "view_scene.h"
#include <materialsystem/imaterialsystem.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>
#include "mathlib/lvector.h"
#include <lColor.h>
#include <renderparm.h>
#include <view.h>
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
// List of all texture ids created during the lifetime of the client Lua state
static CUtlVector< int > g_TextureIDs;

/// <summary>
/// Creates a new TextureID, keeping track of it, so it can be destroyed on shutdown
/// of the client Lua state.
/// </summary>
/// <param name="procedural"></param>
/// <returns></returns>
int surface_SafeCreateNewTextureID( bool procedural )
{
    int textureID = vgui::surface()->CreateNewTextureID( procedural );

    if ( textureID != -1 )
    {
        g_TextureIDs.AddToTail( textureID );
    }

    return textureID;
}

/// <summary>
/// Destroys all TextureIDs created during the lifetime of the client Lua state
/// </summary>
void surface_DestroyAllTextureIDs()
{
    for ( int i = 0; i < g_TextureIDs.Count(); i++ )
    {
        vgui::surface()->DestroyTextureID( g_TextureIDs[i] );
    }

    g_TextureIDs.RemoveAll();
}
#endif

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
    lua_ITexture **ppTexture = ( lua_ITexture ** )lua_newuserdata( L, sizeof( pTexture ) );
    *ppTexture = pTexture;
    LUA_SAFE_SET_METATABLE( L, "ITexture" );
}

LUALIB_API lua_ITexture *luaL_checkitexture( lua_State *L, int narg )
{
    lua_ITexture **ppData = ( lua_ITexture ** )luaL_checkudata( L, narg, "ITexture" );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ITexture expected, got NULL" );

    return *ppData;
}

#ifdef CLIENT_DLL
static int render_CreateRenderTargetTextureEx( lua_State *L )
{
    const char *name = luaL_checkstring( L, 1 );
    int width = luaL_checknumber( L, 2 );
    int height = luaL_checknumber( L, 3 );
    int sizeMode = ( int )luaL_optnumber( L, 4, RT_SIZE_DEFAULT );
    int depthMode = ( int )luaL_optnumber( L, 5, MATERIAL_RT_DEPTH_SHARED );
    int textureFlags = ( int )luaL_optnumber( L, 6, TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT );
    int renderTargetFlags = ( int )luaL_optnumber( L, 7, 0 );
    int imageFormat = ( int )luaL_optnumber( L, 8, IMAGE_FORMAT_RGBA8888 );

    g_pMaterialSystem->OverrideRenderTargetAllocation( true );
    ITexture *pTexture = materials->CreateNamedRenderTargetTextureEx(
        name,
        width,
        height,
        ( RenderTargetSizeMode_t )sizeMode,
        ( ImageFormat )imageFormat,
        ( MaterialRenderTargetDepth_t )depthMode,
        textureFlags,
        renderTargetFlags );
    g_pMaterialSystem->OverrideRenderTargetAllocation( false );
    lua_pushitexture( L, pTexture );
    return 1;
}

static int render_GetScreenEffectTexture( lua_State *L )
{
    lua_ITexture *pTexture = GetFullFrameFrameBufferTexture( luaL_checknumber( L, 1 ) );
    lua_pushlightuserdata( L, pTexture );
    return 1;
}

static int render_UpdateScreenEffectTexture( lua_State *L )
{
    const CViewSetup *pViewSetup = view->GetViewSetup();
    UpdateScreenEffectTexture( luaL_checkinteger( L, 1 ), pViewSetup->x, pViewSetup->y, pViewSetup->width, pViewSetup->height );
    return 0;
}

Frustum renderFrustum;
static int render_Push3DView( lua_State *L )
{
    CViewSetup playerView = *view->GetPlayerViewSetup();

    Vector origin = luaL_optvector( L, 1, &playerView.origin );
    QAngle angles = luaL_optangle( L, 2, &playerView.angles );
    float fov = luaL_optnumber( L, 3, playerView.fov );
    int x = ( int )luaL_optnumber( L, 4, playerView.x );
    int y = ( int )luaL_optnumber( L, 5, playerView.y );
    int width = ( int )luaL_optnumber( L, 6, playerView.width );
    int height = ( int )luaL_optnumber( L, 7, playerView.height );
    float zNear = luaL_optnumber( L, 8, playerView.zNear );
    float zFar = luaL_optnumber( L, 9, playerView.zFar );

    CViewSetup viewModelSetup( playerView );
    viewModelSetup.origin = origin;
    viewModelSetup.angles = angles;
    viewModelSetup.fov = fov;
    viewModelSetup.x = x;
    viewModelSetup.y = y;
    viewModelSetup.width = width;
    viewModelSetup.height = height;
    viewModelSetup.zNear = zNear;
    viewModelSetup.zFar = zFar;

    render->Push3DView( viewModelSetup, 0, NULL, renderFrustum );

    return 0;
}

static int render_PopView( lua_State *L )
{
    render->PopView( renderFrustum );
    return 0;
}

static int modelrender_SuppressEngineLighting( lua_State *L )
{
    modelrender->SuppressEngineLighting( lua_toboolean( L, 1 ) );
    return 0;
}

static int render_SetLightingOrigin( lua_State *L )
{
    g_pMaterialSystem->GetRenderContext()->SetLightingOrigin( luaL_checkvector( L, 1 ) );
    return 0;
}

/*directionFace:
TODO: Check that these match the values in the engine
BOX_FRONT	0	Place the light from the front
BOX_BACK	1	Place the light behind
BOX_RIGHT	2	Place the light to the right
BOX_LEFT	3	Place the light to the left
BOX_TOP	4	Place the light to the top
BOX_BOTTOM	5	Place the light to the bottom
*/
static int render_SetAmbientLightCube( lua_State *L )
{
    int directionFace = luaL_checknumber( L, 1 );
    float r = luaL_checknumber( L, 2 );
    float g = luaL_checknumber( L, 3 );
    float b = luaL_checknumber( L, 4 );
    static Vector4D cubeFaces[6];
    int i;

    for ( i = 0; i < 6; i++ )
    {
        if ( i == directionFace )
        {
            cubeFaces[i].Init( r, g, b, 1.0f );
        }
        else
        {
            cubeFaces[i].Init( 0.0f, 0.0f, 0.0f, 0.0f );
        }
    }

    g_pMaterialSystem->GetRenderContext()->SetAmbientLightCube( cubeFaces );
    return 0;
}

static int render_ResetAmbientLightCube( lua_State *L )
{
    float r = luaL_checknumber( L, 1 );
    float g = luaL_checknumber( L, 2 );
    float b = luaL_checknumber( L, 3 );

    static Vector4D cubeFaces[6];
    int i;

    for ( i = 0; i < 6; i++ )
    {
        cubeFaces[i].Init( r, g, b, 1.0f );
    }

    g_pMaterialSystem->GetRenderContext()->SetAmbientLightCube( cubeFaces );
    return 0;
}

static int render_SetLight( lua_State *L )
{
    LightDesc_t desc;
    memset( &desc, 0, sizeof( desc ) );

    desc.m_Type = MATERIAL_LIGHT_DIRECTIONAL;

    desc.m_Color[0] = luaL_checknumber( L, 1 );
    desc.m_Color[1] = luaL_checknumber( L, 2 );
    desc.m_Color[2] = luaL_checknumber( L, 3 );
    desc.m_Color *= luaL_checknumber( L, 4 ) / 255.0f;

    desc.m_Attenuation0 = 1.0f;
    desc.m_Attenuation1 = 0.0f;
    desc.m_Attenuation2 = 0.0f;
    desc.m_Flags = LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0;

    desc.m_Direction = luaL_checkvector( L, 5 );
    VectorNormalize( desc.m_Direction );

    desc.m_Theta = 0.0f;
    desc.m_Phi = 0.0f;
    desc.m_Falloff = 1.0f;

    g_pMaterialSystem->GetRenderContext()->SetLight( luaL_checknumber( L, 6 ), desc );
    return 0;
}

static int render_SetColorModulation( lua_State *L )
{
    float color[3];
    color[0] = luaL_checknumber( L, 1 );
    color[1] = luaL_checknumber( L, 2 );
    color[2] = luaL_checknumber( L, 3 );

    render->SetColorModulation( color );

    return 0;
}

static int render_SetBlend( lua_State *L )
{
    render->SetBlend( luaL_checknumber( L, 1 ) );
    return 0;
}

static int render_ClearBuffers( lua_State *L )
{
    bool bClearColor = lua_toboolean( L, 1 );
    bool bClearDepth = lua_toboolean( L, 2 );
    bool bClearStencil = lua_toboolean( L, 3 );

    g_pMaterialSystem->GetRenderContext()->ClearBuffers( bClearColor, bClearDepth, bClearStencil );
    return 0;
}

static int render_ClearColor( lua_State *L )
{
    lua_Color clr = luaL_checkcolor( L, 1 );
    g_pMaterialSystem->GetRenderContext()->ClearColor4ub( clr.r(), clr.g(), clr.b(), clr.a() );
    return 0;
}

static int render_SetScissorRect( lua_State *L )
{
    int nLeft = luaL_checknumber( L, 1 );
    int nTop = luaL_checknumber( L, 2 );
    int nRight = luaL_checknumber( L, 3 );
    int nBottom = luaL_checknumber( L, 4 );
    bool bEnableScissor = lua_toboolean( L, 5 );

    g_pMaterialSystem->GetRenderContext()->SetScissorRect( nLeft, nTop, nRight, nBottom, bEnableScissor );
    return 0;
}

static int render_SetWriteDepthToDestAlpha( lua_State *L )
{
    bool bEnable = lua_toboolean( L, 1 );
    g_pMaterialSystem->GetRenderContext()->SetIntRenderingParameter( INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA, bEnable );
    return 0;
}

static int render_MainViewOrigin( lua_State *L )
{
    lua_pushvector( L, MainViewOrigin() );
    return 1;
}

static int render_MainViewAngles( lua_State *L )
{
    lua_pushangle( L, MainViewAngles() );
    return 1;
}

static int render_MainViewForward( lua_State *L )
{
    lua_pushvector( L, MainViewForward() );
    return 1;
}
#endif

static const luaL_Reg renderLib[] = {
#ifdef CLIENT_DLL
    { "CreateRenderTargetTextureEx", render_CreateRenderTargetTextureEx },
    { "GetScreenEffectTexture", render_GetScreenEffectTexture },
    { "Push3DView", render_Push3DView },
    { "PopView", render_PopView },
    { "SuppressEngineLighting", modelrender_SuppressEngineLighting },
    { "SetLightingOrigin", render_SetLightingOrigin },
    { "SetAmbientLightCube", render_SetAmbientLightCube },
    { "ResetAmbientLightCube", render_ResetAmbientLightCube },
    { "SetLight", render_SetLight },
    { "SetColorModulation", render_SetColorModulation },
    { "SetBlend", render_SetBlend },
    { "ClearBuffers", render_ClearBuffers },
    { "ClearColor", render_ClearColor },
    { "SetScissorRect", render_SetScissorRect },
    { "UpdateScreenEffectTexture", render_UpdateScreenEffectTexture },
    { "SetWriteDepthToDestAlpha", render_SetWriteDepthToDestAlpha },
    { "MainViewOrigin", render_MainViewOrigin },
    { "MainViewAngles", render_MainViewAngles },
    { "MainViewForward", render_MainViewForward },
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
    lua_pushnumber( L, luaL_checkitexture( L, 1 )->GetActualWidth() );
    return 1;
}

static int ITexture_GetActualHeight( lua_State *L )
{
    lua_pushnumber( L, luaL_checkitexture( L, 1 )->GetActualHeight() );
    return 1;
}

static int ITexture_GetMappingWidth( lua_State *L )
{
    lua_pushnumber( L, luaL_checkitexture( L, 1 )->GetMappingWidth() );
    return 1;
}

static int ITexture_GetMappingHeight( lua_State *L )
{
    lua_pushnumber( L, luaL_checkitexture( L, 1 )->GetMappingHeight() );
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
    LUA_PUSH_NEW_METATABLE( L, LUA_ITEXTURELIBNAME );
    luaL_register( L, NULL, ITextureMetaLib );
    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_ITEXTURELIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "ITexture" */
    return 1;
}
