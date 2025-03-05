#include "cbase.h"
#include "basescripted.h"
#include "luamanager.h"
#ifdef CLIENT_DLL
#include "lc_baseanimating.h"
#else
#include "lbaseanimating.h"
#endif
#include "lbaseentity_shared.h"
#include "lvphysics_interface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// clang-format off

IMPLEMENT_NETWORKCLASS_ALIASED( BaseScripted, DT_BaseScripted )

BEGIN_NETWORK_TABLE( CBaseScripted, DT_BaseScripted )
#ifdef CLIENT_DLL
    RecvPropString( RECVINFO( m_iScriptedClassname ) ),
#else
    SendPropString( SENDINFO( m_iScriptedClassname ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CBaseScripted ) END_PREDICTION_DATA()

static bool WORKAROUND_NASTY_FORMATTING_BUG;  // clang-format on

#ifdef CLIENT_DLL
static C_BaseEntity *CCBaseScriptedFactory( void )
{
    return static_cast< C_BaseEntity * >( new CBaseScripted );
};
#endif

#ifndef CLIENT_DLL
static CUtlDict< CEntityFactory< CBaseScripted > *, unsigned short >
    m_EntityFactoryDatabase;
#endif

// Experiment; Commented to instead go ask Lua when we encounter a non-C-registered entities.
// void RegisterScriptedEntity( const char *className )
//{
// #ifdef CLIENT_DLL
//    if ( GetClassMap().FindFactory( className ) )
//    {
//        return;
//    }
//
//    GetClassMap().Add( className, "CBaseScripted", sizeof( CBaseScripted ), &CCBaseScriptedFactory, true );
// #else
//    if ( EntityFactoryDictionary()->FindFactory( className ) )
//    {
//        return;
//    }
//
//    unsigned short lookup = m_EntityFactoryDatabase.Find( className );
//    if ( lookup != m_EntityFactoryDatabase.InvalidIndex() )
//    {
//        return;
//    }
//
//    CEntityFactory< CBaseScripted > *pFactory =
//        new CEntityFactory< CBaseScripted >( className );
//
//    lookup = m_EntityFactoryDatabase.Insert( className, pFactory );
//    Assert( lookup != m_EntityFactoryDatabase.InvalidIndex() );
// #endif
//}

void ResetEntityFactoryDatabase( void )
{
#ifdef CLIENT_DLL
#ifdef LUA_SDK
    GetClassMap().RemoveAllScripted();
#endif
#else
    for ( int i = m_EntityFactoryDatabase.First();
          i != m_EntityFactoryDatabase.InvalidIndex();
          i = m_EntityFactoryDatabase.Next( i ) )
    {
        delete m_EntityFactoryDatabase[i];
    }
    m_EntityFactoryDatabase.RemoveAll();
#endif
}

CBaseScripted::CBaseScripted( void )
{
#ifdef LUA_SDK
    // UNDONE: We're done in CBaseEntity
    m_nTableReference = LUA_NOREF;
#endif
}

CBaseScripted::~CBaseScripted( void )
{
    // Andrew; This is actually done in CBaseEntity. I'm doing it here because
    // this is the class that initialized the reference.
#ifdef LUA_SDK
    if ( L )
        lua_unref( L, m_nTableReference );
#endif
}

void CBaseScripted::LoadScriptedEntity( void )
{
    lua_getglobal( L, LUA_SCRIPTEDENTITIESLIBNAME );
    if ( lua_istable( L, -1 ) )
    {
        lua_getfield( L, -1, "Get" );
        if ( lua_isfunction( L, -1 ) )
        {
            lua_remove( L, -2 );
            lua_pushstring( L, GetClassname() );
            luasrc_pcall( L, 1, 1 );

            Assert( lua_istable( L, -1 ) );  // The entity should exist, since we just created it
        }
        else
        {
            lua_pop( L, 2 );
        }
    }
    else
    {
        lua_pop( L, 1 );
    }
}

void CBaseScripted::InitScriptedEntity( void )
{
#ifdef LUA_SDK
#if 0
#ifndef CLIENT_DLL
    // Let the instance reinitialize itself for the client.
    if (lua_isrefvalid(L, m_nTableReference))
        return;
#endif
#endif

    SetThink( &CBaseScripted::Think );
#ifdef CLIENT_DLL
    SetNextClientThink( gpGlobals->curtime );
#endif
    SetNextThink( gpGlobals->curtime );

    SetTouch( &CBaseScripted::Touch );

    char className[255];
#if defined( CLIENT_DLL )
    if ( strlen( GetScriptedClassname() ) > 0 )
        Q_strncpy( className, GetScriptedClassname(), sizeof( className ) );
    else
        Q_strncpy( className, GetClassname(), sizeof( className ) );
#else
    Q_strncpy( m_iScriptedClassname.GetForModify(), GetClassname(), sizeof( className ) );
    Q_strncpy( className, GetClassname(), sizeof( className ) );
#endif
    Q_strlower( className );
    SetClassname( className );

    if ( !lua_isrefvalid( L, m_nTableReference ) )
    {
        LoadScriptedEntity();
        m_nTableReference = luaL_ref( L, LUA_REGISTRYINDEX );
    }
    else
    {
        lua_getglobal( L, "table" );
        if ( lua_istable( L, -1 ) )
        {
            lua_getfield( L, -1, "Merge" );
            if ( lua_isfunction( L, -1 ) )
            {
                lua_remove( L, -2 );
                lua_getref( L, m_nTableReference );
                LoadScriptedEntity();
                luasrc_pcall( L, 2, 0 );
            }
            else
            {
                lua_pop( L, 2 );
            }
        }
        else
        {
            lua_pop( L, 1 );
        }
    }

    // Get (or init) the ref table
    LUA_GET_REF_TABLE( L, this );

#ifdef CLIENT_DLL
    lua_getfield( L, -1, "RenderGroup" );
    if ( lua_isnumber( L, -1 ) )
    {
        m_iRenderGroupOverride = ( int )lua_tointeger( L, -1 );
    }
    lua_pop( L, 1 );  // Pop the RenderGroup field
#endif

    // Experiment; TODO: Remaining members: https://wiki.facepunch.com/gmod/Structures/ENT
    lua_getfield( L, -1, "ClassNameOverride" );
    if ( lua_isstring( L, -1 ) )
    {
        DevWarning( "`ClassNameOverride` not yet implemented! For entity with class '%s'", className );
    }
    lua_pop( L, 1 );  // Pop the ClassNameOverride field

    // Pop the LUA_GET_REF_TABLE
    lua_pop( L, 1 );

    LUA_CALL_HOOK_BEGIN( "PreEntityInitialize" );
    CBaseEntity::PushLuaInstanceSafe( L, this );
    LUA_CALL_HOOK_END( 1, 0 );

    LUA_CALL_ENTITY_METHOD_BEGIN( "Initialize" );
    LUA_CALL_ENTITY_METHOD_END( 0, 0 );
#endif
}

#ifdef CLIENT_DLL
RenderGroup_t CBaseScripted::GetRenderGroup()
{
    if ( m_iRenderGroupOverride > -1 )
    {
        return ( RenderGroup_t )m_iRenderGroupOverride;
    }

    return BaseClass::GetRenderGroup();
}

int CBaseScripted::DrawModel( int flags )
{
#ifdef LUA_SDK
    LUA_CALL_ENTITY_METHOD_BEGIN( "Draw" );
    lua_pushinteger( L, flags );
    LUA_CALL_ENTITY_METHOD_END( 1, 1 );

    LUA_RETURN_INTEGER();
#endif

    return BaseClass::DrawModel( flags );
}

void CBaseScripted::OnDataChanged( DataUpdateType_t updateType )
{
    BaseClass::OnDataChanged( updateType );

    if ( updateType == DATA_UPDATE_CREATED )
    {
        if ( m_iScriptedClassname.Get() )
        {
            SetClassname( m_iScriptedClassname.Get() );
            InitScriptedEntity();
        }
    }
}

const char *CBaseScripted::GetScriptedClassname( void )
{
    if ( m_iScriptedClassname.Get() )
        return m_iScriptedClassname.Get();
    return BaseClass::GetClassname();
}
#endif

void CBaseScripted::Spawn( void )
{
    BaseClass::Spawn();

#ifndef CLIENT_DLL
    InitScriptedEntity();
#endif
}

void CBaseScripted::Precache( void )
{
    BaseClass::Precache();

    // InitScriptedEntity();
}

// Override the base class, so we call OnRemove on the Lua scripted weapon.
void CBaseScripted::Remove()
{
    bool fullUpdate = false;  // TODO: implement this argument https://wiki.facepunch.com/gmod/Entity:OnRemove
    LUA_CALL_ENTITY_METHOD_BEGIN( "OnRemove" );
    lua_pushboolean( L, fullUpdate );  // doc: fullUpdate (always false, unimplemented currently)
    LUA_CALL_ENTITY_METHOD_END( 1, 0 );

    BaseClass::Remove();
}

#ifdef CLIENT_DLL
void CBaseScripted::ClientThink()
{
#ifdef LUA_SDK
    LUA_CALL_ENTITY_METHOD_BEGIN( "ClientThink" );
    LUA_CALL_ENTITY_METHOD_END( 0, 0 );
#endif
}
#endif

void CBaseScripted::Think()
{
#ifdef LUA_SDK
    LUA_CALL_ENTITY_METHOD_BEGIN( "Think" );
    LUA_CALL_ENTITY_METHOD_END( 0, 0 );
#endif
}

void CBaseScripted::StartTouch( CBaseEntity *pOther )
{
#ifdef LUA_SDK
    LUA_CALL_ENTITY_METHOD_BEGIN( "StartTouch" );
    CBaseEntity::PushLuaInstanceSafe( L, pOther );
    LUA_CALL_ENTITY_METHOD_END( 1, 0 );
#endif
}

void CBaseScripted::Touch( CBaseEntity *pOther )
{
#ifdef LUA_SDK
    LUA_CALL_ENTITY_METHOD_BEGIN( "Touch" );
    CBaseEntity::PushLuaInstanceSafe( L, pOther );
    LUA_CALL_ENTITY_METHOD_END( 1, 0 );
#endif
}

void CBaseScripted::EndTouch( CBaseEntity *pOther )
{
#ifdef LUA_SDK
    LUA_CALL_ENTITY_METHOD_BEGIN( "EndTouch" );
    CBaseEntity::PushLuaInstanceSafe( L, pOther );
    LUA_CALL_ENTITY_METHOD_END( 1, 0 );
#endif
}

void CBaseScripted::VPhysicsUpdate( IPhysicsObject *pPhysics )
{
    BaseClass::VPhysicsUpdate( pPhysics );

#ifdef LUA_SDK
    LUA_CALL_ENTITY_METHOD_BEGIN( "PhysicsUpdate" );
    lua_pushphysicsobject( L, pPhysics );
    LUA_CALL_ENTITY_METHOD_END( 1, 0 );
#endif
}

#ifndef CLIENT_DLL
int CBaseScripted::UpdateTransmitState()
{
#ifdef LUA_SDK
    LUA_CALL_ENTITY_METHOD_BEGIN( "UpdateTransmitState" );
    LUA_CALL_ENTITY_METHOD_END( 0, 1 );

    LUA_RETURN_INTEGER();
#endif

    return BaseClass::UpdateTransmitState();
}
#endif
