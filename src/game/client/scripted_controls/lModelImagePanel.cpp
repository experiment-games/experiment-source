//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include <vgui_controls/PHandle.h>
#include "luamanager.h"
#include "luasrclib.h"
#include <LKeyValues.h>
#include <vgui_int.h>
#include <vgui/IInput.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Panel.h>

#include "scripted_controls/lPanel.h"
#include "scripted_controls/lModelImagePanel.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include <cpng.h>
#include <renderparm.h>
#include <tier3/mdlutils.h>
#include <matsys_controls/matsyscontrols.h>
#include <mathlib/lvector.h>
#include "view.h"
#include "iviewrender.h"
#include "view_shared.h"
#include <model_types.h>
#include <basemodelpanel.h>

using namespace vgui;

// Taken from MDLPanel.h
struct MDLData_t
{
    CMDL m_MDL;
    matrix3x4_t m_MDLToWorld;
    bool m_bDisabled;
    float m_flCycleStartTime;
};

const char *g_pszModelImagePanelRenderTargetName = "_rt_ModelImagePanel";

LModelImagePanel::LModelImagePanel( Panel *parent, const char *panelName, lua_State *L )
    : EditablePanel( parent, panelName )
{
    m_lua_State = L;
    m_pszModelImagePath = NULL;
    m_pszModelPath = NULL;
    m_iSkin = 0;
    m_pszBodyGroups = NULL;
    m_pRenderMaterial = NULL;
    m_nTextureID = -1;
    m_iCachedTextureWide = 0;
    m_iCachedTextureTall = 0;
}

LModelImagePanel::~LModelImagePanel()
{
}

void LModelImagePanel::PushPanelToLua( lua_State *L )
{
    lua_pushmodelimagepanel( L, this );
}

/// <summary>
/// Get a name for png materials (prefixed with ! and no png extension)
/// </summary>
/// <param name="pngImagePath"></param>
/// <returns></returns>
static const char *PathToMaterialName( const char *pngImagePath )
{
    // Get a name for png materials (prefixed with ! and no png extension)
    char nameWithoutExtension[MAX_PATH];
    Q_StripExtension( pngImagePath, nameWithoutExtension, sizeof( nameWithoutExtension ) );
    static char materialName[MAX_PATH];
    Q_snprintf( materialName, sizeof( materialName ), "!%s", nameWithoutExtension );

    return materialName;
}

/// <summary>
/// This function is used to render a model to a PNG file given a camera setup.
/// If pszSavePath is not NULL, the PNG file will be saved to that path. If it is NULL, the PNG
/// file will be saved to materials/spawnicons/<path to the model relative to GAME root>.png
/// </summary>
/// <param name="camera"></param>
/// <param name="pszSavePath"></param>
void LModelImagePanel::RebuildSpawnIcon( Camera_t camera, const char *pszSavePath /*= NULL*/ )
{
    char pngPathFull[MAX_PATH];

    if ( pszSavePath != NULL )
    {
        Q_strncpy( pngPathFull, pszSavePath, sizeof( pngPathFull ) );
    }
    else
    {
        // Build the path relative to the icon texture
        Q_snprintf( pngPathFull, sizeof( pngPathFull ), "materials/spawnicons/%s", m_pszModelPath );
        Q_StripExtension( pngPathFull, pngPathFull, sizeof( pngPathFull ) );  // strip .mdl
        Q_DefaultExtension( pngPathFull, ".png", sizeof( pngPathFull ) );
    }

    int x, y, w, h;
    GetBounds( x, y, w, h );
    ParentLocalToScreen( x, y );

    int outputWidth = 256;
    int outputHeight = 256;

    CMatRenderContextPtr pRenderContext( materials );

    ITexture *pTexture = materials->FindTexture( "_rt_FullFrameFB1", TEXTURE_GROUP_RENDER_TARGET );
    pRenderContext->PushRenderTargetAndViewport( pTexture, 0, 0, outputWidth, outputHeight );

    int viewportX, viewportY, viewportWidth, viewportHeight;
    pRenderContext->GetViewport( viewportX, viewportY, viewportWidth, viewportHeight );

    pRenderContext->ClearColor4ub( 0, 0, 0, 0 );
    pRenderContext->ClearBuffers( true, true, true );

    CViewSetup viewSetup;
    memset( &viewSetup, 0, sizeof( viewSetup ) );
    viewSetup.origin = camera.m_origin;
    viewSetup.angles = camera.m_angles;
    //viewSetup.m_bRenderToSubrectOfLargerScreen = true;
    viewSetup.zNear = camera.m_flZNear;
    viewSetup.zFar = camera.m_flZFar;
    viewSetup.fov = camera.m_flFOV;

    viewSetup.x = viewportX;
    viewSetup.y = viewportY;
    viewSetup.width = ( float )outputWidth;
    viewSetup.height = ( float )outputHeight;
    viewSetup.m_bOrtho = true;

    if ( g_pMaterialSystemHardwareConfig->GetHDREnabled() )
    {
        ITexture *pCubemapTexture = materials->FindTexture( "editor/cubemap.hdr", NULL, true );
        pRenderContext->BindLocalCubemap( pCubemapTexture );
    }
    else
    {
        ITexture *pCubemapTexture = materials->FindTexture( "editor/cubemap", NULL, true );
        pRenderContext->BindLocalCubemap( pCubemapTexture );
    }

    MDLCACHE_CRITICAL_SECTION();

    //CModelPanelModel *pTemporaryEntity = new CModelPanelModel;

    //if ( !pTemporaryEntity )
    //    return;

    //if ( pTemporaryEntity->InitializeAsClientEntity( m_pszModelPath, RENDER_GROUP_OPAQUE_ENTITY ) == false )
    //{
    //    pTemporaryEntity->Remove();
    //    return;
    //}

    ////pTemporaryEntity->DontRecordInTools();
    ////pTemporaryEntity->AddEffects( EF_NODRAW );
    //pTemporaryEntity->SetModel( m_pszModelPath );
    //pTemporaryEntity->m_nSkin = m_iSkin;

    ///*FOR_EACH_MAP_FAST( m_pModelInfo->m_mapBodygroupValues, i )
    //{
    //    pTemporaryEntity->SetBodygroup( m_pModelInfo->m_mapBodygroupValues.Key( i ), m_pModelInfo->m_mapBodygroupValues[i] );
    //}*/

    //pTemporaryEntity->SetAbsOrigin( vec3_origin );
    //pTemporaryEntity->SetAbsAngles( QAngle() );

    pRenderContext->SetLightingOrigin( camera.m_origin );
    pRenderContext->SetAmbientLight( 0.4, 0.4, 0.4 );

    static Vector white[6] =
        {
            Vector( 0.4, 0.4, 0.4 ),
            Vector( 0.4, 0.4, 0.4 ),
            Vector( 0.4, 0.4, 0.4 ),
            Vector( 0.4, 0.4, 0.4 ),
            Vector( 0.4, 0.4, 0.4 ),
            Vector( 0.4, 0.4, 0.4 ),
        };

    g_pStudioRender->SetAmbientLightColors( white );

    // spotlight
	/*Vector vecMins, vecMaxs;
    pTemporaryEntity->GetRenderBounds( vecMins, vecMaxs );
    LightDesc_t spotLight( vec3_origin + Vector( 0, 0, 200 ), Vector( 1, 1, 1 ), pTemporaryEntity->GetAbsOrigin() + Vector( 0, 0, ( vecMaxs.z - vecMins.z ) * 0.75 ), 0.035, 0.873 );
    g_pStudioRender->SetLocalLights( 1, &spotLight );*/

    int flags = 0;
    flags |= STUDIO_WIREFRAME;

    CBaseAnimating *pAnimating = CBasePlayer::GetLocalPlayer();

    Vector m_LightingOrigin = camera.m_origin;
    ModelRenderInfo_t sInfo;
    sInfo.origin = vec3_origin;
    sInfo.angles = QAngle(0, 0, 0);
    sInfo.pRenderable = pAnimating;
    sInfo.pModel = pAnimating->GetModel();
    sInfo.pModelToWorld = &pAnimating->EntityToWorldTransform();
    sInfo.pLightingOffset = NULL;
    sInfo.pLightingOrigin = &m_LightingOrigin;
    sInfo.flags = flags;
    sInfo.entity_index = -1;
    sInfo.skin = m_iSkin;
    sInfo.body = 0;
    sInfo.hitboxset = 0;
    sInfo.instance = pAnimating->GetModelInstance();

    Frustum dummyFrustum;
    render->Push3DView( viewSetup, 0, pTexture, dummyFrustum );

    modelrender->SuppressEngineLighting( true );
    float color[3] = { 1.0f, 1.0f, 1.0f };
    render->SetColorModulation( color );
    render->SetBlend( 1.0f );

    pRenderContext->MatrixMode( MATERIAL_MODEL );
    pRenderContext->PushMatrix();
    pRenderContext->LoadIdentity();
    int drawn = modelrender->DrawModelEx( sInfo );
    pRenderContext->MatrixMode( MATERIAL_MODEL );
    pRenderContext->PopMatrix();

    Assert( drawn != 0 );

    //pTemporaryEntity->DrawModel( STUDIO_RENDER );

    modelrender->SuppressEngineLighting( false );

    //pTemporaryEntity->Remove();

    unsigned char *pImageData = new unsigned char[outputWidth * outputHeight * 4];

    pRenderContext->ReadPixels( 0, 0, outputWidth, outputHeight, pImageData, IMAGE_FORMAT_RGBA8888 );

    //CViewSetup view2d;
    //view2d.x = viewSetup.x;
    //view2d.y = viewSetup.y;
    //view2d.width = viewSetup.width;
    //view2d.height = viewSetup.height;

    //render->Push2DView( view2d, 0, NULL, dummyFrustum );
    //surface()->DrawSetColor( 0, 255, 0, 255 );
    //surface()->DrawFilledRect( 50, 50, 10, 10 );
    //render->PopView( dummyFrustum );

    render->PopView( dummyFrustum );

    pRenderContext->BindLocalCubemap( NULL );

    pRenderContext->PopRenderTargetAndViewport();

    PNG_WriteToFile( pngPathFull, pImageData, outputWidth, outputHeight );
}

/// <summary>
/// Sets the PNG image to be displayed on the panel. This loads it into the material system
/// and creates (or updates) a texture ID for the panel to use. Additionally this precaches
/// the size of the loaded texture.
/// </summary>
/// <param name="pngImagePath"></param>
void LModelImagePanel::SetModelImage( const char *pngImagePath )
{
    // If there's no change, don't waste time
    if ( m_pszModelImagePath != nullptr && Q_strcmp( m_pszModelImagePath, pngImagePath ) == 0 )
        return;

    m_pszModelImagePath = pngImagePath;

    if ( m_nTextureID == -1 )
        m_nTextureID = surface()->CreateNewTextureID();

    const char *materialName = PathToMaterialName( pngImagePath );

    IMaterial *pMaterial = CPngTextureRegen::GetOrCreateProceduralMaterial( materialName, pngImagePath );
    g_pMatSystemSurface->DrawSetTextureMaterial( m_nTextureID, pMaterial );

    surface()->DrawGetTextureSize( m_nTextureID, m_iCachedTextureWide, m_iCachedTextureTall );
}

/// <summary>
/// Paints the texture to the panel
/// TODO: Implement, without conflicting with the base class
/// </summary>
void LModelImagePanel::Paint()
{
    //Camera_t camera;
    //camera.m_origin = Vector( 60, 60, 64 );
    //float rotatingByTime = Plat_FloatTime() * 100;
    //camera.m_angles = QAngle( rotatingByTime, 0, 0 );
    //camera.m_flFOV = 70;
    //camera.m_flZNear = 1;
    //camera.m_flZFar = 10000;
    //RebuildSpawnIcon( camera );

    // Draw a little square to show this is called
    surface()->DrawSetColor( 255, 0, 0, 255 );
    surface()->DrawFilledRect( 0, 0, 10, 10 );

    //if ( m_nTextureID == -1 )
    //    return;

    //int wide, tall;
    //GetSize( wide, tall );

    //surface()->DrawSetColor( 255, 255, 255, 255 );
    //surface()->DrawSetTexture( m_nTextureID );
    //surface()->DrawTexturedRect( 0, 0, wide, tall );
}

/*
** access functions (stack -> C)
*/

LUA_API lua_ModelImagePanel *lua_tomodelimagepanel( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );

    if ( phPanel == NULL )
        return NULL;

    return dynamic_cast< lua_ModelImagePanel * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushmodelimagepanel( lua_State *L, lua_ModelImagePanel *plModelImagePanel )
{
    LUA_PUSH_PANEL_USERDATA( L, plModelImagePanel, lua_ModelImagePanel, "ModelImagePanel" );
}

LUALIB_API lua_ModelImagePanel *luaL_checkmodelimagepanel( lua_State *L, int narg )
{
    lua_ModelImagePanel *d = lua_tomodelimagepanel( L, narg );

    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ModelImagePanel expected, got INVALID_PANEL" );

    return d;
}

static int ModelImagePanel_SetModel( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = luaL_checkmodelimagepanel( L, 1 );
    const char *modelPath = luaL_checkstring( L, 2 );
    int skin = luaL_optint( L, 3, 0 );
    const char *bodyGroups = luaL_optstring( L, 4, NULL );

    pModelImagePanel->SetModel( modelPath, skin, bodyGroups );

    return 0;
}

static int ModelImagePanel_GetModel( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = luaL_checkmodelimagepanel( L, 1 );
    lua_pushstring( L, pModelImagePanel->GetModel() );
    return 1;
}

static int ModelImagePanel_SetSkin( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = luaL_checkmodelimagepanel( L, 1 );
    pModelImagePanel->SetSkin( luaL_checkint( L, 2 ) );
    return 0;
}

static int ModelImagePanel_GetSkin( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = luaL_checkmodelimagepanel( L, 1 );
    lua_pushinteger( L, pModelImagePanel->GetSkin() );
    return 1;
}

static int ModelImagePanel_SetBodyGroups( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = luaL_checkmodelimagepanel( L, 1 );
    pModelImagePanel->SetBodyGroups( luaL_checkstring( L, 2 ) );
    return 0;
}

static int ModelImagePanel_GetBodyGroups( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = luaL_checkmodelimagepanel( L, 1 );
    lua_pushstring( L, pModelImagePanel->GetBodyGroups() );
    return 1;
}

static int ModelImagePanel_RebuildSpawnIcon( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = luaL_checkmodelimagepanel( L, 1 );
    const char *pszSavePath = luaL_optstring( L, 3, NULL );
    Camera_t camera;

    lua_getfield( L, 2, "origin" );
    Vector origin = lua_tovector( L, -1 );
    lua_pop( L, 1 );
    camera.m_origin = origin;

    lua_getfield( L, 2, "angles" );
    QAngle angles = lua_toangle( L, -1 );
    lua_pop( L, 1 );
    camera.m_angles = angles;

    lua_getfield( L, 2, "fieldOfView" );
    float fieldOfView = luaL_optnumber( L, -1, 70 );
    lua_pop( L, 1 );
    camera.m_flFOV = fieldOfView;

    lua_getfield( L, 2, "zNear" );
    float zNear = luaL_optnumber( L, -1, 1 );
    lua_pop( L, 1 );
    camera.m_flZNear = zNear;

    lua_getfield( L, 2, "zFar" );
    float zFar = luaL_optnumber( L, -1, 10000 );
    lua_pop( L, 1 );
    camera.m_flZFar = zFar;

    pModelImagePanel->RebuildSpawnIcon( camera, pszSavePath );

    return 0;
}

static int ModelImagePanel_SetModelImage( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = luaL_checkmodelimagepanel( L, 1 );
    pModelImagePanel->SetModelImage( luaL_checkstring( L, 2 ) );
    return 0;
}

static int ModelImagePanel___index( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = lua_tomodelimagepanel( L, 1 );
    LUA_METATABLE_INDEX_CHECK_VALID( L, Panel_IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pModelImagePanel );

    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pModelImagePanel );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "Panel" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    lua_pushnil( L );
    return 1;
}

static int ModelImagePanel___newindex( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = lua_tomodelimagepanel( L, 1 );

    if ( pModelImagePanel == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LUA_GET_REF_TABLE( L, pModelImagePanel );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, luaL_checkstring( L, 2 ) );
    lua_pop( L, 1 );

    return 0;
}

static int ModelImagePanel___eq( lua_State *L )
{
    lua_pushboolean( L, lua_tomodelimagepanel( L, 1 ) == lua_tomodelimagepanel( L, 2 ) );
    return 1;
}

static int ModelImagePanel___tostring( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel = lua_tomodelimagepanel( L, 1 );

    if ( pModelImagePanel == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pModelImagePanel->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "ModelImagePanel: \"%s\"", pName );
    }

    return 1;
}

static const luaL_Reg ModelImagePanelmeta[] = {
    { "SetModel", ModelImagePanel_SetModel },
    { "GetModel", ModelImagePanel_GetModel },
    { "SetSkin", ModelImagePanel_SetSkin },
    { "GetSkin", ModelImagePanel_GetSkin },
    { "SetBodyGroups", ModelImagePanel_SetBodyGroups },
    { "GetBodyGroups", ModelImagePanel_GetBodyGroups },
    { "RebuildSpawnIcon", ModelImagePanel_RebuildSpawnIcon },
    { "SetModelImage", ModelImagePanel_SetModelImage },
    { "__index", ModelImagePanel___index },
    { "__newindex", ModelImagePanel___newindex },
    { "__eq", ModelImagePanel___eq },
    { "__tostring", ModelImagePanel___tostring },
    { "__gc", Panel___gc },
    { NULL, NULL } };

static int luasrc_ModelImagePanel( lua_State *L )
{
    lua_ModelImagePanel *pModelImagePanel =
        new lua_ModelImagePanel(
            luaL_optpanel( L, 1, VGui_GetClientLuaRootPanel() ),
            luaL_checkstring( L, 2 ),
            L );
    lua_pushmodelimagepanel( L, pModelImagePanel );
    return 1;
}

static const luaL_Reg ModelImagePanel_funcs[] = { { "ModelImagePanel", luasrc_ModelImagePanel },
                                                  { NULL, NULL } };

/*
** Open ModelImagePanel object
*/
LUALIB_API int luaopen_vgui_ModelImagePanel( lua_State *L )
{
    luaL_newmetatable( L, "ModelImagePanel" );
    luaL_register( L, NULL, ModelImagePanelmeta );
    lua_pushstring( L, LUA_PANELLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */
    luaL_register( L, LUA_VGUILIBNAME, ModelImagePanel_funcs );
    lua_pop( L, 2 );
    return 0;
}
