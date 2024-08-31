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
#include <lbaseflex_shared.h>
#include <viewrender.h>
#include <filesystem.h>

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

LModelImagePanel::LModelImagePanel( Panel *parent, const char *panelName, lua_State *L /* = nullptr */ )
    : EditablePanel( parent, panelName, L )
{
    m_pszModelImagePath = NULL;
    m_pszModelPath = NULL;
    m_iSkin = 0;
    m_pszBodyGroups = NULL;
    m_nTextureID = -1;
}

LModelImagePanel::~LModelImagePanel()
{
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

static void SetupViewForEntity_Prop( lua_CBaseFlex *pEntity, CViewSetup &viewSetup, Vector vecPos, Vector vecMiddle, float flSize )
{
    QAngle viewAngle( 25, 220, 0 );
    Vector forward;
    AngleVectors( viewAngle, &forward );

    Vector vecViewPos = vecPos + forward * flSize * -15;
    viewSetup.fov = 4 + flSize * 0.04;
    viewSetup.origin = vecViewPos + vecMiddle;
    viewSetup.zNear = 1;
    viewSetup.zFar = vecViewPos.DistTo( vecPos ) + flSize * 2;
    viewSetup.angles = viewAngle;
}

static void SetupViewForEntity_Ragdoll( lua_CBaseFlex *pEntity, CViewSetup &viewSetup, Vector vecPos, Vector vecMiddle, float flSize )
{
    Vector vecViewPos;
    QAngle viewAngle;

    Vector attachmentOrigin;
    QAngle attachmentAngles;
    bool bFound = pEntity->GetAttachment( pEntity->LookupAttachment( "eyes" ), attachmentOrigin, attachmentAngles );

    if ( !bFound )
    {
        attachmentOrigin = pEntity->GetAbsOrigin();
        attachmentAngles = pEntity->GetAbsAngles();
    }

    Vector forward;
    Vector up;

    AngleVectors( viewAngle, &forward, NULL, &up );

    viewAngle = attachmentAngles + QAngle( -10, 160, 0 );
    vecViewPos = attachmentOrigin + forward * -60 + up * -2;

    viewSetup.fov = 10;
    viewSetup.origin = vecViewPos;
    viewSetup.zNear = 0.1;
    viewSetup.zFar = 100;
    viewSetup.angles = viewAngle;
}

static void SetupViewForEntity_Ragdoll_Head( lua_CBaseFlex *pEntity, CViewSetup &viewSetup, Vector vecPos, Vector vecMiddle, float flSize )
{
    Vector vecViewPos;
    QAngle viewAngle;

    Vector attachmentOrigin;
    QAngle attachmentAngles;
    bool bFound = pEntity->GetAttachment( pEntity->LookupAttachment( "head" ), attachmentOrigin, attachmentAngles );

    if ( !bFound )
    {
        attachmentOrigin = pEntity->GetAbsOrigin();
        attachmentAngles = pEntity->GetAbsAngles();
    }

    Vector forward;
    Vector up;

    AngleVectors( viewAngle, &forward, NULL, &up );

    viewAngle = attachmentAngles + QAngle( -10, 160, 0 );
    vecViewPos = attachmentOrigin + forward * -67 + up * -7 + forward * 1.5;

    viewSetup.fov = 10;
    viewSetup.origin = vecViewPos;
    viewSetup.zNear = 0.1;
    viewSetup.zFar = 100;
    viewSetup.angles = viewAngle;
}

static void SetupViewForEntity_Ragdoll_Facemask( lua_CBaseFlex *pEntity, CViewSetup &viewSetup, Vector vecPos, Vector vecMiddle, float flSize )
{
    Vector vecViewPos;
    QAngle viewAngle;

    Vector attachmentOrigin;
    QAngle attachmentAngles;
    bool bFound = pEntity->GetAttachment( pEntity->LookupAttachment( "facemask" ), attachmentOrigin, attachmentAngles );

    if ( !bFound )
    {
        attachmentOrigin = pEntity->GetAbsOrigin();
        attachmentAngles = pEntity->GetAbsAngles();
    }

    Vector forward;
    Vector right;
    Vector up;

    AngleVectors( viewAngle, &forward, &right, &up );

    viewAngle = attachmentAngles;

    matrix3x4_t matrix;
    MatrixBuildRotationAboutAxis( right, -10, matrix );
    MatrixBuildRotationAboutAxis( up, 160, matrix );
    QAngle newAngle;
    MatrixToAngles( matrix, newAngle );

    viewAngle = newAngle;

    vecViewPos = attachmentOrigin + forward * -67 + up * -2 + right * -1;

    viewSetup.fov = 10;
    viewSetup.origin = vecViewPos;
    viewSetup.zNear = 0.1;
    viewSetup.zFar = 100;
    viewSetup.angles = viewAngle;
}

static void SetupViewForEntity_Ragdoll_Forward( lua_CBaseFlex *pEntity, CViewSetup &viewSetup, Vector vecPos, Vector vecMiddle, float flSize )
{
    Vector vecViewPos;
    QAngle viewAngle;

    Vector attachmentOrigin;
    QAngle attachmentAngles;
    bool bFound = pEntity->GetAttachment( pEntity->LookupAttachment( "forward" ), attachmentOrigin, attachmentAngles );

    if ( !bFound )
    {
        attachmentOrigin = pEntity->GetAbsOrigin();
        attachmentAngles = pEntity->GetAbsAngles();
    }

    Vector forward;
    Vector right;
    Vector up;

    AngleVectors( viewAngle, &forward, &right, &up );

    viewAngle = attachmentAngles + QAngle( 10, -20, 0 );
    vecViewPos = attachmentOrigin + forward * -67 + up * -1 + right * 2.5;

    viewSetup.fov = 10;
    viewSetup.origin = vecViewPos;
    viewSetup.zNear = 0.1;
    viewSetup.zFar = 100;
    viewSetup.angles = viewAngle;
}

static void SetupViewForEntity_DOD( lua_CBaseFlex *pEntity, CViewSetup &viewSetup, Vector vecPos, Vector vecMiddle, float flSize )
{
    Vector vecViewPos;
    QAngle viewAngle;

    Vector forward;
    Vector right;
    Vector up;

    AngleVectors( viewAngle, &forward, &right, &up );

    viewAngle = QAngle( 0, 160, 0 );
    vecViewPos = vecPos + forward * -67 + up * 30 + right * 2.5;

    viewSetup.fov = 10;
    viewSetup.origin = vecViewPos + vecMiddle;
    viewSetup.zNear = 1;
    viewSetup.zFar = vecViewPos.DistTo( vecPos ) + flSize * 2;
    viewSetup.angles = viewAngle;
}

static void SetupViewForEntity_CS( lua_CBaseFlex *pEntity, CViewSetup &viewSetup, Vector vecPos, Vector vecMiddle, float flSize )
{
    Vector vecViewPos;
    QAngle viewAngle;

    Vector forward;
    Vector right;
    Vector up;

    AngleVectors( viewAngle, &forward, &right, &up );

    viewAngle = QAngle( 0, 160, 0 );
    vecViewPos = vecPos + forward * -70 + up * 32.4 + right * 1.5;

    viewSetup.fov = 10;
    viewSetup.origin = vecViewPos + vecMiddle;
    viewSetup.zNear = 1;
    viewSetup.zFar = vecViewPos.DistTo( vecPos ) + flSize * 2;
    viewSetup.angles = viewAngle;
}

static void SetupViewForEntity_Special( lua_CBaseFlex *pEntity, CViewSetup &viewSetup, Vector vecPos, Vector vecMiddle, float flSize, float x, float y, float z )
{
    Vector vecViewPos;
    QAngle viewAngle;

    Vector forward;
    Vector right;
    Vector up;

    AngleVectors( viewAngle, &forward, &right, &up );

    viewAngle = QAngle( 15, 140, 0 );
    vecViewPos = vecPos + forward * x + up * y + right * z;

    viewSetup.fov = 20;
    viewSetup.origin = vecViewPos + vecMiddle;
    viewSetup.zNear = 1;
    viewSetup.zFar = vecViewPos.DistTo( vecPos ) + flSize * 2;
    viewSetup.angles = viewAngle;
}

static void SetupViewForEntity( lua_CBaseFlex *pEntity, CViewSetup &viewSetup )
{
    Vector vecPos = pEntity->GetAbsOrigin();
    Vector vecMins, vecMaxs;
    pEntity->GetRenderBounds( vecMins, vecMaxs );
    Vector vecMiddle = ( vecMins + vecMaxs ) * 0.5;
    float flSize = 0;
    flSize = MAX( flSize, fabs( vecMins.x ) + fabs( vecMaxs.x ) );
    flSize = MAX( flSize, fabs( vecMins.y ) + fabs( vecMaxs.y ) );
    flSize = MAX( flSize, fabs( vecMins.z ) + fabs( vecMaxs.z ) );

    pEntity->SetAbsOrigin( vecPos );

    if ( pEntity->LookupAttachment( "eyes" ) )
    {
        SetupViewForEntity_Ragdoll( pEntity, viewSetup, vecPos, vecMiddle, flSize );
    }
    else if ( pEntity->LookupAttachment( "head" ) )
    {
        SetupViewForEntity_Ragdoll_Head( pEntity, viewSetup, vecPos, vecMiddle, flSize );
    }
    else if ( pEntity->LookupAttachment( "facemask" ) )
    {
        SetupViewForEntity_Ragdoll_Facemask( pEntity, viewSetup, vecPos, vecMiddle, flSize );
    }
    else if ( pEntity->LookupAttachment( "forward" ) )
    {
        SetupViewForEntity_Ragdoll_Forward( pEntity, viewSetup, vecPos, vecMiddle, flSize );
    }
    else
    {
        SetupViewForEntity_Prop( pEntity, viewSetup, vecPos, vecMiddle, flSize );
    }
}

/// <summary>
/// This function is used to render a model to a PNG file given a camera setup.
/// If no camera is given, it will try calculate the best fit view for the model.
/// If pszSavePath is not NULL, the PNG file will be saved to that path. If it is NULL, the PNG
/// file will be saved to materials/spawnicons/<path to the model relative to GAME root>.png
/// </summary>
/// <param name="camera"></param>
/// <param name="pszSavePath"></param>
void LModelImagePanel::RebuildSpawnIcon( Camera_t *camera /* = NULL*/, const char *pszSavePath /* = NULL*/ )
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

    if ( pEntity == NULL )
        return;

    pEntity->DontRecordInTools();
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
    pRenderContext->MatrixMode( MATERIAL_MODEL );
    pRenderContext->LoadIdentity();

    pRenderContext->MatrixMode( MATERIAL_PROJECTION );
    pRenderContext->PushMatrix();

    pRenderContext->MatrixMode( MATERIAL_VIEW );
    pRenderContext->PushMatrix();

    pRenderContext->PushRenderTargetAndViewport( NULL, 0, 0, outputWidth, outputHeight );

    pRenderContext->ClearColor4ub( 0, 0, 0, 0 );
    pRenderContext->ClearBuffers( true, true, true );

    CViewSetup viewSetup;
    memset( &viewSetup, 0, sizeof( viewSetup ) );
    viewSetup.x = 0;
    viewSetup.y = 0;
    viewSetup.width = outputWidth;
    viewSetup.height = outputHeight;

    if ( camera )
    {
        viewSetup.fov = camera->m_flFOV;
        viewSetup.origin = camera->m_origin;
        viewSetup.angles = camera->m_angles;
        viewSetup.zNear = camera->m_flZNear;
        viewSetup.zFar = camera->m_flZFar;
    }
    else
    {
        SetupViewForEntity( pEntity, viewSetup );
    }

    viewSetup.m_eStereoEye = STEREO_EYE_MONO;

    Frustum dummyFrustum;
    render->Push3DView( viewSetup, 0, NULL, dummyFrustum );

    pRenderContext->FogMode( MATERIAL_FOG_NONE );
    pRenderContext->SetLightingOrigin( vec3_origin );
    pRenderContext->SetAmbientLight( 0.6, 0.6, 0.6 );

    // front, back, right, left, top, bottom?
    static Vector semiBright[6] =
        {
            Vector( 0.6, 0.6, 0.6 ),
            Vector( 0.4, 0.4, 0.4 ),
            Vector( 0.6, 0.6, 0.6 ),
            Vector( 0.6, 0.6, 0.6 ),
            Vector( 0.6, 0.6, 0.6 ),
            Vector( 0.4, 0.4, 0.4 ),
        };

    g_pStudioRender->SetAmbientLightColors( semiBright );
    g_pStudioRender->SetLocalLights( 0, NULL );

    Vector vecMins, vecMaxs;
    pEntity->GetRenderBounds( vecMins, vecMaxs );
    Vector vecEntityPosition = pEntity->GetAbsOrigin();

    Vector vecAboveEntity = vecEntityPosition + Vector( 0, 0, vecMaxs.z - vecMins.z ) + Vector( 0, 64, 200 );
    Vector vecEntityAboveCenter = pEntity->GetAbsOrigin() + Vector( 0, 0, ( vecMaxs.z - vecMins.z ) * 0.75 );

    LightDesc_t pLight;
    memset( &pLight, 0, sizeof( pLight ) );
    pLight.m_Type = MATERIAL_LIGHT_SPOT;
    pLight.m_Color.Init( 1, 1, 1 );
    pLight.m_Position = vecAboveEntity;
    pLight.m_Direction = vecEntityAboveCenter - vecAboveEntity;
    VectorNormalizeFast( pLight.m_Direction );
    pLight.m_Attenuation0 = 1;
    pLight.m_Attenuation1 = 0;
    pLight.m_Attenuation2 = 0;
    pLight.m_Theta = DEG2RAD( 30 );
    pLight.m_Phi = DEG2RAD( 45 );
    pLight.m_Range = 0.0;
    pLight.RecalculateDerivedValues();

    g_pStudioRender->SetLocalLights( 1, &pLight );

    modelrender->SuppressEngineLighting( true );

    float color[3] = { 1.0f, 1.0f, 1.0f };
    render->SetColorModulation( color );
    render->SetBlend( 1.0f );

    pRenderContext->SetIntRenderingParameter( INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA, false );

    pRenderContext->OverrideAlphaWriteEnable( true, true );

    pEntity->DrawModel( STUDIO_RENDER );

    unsigned char *pImage = ( unsigned char * )malloc( outputWidth * outputHeight * 4 );

    pRenderContext->ReadPixels( 0, 0, outputWidth, outputHeight, pImage, IMAGE_FORMAT_RGBA8888 );

    PNG_WriteToFile( pngPathFull, pImage, outputWidth, outputHeight );

    modelrender->SuppressEngineLighting( false );

    render->PopView( dummyFrustum );

    free( pImage );

    // restore our previous state
    pRenderContext->OverrideAlphaWriteEnable( false, true );
    pRenderContext->PopRenderTargetAndViewport();

    pRenderContext->MatrixMode( MATERIAL_PROJECTION );
    pRenderContext->PopMatrix();

    pRenderContext->MatrixMode( MATERIAL_VIEW );
    pRenderContext->PopMatrix();

    m_pszModelImagePath = nullptr;
    SetModelImage( pngPath );

    pEntity->Remove();
    pEntity = NULL;
}

/// <summary>
/// Loads the model image if it exists in the spawnicons directory already.
/// </summary>
void LModelImagePanel::LoadIfExistsOrRebuild()
{
    char pngPathFull[MAX_PATH];
    char pngPath[MAX_PATH];

    // Build the path relative to the icon texture
    Q_snprintf( pngPathFull, sizeof( pngPathFull ), "materials/spawnicons/%s", m_pszModelPath );
    Q_StripExtension( pngPathFull, pngPathFull, sizeof( pngPathFull ) );  // strip .mdl
    Q_DefaultExtension( pngPathFull, ".png", sizeof( pngPathFull ) );

    // trim materials/ from the front
    Q_strncpy( pngPath, pngPathFull + 10, sizeof( pngPath ) );

    if ( filesystem->FileExists( pngPathFull, "GAME" ) )
    {
        SetModelImage( pngPath );
    }
    else
    {
        // TODO: Async this
        RebuildSpawnIcon();
    }
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

    if ( m_nTextureID != -1 )
    {
        surface()->DestroyTextureID( m_nTextureID );
    }

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
LUALIB_API lua_ModelImagePanel *luaL_checkmodelimagepanel( lua_State *L, int narg )
{
    lua_ModelImagePanel *d = lua_tomodelimagepanel( L, narg );

    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ModelImagePanel expected, got INVALID_PANEL" );

    return d;
}

LUA_REGISTRATION_INIT( ModelImagePanel )

LUA_BINDING_BEGIN( ModelImagePanel, GetPanelBaseClassName, "class|static", "Gets the base class name of the panel" )
{
    lua_pushstring( L, lua_ModelImagePanel::GetPanelBaseClassName() );
    return 1;
}
LUA_BINDING_END( "string", "The base class name of the panel" )

LUA_BINDING_BEGIN( ModelImagePanel, GetPanelClassName, "class|static", "Gets the class name of the panel" )
{
    lua_pushstring( L, lua_ModelImagePanel::GetPanelClassName() );
    return 1;
}
LUA_BINDING_END( "string", "The class name of the panel" )

LUA_BINDING_BEGIN( ModelImagePanel, SetModel, "class", "Sets the model for the panel" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( luaL_checkmodelimagepanel, 1, "modelImagePanel" );
    const char *modelPath = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "modelPath" );
    int skin = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "skin" );
    const char *bodyGroups = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 4, NULL, "bodyGroups" );

    pModelImagePanel->SetModel( modelPath, skin, bodyGroups );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ModelImagePanel, GetModel, "class", "Gets the model path from the panel" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( luaL_checkmodelimagepanel, 1, "modelImagePanel" );
    lua_pushstring( L, pModelImagePanel->GetModel() );
    return 1;
}
LUA_BINDING_END( "string", "The model path" )

LUA_BINDING_BEGIN( ModelImagePanel, SetSkin, "class", "Sets the skin for the model" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( luaL_checkmodelimagepanel, 1, "modelImagePanel" );
    int skin = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "skin" );
    pModelImagePanel->SetSkin( skin );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ModelImagePanel, GetSkin, "class", "Gets the skin index from the model" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( luaL_checkmodelimagepanel, 1, "modelImagePanel" );
    lua_pushinteger( L, pModelImagePanel->GetSkin() );
    return 1;
}
LUA_BINDING_END( "integer", "The skin index" )

LUA_BINDING_BEGIN( ModelImagePanel, SetBodyGroups, "class", "Sets the body groups for the model" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( luaL_checkmodelimagepanel, 1, "modelImagePanel" );
    const char *bodyGroups = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "bodyGroups" );
    pModelImagePanel->SetBodyGroups( bodyGroups );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ModelImagePanel, GetBodyGroups, "class", "Gets the body groups from the model" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( luaL_checkmodelimagepanel, 1, "modelImagePanel" );
    lua_pushstring( L, pModelImagePanel->GetBodyGroups() );
    return 1;
}
LUA_BINDING_END( "string", "The body groups" )

LUA_BINDING_BEGIN( ModelImagePanel, RebuildSpawnIcon, "class", "Rebuilds the spawn icon with the specified camera settings" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( luaL_checkmodelimagepanel, 1, "modelImagePanel" );
    const char *pszSavePath = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, NULL, "savePath" );
    Camera_t camera;

    if ( !LUA_BINDING_ARGUMENT( lua_istable, 2, "data" ) )
    {
        luaL_argerror( L, 2, "table expected" );
        return 0;
    }

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

    pModelImagePanel->RebuildSpawnIcon( &camera, pszSavePath );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ModelImagePanel, SetModelImage, "class", "Sets the image for the model" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( luaL_checkmodelimagepanel, 1, "modelImagePanel" );
    const char *imagePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "imagePath" );
    pModelImagePanel->SetModelImage( imagePath );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ModelImagePanel, __index, "class", "Metamethod called when a non-existent field is indexed" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( lua_tomodelimagepanel, 1, "modelImagePanel" );

    LUA_METATABLE_INDEX_CHECK_VALID( L, PanelIsValid );
    LUA_METATABLE_INDEX_CHECK( L, pModelImagePanel );

    // const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pModelImagePanel );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "ModelImagePanel" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    LUA_METATABLE_INDEX_DERIVE_INDEX( L, "Panel" );

    lua_pushnil( L );
    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( ModelImagePanel, __newindex, "class", "Metamethod called when a new field is added to the panel" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( lua_tomodelimagepanel, 1, "modelImagePanel" );

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
    lua_setfield( L, -2, LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" ) );
    lua_pop( L, 1 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ModelImagePanel, __eq, "class", "Metamethod called when two panels are compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( lua_tomodelimagepanel, 1, "modelImagePanelA" ) == LUA_BINDING_ARGUMENT( lua_tomodelimagepanel, 2, "modelImagePanelB" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the panels are equal" )

LUA_BINDING_BEGIN( ModelImagePanel, __tostring, "class", "Metamethod called when the panel is converted to a string" )
{
    lua_ModelImagePanel *pModelImagePanel = LUA_BINDING_ARGUMENT( lua_tomodelimagepanel, 1, "modelImagePanel" );

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
LUA_BINDING_END()

LUA_BINDING_BEGIN( ModelImagePanel, __gc, "class", "Metamethod that is called when the panel is garbage collected" )
{
    return PanelCollectGarbage( L );
}
LUA_BINDING_END()

LUA_REGISTRATION_INIT( Panels )

LUA_BINDING_BEGIN( Panels, ModelImagePanel, "library", "Creates a new model image panel" )
{
    Panel *parent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optpanel, 1, VGui_GetClientLuaRootPanel(), "parent" );
    const char *name = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "ModelImagePanel", "name" );
    lua_ModelImagePanel *pPanel = new lua_ModelImagePanel( parent, name, L );
    LModelImagePanel::PushLuaInstanceSafe( L, pPanel );
    return 1;
}
LUA_BINDING_END( "ModelImagePanel", "The new model image panel" )

/*
** Open ModelImagePanel object
*/
LUALIB_API int luaopen_vgui_ModelImagePanel( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, "ModelImagePanel" );

    LUA_REGISTRATION_COMMIT( ModelImagePanel );

    lua_pushstring( L, LUA_PANELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Panels );

    lua_pop( L, 2 );
    return 0;
}
