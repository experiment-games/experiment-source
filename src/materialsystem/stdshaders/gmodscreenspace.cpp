#include "BaseVSShader.h"

// Include the pixel shader for our own shader.
// Note that the shader compiler generates both 2.0 and 2.0b versions.
#include "gmodscreenspace_vs20.inc"
#include "gmodscreenspace_ps20.inc"
#include "gmodscreenspace_ps20b.inc"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_SHADER( gmodscreenspace, "Help for GMOD Compatibility Shader" )

BEGIN_SHADER_PARAMS
SHADER_PARAM( BLUR, SHADER_PARAM_TYPE_FLOAT, "1.0", "" )
SHADER_PARAM( BLOOMAMOUNT, SHADER_PARAM_TYPE_FLOAT, "1.0", "" )
END_SHADER_PARAMS

SHADER_INIT_PARAMS()
{
    if ( !( params[BLUR]->IsDefined() ) )
    {
        params[BLUR]->SetFloatValue( 1.0 );
    }

    if ( !( params[BLOOMAMOUNT]->IsDefined() ) )
    {
        params[BLOOMAMOUNT]->SetFloatValue( 1.0 );
    }
}

SHADER_INIT
{
    if ( params[BASETEXTURE]->IsDefined() )
    {
        LoadTexture( BASETEXTURE );
    }
}

// We want this shader to operate on the frame buffer itself. Therefore we need to set this to true.
bool NeedsFullFrameBufferTexture( IMaterialVar **params, bool bCheckSpecificToThisFrame /* = true */ ) const
{
    return true;
}

SHADER_FALLBACK
{
    if ( g_pHardwareConfig->GetDXSupportLevel() < 90 )
    {
        Assert( 0 );
        return "Wireframe";
    }

    return 0;
}

SHADER_DRAW
{
    SHADOW_STATE
    {
        pShaderShadow->EnableDepthWrites( false );
        pShaderShadow->EnableAlphaWrites( true );
        pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
        pShaderShadow->VertexShaderVertexFormat( VERTEX_POSITION, 1, 0, 0 );

        // Render targets are pegged as sRGB on POSIX, so just force these reads and writes
        bool bForceSRGBReadAndWrite = IsOSX() && g_pHardwareConfig->CanDoSRGBReadFromRTs();
        pShaderShadow->EnableSRGBRead( SHADER_SAMPLER0, bForceSRGBReadAndWrite );
        pShaderShadow->EnableSRGBWrite( bForceSRGBReadAndWrite );

        // Pre-cache shaders
        gmodscreenspace_vs20_Static_Index vshIndex;
        pShaderShadow->SetVertexShader( "gmodscreenspace_vs20", vshIndex.GetIndex() );

        if ( g_pHardwareConfig->SupportsPixelShaders_2_b() || g_pHardwareConfig->ShouldAlwaysUseShaderModel2bShaders() )
        {
            DECLARE_STATIC_PIXEL_SHADER( gmodscreenspace_ps20b );
#ifndef _X360
            SET_STATIC_PIXEL_SHADER_COMBO( APPROX_SRGB_ADAPTER, bForceSRGBReadAndWrite );
#endif
            SET_STATIC_PIXEL_SHADER( gmodscreenspace_ps20b );
        }
        else
        {
            DECLARE_STATIC_PIXEL_SHADER( gmodscreenspace_ps20 );
            SET_STATIC_PIXEL_SHADER( gmodscreenspace_ps20 );
        }

        if ( IS_FLAG_SET( MATERIAL_VAR_ADDITIVE ) )
            EnableAlphaBlending( SHADER_BLEND_ONE, SHADER_BLEND_ONE );
    }

    DYNAMIC_STATE
    {
        BindTexture( SHADER_SAMPLER0, BASETEXTURE, -1 );

        float v[4];

        // The temp buffer is 1/4 back buffer size
        ITexture *src_texture = params[BASETEXTURE]->GetTextureValue();
        int width = src_texture->GetActualWidth();
        int height = src_texture->GetActualHeight();
        float dX = 1.0f / width;
        float dY = 1.0f / height;

        float blurStrength = max( params[BLUR]->GetFloatValue(), 0.0f );

        // Tap offsets
        v[0] = 1.3366f * dX * blurStrength;
        v[1] = 1.3366f * dY * blurStrength;
        pShaderAPI->SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, v, 1 );
        v[0] = 3.4295f * dX * blurStrength;
        v[1] = 3.4295f * dY * blurStrength;
        pShaderAPI->SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_1, v, 1 );
        v[0] = 5.4264f * dX * blurStrength;
        v[1] = 5.4264f * dY * blurStrength;
        pShaderAPI->SetVertexShaderConstant( VERTEX_SHADER_SHADER_SPECIFIC_CONST_2, v, 1 );

        v[0] = 7.4359f * dX * blurStrength;
        v[1] = 7.4359f * dY * blurStrength;
        pShaderAPI->SetPixelShaderConstant( 0, v, 1 );
        v[0] = 9.4436f * dX * blurStrength;
        v[1] = 9.4436f * dY * blurStrength;
        pShaderAPI->SetPixelShaderConstant( 1, v, 1 );
        v[0] = 11.4401f * dX * blurStrength;
        v[1] = 11.4401f * dY * blurStrength;
        pShaderAPI->SetPixelShaderConstant( 2, v, 1 );

        v[0] = v[1] = v[2] = params[BLOOMAMOUNT]->GetFloatValue();
        pShaderAPI->SetPixelShaderConstant( 3, v, 1 );

        pShaderAPI->SetVertexShaderIndex( 0 );

        if ( g_pHardwareConfig->SupportsPixelShaders_2_b() || g_pHardwareConfig->ShouldAlwaysUseShaderModel2bShaders() )
        {
            DECLARE_DYNAMIC_PIXEL_SHADER( gmodscreenspace_ps20b );
            SET_DYNAMIC_PIXEL_SHADER( gmodscreenspace_ps20b );
        }
        else
        {
            DECLARE_DYNAMIC_PIXEL_SHADER( gmodscreenspace_ps20 );
            SET_DYNAMIC_PIXEL_SHADER( gmodscreenspace_ps20 );
        }
    }
    Draw();
}
END_SHADER
