#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lbaseentity_shared.h"
#include "mathlib/lvector.h"
#include "model_types.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( CBaseEntity )

LUA_BINDING_BEGIN( CBaseEntity, SpawnClientEntity, "class", "Spawn client entity." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->SpawnClientEntity();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, Interp_HierarchyUpdateInterpolationAmounts, "class", "Update interpolation amounts." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->Interp_HierarchyUpdateInterpolationAmounts();
    return 0;
}
LUA_BINDING_END()

// Experiment; not binding this, since I feel it'll conflict with gmod Init on scripted ents
//LUA_BINDING_BEGIN( CBaseEntity, Init, "class", "Initialize." )
//{
//    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
//    int entnum = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "entityNumber" );
//    int iSerialNum = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "serialNumber" );
//    lua_pushboolean( L, pEntity->Init( entnum, iSerialNum ) );
//    return 1;
//}
//LUA_BINDING_END( "boolean", "true if successful, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, Terminate, "class", "Called in the destructor to shutdown everything." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->Term();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, EnableInToolView, "class", "Enable in tool view." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bEnable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" );
    pEntity->EnableInToolView( bEnable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, IsEnabledInToolView, "class", "Is enabled in tool view." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsEnabledInToolView() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if enabled, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, SetIK, "class", "Set IK." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bUseIK = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "useIK" );
    if ( bUseIK )
        pEntity->m_EntClientFlags &= ~ENTCLIENTFLAG_DONTUSEIK;
    else
        pEntity->m_EntClientFlags |= ENTCLIENTFLAG_DONTUSEIK;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, SetToolRecording, "class", "Set tool recording." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    bool bRecording = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "recording" );
    pEntity->SetToolRecording( bRecording );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, IsToolRecording, "class", "Is tool recording." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsToolRecording() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if recording, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, HasRecordedThisFrame, "class", "Has recorded this frame." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->HasRecordedThisFrame() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if recorded, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, RecordToolMessage, "class", "Record tool message." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->RecordToolMessage();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, DontRecordInTools, "class", "Don't record in tools." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->DontRecordInTools();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, ShouldRecordInTools, "class", "Should record in tools." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->ShouldRecordInTools() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if should record, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, Release, "class", "Release." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->Release();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, GetRenderOrigin, "class", "Get render origin." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector v = pEntity->GetRenderOrigin();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "vector", "Render origin." )

LUA_BINDING_BEGIN( CBaseEntity, GetRenderAngles, "class", "Get render angles." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    QAngle v = pEntity->GetRenderAngles();
    lua_pushangle( L, v );
    return 1;
}
LUA_BINDING_END( "angle", "Render angles." )

LUA_BINDING_BEGIN( CBaseEntity, GetObserverCameraOrigin, "class", "Get observer camera origin." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    Vector v = pEntity->GetObserverCamOrigin();
    lua_pushvector( L, v );
    return 1;
}
LUA_BINDING_END( "vector", "Observer camera origin." )

LUA_BINDING_BEGIN( CBaseEntity, IsTwoPass, "class", "Is two pass." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->IsTwoPass() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if two pass, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, UsesPowerOfTwoFrameBufferTexture, "class", "Uses power of two frame buffer texture." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->UsesPowerOfTwoFrameBufferTexture() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if uses power of two frame buffer texture, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, UsesFullFrameBufferTexture, "class", "Uses full frame buffer texture." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushboolean( L, pEntity->UsesFullFrameBufferTexture() );
    return 1;
}
LUA_BINDING_END( "boolean", "true if uses full frame buffer texture, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, DrawModel, "class", "Draw model." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    int flags = ( int )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, STUDIO_RENDER, "flags" );
    lua_pushinteger( L, pEntity->DrawModel( flags ) );
    return 1;
}
LUA_BINDING_END( "integer", "Draw model result." )

LUA_BINDING_BEGIN( CBaseEntity, ComputeRenderEffectsBlend, "class", "Compute RenderFX blend." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    pEntity->ComputeFxBlend();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseEntity, GetRenderEffectsBlend, "class", "Get RenderFX blend." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    lua_pushinteger( L, pEntity->GetFxBlend() );
    return 1;
}
LUA_BINDING_END( "integer", "RenderFX blend." )

// Experiment; not binding this, since it's commented as being there for backwards compatibility
//LUA_BINDING_BEGIN( CBaseEntity, LODTest, "class", "Level of detail test." )
//{
//    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
//    lua_pushboolean( L, pEntity->LODTest() );
//    return 1;
//}
//LUA_BINDING_END( "boolean", "true if LOD test passed, false otherwise." )

LUA_BINDING_BEGIN( CBaseEntity, SetNextClientThink, "class", "Set next client think." )
{
    lua_CBaseEntity *pEntity = LUA_BINDING_ARGUMENT( luaL_checkentity, 1, "entity" );
    float flNextThink = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "nextThink" );
    pEntity->SetNextClientThink( flNextThink );
    return 0;
}
LUA_BINDING_END()

/*
** Open CBaseEntity object
*/
LUALIB_API int luaopen_CBaseEntity( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEENTITYMETANAME );

    LUA_REGISTRATION_COMMIT( CBaseEntity );

    return 1;
}
