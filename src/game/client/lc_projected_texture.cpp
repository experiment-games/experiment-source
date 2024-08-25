#include "cbase.h"
#include "lc_projected_texture.h"
#include "fmtstr.h"
#include "lua.hpp"
#include "luasrclib.h"
#include <lColor.h>
#include <litexture.h>
#include <lbaseentity_shared.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_ProjectedTexture *lua_toprojectedtexture( lua_State *L, int idx )
{
    lua_ProjectedTexture **ppProjectedTexture = ( lua_ProjectedTexture ** )lua_touserdata( L, idx );
    return *ppProjectedTexture;
}

/*
** push functions (C -> stack)
*/

LUALIB_API lua_ProjectedTexture *luaL_checkprojectedtexture( lua_State *L, int narg )
{
    lua_ProjectedTexture **ppProjectedTexture = ( lua_ProjectedTexture ** )lua_touserdata( L, narg );

    if ( *ppProjectedTexture == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ProjectedTexture expected, got invalid object" );

    return *ppProjectedTexture;
}

LUALIB_API lua_ProjectedTexture *luaL_optprojectedtexture( lua_State *L, int narg, lua_ProjectedTexture *def )
{
    return luaL_opt( L, luaL_checkprojectedtexture, narg, def );
}

/*
** type for ProjectedTexture functions, based on C_EnvProjectedTexture
*/
void *LProjectedTexture::CreateLuaInstance( lua_State *L, LProjectedTexture *pInstance )
{
    lua_ProjectedTexture **ppProjectedTexture =
        ( lua_ProjectedTexture ** )lua_newuserdata( L, sizeof( lua_ProjectedTexture ) );
    *ppProjectedTexture = pInstance;
    return ppProjectedTexture;
}

LProjectedTexture::LProjectedTexture( void )
{
    m_LightHandle = CLIENTSHADOW_INVALID_HANDLE;
    m_nTableReference = LUA_NOREF;

    m_angRotation.Init();
    m_vecOrigin.Init();

    m_FlashlightState = FlashlightState_t();
}

LProjectedTexture::~LProjectedTexture( void )
{
    ShutDownLightHandle();

    if ( L )
        lua_unref( L, m_nTableReference );
}

void LProjectedTexture::SetupRefTable( lua_State *L )
{
    lua_newtable( L );
    m_nTableReference = luaL_ref( L, LUA_REGISTRYINDEX );
}

void LProjectedTexture::ShutDownLightHandle( void )
{
    m_bIsValid = false;

    if ( m_LightHandle != CLIENTSHADOW_INVALID_HANDLE )
    {
        g_pClientShadowMgr->DestroyFlashlight( m_LightHandle );
        m_LightHandle = CLIENTSHADOW_INVALID_HANDLE;
    }
}

/// <summary>
/// Forces the projected texture to update based on the values now in
/// the flashlight state.
/// </summary>
void LProjectedTexture::ForceUpdateLight()
{
    Vector vForward, vRight, vUp, vPos = GetAbsOrigin();
    FlashlightState_t &state = m_FlashlightState;

    if ( m_hTargetEntity != NULL )
    {
        const QAngle &angles = GetAbsAngles();

        C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
        if ( pPlayer )
        {
            const QAngle playerAngles = pPlayer->GetAbsAngles();

            Vector vPlayerForward, vPlayerRight, vPlayerUp;
            AngleVectors( playerAngles, &vPlayerForward, &vPlayerRight, &vPlayerUp );

            matrix3x4_t mRotMatrix;
            AngleMatrix( angles, mRotMatrix );

            VectorITransform( vPlayerForward, mRotMatrix, vForward );
            VectorITransform( vPlayerRight, mRotMatrix, vRight );
            VectorITransform( vPlayerUp, mRotMatrix, vUp );

            float dist = ( m_hTargetEntity->GetAbsOrigin() - GetAbsOrigin() ).Length();
            vPos = m_hTargetEntity->GetAbsOrigin() - vForward * dist;

            VectorNormalize( vForward );
            VectorNormalize( vRight );
            VectorNormalize( vUp );
        }
    }
    else
    {
        AngleVectors( GetAbsAngles(), &vForward, &vRight, &vUp );
    }

    state.m_vecLightOrigin = vPos;
    BasisToQuaternion( vForward, vRight, vUp, state.m_quatOrientation );

    if ( m_LightHandle == CLIENTSHADOW_INVALID_HANDLE )
    {
        m_LightHandle = g_pClientShadowMgr->CreateFlashlight( state );
    }
    else
    {
        if ( m_hTargetEntity != NULL )
        {
            g_pClientShadowMgr->UpdateFlashlightState( m_LightHandle, state );
        }
    }

    if ( m_hTargetEntity )
    {
        g_pClientShadowMgr->SetFlashlightTarget( m_LightHandle, m_hTargetEntity );
    }

    g_pClientShadowMgr->SetFlashlightLightWorld( m_LightHandle, m_bLightWorld );

    //if ( bForceUpdate == false )
    //{
    //    g_pClientShadowMgr->UpdateProjectedTexture( m_LightHandle, true );
    //}

    m_bIsValid = true;
}

LUA_REGISTRATION_INIT( ProjectedTexture )

LUA_BINDING_BEGIN( ProjectedTexture, GetAngles, "class", "Get the angle of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushangle( L, projectedTexture->GetAbsAngles() );

    return 1;
}
LUA_BINDING_END( "Angle", "The angle of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetBrightness, "class", "Get the brightness of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fBrightnessScale );

    return 1;
}
LUA_BINDING_END( "number", "The brightness of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetColor, "class", "Get the color of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_Color color( projectedTexture->m_FlashlightState.m_Color );
    lua_pushcolor( L, color );

    return 1;
}
LUA_BINDING_END( "Color", "The color of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetConstantAttenuation, "class", "Get the constant attenuation of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fConstantAtten );

    return 1;
}
LUA_BINDING_END( "number", "The constant attenuation of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetEnableShadows, "class", "Get whether shadows are enabled for this projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushboolean( L, projectedTexture->m_FlashlightState.m_bEnableShadows );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether shadows are enabled for this projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetFarZ, "class", "Get the projection distance of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_FarZ );

    return 1;
}
LUA_BINDING_END( "number", "The projection distance of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetHorizontalFov, "class", "Get the horizontal FOV of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fHorizontalFOVDegrees );

    return 1;
}
LUA_BINDING_END( "number", "The horizontal FOV of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetLightWorld, "class", "Get whenever or not the Texture should light up world geometry." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushboolean( L, projectedTexture->m_bLightWorld );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the texture should light up world geometry." )

LUA_BINDING_BEGIN( ProjectedTexture, GetLinearAttenuation, "class", "Get the linear attenuation of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fLinearAtten );

    return 1;
}
LUA_BINDING_END( "number", "The linear attenuation of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetNearZ, "class", "Get the NearZ value of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_NearZ );

    return 1;
}
LUA_BINDING_END( "number", "The NearZ value of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetScissorRectangle, "class", "Get the scissor settings of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushboolean( L, projectedTexture->m_FlashlightState.m_bScissor );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_nLeft );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_nTop );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_nRight );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_nBottom );

    return 5;
}
LUA_BINDING_END( "boolean", "Whether the projected texture is cut off.", "number", "The left margin", "number", "The top margin", "number", "The right margin", "number", "The bottom margin" )

LUA_BINDING_BEGIN( ProjectedTexture, GetOrthographic, "class", "Returns the current orthographic settings of the Projected Texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushboolean( L, projectedTexture->m_FlashlightState.m_bOrtho );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fOrthoLeft );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fOrthoTop );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fOrthoRight );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fOrthoBottom );

    return 5;
}
LUA_BINDING_END( "boolean", "Whether the projected texture is orthographic.", "number", "The left margin", "number", "The top margin", "number", "The right margin", "number", "The bottom margin" )

LUA_BINDING_BEGIN( ProjectedTexture, GetPosition, "class", "Get the position of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushvector( L, projectedTexture->GetAbsOrigin() );

    return 1;
}
LUA_BINDING_END( "Vector", "The position of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetQuadraticAttenuation, "class", "Get the quadratic attenuation of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fQuadraticAtten );

    return 1;
}
LUA_BINDING_END( "number", "The quadratic attenuation of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetShadowDepthBias, "class", "Get the shadow depth bias of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_flShadowDepthBias );

    return 1;
}
LUA_BINDING_END( "number", "The shadow depth bias of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetShadowFilter, "class", "Get the shadow filter size of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_flShadowFilterSize );

    return 1;
}
LUA_BINDING_END( "number", "The shadow filter size of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetShadowSlopeScaleDepthBias, "class", "Get the shadow depth slope scale bias of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_flShadowSlopeScaleDepthBias );

    return 1;
}
LUA_BINDING_END( "number", "The shadow depth slope scale bias of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetTargetEntity, "class", "Get the target entity of this projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    CBaseEntity::PushLuaInstanceSafe( L, projectedTexture->m_hTargetEntity );

    return 1;
}
LUA_BINDING_END( "Entity", "The target entity of this projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetTexture, "class", "Get the texture of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushitexture( L, projectedTexture->m_FlashlightState.m_pSpotlightTexture );

    return 1;
}
LUA_BINDING_END( "ITexture", "The texture of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetTextureFrame, "class", "Get the texture frame of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_nSpotlightTextureFrame );

    return 1;
}
LUA_BINDING_END( "number", "The texture frame of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, GetVerticalFov, "class", "Get the vertical FOV of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushnumber( L, projectedTexture->m_FlashlightState.m_fVerticalFOVDegrees );

    return 1;
}
LUA_BINDING_END( "number", "The vertical FOV of the projected texture." )

LUA_BINDING_BEGIN( ProjectedTexture, IsValid, "class", "Check if the projected texture is valid." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushboolean( L, projectedTexture->m_bIsValid );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the projected texture is valid." )

LUA_BINDING_BEGIN( ProjectedTexture, Remove, "class", "Removes the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    projectedTexture->ShutDownLightHandle();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetAngles, "class", "Set the angles (direction) of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const QAngle &angles = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" );
    projectedTexture->m_angRotation = angles;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetBrightness, "class", "Set the brightness of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float brightness = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "brightness" );
    projectedTexture->m_FlashlightState.m_fBrightnessScale = brightness;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetColor, "class", "Set the color of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const lua_Color &color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 2, "color" );
    projectedTexture->m_FlashlightState.m_Color[0] = color.r();
    projectedTexture->m_FlashlightState.m_Color[1] = color.g();
    projectedTexture->m_FlashlightState.m_Color[2] = color.b();
    projectedTexture->m_FlashlightState.m_Color[3] = color.a();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetConstantAttenuation, "class", "Set the constant attenuation of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float constAtten = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "constAtten" );
    projectedTexture->m_FlashlightState.m_fConstantAtten = constAtten;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetEnableShadows, "class", "Enable or disable shadows cast from the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const bool newState = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "newState" );
    projectedTexture->m_FlashlightState.m_bEnableShadows = newState;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetFarZ, "class", "Set the distance at which the projected texture ends." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float farZ = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "farZ" );
    projectedTexture->m_FlashlightState.m_FarZ = farZ;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetFov, "class", "Set the angle of projection." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float fov = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "fov" );
    projectedTexture->m_FlashlightState.m_fHorizontalFOVDegrees = fov;
    projectedTexture->m_FlashlightState.m_fVerticalFOVDegrees = fov;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetHorizontalFov, "class", "Set the horizontal angle of projection without affecting the vertical angle." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float hFov = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "horizontalFov" );
    projectedTexture->m_FlashlightState.m_fHorizontalFOVDegrees = hFov;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetLightWorld, "class", "Set whenever or not the Texture should light up world geometry." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const bool lightWorld = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "lightWorld" );
    projectedTexture->m_bLightWorld = lightWorld;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetLinearAttenuation, "class", "Set the linear attenuation of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float linearAtten = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "linearAtten" );
    projectedTexture->m_FlashlightState.m_fLinearAtten = linearAtten;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetNearZ, "class", "Set the distance at which the projected texture begins its projection." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float nearZ = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "nearZ" );
    projectedTexture->m_FlashlightState.m_NearZ = nearZ;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetScissorRectangle, "class", "Set the scissor settings of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const bool scissor = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "scissor" );
    const int left = LUA_BINDING_ARGUMENT( luaL_checkint, 3, "left" );
    const int top = LUA_BINDING_ARGUMENT( luaL_checkint, 4, "top" );
    const int right = LUA_BINDING_ARGUMENT( luaL_checkint, 5, "right" );
    const int bottom = LUA_BINDING_ARGUMENT( luaL_checkint, 6, "bottom" );

    projectedTexture->m_FlashlightState.m_bScissor = scissor;
    projectedTexture->m_FlashlightState.m_nLeft = left;
    projectedTexture->m_FlashlightState.m_nTop = top;
    projectedTexture->m_FlashlightState.m_nRight = right;
    projectedTexture->m_FlashlightState.m_nBottom = bottom;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetOrthographic, "class", "Changes the current projected texture between orthographic and perspective projection." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const bool orthographic = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "orthographic" );
    const float left = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "left" );
    const float top = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "top" );
    const float right = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "right" );
    const float bottom = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "bottom" );

    projectedTexture->m_FlashlightState.m_bOrtho = orthographic;
    projectedTexture->m_FlashlightState.m_fOrthoLeft = left;
    projectedTexture->m_FlashlightState.m_fOrthoTop = top;
    projectedTexture->m_FlashlightState.m_fOrthoRight = right;
    projectedTexture->m_FlashlightState.m_fOrthoBottom = bottom;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetPosition, "class", "Move the Projected Texture to the specified position." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const Vector &position = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" );
    projectedTexture->SetAbsOrigin( position );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetQuadraticAttenuation, "class", "Set the quadratic attenuation of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float quadAtten = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "quadAtten" );
    projectedTexture->m_FlashlightState.m_fQuadraticAtten = quadAtten;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetShadowDepthBias, "class", "Set the shadow depth bias of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float bias = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bias" );
    projectedTexture->m_FlashlightState.m_flShadowDepthBias = bias;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetShadowFilter, "class", "Set the shadow filter size of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float filter = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "filter" );
    projectedTexture->m_FlashlightState.m_flShadowFilterSize = filter;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetShadowSlopeScaleDepthBias, "class", "Set the shadow depth slope scale bias of the projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float bias = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bias" );
    projectedTexture->m_FlashlightState.m_flShadowSlopeScaleDepthBias = bias;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetTargetEntity, "class", "Set the target entity for this projected texture." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    CBaseEntity *target = LUA_BINDING_ARGUMENT( lua_toentity, 2, "entity" ); // toentity to allow NULL
    projectedTexture->m_hTargetEntity = target;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetTexture, "class", "Set the texture to be projected." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const char *texture = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "texture" );
    projectedTexture->m_FlashlightState.m_pSpotlightTexture = materials->FindTexture( texture, TEXTURE_GROUP_OTHER );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetTextureFrame, "class", "For animated textures, this will choose which frame in the animation will be projected." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const int frame = LUA_BINDING_ARGUMENT( luaL_checkint, 2, "frame" );
    projectedTexture->m_FlashlightState.m_nSpotlightTextureFrame = frame;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, SetVerticalFov, "class", "Set the vertical angle of projection without affecting the horizontal angle." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    const float vFov = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "verticalFov" );
    projectedTexture->m_FlashlightState.m_fVerticalFOVDegrees = vFov;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, Update, "class", "Updates the projected light and applies all previously set parameters." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    projectedTexture->ForceUpdateLight();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, __index, "class", "Metamethod that is called when a non existing field is indexed" )
{
    lua_ProjectedTexture *pProjectedTexture = LUA_BINDING_ARGUMENT( lua_toprojectedtexture, 1, "projectedTexture" );

    LUA_METATABLE_INDEX_CHECK_VALID( L, ProjectedTexture_IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pProjectedTexture );

    //const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pProjectedTexture );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, LUA_BASEENTITYMETANAME );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    lua_pushnil( L );

    return 1;
}
LUA_BINDING_END( "any", "The value of the field" )

LUA_BINDING_BEGIN( ProjectedTexture, __newindex, "class", "Metamethod that is called a new field is added" )
{
    lua_ProjectedTexture *pProjectedTexture = LUA_BINDING_ARGUMENT( lua_toprojectedtexture, 1, "projectedTexture" );

    if ( pProjectedTexture == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index a NULL entity", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    LUA_GET_REF_TABLE( L, pProjectedTexture );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, field );
    lua_pop( L, 1 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( ProjectedTexture, __eq, "class", "Metamethod that is called when comparing two entities" )
{
    lua_ProjectedTexture *pProjectedTexture1 = LUA_BINDING_ARGUMENT( lua_toprojectedtexture, 1, "projectedTexture" );
    lua_ProjectedTexture *pProjectedTexture2 = LUA_BINDING_ARGUMENT( lua_toprojectedtexture, 2, "otherProjectedTexture" );

    lua_pushboolean( L, pProjectedTexture1 == pProjectedTexture2 );
    return 1;
}
LUA_BINDING_END( "boolean", "True if entities are equal." )

LUA_BINDING_BEGIN( ProjectedTexture, __tostring, "class", "Converts the projected texture to a string." )
{
    lua_ProjectedTexture *projectedTexture = LUA_BINDING_ARGUMENT( luaL_checkprojectedtexture, 1, "projectedTexture" );
    lua_pushfstring( L, "ProjectedTexture: %p", projectedTexture );

    return 1;
}
LUA_BINDING_END( "string", "The string representation of the projected texture." )

/*
** ProjectedTextures Library
*/

LUA_REGISTRATION_INIT( ProjectedTextures );

LUA_BINDING_BEGIN( ProjectedTextures, Create, "library", "Creates a new projected texture." )
{
    FlashlightState_t state;
    lua_ProjectedTexture *projectedTexture = new lua_ProjectedTexture();
    LProjectedTexture::PushLuaInstanceSafe( L, projectedTexture );
    return 1;
}
LUA_BINDING_END( "ProjectedTexture", "The created projected texture." )

/*
** Open ProjectedTexture object
*/
LUALIB_API int luaopen_ProjectedTexture( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_PROJECTEDTEXTUREMETANAME );

    lua_pushstring( L, LUA_PROJECTEDTEXTUREMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "ProjectedTexture" */

    LUA_REGISTRATION_COMMIT( ProjectedTexture );

    LUA_REGISTRATION_COMMIT_LIBRARY( ProjectedTextures );
    lua_pop( L, 1 );  // pop metatable

    return 1;
}
