#ifndef L_ITEXTURE_H
#define L_ITEXTURE_H

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

int CreateNewAutoDestroyTextureId( bool procedural = false );
void DestroyCreatedTextureIds();

#endif  // L_ITEXTURE_H
