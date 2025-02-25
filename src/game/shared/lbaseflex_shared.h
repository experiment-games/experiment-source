#ifndef LBASEFLEX_SHARED_H
#define LBASEFLEX_SHARED_H

#ifdef _WIN32
#pragma once
#endif

/* type for C_BaseFlex functions */
#ifdef CLIENT_DLL
#include <c_baseflex.h>
typedef C_BaseFlex lua_CBaseFlex;
#else
#include <baseflex.h>
typedef CBaseFlex lua_CBaseFlex;
#endif

/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseFlex *( lua_tobaseflex )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CBaseFlex *( luaL_checkbaseflex )( lua_State *L, int narg );

#ifdef CLIENT_DLL
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
#endif

#endif  // LBASEFLEX_SHARED_H
