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
#include <lc_baseflex.h>
#include <viewrender.h>

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
    m_nTextureID = -1;
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
    char pngPath[MAX_PATH];

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

    // trim materials/ from the front
    Q_strncpy( pngPath, pngPathFull + 10, sizeof( pngPath ) );

    lua_CBaseFlex *pEntity = g_pClientSideEntityManager->CreateClientSideEntity( m_pszModelPath, RENDER_GROUP_OTHER );
    pEntity->AddEffects( EF_NODRAW );
    pEntity->m_nSkin = m_iSkin;

    if ( m_pszBodyGroups != NULL )
    {
        int i = 0;
        while ( m_pszBodyGroups[i] != '\0' && i < pEntity->GetNumBodyGroups() )
        {
            char value = m_pszBodyGroups[i];

            if ( value >= '0' && value <= '9' )
                pEntity->SetBodygroup( i, value - '0' );
            else if ( value >= 'a' && value <= 'z' )
                pEntity->SetBodygroup( i, value - 'a' + 10 );

            i++;
        }
    }

    int x, y, w, h;
    GetBounds( x, y, w, h );
    ParentLocalToScreen( x, y );

    int outputWidth = 256;
    int outputHeight = 256;

    CMatRenderContextPtr pRenderContext( materials );
    pRenderContext->MatrixMode( MATERIAL_PROJECTION );
    pRenderContext->PushMatrix();

    pRenderContext->MatrixMode( MATERIAL_VIEW );
    pRenderContext->PushMatrix();

    pRenderContext->PushRenderTargetAndViewport( NULL, 0, 0, outputWidth, outputHeight );

    pRenderContext->ClearColor4ub( 0, 0, 0, 0 );
    pRenderContext->ClearBuffers( true, false );

    CViewSetup viewSetup;
    memset( &viewSetup, 0, sizeof( viewSetup ) );
    viewSetup.x = 0;
    viewSetup.y = 0;
    viewSetup.width = outputWidth;
    viewSetup.height = outputHeight;
    viewSetup.fov = ScaleFOVByWidthRatio( camera.m_flFOV, ( ( float )outputWidth / ( float )outputHeight ) / ( 4.0f / 3.0f ) );
    viewSetup.origin = camera.m_origin;
    viewSetup.angles = camera.m_angles;
    viewSetup.zNear = camera.m_flZNear;
    viewSetup.zFar = camera.m_flZFar;
    viewSetup.m_eStereoEye = STEREO_EYE_MONO;

    Frustum dummyFrustum;
    render->Push3DView( viewSetup, 0, NULL, dummyFrustum );

    modelrender->SuppressEngineLighting( true );
    // pRenderContext->FogMode( MATERIAL_FOG_NONE );
    // pRenderContext->SetAmbientLight( 255, 255, 255 );
    //float color[3] = { 1.0f, 1.0f, 1.0f };
    //render->SetColorModulation( color );
    //render->SetBlend( 1.0f );

    //pRenderContext->OverrideAlphaWriteEnable( true, true );
    pRenderContext->SetIntRenderingParameter( INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA, false );

    pEntity->DrawModel( STUDIO_RENDER );

    //pRenderContext->OverrideAlphaWriteEnable( false, true );

    unsigned char *pImage = ( unsigned char * )malloc( outputWidth * outputHeight * 4 );

    pRenderContext->ReadPixels( 0, 0, outputWidth, outputHeight, pImage, IMAGE_FORMAT_RGBA8888 );

    PNG_WriteToFile( pngPathFull, pImage, outputWidth, outputHeight );

    modelrender->SuppressEngineLighting( false );

    render->PopView( dummyFrustum );

    free( pImage );

    // restore our previous state
    pRenderContext->PopRenderTargetAndViewport();

    pRenderContext->MatrixMode( MATERIAL_PROJECTION );
    pRenderContext->PopMatrix();

    pRenderContext->MatrixMode( MATERIAL_VIEW );
    pRenderContext->PopMatrix();

    pEntity->AddEFlags( EFL_KILLME );

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
