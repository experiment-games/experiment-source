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
#include <litexture.h>
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Renders );

#ifdef CLIENT_DLL
// Frustrums for PushView2D and -3D functions
static Frustum renderFrustum3D;
static Frustum renderFrustum2D;

LUA_BINDING_BEGIN( Renders, CreateRenderTargetTextureEx, "library", "Create a new render target texture.", "client" )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "width" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "height" );
    int sizeMode = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, RT_SIZE_DEFAULT, "sizeMode" );
    int depthMode = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, MATERIAL_RT_DEPTH_SHARED, "depthMode" );
    int textureFlags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 6, TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, "textureFlags" );
    int renderTargetFlags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 7, 0, "renderTargetFlags" );
    int imageFormat = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 8, IMAGE_FORMAT_RGBA8888, "imageFormat" );

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
LUA_BINDING_END( "Texture", "The created render target texture." )

LUA_BINDING_BEGIN( Renders, GetScreenEffectTexture, "library", "Get the screen effect texture.", "client" )
{
    lua_ITexture *pTexture = GetFullFrameFrameBufferTexture( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "textureIndex" ) );
    lua_pushitexture( L, pTexture );
    return 1;
}
LUA_BINDING_END( "Texture", "The screen effect texture." )

LUA_BINDING_BEGIN( Renders, UpdateScreenEffectTexture, "library", "Update the screen effect texture.", "client" )
{
    const CViewSetup *pViewSetup = view->GetViewSetup();
    UpdateScreenEffectTexture( LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "textureIndex" ), pViewSetup->x, pViewSetup->y, pViewSetup->width, pViewSetup->height );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PushView3D, "library", "Push a 3D view.", "client" )
{
    CViewSetup playerView = *view->GetPlayerViewSetup();

    Vector origin = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optvector, 1, &playerView.origin, "origin" );
    QAngle angles = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optangle, 2, &playerView.angles, "angles" );
    float fov = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, playerView.fov, "fov" );
    int x = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, playerView.x, "x" );
    int y = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, playerView.y, "y" );
    int width = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 6, playerView.width, "width" );
    int height = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 7, playerView.height, "height" );
    float zNear = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 8, playerView.zNear, "zNear" );
    float zFar = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 9, playerView.zFar, "zFar" );

    CViewSetup viewSetup( playerView );
    viewSetup.origin = origin;
    viewSetup.angles = angles;
    viewSetup.fov = fov;
    viewSetup.x = x;
    viewSetup.y = y;
    viewSetup.width = width;
    viewSetup.height = height;
    viewSetup.zNear = zNear;
    viewSetup.zFar = zFar;

    render->Push3DView( viewSetup, 0, NULL, renderFrustum3D );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PopView3D, "library", "Pop a 3D view.", "client" )
{
    render->PopView( renderFrustum3D );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PushView2D, "library", "Push a 2D view.", "client" )
{
    CViewSetup viewSetup = *view->GetPlayerViewSetup();
    render->Push2DView( viewSetup, 0, NULL, renderFrustum2D );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PopView2D, "library", "Pop a 2D view.", "client" )
{
    render->PopView( renderFrustum2D );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, SuppressEngineLighting, "library", "Suppress engine lighting.", "client" )
{
    modelrender->SuppressEngineLighting( LUA_BINDING_ARGUMENT( lua_toboolean, 1, "suppress" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, SetLightingOrigin, "library", "Set the lighting origin.", "client" )
{
    g_pMaterialSystem->GetRenderContext()->SetLightingOrigin( LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "lightingOrigin" ) );
    return 0;
}
LUA_BINDING_END()

/*
directionFace:
    TODO: Check that these match the values in the engine
    BOX_FRONT	0	Place the light from the front
    BOX_BACK	1	Place the light behind
    BOX_RIGHT	2	Place the light to the right
    BOX_LEFT	3	Place the light to the left
    BOX_TOP	4	Place the light to the top
    BOX_BOTTOM	5	Place the light to the bottom
*/
LUA_BINDING_BEGIN( Renders, SetAmbientLightCube, "library", "Set the ambient light cube.", "client" )
{
    int directionFace = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "directionFace" );
    float r = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "r" );
    float g = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "g" );
    float b = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "b" );
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
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, ResetAmbientLightCube, "library", "Reset the ambient light cube.", "client" )
{
    float r = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "r" );
    float g = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "g" );
    float b = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "b" );

    static Vector4D cubeFaces[6];
    int i;

    for ( i = 0; i < 6; i++ )
    {
        cubeFaces[i].Init( r, g, b, 1.0f );
    }

    g_pMaterialSystem->GetRenderContext()->SetAmbientLightCube( cubeFaces );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, SetLight, "library", "Set a light.", "client" )
{
    LightDesc_t desc;
    memset( &desc, 0, sizeof( desc ) );

    desc.m_Type = MATERIAL_LIGHT_DIRECTIONAL;

    desc.m_Color[0] = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "r" );
    desc.m_Color[1] = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "g" );
    desc.m_Color[2] = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "b" );
    desc.m_Color *= LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "intensity" ) / 255.0f;

    desc.m_Attenuation0 = 1.0f;
    desc.m_Attenuation1 = 0.0f;
    desc.m_Attenuation2 = 0.0f;
    desc.m_Flags = LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0;

    desc.m_Direction = LUA_BINDING_ARGUMENT( luaL_checkvector, 5, "direction" );
    VectorNormalize( desc.m_Direction );

    desc.m_Theta = 0.0f;
    desc.m_Phi = 0.0f;
    desc.m_Falloff = 1.0f;

    g_pMaterialSystem->GetRenderContext()->SetLight( LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "lightIndex" ), desc );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, SetColorModulation, "library", "Set the color modulation.", "client" )
{
    float color[3];
    color[0] = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "r" );
    color[1] = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "g" );
    color[2] = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "b" );

    render->SetColorModulation( color );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, SetBlend, "library", "Set the blend.", "client" )
{
    render->SetBlend( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "blend" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, ClearBuffers, "library", "Clear the buffers.", "client" )
{
    bool bClearColor = LUA_BINDING_ARGUMENT( lua_toboolean, 1, "clearColor" );
    bool bClearDepth = LUA_BINDING_ARGUMENT( lua_toboolean, 2, "clearDepth" );
    bool bClearStencil = LUA_BINDING_ARGUMENT( lua_toboolean, 3, "clearStencil" );

    g_pMaterialSystem->GetRenderContext()->ClearBuffers( bClearColor, bClearDepth, bClearStencil );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, ClearColor, "library", "Clear the color.", "client" )
{
    lua_Color clr = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    g_pMaterialSystem->GetRenderContext()->ClearColor4ub( clr.r(), clr.g(), clr.b(), clr.a() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, SetScissorRect, "library", "Set the scissor rectangle.", "client" )
{
    int nLeft = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "left" );
    int nTop = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "top" );
    int nRight = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "right" );
    int nBottom = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "bottom" );
    bool bEnableScissor = LUA_BINDING_ARGUMENT( lua_toboolean, 5, "enableScissor" );

    g_pMaterialSystem->GetRenderContext()->SetScissorRect( nLeft, nTop, nRight, nBottom, bEnableScissor );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, SetWriteDepthToDestAlpha, "library", "Set the write depth to destination alpha.", "client" )
{
    bool bEnable = LUA_BINDING_ARGUMENT( lua_toboolean, 1, "enable" );
    g_pMaterialSystem->GetRenderContext()->SetIntRenderingParameter( INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA, bEnable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, MainViewOrigin, "library", "Get the main view origin.", "client" )
{
    lua_pushvector( L, MainViewOrigin() );
    return 1;
}
LUA_BINDING_END( "Vector", "The main view origin." )

LUA_BINDING_BEGIN( Renders, MainViewAngles, "library", "Get the main view angles.", "client" )
{
    lua_pushangle( L, MainViewAngles() );
    return 1;
}
LUA_BINDING_END( "Angle", "The main view angles." )

LUA_BINDING_BEGIN( Renders, MainViewForward, "library", "Get the main view forward.", "client" )
{
    lua_pushvector( L, MainViewForward() );
    return 1;
}
LUA_BINDING_END( "Vector", "The main view forward." )
#endif  // CLIENT_DLL

/*
** Open render library
*/
LUALIB_API int luaopen_render( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Renders );
    return 1;
}
