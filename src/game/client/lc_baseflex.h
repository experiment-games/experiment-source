//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//
#ifndef LC_BASEFLEX_H
#define LC_BASEFLEX_H

#ifdef _WIN32
#pragma once
#endif

#include <c_baseflex.h>

/* type for C_BaseFlex functions */
typedef C_BaseFlex lua_CBaseFlex;

/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseFlex *( lua_tobaseflex )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushbaseflex )( lua_State *L, lua_CBaseFlex *pEntity );

LUALIB_API lua_CBaseFlex *( luaL_checkbaseflex )( lua_State *L, int narg );

/*
** Manager for client side entities
*/
class CClientSideEntityManager
{
   public:
    CClientSideEntityManager();
    ~CClientSideEntityManager();

    lua_CBaseFlex *CreateClientSideEntity( const char *pszModelName, RenderGroup_t renderGroup );

   protected:
    void InitClientEntity( lua_CBaseFlex *pClientSideEntity, const model_t *pModel, RenderGroup_t renderGroup );
};

extern CClientSideEntityManager *g_pClientSideEntityManager;

#endif  // LC_BASEFLEX_H
