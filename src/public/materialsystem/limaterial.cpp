//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//
//===========================================================================//
#include "cbase.h"
#include "imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "limaterial.h"
#include "mathlib/lvector.h"
#include "lColor.h"
#include "materialsystem/itexture.h"
#include <litexture.h>
#include <cpng.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_IMaterial *lua_tomaterial( lua_State *L, int idx )
{
    lua_IMaterial **ppMaterial = ( lua_IMaterial ** )lua_touserdata( L, idx );
    return *ppMaterial;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushmaterial( lua_State *L, lua_IMaterial *pMaterial )
{
    lua_IMaterial **ppMaterial = ( lua_IMaterial ** )lua_newuserdata( L, sizeof( pMaterial ) );
    *ppMaterial = pMaterial;
    LUA_SAFE_SET_METATABLE( L, LUA_MATERIALMETANAME );
}

LUALIB_API lua_IMaterial *luaL_checkmaterial( lua_State *L, int narg )
{
    lua_IMaterial **d = ( lua_IMaterial ** )luaL_checkudata( L, narg, LUA_MATERIALMETANAME );

    if ( *d == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "IMaterial expected, got NULL" );

    return *d;
}

LUA_REGISTRATION_INIT( Material );

LUA_BINDING_BEGIN( Material, AddReference, "class", "Add reference." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->AddRef();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, AlphaModulate, "class", "Alpha modulate." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->AlphaModulate( LUA_BINDING_ARGUMENT(luaL_checknumber, 2, "alpha") );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, ColorModulate, "class", "Color modulate." )
{
    //LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->ColorModulate( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ), luaL_checknumber( L, 4 ) );
    float red = LUA_BINDING_ARGUMENT(luaL_checknumber, 2, "red");
    float green = LUA_BINDING_ARGUMENT(luaL_checknumber, 3, "green");
    float blue = LUA_BINDING_ARGUMENT(luaL_checknumber, 4, "blue");
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->ColorModulate( red, green, blue );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, DecrementReferenceCount, "class", "Decrement reference count." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->DecrementReferenceCount();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, DeleteIfUnreferenced, "class", "Delete if unreferenced." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->DeleteIfUnreferenced();
    return 0;
}
LUA_BINDING_END()

lua_Color ColorAtPosition( unsigned char *imageData, int width, int height, int x, int y )
{
    if ( x < 0 || x >= width || y < 0 || y >= height )
    {
        return lua_Color();
    }

    int i = ( y * width + x ) * 4;

    return lua_Color( imageData[i], imageData[i + 1], imageData[i + 2], imageData[i + 3] );
}

LUA_BINDING_BEGIN( Material, GetColor, "class", "Get color." )
{
    int x = LUA_BINDING_ARGUMENT(luaL_checknumber, 2, "x");
    int y = LUA_BINDING_ARGUMENT(luaL_checknumber, 3, "y");

    bool bFound = false;
    IMaterialVar *pVar = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->FindVar( "$basetexture", &bFound );
    ITexture *pTexture = pVar->GetTextureValue();
    CPngMaterialProxy *pMaterialProxy = CPngTextureRegen::GetProceduralMaterialProxy( pTexture->GetName() );
    unsigned char *pTextureData = pMaterialProxy->GetTexturePointer();

    if ( !pTextureData )
    {
        lua_pushcolor( L, lua_Color() );
        return 1;
    }

    int width = pTexture->GetActualWidth();
    int height = pTexture->GetActualHeight();

    Assert( x >= 0 && x < width );
    Assert( y >= 0 && y < height );

    lua_Color color = ColorAtPosition( pTextureData, width, height, x, y );
    lua_pushcolor( L, color );
    return 1;
}
LUA_BINDING_END( "Color", "Color at position." )

LUA_BINDING_BEGIN( Material, GetColorModulation, "class", "Get color modulation." )
{
    float r, g, b;
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetColorModulation( &r, &g, &b );
    lua_pushnumber( L, r );
    lua_pushnumber( L, g );
    lua_pushnumber( L, b );
    return 3;
}
LUA_BINDING_END( "number", "Red modulation.", "number", "Green modulation.", "number", "Blue modulation." )

LUA_BINDING_BEGIN( Material, GetEnumerationID, "class", "Get enumeration ID." )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetEnumerationID() );
    return 1;
}
LUA_BINDING_END( "number", "Enumeration ID." )

LUA_BINDING_BEGIN( Material, GetMappingHeight, "class", "Get mapping height." )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetMappingHeight() );
    return 1;
}
LUA_BINDING_END( "number", "Mapping height." )

LUA_BINDING_BEGIN( Material, GetMappingWidth, "class", "Get mapping width." )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetMappingWidth() );
    return 1;
}
LUA_BINDING_END( "number", "Mapping width." )

LUA_BINDING_BEGIN( Material, GetMaterialPage, "class", "Get material page." )
{
    lua_pushmaterial( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetMaterialPage() );
    return 1;
}
LUA_BINDING_END( "Material", "Material page." )

LUA_BINDING_BEGIN( Material, GetMaterialVariableFlag, "class", "Get material variable flag." )
{
    int flag = LUA_BINDING_ARGUMENT(luaL_checknumber, 2, "flag");
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetMaterialVarFlag( ( MaterialVarFlags_t )flag ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if flag is set, false otherwise." )

LUA_BINDING_BEGIN( Material, GetMorphFormat, "class", "Get morph format." )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetMorphFormat() );
    return 1;
}
LUA_BINDING_END( "number", "Morph format." )

LUA_BINDING_BEGIN( Material, GetName, "class", "Get name of material." )
{
    lua_pushstring( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetName() );
    return 1;
}
LUA_BINDING_END( "string", "Name of material." )

LUA_BINDING_BEGIN( Material, GetAnimationFrameAmount, "class", "Get number of animation frames." )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetNumAnimationFrames() );
    return 1;
}
LUA_BINDING_END( "number", "Number of animation frames." )

LUA_BINDING_BEGIN( Material, GetPassAmount, "class", "Get number of passes." )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetNumPasses() );
    return 1;
}
LUA_BINDING_END( "number", "Number of passes." )

LUA_BINDING_BEGIN( Material, GetPropertyFlag, "class", "Get property flag." )
{
    int flag = LUA_BINDING_ARGUMENT(luaL_checknumber, 2, "flag");
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetPropertyFlag( ( MaterialPropertyTypes_t )flag ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if flag is set, false otherwise." )

LUA_BINDING_BEGIN( Material, GetReflectivity, "class", "Get reflectivity." )
{
    Vector vector;
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetReflectivity( vector );
    lua_pushvector( L, vector );
    return 1;
}
LUA_BINDING_END( "vector", "Reflectivity." )

LUA_BINDING_BEGIN( Material, GetShaderName, "class", "Get shader name." )
{
    lua_pushstring( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetShaderName() );
    return 1;
}
LUA_BINDING_END( "string", "Shader name." )

LUA_BINDING_BEGIN( Material, GetFloat, "class", "Get float." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        lua_pushnil( L );
        return 1;
    }

    lua_pushnumber( L, materialVar->GetFloatValue() );

    return 1;
}
LUA_BINDING_END( "number", "Float value." )

LUA_BINDING_BEGIN( Material, GetInt, "class", "Get integer." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        lua_pushnil( L );
        return 1;
    }

    lua_pushinteger( L, materialVar->GetIntValue() );

    return 1;
}
LUA_BINDING_END( "number", "Integer value." )

LUA_BINDING_BEGIN( Material, GetString, "class", "Get string." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        lua_pushnil( L );
        return 1;
    }

    lua_pushstring( L, materialVar->GetStringValue() );

    return 1;
}
LUA_BINDING_END( "string", "String value." )

LUA_BINDING_BEGIN( Material, GetTexture, "class", "Get texture." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        lua_pushnil( L );
        return 1;
    }

    ITexture *texture = materialVar->GetTextureValue();
    lua_pushitexture( L, texture );

    return 1;
}
LUA_BINDING_END( "Texture", "Texture value." )

LUA_BINDING_BEGIN( Material, GetVector, "class", "Get vector." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        lua_pushnil( L );
        return 1;
    }

    const float *vectorValues = materialVar->GetVecValue();

    // Turn the vector into a table with the vector metatable
    Vector vector( vectorValues[0], vectorValues[1], vectorValues[2] );
    lua_pushvector( L, vector );

    return 1;
}
LUA_BINDING_END( "vector", "Vector value." )

LUA_BINDING_BEGIN( Material, GetVector4D, "class", "Get vector 4D." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        lua_pushnil( L );
        return 1;
    }

    const float *vectorValues = materialVar->GetVecValue();

    lua_pushnumber( L, vectorValues[0] );
    lua_pushnumber( L, vectorValues[1] );
    lua_pushnumber( L, vectorValues[2] );
    lua_pushnumber( L, vectorValues[3] );

    return 4;
}
LUA_BINDING_END( "number", "X value.", "number", "Y value.", "number", "Z value.", "number", "W value." )

LUA_BINDING_BEGIN( Material, GetTextureGroupName, "class", "Get texture group name." )
{
    lua_pushstring( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetTextureGroupName() );
    return 1;
}
LUA_BINDING_END( "string", "Texture group name." )

LUA_BINDING_BEGIN( Material, GetTextureMemoryBytes, "class", "Get texture memory bytes." )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetTextureMemoryBytes() );
    return 1;
}
LUA_BINDING_END( "number", "Texture memory bytes." )

LUA_BINDING_BEGIN( Material, HasProxy, "class", "Has proxy." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->HasProxy() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if has proxy, false otherwise." )

LUA_BINDING_BEGIN( Material, IncrementReferenceCount, "class", "Increment reference count." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->IncrementReferenceCount();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, InMaterialPage, "class", "In material page." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->InMaterialPage() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if in material page, false otherwise." )

LUA_BINDING_BEGIN( Material, IsAlphaTested, "class", "Is alpha tested." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->IsAlphaTested() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if alpha tested, false otherwise." )

LUA_BINDING_BEGIN( Material, IsErrorMaterial, "class", "Is error material." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->IsErrorMaterial() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if error material, false otherwise." )

LUA_BINDING_BEGIN( Material, IsSpriteCard, "class", "Is sprite card." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->IsSpriteCard() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if sprite card, false otherwise." )

LUA_BINDING_BEGIN( Material, IsTranslucent, "class", "Is translucent." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->IsTranslucent() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if translucent, false otherwise." )

LUA_BINDING_BEGIN( Material, IsTwoSided, "class", "Is two sided." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->IsTwoSided() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if two sided, false otherwise." )

LUA_BINDING_BEGIN( Material, IsVertexLit, "class", "Is vertex lit." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->IsVertexLit() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if vertex lit, false otherwise." )

LUA_BINDING_BEGIN( Material, NeedsFullFrameBufferTexture, "class", "Needs full frame buffer texture." )
{
    bool bCheckSpecificToThisFrame = luaL_optboolean( L, 1, 1 );
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->NeedsFullFrameBufferTexture( bCheckSpecificToThisFrame ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if needs full frame buffer texture, false otherwise." )

LUA_BINDING_BEGIN( Material, NeedsLightmapBlendAlpha, "class", "Needs lightmap blend alpha." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->NeedsLightmapBlendAlpha() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if needs lightmap blend alpha, false otherwise." )

LUA_BINDING_BEGIN( Material, NeedsPowerOfTwoFrameBufferTexture, "class", "Needs power of two frame buffer texture." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->NeedsPowerOfTwoFrameBufferTexture( luaL_optboolean( L, 1, 1 ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if needs power of two frame buffer texture, false otherwise." )

LUA_BINDING_BEGIN( Material, NeedsSoftwareLighting, "class", "Needs software lighting." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->NeedsSoftwareLighting() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if needs software lighting, false otherwise." )

LUA_BINDING_BEGIN( Material, NeedsSoftwareSkinning, "class", "Needs software skinning." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->NeedsSoftwareSkinning() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if needs software skinning, false otherwise." )

LUA_BINDING_BEGIN( Material, NeedsTangentSpace, "class", "Needs tangent space." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->NeedsTangentSpace() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if needs tangent space, false otherwise." )

LUA_BINDING_BEGIN( Material, RecomputeStateSnapshots, "class", "Recompute state snapshots." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->RecomputeStateSnapshots();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, Refresh, "class", "Refresh." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->Refresh();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, RefreshPreservingMaterialVariables, "class", "Refresh preserving material vars." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->RefreshPreservingMaterialVars();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, Release, "class", "Release." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->Release();
    g_pMaterialSystem->UncacheUnusedMaterials();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, SetMaterialVariableFlag, "class", "Set material variable flag." )
{
    int flag = LUA_BINDING_ARGUMENT(luaL_checknumber, 1, "flag");
    bool on = LUA_BINDING_ARGUMENT(luaL_checkboolean, 2, "on");
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->SetMaterialVarFlag( ( MaterialVarFlags_t )flag, on );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, SetShader, "class", "Set shader." )
{
    const char *shaderName = luaL_checkstring( L, 1 );
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->SetShader( shaderName );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, SetFloat, "class", "Set float." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        return 0;
    }

    materialVar->SetFloatValue( luaL_checknumber( L, 3 ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, SetInt, "class", "Set integer." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        return 0;
    }

    materialVar->SetIntValue( luaL_checknumber( L, 3 ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, SetString, "class", "Set string." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        return 0;
    }

    materialVar->SetStringValue( luaL_checkstring( L, 3 ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, SetTexture, "class", "Set texture." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        return 0;
    }

    ITexture *texture = luaL_checkitexture( L, 3 );
    materialVar->SetTextureValue( texture );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, SetVector, "class", "Set vector." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        return 0;
    }

    Vector vector = luaL_checkvector( L, 3 );
    materialVar->SetVecValue( vector.x, vector.y, vector.z );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, SetVector4D, "class", "Set vector 4D." )
{
    bool foundVar;
    const char *varName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "varName" );
    IMaterial *material = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" );
    IMaterialVar *materialVar = material->FindVar( varName, &foundVar );

    if ( !foundVar )
    {
        return 0;
    }

    float x = LUA_BINDING_ARGUMENT(luaL_checknumber, 3, "x");
    float y = LUA_BINDING_ARGUMENT(luaL_checknumber, 4, "y");
    float z = LUA_BINDING_ARGUMENT(luaL_checknumber, 5, "z");
    float w = LUA_BINDING_ARGUMENT(luaL_checknumber, 6, "w");
    materialVar->SetVecValue( x, y, z, w );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, SetUseFixedFunctionBakedLighting, "class", "Set use fixed function baked lighting." )
{
    bool useFixedFunctionBakedLighting = LUA_BINDING_ARGUMENT(luaL_checkboolean, 1, "useFixedFunctionBakedLighting");
    LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->SetUseFixedFunctionBakedLighting( useFixedFunctionBakedLighting );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, ShaderParamCount, "class", "Shader param count." )
{
    int count = LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->ShaderParamCount();
    lua_pushinteger( L, count );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, UsesEnvCubemap, "class", "Uses env cubemap." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->UsesEnvCubemap() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, WasReloadedFromWhitelist, "class", "Was reloaded from whitelist." )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->WasReloadedFromWhitelist() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Material, __tostring, "class", "__tostring." )
{
    lua_pushfstring( L, "IMaterial: %s", LUA_BINDING_ARGUMENT( luaL_checkmaterial, 1, "material" )->GetName() );
    return 1;
}
LUA_BINDING_END()

/*
** Open IMaterial object
*/
LUALIB_API int luaopen_IMaterial( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_MATERIALMETANAME );

    LUA_REGISTRATION_COMMIT( Material );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_MATERIALMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Material" */
    return 1;
}
