#ifndef LC_RENDER_H
#define LC_RENDER_H

#ifdef _WIN32
#pragma once
#endif

/* type for ITexture functions */
typedef ITexture lua_ITexture;

/* type for C_BaseAnimating functions */
typedef C_BaseAnimating lua_CBaseAnimating;

/*
** access functions (stack -> C)
*/
LUALIB_API lua_ITexture *( luaL_toitexture )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/

LUALIB_API void( lua_pushitexture )( lua_State *L, lua_ITexture *pTexture );

LUALIB_API lua_ITexture *( luaL_checktexture )( lua_State *L, int narg );

#endif  // LC_RENDER_H
