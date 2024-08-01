//===== Copy	right © 1996-2005, Valve Corporation, All rights reserved. ==//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//
#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseflex_shared.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "mathlib/lvector.h"
#include "lvphysics_interface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#include <model_types.h>

#ifdef CLIENT_DLL
static CClientSideEntityManager s_ClientSideEntityManager;
CClientSideEntityManager *g_pClientSideEntityManager = &s_ClientSideEntityManager;

CClientSideEntityManager::CClientSideEntityManager()
{
}

CClientSideEntityManager::~CClientSideEntityManager()
{
}

lua_CBaseFlex *CClientSideEntityManager::CreateClientSideEntity( const char *pszModelName, RenderGroup_t renderGroup )
{
    lua_CBaseFlex *pClientSideEntity = new lua_CBaseFlex();

    int nModelIndex = modelinfo->GetModelClientSideIndex( pszModelName );

    model_t *pModel = nullptr;

    // First try get the existing model
    if ( nModelIndex != -1 )
    {
        pModel = ( model_t * )modelinfo->GetModel( nModelIndex );

        // After a map change, the pointer to the model may give an invalid studio model
        // in that case, we need to force the model to be loaded again
        if ( modelinfo->GetStudiomodel( pModel ) == nullptr )
        {
            pModel = nullptr;
        }
    }

    if ( pModel == nullptr )
    {
        // Force loading of the model into memory immediately
        pModel = ( model_t * )engine->LoadModel( pszModelName, true );
        modelinfo->RegisterDynamicModel( pszModelName, true );
    }

    InitClientEntity( pClientSideEntity, pModel, renderGroup );

    if ( pClientSideEntity->InitializeAsClientEntity( pszModelName, renderGroup ) == false )
    {
        pClientSideEntity->Remove();
        DevWarning( "Failed to initialize clientside entity for model %s\n", pszModelName );
        return NULL;
    }

    return pClientSideEntity;
}

void CClientSideEntityManager::InitClientEntity( lua_CBaseFlex *pClientSideEntity, const model_t *pModel, RenderGroup_t renderGroup )
{
    pClientSideEntity->SetModelPointer( pModel );
    pClientSideEntity->m_RenderGroup = renderGroup;

    pClientSideEntity->m_nBody = 0;
    pClientSideEntity->m_nSkin = 0;
    pClientSideEntity->SetRenderMode( kRenderNormal );
    pClientSideEntity->m_nRenderFX = kRenderFxNone;
    pClientSideEntity->Interp_SetupMappings( pClientSideEntity->GetVarMapping() );
    pClientSideEntity->SetAbsOrigin( vec3_origin );
}
#endif

/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseFlex *lua_tobaseflex( lua_State *L, int idx )
{
    CBaseHandle *hEntity =
        dynamic_cast< CBaseHandle * >( ( CBaseHandle * )lua_touserdata( L, idx ) );
    if ( hEntity == NULL )
        return NULL;
    IHandleEntity *pEntity = hEntity->Get();
    return dynamic_cast< lua_CBaseFlex * >( pEntity );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CBaseFlex *luaL_checkbaseflex( lua_State *L, int narg )
{
    lua_CBaseFlex *d = lua_tobaseflex( L, narg );

    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CBaseFlex expected, got NULL entity" );

    return d;
}

static int CBaseFlex_GetFlexIDByName( lua_State *L )
{
    lua_CBaseFlex *pEntity = luaL_checkbaseflex( L, 1 );
    const char *szFlexName = luaL_checkstring( L, 2 );

    LocalFlexController_t iFlexController = pEntity->FindFlexController( szFlexName );

    lua_pushnumber( L, iFlexController );
    return 1;
}

static int CBaseFlex_GetFlexWeight( lua_State *L )
{
    lua_CBaseFlex *pEntity = luaL_checkbaseflex( L, 1 );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )( int )luaL_checknumber( L, 2 );

    lua_pushnumber( L, pEntity->GetFlexWeight( iFlexController ) );
    return 1;
}

static int CBaseFlex_SetFlexWeight( lua_State *L )
{
    lua_CBaseFlex *pEntity = luaL_checkbaseflex( L, 1 );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )( int )luaL_checknumber( L, 2 );
    float flWeight = luaL_checknumber( L, 3 );

    pEntity->SetFlexWeight( iFlexController, flWeight );
    return 0;
}

static int CBaseAnimating_HasFlexManipulator( lua_State *L )
{
    lua_CBaseFlex *pEntity = luaL_checkbaseflex( L, 1 );

    for ( LocalFlexController_t i = LocalFlexController_t( 0 ); i < pEntity->GetNumFlexControllers(); i++ )
    {
        if ( pEntity->GetFlexWeight( i ) != 0.0f )
        {
            lua_pushboolean( L, true );
            return 1;
        }
    }

    lua_pushboolean( L, false );
    return 1;
}
//
//static int CBaseFlex___index( lua_State *L )
//{
//    CBaseFlex *pEntity = lua_tobaseflex( L, 1 );
//
//    LUA_METATABLE_INDEX_CHECK_VALID( L, CBaseEntity_IsValid );
//    LUA_METATABLE_INDEX_CHECK( L, pEntity );
//
//    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pEntity );
//
//    if ( lua_getmetatable( L, 1 ) )
//    {
//        LUA_METATABLE_INDEX_CHECK_TABLE( L );
//    }
//
//    luaL_getmetatable( L, LUA_CBASEFLEXLIBNAME );
//    LUA_METATABLE_INDEX_CHECK_TABLE( L );
//
//    LUA_METATABLE_INDEX_DERIVE_INDEX( L, LUA_BASEANIMATINGLIBNAME );
//
//    lua_pushnil( L );
//    return 1;
//}

//static int CBaseFlex___newindex( lua_State *L )
//{
//    CBaseFlex *pEntity = lua_tobaseflex( L, 1 );
//
//    LUA_METATABLE_INDEX_CHECK_VALID( L, CBaseEntity_IsValid );
//    LUA_METATABLE_INDEX_CHECK( L, pEntity );
//
//    const char *field = luaL_checkstring( L, 2 );
//
//    LUA_GET_REF_TABLE( L, pEntity );
//    lua_pushvalue( L, 3 );
//    lua_setfield( L, -2, field );
//    lua_pop( L, 1 );
//
//    return 0;
//}

static int CBaseFlex___eq( lua_State *L )
{
    lua_pushboolean( L, lua_tobaseflex( L, 1 ) == lua_tobaseflex( L, 2 ) );
    return 1;
}

static int CBaseFlex___tostring( lua_State *L )
{
    lua_CBaseFlex *pEntity = lua_tobaseflex( L, 1 );
    if ( pEntity == NULL )
        lua_pushstring( L, "NULL" );
    else
        lua_pushfstring( L, "CBaseFlex: %d \"%s\"", pEntity->entindex(), pEntity->GetClassname() );
    return 1;
}

static const luaL_Reg CBaseFlexmeta[] = {
    { "GetFlexIDByName", CBaseFlex_GetFlexIDByName },
    { "GetFlexWeight", CBaseFlex_GetFlexWeight },
    { "SetFlexWeight", CBaseFlex_SetFlexWeight },
    { "HasFlexManipulator", CBaseAnimating_HasFlexManipulator },

    // { "__index", CBaseFlex___index }, // In Lua now
    // { "__newindex", CBaseFlex___newindex }, // Conflicts when storing with CBaseEntity ref table
    { "__eq", CBaseFlex___eq },
    { "__tostring", CBaseFlex___tostring },
    { NULL, NULL } };

#ifdef CLIENT_DLL
static int CBaseFlex_ClientsideModel( lua_State *L )
{
    const char *pszModelName = luaL_checkstring( L, 1 );
    int renderGroup = ( int )luaL_optnumber( L, 2, RENDER_GROUP_OTHER );

    if ( pszModelName == NULL )
    {
        luaL_error( L, "ClientsideModel: Invalid model name" );
        return 0;
    }

    lua_CBaseFlex *pEntity = g_pClientSideEntityManager->CreateClientSideEntity( pszModelName, ( RenderGroup_t )renderGroup );
    CBaseEntity::PushLuaInstanceSafe( L, pEntity );

    return 1;
}
#endif

static const luaL_Reg CBaseFlex_funcs[] = {
#ifdef CLIENT_DLL
    { "ClientsideModel", CBaseFlex_ClientsideModel },
#endif
    { NULL, NULL } };

/*
** Open CBaseFlex object
*/
LUALIB_API int luaopen_CBaseFlex_shared( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_CBASEFLEXLIBNAME );
    luaL_register( L, NULL, CBaseFlexmeta );

    luaL_register( L, LUA_GNAME, CBaseFlex_funcs );
    lua_pop( L, 1 );
    return 1;
}
