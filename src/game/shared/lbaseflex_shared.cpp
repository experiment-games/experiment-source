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

LUA_REGISTRATION_INIT( CBaseFlex )

LUA_BINDING_BEGIN( CBaseFlex, GetFlexIdByName, "class", "Get flex ID by name." )
{
    lua_CBaseFlex *pEntity = LUA_BINDING_ARGUMENT( luaL_checkbaseflex, 1, "entity" );
    const char *szFlexName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "flexName" );

    LocalFlexController_t iFlexController = pEntity->FindFlexController( szFlexName );

    lua_pushnumber( L, iFlexController );
    return 1;
}
LUA_BINDING_END( "number", "Flex ID." )

LUA_BINDING_BEGIN( CBaseFlex, GetFlexWeight, "class", "Get flex weight." )
{
    lua_CBaseFlex *pEntity = LUA_BINDING_ARGUMENT( luaL_checkbaseflex, 1, "entity" );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flexId" );

    lua_pushnumber( L, pEntity->GetFlexWeight( iFlexController ) );
    return 1;
}
LUA_BINDING_END( "number", "Flex weight." )

LUA_BINDING_BEGIN( CBaseFlex, SetFlexWeight, "class", "Set flex weight." )
{
    lua_CBaseFlex *pEntity = LUA_BINDING_ARGUMENT( luaL_checkbaseflex, 1, "entity" );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flexId" );
    float flWeight = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "weight" );

    pEntity->SetFlexWeight( iFlexController, flWeight );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseFlex, HasFlexManipulator, "class", "Check if the entity has flex manipulator." )
{
    lua_CBaseFlex *pEntity = LUA_BINDING_ARGUMENT( luaL_checkbaseflex, 1, "entity" );

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
LUA_BINDING_END( "boolean", "True if the entity has flex manipulator." )

LUA_BINDING_BEGIN( CBaseFlex, __eq, "class", "Equality operator." )
{
    lua_CBaseFlex *pEntityA = LUA_BINDING_ARGUMENT( luaL_checkbaseflex, 1, "entityA" );
    lua_CBaseFlex *pEntityB = LUA_BINDING_ARGUMENT( luaL_checkbaseflex, 2, "entityB" );

    lua_pushboolean( L, pEntityA == pEntityB );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the entities are equal." )

LUA_BINDING_BEGIN( CBaseFlex, __tostring, "class", "To string operator." )
{
    lua_CBaseFlex *pEntity = LUA_BINDING_ARGUMENT( luaL_checkbaseflex, 1, "entity" );

    if ( pEntity == NULL )
        lua_pushstring( L, "NULL" );
    else
        lua_pushfstring( L, "CBaseFlex: %d \"%s\"", pEntity->entindex(), pEntity->GetClassname() );

    return 1;
}
LUA_BINDING_END( "string", "String representation of the entity." )

LUA_REGISTRATION_INIT( _G )

#ifdef CLIENT_DLL
LUA_BINDING_BEGIN( _G, ClientsideModel, "library", "Create a clientside model.", "client" )
{
    const char *pszModelName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "model" );
    int renderGroup = ( int )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, RENDER_GROUP_OTHER, "renderGroup" );

    if ( pszModelName == NULL )
    {
        luaL_error( L, "ClientsideModel: Invalid model name" );
        return 0;
    }

    lua_CBaseFlex *pEntity = g_pClientSideEntityManager->CreateClientSideEntity( pszModelName, ( RenderGroup_t )renderGroup );
    CBaseEntity::PushLuaInstanceSafe( L, pEntity );

    return 1;
}
LUA_BINDING_END( "Entity", "Clientside model entity." )
#endif

/*
** Open CBaseFlex object
*/
LUALIB_API int luaopen_CBaseFlex_shared( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_CBASEFLEXLIBNAME );

    LUA_REGISTRATION_COMMIT( CBaseFlex );

    LUA_REGISTRATION_COMMIT_LIBRARY( _G );

    return 1;
}
