#include "cbase.h"
#include "materialsystem/itexture.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "litexture.h"
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

LUA_REGISTRATION_INIT( Texture );

#ifdef CLIENT_DLL
// List of all texture ids created during the lifetime of the client Lua state
static CUtlVector< int > g_TextureIDs;

/// <summary>
/// Creates a new TextureID, keeping track of it, so it can be destroyed on shutdown
/// of the client Lua state.
/// </summary>
/// <param name="procedural"></param>
/// <returns></returns>
int CreateNewAutoDestroyTextureId( bool procedural )
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
void DestroyCreatedTextureIds()
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
    LUA_SAFE_SET_METATABLE( L, LUA_ITEXTUREMETANAME );
}

LUALIB_API lua_ITexture *luaL_checkitexture( lua_State *L, int narg )
{
    lua_ITexture **ppData = ( lua_ITexture ** )luaL_checkudata( L, narg, LUA_ITEXTUREMETANAME );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "ITexture expected, got NULL" );

    return *ppData;
}

/*
** ITexture metatable
*/
LUA_REGISTRATION_INIT( ITexture )

LUA_BINDING_BEGIN( Texture, Download, "class", "Downloads the texture into the material system." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    pTexture->Download();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Texture, GetName, "class", "Gets the name of the texture." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushstring( L, pTexture->GetName() );
    return 1;
}
LUA_BINDING_END( "string", "The name of the texture." )

LUA_BINDING_BEGIN( Texture, GetActualWidth, "class", "Gets the actual width of the texture." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushinteger( L, pTexture->GetActualWidth() );
    return 1;
}
LUA_BINDING_END( "integer", "The actual width of the texture." )

LUA_BINDING_BEGIN( Texture, GetActualHeight, "class", "Gets the actual height of the texture." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushinteger( L, pTexture->GetActualHeight() );
    return 1;
}
LUA_BINDING_END( "integer", "The actual height of the texture." )

LUA_BINDING_BEGIN( Texture, GetMappingWidth, "class", "Gets the mapping width of the texture." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushinteger( L, pTexture->GetMappingWidth() );
    return 1;
}
LUA_BINDING_END( "integer", "The mapping width of the texture." )

LUA_BINDING_BEGIN( Texture, GetMappingHeight, "class", "Gets the mapping height of the texture." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushinteger( L, pTexture->GetMappingHeight() );
    return 1;
}
LUA_BINDING_END( "integer", "The mapping height of the texture." )

LUA_BINDING_BEGIN( Texture, GetNumAnimationFrames, "class", "Gets the number of animation frames of the texture." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushinteger( L, pTexture->GetNumAnimationFrames() );
    return 1;
}
LUA_BINDING_END( "integer", "The number of animation frames of the texture." )

LUA_BINDING_BEGIN( Texture, IsTranslucent, "class", "Checks if the texture is translucent." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushboolean( L, pTexture->IsTranslucent() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the texture is translucent, false otherwise." )

LUA_BINDING_BEGIN( Texture, IsMipmapped, "class", "Checks if the texture is mipmapped." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushboolean( L, pTexture->IsMipmapped() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the texture is mipmapped, false otherwise." )

LUA_BINDING_BEGIN( Texture, IsError, "class", "Checks if the texture is in an error state." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushboolean( L, pTexture->IsError() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the texture is in an error state, false otherwise." )

LUA_BINDING_BEGIN( Texture, IsErrorTexture, "class", "Checks if the texture is an error texture." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    lua_pushboolean( L, IsErrorTexture( pTexture ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the texture is an error texture, false otherwise." )

LUA_BINDING_BEGIN( Texture, Release, "class", "Release the texture." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( luaL_checkitexture, 1, "texture" );
    pTexture->Release();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Texture, __tostring, "class", "Returns a string representation of the texture." )
{
    lua_ITexture *pTexture = LUA_BINDING_ARGUMENT( lua_toitexture, 1, "texture" );
    lua_pushfstring( L, "ITexture: %s", pTexture ? pTexture->GetName() : "NULL" );
    return 1;
}
LUA_BINDING_END()

/*
** Open render ITexture metatable
*/
LUALIB_API int luaopen_ITexture( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_ITEXTUREMETANAME );

    LUA_REGISTRATION_COMMIT( Texture );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_ITEXTUREMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Texture" */
    return 1;
}
