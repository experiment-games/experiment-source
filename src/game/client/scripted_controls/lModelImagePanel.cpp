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
#include <view.h>

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

// Note: https://wiki.facepunch.com/gmod/Panel:RebuildSpawnIconEx
// TODO: Create compat function in Lua that passes default values to this function and wraps this in RebuildSpawnIconEx, accepting a table
// Note: I think the .ent (Entity) field is ignored for this function (since we already know the model, skin and bodygroups from SetModel)

/// <summary>
/// This function is used to render a model to a PNG file given a camera setup.
/// If pszSavePath is not NULL, the PNG file will be saved to that path. If it is NULL, the PNG
/// file will be saved to materials/spawnicons/<path to the model relative to GAME root>.png
/// </summary>
/// <param name="camera"></param>
/// <param name="pszSavePath"></param>
void LModelImagePanel::RebuildSpawnIcon( Camera_t camera, const char *pszSavePath /*= NULL*/ )
{
    // - Get the model path, skin and bodygroups from the panel
    // - Render it using the specified camera position, angle and FOV
    // - Save it toa .png file in materials/spawnicons/<path to the model relative to GAME root>.png
    // - Set the image to this panel

    int wide, tall;
    GetSize( wide, tall );

    int outputWidth = 256;
    int outputHeight = 256;

   /* CMatRenderContextPtr pRenderContext( materials );

    bool renderToTexture = false;
    vgui::MatSystemSurface()->Begin3DPaint( 0, 0, outputWidth, outputHeight, renderToTexture );

    pRenderContext->MatrixMode( MATERIAL_MODEL );
    pRenderContext->LoadIdentity();

    pRenderContext->MatrixMode( MATERIAL_VIEW );
    pRenderContext->LoadMatrix( view );

    pRenderContext->MatrixMode( MATERIAL_PROJECTION );
    pRenderContext->LoadMatrix( projection );

    pRenderContext->ClearBuffers( true, true );

    pRenderContext->CullMode( MATERIAL_CULLMODE_CCW );
    pRenderContext->SetIntRenderingParameter( INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA, false );*/

    MDLHandle_t modelHandle = vgui::MDLCache()->FindMDL( m_pszModelPath );

    if ( vgui::MDLCache()->IsErrorModel( modelHandle ) )
    {
        DevWarning( "Failed to load model %s\n", m_pszModelPath );
        return;
    }

    VMatrix view, projection;
    ComputeViewMatrix( &view, camera );
    ComputeProjectionMatrix( &projection, camera, outputWidth, outputHeight );

    VMatrix worldToCamera;
    MatrixInverseTR( view, worldToCamera );
    Vector vecOrigin, vecRight, vecUp, vecForward;
    MatrixGetColumn( worldToCamera, 0, &vecRight );
    MatrixGetColumn( worldToCamera, 1, &vecUp );
    MatrixGetColumn( worldToCamera, 2, &vecForward );
    MatrixGetColumn( worldToCamera, 3, &vecOrigin );

    MDLData_t rootModel;
    rootModel.m_MDL.SetMDL( modelHandle );
    rootModel.m_MDL.m_bWorldSpaceViewTarget = true;
    rootModel.m_MDL.m_vecViewTarget = vecOrigin;
    rootModel.m_bDisabled = false;
    rootModel.m_flCycleStartTime = 0.0f;

    StudioRenderConfig_t studioRenderConfig;
    memset( &studioRenderConfig, 0, sizeof( studioRenderConfig ) );
    studioRenderConfig.fEyeShiftX = 0.0f;
    studioRenderConfig.fEyeShiftY = 0.0f;
    studioRenderConfig.fEyeShiftZ = 0.0f;
    studioRenderConfig.fEyeSize = 0.0f;
    studioRenderConfig.fEyeGlintPixelWidthLODThreshold = 0.0f;
    studioRenderConfig.maxDecalsPerModel = 0;
    studioRenderConfig.drawEntities = 1;
    studioRenderConfig.skin = m_iSkin;
    studioRenderConfig.fullbright = 1;
    studioRenderConfig.bEyeMove = false;
    studioRenderConfig.bSoftwareSkin = false;
    studioRenderConfig.bNoHardware = false;
    studioRenderConfig.bNoSoftware = false;
    studioRenderConfig.bTeeth = true;
    studioRenderConfig.bEyes = true;
    studioRenderConfig.bFlex = false;
    studioRenderConfig.bWireframe = true;
    studioRenderConfig.bDrawNormals = true;
    studioRenderConfig.bDrawTangentFrame = false;
    studioRenderConfig.bDrawZBufferedWireframe = false;
    studioRenderConfig.bSoftwareLighting = false;
    studioRenderConfig.bShowEnvCubemapOnly = false;
    studioRenderConfig.bWireframeDecals = false;

    StudioRenderConfig_t oldStudioRenderConfig;
    StudioRender()->GetCurrentConfig( oldStudioRenderConfig );

    StudioRender()->UpdateConfig( studioRenderConfig );

    // Build the path relative to the icon texture
    char pngPath[MAX_PATH];
    char pngPathFull[MAX_PATH];
    Q_snprintf( pngPath, sizeof( pngPath ), "spawnicons/%s", m_pszModelPath );
    Q_StripExtension( pngPath, pngPath, sizeof( pngPath ) );  // strip .mdl
    Q_DefaultExtension( pngPath, ".png", sizeof( pngPath ) );
    Q_snprintf( pngPathFull, sizeof( pngPathFull ), "materials/%s", pngPath );

    // const char *materialName = PathToMaterialName( pngPath );


	// Color + alpha modulation
    Color m_Color( 255, 255, 255, 255 );
    Vector white( m_Color.r() / 255.0f, m_Color.g() / 255.0f, m_Color.b() / 255.0f );
    g_pStudioRender->SetColorModulation( white.Base() );
    g_pStudioRender->SetAlphaModulation( m_Color.a() / 255.0f );

    DrawModelInfo_t info;
    info.m_pStudioHdr = g_pMDLCache->GetStudioHdr( modelHandle );
    info.m_pHardwareData = g_pMDLCache->GetHardwareData( modelHandle );
    info.m_Decals = STUDIORENDER_DECAL_INVALID;
    info.m_Skin = m_iSkin;
    info.m_Body = 0;
    info.m_HitboxSet = 0;
    info.m_pClientEntity = NULL;
    info.m_pColorMeshes = NULL;
    info.m_bStaticLighting = false;
    info.m_Lod = 0;

    matrix3x4_t *pBoneToWorld = g_pStudioRender->LockBoneMatrices( info.m_pStudioHdr->numbones );
    rootModel.m_MDL.SetUpBones( rootModel.m_MDLToWorld, info.m_pStudioHdr->numbones, pBoneToWorld );

    Vector vecWorldViewTarget;
    if ( rootModel.m_MDL.m_bWorldSpaceViewTarget )
    {
        vecWorldViewTarget = rootModel.m_MDL.m_vecViewTarget;
    }
    else
    {
        VectorTransform( rootModel.m_MDL.m_vecViewTarget, rootModel.m_MDLToWorld, vecWorldViewTarget );
    }
    g_pStudioRender->SetEyeViewTarget( info.m_pStudioHdr, info.m_Body, vecWorldViewTarget );

    CMatRenderContextPtr pRenderContext( g_pMaterialSystem );

    // Set default flex values
    float *pFlexWeights = NULL;
    //const int nFlexDescCount = info.m_pStudioHdr->numflexdesc;
    //if ( nFlexDescCount )
    //{
    //    CStudioHdr cStudioHdr( info.m_pStudioHdr, g_pMDLCache );

    //    g_pStudioRender->LockFlexWeights( info.m_pStudioHdr->numflexdesc, &pFlexWeights );
    //    cStudioHdr.RunFlexRules( m_pFlexControls, pFlexWeights );
    //    g_pStudioRender->UnlockFlexWeights();
    //}

    Vector vecModelOrigin;
    MatrixGetColumn( rootModel.m_MDLToWorld, 3, vecModelOrigin );
    g_pStudioRender->DrawModel( NULL, info, const_cast< matrix3x4_t * >( pBoneToWorld ), pFlexWeights, NULL, vecModelOrigin, STUDIORENDER_DRAW_ENTIRE_MODEL );

    //CStudioHdr studioHdr( g_pMDLCache->GetStudioHdr( rootModel.m_MDL.GetMDL() ), g_pMDLCache );
    //// SetupFlexWeights();

    //matrix3x4_t *pBoneToWorld = g_pStudioRender->LockBoneMatrices( studioHdr.numbones() );

    //float *poseParameters = NULL;
    //MDLSquenceLayer_t sequenceLayers[8];
    //int numSequenceLayers = 0;
    //V_memset( sequenceLayers, 0, sizeof( sequenceLayers ) );
    //rootModel.m_MDL.SetUpBones( rootModel.m_MDLToWorld, studioHdr.numbones(), pBoneToWorld, poseParameters, sequenceLayers, numSequenceLayers );
    //g_pStudioRender->UnlockBoneMatrices();

    //rootModel.m_MDL.Draw( rootModel.m_MDLToWorld, pBoneToWorld );
    /*DrawModelInfo_t info;
    info.m_nLocalLightCount = 0;
    info.m_bStaticLighting = true;
    info.m_Body = 0;
    info.m_HitboxSet = 0;

    float color[3];
    color[0] = color[1] = color[2] = 1.0f;
    g_pStudioRender->SetColorModulation( color );
    g_pStudioRender->SetAlphaModulation( 1.0f );

    DrawModelResults_t results;
    DrawModelState_t state = { 0 };
    state.m_pStudioHdr = g_pMDLCache->GetStudioHdr( rootModel.m_MDL.GetMDL() );
    state.m_drawFlags |= STUDIORENDER_DRAW_ENTIRE_MODEL;
    state.m_drawFlags |= STUDIORENDER_DRAW_STATIC_LIGHTING;
    state.m_drawFlags |= STUDIORENDER_DRAW_WIREFRAME;

    pRenderContext->MatrixMode( MATERIAL_MODEL );
    pRenderContext->PushMatrix();
    pRenderContext->LoadIdentity();
    g_pStudioRender->ClearAllShadows();
    pRenderContext->DisableAllLocalLights();

    info.m_pStudioHdr = state.m_pStudioHdr;
    info.m_Skin = m_iSkin;
    info.m_pClientEntity = static_cast< void * >( state.m_pRenderable );
    info.m_Lod = 0;*/

    //g_pStudioRender->DrawModelStaticProp( info, *pBoneToWorld, drawFlags );

    //g_pStudioRender->DrawModel( &results, info, pBoneToWorld, NULL, NULL, camera.m_origin, state.m_drawFlags );

    // Now get the pixels from the render target
    unsigned char *imageData = new unsigned char[outputWidth * outputHeight * 4];

    pRenderContext->ReadPixels( 0, 0, outputWidth, outputHeight, imageData, IMAGE_FORMAT_RGBA8888 );

    PNG_WriteToFile( pngPathFull, imageData, outputWidth, outputHeight );

    free( imageData );

    pRenderContext->CullMode( MATERIAL_CULLMODE_CW );
    pRenderContext->Flush();

    StudioRender()->UpdateConfig( oldStudioRenderConfig );

    //vgui::MatSystemSurface()->End3DPaint();

    vgui::MDLCache()->Release( modelHandle );

    SetModelImage( pngPath );
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
     if ( m_nTextureID == -1 )
         return;

     int wide, tall;
     GetSize( wide, tall );

     surface()->DrawSetColor( 255, 255, 255, 255 );
     surface()->DrawSetTexture( m_nTextureID );
     surface()->DrawTexturedRect( 0, 0, wide, tall );
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
    float fieldOfView = lua_tonumber( L, -1 );
    lua_pop( L, 1 );
    camera.m_flFOV = fieldOfView;

    lua_getfield( L, 2, "zNear" );
    float zNear = lua_tonumber( L, -1 );
    lua_pop( L, 1 );
    camera.m_flZNear = zNear;

    lua_getfield( L, 2, "zFar" );
    float zFar = lua_tonumber( L, -1 );
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
