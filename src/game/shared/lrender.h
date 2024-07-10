#ifndef LC_RENDER_H
#define LC_RENDER_H

#ifdef _WIN32
#pragma once
#endif

/* type for ITexture functions */
typedef ITexture lua_ITexture;

/*
** access functions (stack -> C)
*/
LUALIB_API lua_ITexture *( luaL_toitexture )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/

LUALIB_API void( lua_pushitexture )( lua_State *L, lua_ITexture *pTexture );

LUALIB_API lua_ITexture *( luaL_checkitexture )( lua_State *L, int narg );

int surface_SafeCreateNewTextureID( bool procedural = false );
void surface_DestroyAllTextureIDs();

#endif  // LC_RENDER_H
