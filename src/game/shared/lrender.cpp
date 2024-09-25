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
#include <shaderapi/ishaderapi.h>
#include <utlstack.h>
#include "beamdraw.h"
#include "materialsystem/limaterial.h"
#include "iviewrender_beams.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Renders );

#ifdef CLIENT_DLL

// Minifaction and magnification filter stacks
static CUtlStack< ShaderAPITextureHandle_t > filterTextureHandlesMinification;
static CUtlStack< ShaderAPITextureHandle_t > filterTextureHandlesMagnification;

#ifdef GAME_DLL

LUA_BINDING_BEGIN( Renders, SpawnBeam, "library", "Spawns a temporary beam entity to render a beam. The alpha of the color servers as the brightness of it.", "server" )
{
    Vector start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector end = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    int modelIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "modelIndex" );
    int haloIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "haloIndex" );
    unsigned char frameStart = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "frameStart" );
    unsigned char frameRate = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "frameRate" );
    float life = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "life" );
    unsigned char width = LUA_BINDING_ARGUMENT( luaL_checknumber, 8, "width" );
    unsigned char endWidth = LUA_BINDING_ARGUMENT( luaL_checknumber, 9, "endWidth" );
    unsigned char fadeLength = LUA_BINDING_ARGUMENT( luaL_checknumber, 10, "fadeLength" );
    unsigned char noise = LUA_BINDING_ARGUMENT( luaL_checknumber, 11, "noise" );
    lua_Color colorAndBrightness = LUA_BINDING_ARGUMENT( luaL_checkcolor, 12, "colorAndBrightness" );
    unsigned char speed = LUA_BINDING_ARGUMENT( luaL_checknumber, 13, "speed" );

    UTIL_Beam(
        start,
        end,
        modelIndex,
        haloIndex,
        frameStart,
        frameRate,
        life,
        width,
        endWidth,
        fadeLength,
        noise,
        colorAndBrightness.r(),
        colorAndBrightness.g(),
        colorAndBrightness.b(),
        colorAndBrightness.a(),
        speed );

    return 0;
}
LUA_BINDING_END()

#endif

LUA_BINDING_BEGIN( Renders, GetRenderTarget, "library", "Get the currently active render target.", "client" )
{
    CMatRenderContextPtr pRenderContext( materials );
    ITexture *pTexture = pRenderContext->GetRenderTarget();
    lua_pushitexture( L, pTexture );

    return 1;
}
LUA_BINDING_END( "Texture", "The currently active render target texture." )

LUA_BINDING_BEGIN( Renders, CopyRenderTargetToTexture, "library", "Copies the currently active Render Target to the specified texture.", "client" )
{
    CMatRenderContextPtr pRenderContext( materials );
    ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    pRenderContext->CopyRenderTargetToTexture( pTexture );

    return 0;
}
LUA_BINDING_END()

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

LUA_BINDING_BEGIN( Renders, CullMode, "library", "Sets the cull mode that decides how back faces are culled when rendering geometry.", "client" )
{
    CMatRenderContextPtr pRenderContext( materials );
    MaterialCullMode_t cullMode = LUA_BINDING_ARGUMENT_ENUM( MaterialCullMode_t, 1, "cullMode" );
    pRenderContext->CullMode( cullMode );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, GetScreenEffectTexture, "library", "Get the screen effect texture.", "client" )
{
    lua_ITexture *pTexture = GetFullFrameFrameBufferTexture( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "textureIndex" ) );
    lua_pushitexture( L, pTexture );
    return 1;
}
LUA_BINDING_END( "Texture", "The screen effect texture." )

LUA_BINDING_BEGIN( Renders, SetClippingEnabled, "library", "Set the clipping enabled.", "client" )
{
    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->EnableClipping( LUA_BINDING_ARGUMENT( lua_toboolean, 1, "isEnabled" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, UpdateScreenEffectTexture, "library", "Update the screen effect texture.", "client" )
{
    const CViewSetup *pViewSetup = view->GetViewSetup();
    UpdateScreenEffectTexture(
        LUA_BINDING_ARGUMENT_WITH_DEFAULT(
            luaL_optnumber, 1, 0, "textureIndex" ),
        pViewSetup->x,
        pViewSetup->y,
        pViewSetup->width,
        pViewSetup->height );
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

    render->Push3DView( viewSetup, 0, NULL, view->GetFrustum() );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PopView3D, "library", "Pop a 3D view.", "client" )
{
    render->PopView( view->GetFrustum() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PushView2D, "library", "Push a 2D view.", "client" )
{
    CViewSetup viewSetup = *view->GetPlayerViewSetup();
    render->Push2DView( viewSetup, 0, NULL, view->GetFrustum() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PopView2D, "library", "Pop a 2D view.", "client" )
{
    render->PopView( view->GetFrustum() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PushCustomClipPlane, "library", "Push a custom clip plane.", "client" )
{
    Vector normal = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "normal" );
    float distance = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "distance" );

    Vector4D plane;
    VectorCopy( normal, plane.AsVector3D() );
    plane.w = distance + 0.1f;

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->PushCustomClipPlane( plane.Base() );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PopCustomClipPlane, "library", "Pop a custom clip plane.", "client" )
{
    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->PopCustomClipPlane();

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
    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->SetLightingOrigin( LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "lightingOrigin" ) );
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

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->SetAmbientLightCube( cubeFaces );

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

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->SetAmbientLightCube( cubeFaces );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, RenderFlashlights, "library", "Render flashlights.", "client" )
{
    // Check that a function is provided
    luaL_checktype( L, 1, LUA_TFUNCTION );

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->SetFlashlightMode( true );

    // Execute the function
    lua_call( L, 1, 0 );

    pRenderContext->SetFlashlightMode( false );

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

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->SetLight( LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "lightIndex" ), desc );

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

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->ClearBuffers( bClearColor, bClearDepth, bClearStencil );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, ClearColor, "library", "Clear the color.", "client" )
{
    lua_Color clr = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->ClearColor4ub( clr.r(), clr.g(), clr.b(), clr.a() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, SetScissorRectangle, "library", "Set the scissor rectangle.", "client" )
{
    int nLeft = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "left" );
    int nTop = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "top" );
    int nRight = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "right" );
    int nBottom = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "bottom" );
    bool bEnableScissor = LUA_BINDING_ARGUMENT( lua_toboolean, 5, "enableScissor" );

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->SetScissorRect( nLeft, nTop, nRight, nBottom, bEnableScissor );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, SetWriteDepthToDestinationAlpha, "library", "Set the write depth to destination alpha.", "client" )
{
    bool bEnable = LUA_BINDING_ARGUMENT( lua_toboolean, 1, "enable" );

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->SetIntRenderingParameter( INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA, bEnable );
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

static ShaderTexFilterMode_t FilterModeFromEnumeration( int enumeration )
{
    switch ( enumeration )
    {
        case 0:
            return SHADER_TEXFILTERMODE_NEAREST;
        case 1:
            return SHADER_TEXFILTERMODE_NEAREST;
        case 2:
            return SHADER_TEXFILTERMODE_LINEAR;
        case 3:
            return SHADER_TEXFILTERMODE_ANISOTROPIC;
        default:
            return SHADER_TEXFILTERMODE_NEAREST;
            break;
    }
}

LUA_BINDING_BEGIN( Renders, PushFilterMinification, "library", "Push a minification filter. (Non-functional; help wanted)", "client" )
{
    int filterMode = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "filter" );

    // TODO: This doesn't work
    CMatRenderContextPtr pRenderContext( materials );
    ITexture *screenTexture = pRenderContext->GetFrameBufferCopyTexture( 0 );
    ShaderAPITextureHandle_t hTexture = g_pShaderApi->CreateTexture(
        screenTexture->GetActualWidth(),
        screenTexture->GetActualHeight(),
        1,
        screenTexture->GetImageFormat(),
        1,
        1,
        TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
        "filterTextureMinification",
        "RenderTarget" );

    filterTextureHandlesMinification.Push( hTexture );
    g_pShaderApi->ModifyTexture( hTexture );
    g_pShaderApi->TexMinFilter( FilterModeFromEnumeration( filterMode ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PopFilterMinification, "library", "Pop a minification filter.", "client" )
{
    g_pShaderApi->DeleteTexture( filterTextureHandlesMinification.Top() );
    filterTextureHandlesMinification.Pop();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PushFilterMagnification, "library", "Push a magnification filter.", "client" )
{
    int filterMode = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "filter" );

    // TODO: This doesn't work
    CMatRenderContextPtr pRenderContext( materials );
    ITexture *screenTexture = pRenderContext->GetFrameBufferCopyTexture( 0 );
    ShaderAPITextureHandle_t hTexture = g_pShaderApi->CreateTexture(
        screenTexture->GetActualWidth(),
        screenTexture->GetActualHeight(),
        1,
        screenTexture->GetImageFormat(),
        1,
        1,
        TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
        "filterTextureMagnification",
        "RenderTarget" );

    filterTextureHandlesMagnification.Push( hTexture );
    g_pShaderApi->ModifyTexture( hTexture );
    g_pShaderApi->TexMagFilter( FilterModeFromEnumeration( filterMode ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, PopFilterMagnification, "library", "Pop a magnification filter. (Non-functional; help wanted)", "client" )
{
    g_pShaderApi->DeleteTexture( filterTextureHandlesMagnification.Top() );
    filterTextureHandlesMagnification.Pop();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, DepthRange, "library", "Set's the depth range of the upcoming render.", "client" )
{
    float depthMin = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "depthMin" );
    float depthMax = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "depthMax" );

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->DepthRange( depthMin, depthMax );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, GetViewEntity, "library", "Returns the entity the client is using to see (mostly the player itself)", "client" )
{
    int viewEntityId = render->GetViewEntity();
    C_BaseEntity *pCameraObject = cl_entitylist->GetEnt( viewEntityId );
    CBaseEntity::PushLuaInstanceSafe( L, pCameraObject );

    return 1;
}
LUA_BINDING_END( "Entity", "The view entity." )

LUA_BINDING_BEGIN( Renders, SetMaterial, "library", "Binds a material for use in the next render operations", "client" )
{
    IMaterial *pMaterial = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->Bind( pMaterial );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, DrawSprite, "library", "Draws a sprite", "client" )
{
    Vector position = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "position" );
    float width = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "width" );
    float height = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "height" );
    lua_Color color = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 4, lua_Color( 255, 255, 255, 255 ), "color" );

    color32 rawColor = { color.r(), color.g(), color.b(), color.a() };
    DrawSprite( position, width, height, rawColor );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Renders, DrawBeam, "library", "Draws a beam", "client" )
{
    Vector &start = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "start" );
    Vector &end = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "end" );
    float width = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "width" );
    float textureStart = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "textureStart" );
    float textureEnd = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "textureEnd" );
    lua_Color color = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optcolor, 6, lua_Color( 255, 255, 255, 255 ), "color" );

    CMatRenderContextPtr pRenderContext( materials );
    CBeamSegDraw beamDraw;
    beamDraw.Start( pRenderContext, 2, NULL );

    BeamSeg_t seg;
    seg.m_flAlpha = 1.0;
    seg.m_flWidth = width;

    seg.m_vPos = start;
    seg.m_flTexCoord = textureStart;
    seg.m_vColor = color.ToVector();
    beamDraw.NextSeg( &seg );

    seg.m_vPos = end;
    seg.m_flTexCoord = textureEnd;
    seg.m_vColor = color.ToVector();
    beamDraw.NextSeg( &seg );

    beamDraw.End();

    return 0;
}
LUA_BINDING_END()
#endif  // CLIENT_DLL

/*
** Open render library
*/
LUALIB_API int luaopen_render( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Renders );

#ifdef CLIENT_DLL
    LUA_SET_ENUM_LIB_BEGIN( L, "CULL_MODE" );
    lua_pushenum( L, MaterialCullMode_t::MATERIAL_CULLMODE_CCW, "COUNTER_CLOCKWISE" );
    lua_pushenum( L, MaterialCullMode_t::MATERIAL_CULLMODE_CW, "CLOCKWISE" );
    LUA_SET_ENUM_LIB_END( L );
#endif

    return 1;
}
