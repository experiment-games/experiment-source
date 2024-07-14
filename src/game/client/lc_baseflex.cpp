//===== Copy	right © 1996-2005, Valve Corporation, All rights reserved. ==//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//
#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lc_baseflex.h"
#include "lbaseentity_shared.h"
#include "lbaseplayer_shared.h"
#include "mathlib/lvector.h"
#include "lvphysics_interface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#include <model_types.h>

const int MIN_CLIENTSIDE_ENTITIES = 64;
const int MAX_CLIENTSIDE_ENTITIES = 1024;

static CClientSideEntityManager s_ClientSideEntityManager;
CClientSideEntityManager *g_pClientSideEntityManager = &s_ClientSideEntityManager;

CClientSideEntityManager::CClientSideEntityManager()
    : m_ClientSideEntityPool( MIN_CLIENTSIDE_ENTITIES, CUtlMemoryPool::GROW_SLOW )
{
}

CClientSideEntityManager::~CClientSideEntityManager()
{
    m_ClientSideEntityPool.Clear();
    m_ClientSideEntities.RemoveAll();
}

void CClientSideEntityManager::Clear()
{
    FOR_EACH_LL( m_ClientSideEntities, i )
    {
        lua_CBaseFlex *pClientSideEntity = m_ClientSideEntities[i];

        m_ClientSideEntityPool.Free( pClientSideEntity );
    }

    m_ClientSideEntities.RemoveAll();
}

void CClientSideEntityManager::Release()
{
    Clear();
}

lua_CBaseFlex *CClientSideEntityManager::CreateClientSideEntity( const char *pszModelName, RenderGroup_t renderGroup )
{
    if ( m_ClientSideEntityPool.Count() >= MAX_CLIENTSIDE_ENTITIES )
    {
        DevWarning( 1, "Too many clientside ents (maximum is %d)!\n", MAX_CLIENTSIDE_ENTITIES );
        return NULL;
    }

    lua_CBaseFlex *pClientSideEntity = m_ClientSideEntityPool.AllocZero();

    m_ClientSideEntities.AddToTail( pClientSideEntity );

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

    if (pModel == nullptr)
    {
        // Force loading of the model into memory immediately
        pModel = ( model_t * )engine->LoadModel( pszModelName, true );
        modelinfo->RegisterDynamicModel( pszModelName, true );
    }

    InitClientEntity( pClientSideEntity, pModel, renderGroup );

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

    pClientSideEntity->index = -1;

    cl_entitylist->AddNonNetworkableEntity( pClientSideEntity->GetIClientUnknown() );
    Assert( pClientSideEntity->GetClientHandle() != ClientEntityList().InvalidHandle() );

    if ( pClientSideEntity->m_RenderGroup == RENDER_GROUP_OTHER )
    {
        pClientSideEntity->AddToLeafSystem();
    }
    else
    {
        pClientSideEntity->AddToLeafSystem( pClientSideEntity->m_RenderGroup );
    }
}

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

LUA_API void lua_pushbaseflex( lua_State *L, lua_CBaseFlex *pEntity )
{
    CBaseHandle *hEntity =
        ( CBaseHandle * )lua_newuserdata( L, sizeof( CBaseHandle ) );
    hEntity->Set( pEntity );
    luaL_getmetatable( L, LUA_CBASEFLEXLIBNAME );
    lua_setmetatable( L, -2 );
}

LUALIB_API lua_CBaseFlex *luaL_checkbaseflex( lua_State *L, int narg )
{
    lua_CBaseFlex *d = lua_tobaseflex( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CBaseFlex expected, got NULL entity" );
    return d;
}

static int CBaseFlex_SetPos( lua_State *L )
{
    luaL_checkbaseflex( L, 1 )->SetAbsOrigin( luaL_checkvector( L, 2 ) );
    return 0;
}

static int CBaseFlex_GetPos( lua_State *L )
{
    Vector vPos = luaL_checkbaseflex( L, 1 )->GetAbsOrigin();
    lua_pushvector( L, vPos );
    return 1;
}

static int CBaseFlex_SetAngles( lua_State *L )
{
    luaL_checkbaseflex( L, 1 )->SetAbsAngles( luaL_checkangle( L, 2 ) );
    return 0;
}

static int CBaseFlex_GetAngles( lua_State *L )
{
    QAngle vAngles = luaL_checkbaseflex( L, 1 )->GetAbsAngles();
    lua_pushangle( L, vAngles );
    return 1;
}

static int CBaseFlex_Spawn( lua_State *L )
{
    luaL_checkbaseflex( L, 1 )->Spawn();
    return 0;
}

static int CBaseFlex___index( lua_State *L )
{
    CBaseFlex *pEntity = lua_tobaseflex( L, 1 );

    // LUA_METATABLE_INDEX_CHECK_VALID( L, ...IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pEntity );

    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pEntity );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    luaL_getmetatable( L, "CBaseAnimating" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    luaL_getmetatable( L, "CBaseEntity" );
    LUA_METATABLE_INDEX_CHECK_TABLE( L );

    lua_pushnil( L );
    return 1;
}

static int CBaseFlex___newindex( lua_State *L )
{
    CBaseFlex *pEntity = lua_tobaseflex( L, 1 );

    // LUA_METATABLE_INDEX_CHECK_VALID( L, ...IsValid );
    LUA_METATABLE_INDEX_CHECK( L, pEntity );

    const char *field = luaL_checkstring( L, 2 );

    LUA_GET_REF_TABLE( L, pEntity );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, field );
    lua_pop( L, 1 );

    return 0;
}

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
    { "SetPos", CBaseFlex_SetPos },
    { "GetPos", CBaseFlex_GetPos },
    { "SetAngles", CBaseFlex_SetAngles },
    { "GetAngles", CBaseFlex_GetAngles },
    { "Spawn", CBaseFlex_Spawn },
    { "__index", CBaseFlex___index },
    { "__newindex", CBaseFlex___newindex },
    { "__eq", CBaseFlex___eq },
    { "__tostring", CBaseFlex___tostring },
    { NULL, NULL } };

static int CBaseFlex_ClientsideModel( lua_State *L )
{
    const char *pszModelName = luaL_checkstring( L, 1 );
    int renderGroup = luaL_optint( L, 2, RENDER_GROUP_OTHER );

    if ( pszModelName == NULL )
    {
        luaL_error( L, "ClientsideModel: Invalid model name" );
        return 0;
    }

    lua_CBaseFlex *pEntity = g_pClientSideEntityManager->CreateClientSideEntity( pszModelName, ( RenderGroup_t )renderGroup );

    lua_pushbaseflex( L, pEntity );
    return 1;
}

static const luaL_Reg CBaseFlex_funcs[] = {
    { "ClientsideModel", CBaseFlex_ClientsideModel },
    { NULL, NULL } };

/*
** Open CBaseFlex object
*/
LUALIB_API int luaopen_CBaseFlex( lua_State *L )
{
    luaL_register( L, LUA_GNAME, CBaseFlex_funcs );
    lua_pop( L, 1 );

    luaL_newmetatable( L, LUA_CBASEFLEXLIBNAME );
    luaL_register( L, NULL, CBaseFlexmeta );
    lua_pushstring( L, "entity" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "entity" */
    return 1;
}
