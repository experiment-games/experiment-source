#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "mathlib/lvector.h"
#include "lbaseentity_shared.h"
#ifdef CLIENT_DLL
#include "lc_baseanimating.h"
#include <c_experiment_player.h>
#else
#include "lbaseanimating.h"
#include "experiment_player.h"
#endif
#include <istudiorender.h>
#include <materialsystem/limaterial.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_CBaseAnimating *lua_toanimating( lua_State *L, int idx )
{
    CBaseHandle *hEntity =
        dynamic_cast< CBaseHandle * >( ( CBaseHandle * )lua_touserdata( L, idx ) );
    if ( hEntity == NULL )
        return NULL;
    return dynamic_cast< lua_CBaseAnimating * >( hEntity->Get() );
}

/*
** push functions (C -> stack)
*/
LUALIB_API lua_CBaseAnimating *luaL_checkanimating( lua_State *L, int narg )
{
    lua_CBaseAnimating *d = lua_toanimating( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "CBaseAnimating expected, got NULL entity" );
    return d;
}

LUA_REGISTRATION_INIT( CBaseAnimating )

LUA_BINDING_BEGIN( CBaseAnimating, CalculateIkLocks, "class", "Calculate IK locks." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flTime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "currentTime" );

    pAnimating->CalculateIKLocks( flTime );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ComputeHitboxSurroundingBox, "class", "Compute the hitbox surrounding box." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    Vector pVecWorldMins, pVecWorldMaxs;
    lua_pushboolean( L, pAnimating->ComputeHitboxSurroundingBox( &pVecWorldMins, &pVecWorldMaxs ) );
    lua_pushvector( L, pVecWorldMins );
    lua_pushvector( L, pVecWorldMaxs );

    return 3;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "Vector", "The world mins", "Vector", "The world maxs" )

LUA_BINDING_BEGIN( CBaseAnimating, ComputeEntitySpaceHitboxSurroundingBox, "class", "Compute the entity space hitbox surrounding box." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    Vector pVecWorldMins, pVecWorldMaxs;
    lua_pushboolean( L, pAnimating->ComputeEntitySpaceHitboxSurroundingBox( &pVecWorldMins, &pVecWorldMaxs ) );
    lua_pushvector( L, pVecWorldMins );
    lua_pushvector( L, pVecWorldMaxs );

    return 3;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "Vector", "The world mins", "Vector", "The world maxs" )

LUA_BINDING_BEGIN( CBaseAnimating, DoMuzzleFlash, "class", "Sets up muzzle flash parity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->DoMuzzleFlash();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, GetBonePosition, "class", "Get the bone position." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBone = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bone" );
    Vector pVecOrigin;
    QAngle pAngAngles;

    pAnimating->GetBonePosition( iBone, pVecOrigin, pAngAngles );

    lua_pushvector( L, pVecOrigin );
    lua_pushangle( L, pAngAngles );

    return 2;
}
LUA_BINDING_END( "Vector", "The origin", "Angle", "The angles" )

LUA_BINDING_BEGIN( CBaseAnimating, GetCycle, "class", "Get the current cycle." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushnumber( L, pAnimating->GetCycle() );

    return 1;
}
LUA_BINDING_END( "number", "The cycle" )

LUA_BINDING_BEGIN( CBaseAnimating, GetFlexDescFacs, "class", "Get the flex desc facs." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iFlexDescIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flexDescIndex" );

    lua_pushstring( L, pAnimating->GetFlexDescFacs( iFlexDescIndex ) );

    return 1;
}
LUA_BINDING_END( "string", "The flex desc facs" )

LUA_BINDING_BEGIN( CBaseAnimating, GetHitboxSet, "class", "Get the index of the hitbox set in use." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetHitboxSet() );

    return 1;
}
LUA_BINDING_END( "integer", "The hitbox set" )

LUA_BINDING_BEGIN( CBaseAnimating, GetHitboxSetCount, "class", "Get the hitbox set count." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetHitboxSetCount() );

    return 1;
}
LUA_BINDING_END( "integer", "The hitbox set count" )

LUA_BINDING_BEGIN( CBaseAnimating, GetHitboxSetName, "class", "Get the hitbox set name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushstring( L, pAnimating->GetHitboxSetName() );

    return 1;
}
LUA_BINDING_END( "string", "The hitbox set name" )

// static int CBaseAnimating_GetModelWidthScale (lua_State *L) {
//   lua_pushnumber(L, luaL_checkanimating(L, 1)->GetModelWidthScale());
//   return 1;
// }

LUA_BINDING_BEGIN( CBaseAnimating, GetNumFlexControllers, "class", "Get the number of flex controllers." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetNumFlexControllers() );

    return 1;
}
LUA_BINDING_END( "integer", "The number of flex controllers" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBoneCount, "class", "Get the amount of bones this entity has in its model." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CStudioHdr *pstudiohdr = pAnimating->GetModelPtr();

    if ( !pstudiohdr )
    {
        Warning( "CBaseAnimating::GetBoneCount failed: no model\n" );
        return 0;
    }

    lua_pushinteger( L, pstudiohdr->numbones() );

    return 1;
}
LUA_BINDING_END( "integer", "The amount of bones" )

LUA_BINDING_BEGIN( CBaseAnimating, GetModelName, "class", "Get the model path of the entity" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CStudioHdr *pstudiohdr = pAnimating->GetModelPtr();

    if ( !pstudiohdr )
    {
        Warning( "CBaseAnimating::GetModelName failed: no model\n" );
        return 0;
    }

    lua_pushstring( L, pstudiohdr->pszName() );

    return 1;
}
LUA_BINDING_END( "string", "The model name" )

LUA_BINDING_BEGIN( CBaseAnimating, GetAnimTimeInterval, "class", "Get the animation time interval." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushnumber( L, pAnimating->GetAnimTimeInterval() );

    return 1;
}
LUA_BINDING_END( "number", "The animation time interval" )

// static int CBaseAnimating_GetAttachment( lua_State *L )
LUA_BINDING_BEGIN( CBaseAnimating, GetAttachment, "class", "Get the attachment table for the specified attachment (by bone id or attachment name)" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iArg2Type = lua_type( L, 2 );
    Vector pVecOrigin;
    QAngle pVecAngles;
    int boneID = -1;

    if ( iArg2Type == LUA_TNUMBER )
    {
        boneID = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "boneId" );

        if ( !pAnimating->GetAttachment( boneID, pVecOrigin, pVecAngles ) )
        {
            lua_pushnil( L );
            return 1;
        }
    }
    else if ( iArg2Type == LUA_TSTRING )
    {
        boneID = pAnimating->LookupAttachment( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "attachmentName" ) );

        if ( !pAnimating->GetAttachment( boneID, pVecOrigin, pVecAngles ) )
        {
            lua_pushnil( L );
            return 1;
        }
    }
    else
    {
        luaL_typeerror( L, 2, "number or string" );
    }

    if ( boneID == -1 )
    {
        lua_pushnil( L );
        return 1;
    }

    lua_newtable( L );
    lua_pushvector( L, pVecOrigin );
    lua_setfield( L, -2, "Pos" );  // TODO: Write gmod compat and rename this to our own conventions
    lua_pushangle( L, pVecAngles );
    lua_setfield( L, -2, "Ang" );  // TODO: Write gmod compat and rename this to our own conventions
    lua_pushinteger( L, boneID );
    lua_setfield( L, -2, "Bone" );  // TODO: Write gmod compat and rename this to our own conventions

    return 1;
}
LUA_BINDING_END( "table", "The attachment information" )

LUA_BINDING_BEGIN( CBaseAnimating, GetAttachmentLocal, "class", "Get the local attachment." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iAttachment = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "attachment" );
    Vector pVecOrigin;
    QAngle pAngAngles;

    lua_pushboolean( L, pAnimating->GetAttachmentLocal( iAttachment, pVecOrigin, pAngAngles ) );
    lua_pushvector( L, pVecOrigin );
    lua_pushangle( L, pAngAngles );

    return 3;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "Vector", "The origin", "Angle", "The angles" )

LUA_BINDING_BEGIN( CBaseAnimating, FindBodyGroupByName, "class", "Find the bodygroup id by the bodygroup name" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pBodyGroupName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "bodyGroupName" );

    lua_pushinteger( L, pAnimating->FindBodygroupByName( pBodyGroupName ) );

    return 1;
}
LUA_BINDING_END( "integer", "The bodygroup id" )

LUA_BINDING_BEGIN( CBaseAnimating, FindTransitionSequence, "class", "Find the transition sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );
    int iGoalSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "goalSequence" );

    int piDir;
    lua_pushinteger( L, pAnimating->FindTransitionSequence( iSequence, iGoalSequence, &piDir ) );
    lua_pushinteger( L, piDir );

    return 2;
}
LUA_BINDING_END( "integer", "The transition sequence", "integer", "The direction" )

// Experiment; Returns 'this', so pretty useless in Lua I imagine. Disabled:
// LUA_BINDING_BEGIN( CBaseAnimating, GetBaseAnimating, "class", "Get the base animating entity." )
//{
//    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
//
//    CBaseAnimating::PushLuaInstanceSafe( L, pAnimating->GetBaseAnimating() );
//
//    return 1;
//}
// LUA_BINDING_END( "Entity", "The base animating entity" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroup, "class", "Get the bodygroup value by the bodygroup id" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBodyGroup = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bodyGroupId" );

    lua_pushinteger( L, pAnimating->GetBodygroup( iBodyGroup ) );

    return 1;
}
LUA_BINDING_END( "integer", "The bodygroup value" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroupCount, "class", "Get the number of bodygroups" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBodyGroup = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bodyGroupId" );

    lua_pushinteger( L, pAnimating->GetBodygroupCount( iBodyGroup ) );

    return 1;
}
LUA_BINDING_END( "integer", "The number of bodygroups" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroupName, "class", "Get the bodygroup name by the bodygroup id" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBodyGroup = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bodyGroupId" );

    lua_pushstring( L, pAnimating->GetBodygroupName( iBodyGroup ) );

    return 1;
}
LUA_BINDING_END( "string", "The bodygroup name" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroupsCount, "class", "Get the number of bodygroups" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetNumBodyGroups() );

    return 1;
}
LUA_BINDING_END( "integer", "The number of bodygroups this model has" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceName, "class", "Get the sequence name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushstring( L, pAnimating->GetSequenceName( iSequence ) );

    return 1;
}
LUA_BINDING_END( "string", "The sequence name" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceLinearMotion, "class", "Get the sequence linear motion." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );
    Vector pVec;

    pAnimating->GetSequenceLinearMotion( iSequence, &pVec );

    lua_pushvector( L, pVec );

    return 1;
}
LUA_BINDING_END( "Vector", "The sequence linear motion" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceGroundSpeed, "class", "Get the sequence ground speed." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushnumber( L, pAnimating->GetSequenceGroundSpeed( iSequence ) );

    return 1;
}
LUA_BINDING_END( "number", "The sequence ground speed" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceActivityName, "class", "Get the sequence activity name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushstring( L, pAnimating->GetSequenceActivityName( iSequence ) );

    return 1;
}
LUA_BINDING_END( "string", "The sequence activity name" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequenceActivity, "class", "Get the sequence activity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushinteger( L, pAnimating->GetSequenceActivity( iSequence ) );

    return 1;
}
LUA_BINDING_END( "enumeration/ACTIVITY", "The sequence activity" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequence, "class", "Get the sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetSequence() );

    return 1;
}
LUA_BINDING_END( "integer", "The sequence" )

LUA_BINDING_BEGIN( CBaseAnimating, GetPoseParameter, "class", "Get the pose parameter." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    if ( lua_type( L, 2 ) == LUA_TNUMBER )
    {
        int iPoseParameter = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "poseParameter" );
        lua_pushnumber( L, pAnimating->GetPoseParameter( iPoseParameter ) );
    }
    else if ( lua_type( L, 2 ) == LUA_TSTRING )
    {
        const char *pszPoseParameterName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "poseParameterName" );
        lua_pushnumber( L, pAnimating->GetPoseParameter( pszPoseParameterName ) );
    }
    else
    {
        luaL_typeerror( L, 2, "number or string" );
    }

    return 1;
}
LUA_BINDING_END( "number", "The pose parameter" )

LUA_BINDING_BEGIN( CBaseAnimating, GetPoseParameterRange, "class", "Get the pose parameter range." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iPoseParameter = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "poseParameter" );
    float minValue, maxValue;

    lua_pushboolean( L, pAnimating->GetPoseParameterRange( iPoseParameter, minValue, maxValue ) );
    lua_pushnumber( L, minValue );
    lua_pushnumber( L, maxValue );

    return 3;
}
LUA_BINDING_END( "boolean", "Whether the operation was successful", "number", "The minimum value", "number", "The maximum value" )

LUA_BINDING_BEGIN( CBaseAnimating, GetPlaybackRate, "class", "Get the playback rate." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushnumber( L, pAnimating->GetPlaybackRate() );

    return 1;
}
LUA_BINDING_END( "number", "The playback rate" )

LUA_BINDING_BEGIN( CBaseAnimating, IsOnFire, "class", "Check if the entity is on fire." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsOnFire() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity is on fire" )

LUA_BINDING_BEGIN( CBaseAnimating, IsActivityFinished, "class", "Check if the activity is finished." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsActivityFinished() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the activity is finished" )

LUA_BINDING_BEGIN( CBaseAnimating, InvalidateModelCache, "class", "Invalidate the model cache." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->InvalidateMdlCache();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, InvalidateBoneCache, "class", "Invalidate the bone cache." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->InvalidateBoneCache();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, IsRagdoll, "class", "Check if the entity is a ragdoll." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsRagdoll() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the entity is a ragdoll" )

LUA_BINDING_BEGIN( CBaseAnimating, IsSequenceFinished, "class", "Check if the sequence is finished." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->IsSequenceFinished() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the sequence is finished" )

LUA_BINDING_BEGIN( CBaseAnimating, IsSequenceLooping, "class", "Check if the sequence is looping." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    lua_pushboolean( L, pAnimating->IsSequenceLooping( iSequence ) );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the sequence is looping" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupActivity, "class", "Looks up an activity by name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszActivityName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "activityName" );

    lua_pushinteger( L, pAnimating->LookupActivity( pszActivityName ) );

    return 1;
}
LUA_BINDING_END( "enumeration/ACTIVITY", "The activity" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupAttachment, "class", "Lookup the attachment." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszAttachmentName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "attachmentName" );

    lua_pushinteger( L, pAnimating->LookupAttachment( pszAttachmentName ) );

    return 1;
}
LUA_BINDING_END( "integer", "The attachment" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupBone, "class", "Lookup the bone." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszBoneName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "boneName" );

    lua_pushinteger( L, pAnimating->LookupBone( pszBoneName ) );

    return 1;
}
LUA_BINDING_END( "integer", "The bone" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupPoseParameter, "class", "Lookup the pose parameter." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszPoseParameterName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "poseParameterName" );

    lua_pushinteger( L, pAnimating->LookupPoseParameter( pszPoseParameterName ) );

    return 1;
}
LUA_BINDING_END( "integer", "The pose parameter" )

LUA_BINDING_BEGIN( CBaseAnimating, LookupSequence, "class", "Lookup the sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszSequenceName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "sequenceName" );

    lua_pushinteger( L, pAnimating->LookupSequence( pszSequenceName ) );

    return 1;
}
LUA_BINDING_END( "integer", "The sequence" )

LUA_BINDING_BEGIN( CBaseAnimating, GetRagdollOwner, "class", "Returns the player this ragdoll came from. NULL if this is not a player ragdoll (will warn)" )
{
    // TODO: We should probably do this soft-check (toanimating) for every baseanimating function?
    // In this case we do it because pac3 would error for non-animating entities, e.g:
    // [CLIENT] ...periment\addons\pac3\lua\pac3\core\client\parts\bone.lua:308: calling 'GetRagdollOwner' on bad self (CBaseAnimating expected, got NULL entity)
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( lua_toanimating, 1, "entity" );

    if ( !pAnimating )
    {
        CBasePlayer::PushLuaInstanceSafe( L, NULL );
        return 1;
    }

#ifdef CLIENT_DLL
    C_ExperimentRagdoll *pRagdoll = dynamic_cast< C_ExperimentRagdoll * >( pAnimating );
#else
    CExperimentRagdoll *pRagdoll = dynamic_cast< CExperimentRagdoll * >( pAnimating );
#endif

    if ( !pRagdoll )
    {
        CBasePlayer::PushLuaInstanceSafe( L, NULL );
        return 1;
    }

    CBasePlayer::PushLuaInstanceSafe( L, pRagdoll->GetRagdollPlayer() );
    return 1;
}
LUA_BINDING_END( "Player", "The player this ragdoll came from" )

LUA_BINDING_BEGIN( CBaseAnimating, ResetSequence, "class", "Reset the sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = -1;

    if ( lua_type( L, 2 ) == LUA_TNUMBER )
    {
        iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );
    }
    else if ( lua_type( L, 2 ) == LUA_TSTRING )
    {
        iSequence = pAnimating->LookupSequence(
            LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "sequence" ) );
    }
    else
    {
        luaL_typeerror( L, 2, "number or string" );
    }

    auto model = pAnimating->GetModelPtr();

    if ( !model )
    {
        DevWarning( "CBaseAnimating::ResetSequence failed: no model\n" );
        return 0;
    }

    if ( iSequence >= model->GetNumSeq() )
    {
        DevWarning( "CBaseAnimating::ResetSequence failed: invalid sequence %d\n", iSequence );
        return 0;
    }

    pAnimating->ResetSequence( iSequence );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, ResetSequenceInfo, "class", "Reset the sequence info." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->ResetSequenceInfo();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SelectWeightedSequence, "class", "Select the weighted sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iActivity = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "activity" );

#ifdef CLIENT_DLL
    lua_pushinteger( L, pAnimating->SelectWeightedSequence( iActivity ) );
#else
    lua_pushinteger( L, pAnimating->SelectWeightedSequence( ( Activity )iActivity ) );
#endif

    return 1;
}
LUA_BINDING_END( "integer", "The weighted sequence" )

LUA_BINDING_BEGIN( CBaseAnimating, SequenceDuration, "class", "Sequence duration." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNONE:
        default:
            lua_pushnumber( L, pAnimating->SequenceDuration() );
            break;
        case LUA_TNUMBER:
            int iSequence = LUA_BINDING_ARGUMENT_NILLABLE( luaL_checknumber, 2, "sequence" );
            lua_pushnumber( L, pAnimating->SequenceDuration( iSequence ) );
            break;
    }

    return 1;
}
LUA_BINDING_END( "number", "The sequence duration" )

LUA_BINDING_BEGIN( CBaseAnimating, SequenceLoops, "class", "Check if the sequence loops." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushboolean( L, pAnimating->SequenceLoops() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the sequence loops" )

LUA_BINDING_BEGIN( CBaseAnimating, SetBoneController, "class", "Set the value for the specified bone controller." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBoneController = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "boneController" );
    float flValue = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );

    lua_pushnumber( L, pAnimating->SetBoneController( iBoneController, flValue ) );

    return 1;
}
LUA_BINDING_END( "number", "The bone controller value, clamped between acceptable values" )

LUA_BINDING_BEGIN( CBaseAnimating, SetCycle, "class", "Set the cycle." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flCycle = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "cycle" );

    pAnimating->SetCycle( flCycle );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetHitboxSet, "class", "Set the hitbox set." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iHitboxSet = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "hitboxSet" );

    pAnimating->SetHitboxSet( iHitboxSet );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetHitboxSetByName, "class", "Set the hitbox set by name." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszHitboxSetName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "hitboxSetName" );

    pAnimating->SetHitboxSetByName( pszHitboxSetName );

    return 0;
}
LUA_BINDING_END()

// TODO:
// static int CBaseAnimating_SetModelWidthScale (lua_State *L) {
//   luaL_checkanimating(L, 1)->SetModelWidthScale(luaL_checknumber(L, 2));
//   return 0;
// }

LUA_BINDING_BEGIN( CBaseAnimating, SetPoseParameter, "class", "Set the pose parameter." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            lua_pushnumber( L, pAnimating->SetPoseParameter( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "poseParameter" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" ) ) );
            break;
        case LUA_TSTRING:
        default:
            lua_pushnumber( L, pAnimating->SetPoseParameter( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "poseParameter" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" ) ) );
            break;
    }

    return 1;
}
LUA_BINDING_END( "number", "The pose parameter" )

LUA_BINDING_BEGIN( CBaseAnimating, SetBodyGroup, "class", "Set the bodygroup value by the bodygroup id" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBodyGroup = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bodyGroupId" );
    int iValue = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );

    pAnimating->SetBodygroup( iBodyGroup, iValue );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetBodyGroups, "class", "Set the bodygroup values by the bodygroup string. Each hexadecimal character represents the bodygroup at its index, e.g: 0a00001 sets bodygroup 1 to 10(a) and bodygroup 6 to 1, the rest are set to 0" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    const char *pBodyGroups = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "bodyGroupString" );
    int iNumGroups = Q_strlen( pBodyGroups );

    if ( iNumGroups <= 0 )
        return 0;

    int nMaxGroups = pAnimating->GetNumBodyGroups();

    for ( int iGroup = 0; iGroup < iNumGroups && iGroup < nMaxGroups; ++iGroup )
    {
        char c = pBodyGroups[iGroup];
        int iValue = 0;

        if ( c >= '0' && c <= '9' )
        {
            iValue = c - '0';  // 0 - 9
        }
        else if ( c >= 'a' && c <= 'z' )
        {
            iValue = c - 'a' + 10;  // 10 - 35
        }

        pAnimating->SetBodygroup( iGroup, iValue );
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroupsAsString, "class", "Get the bodygroup values as a string of hexadecimal values. Each hexadecimal character represents the bodygroup at its index, e.g: 0a00001 means bodygroup 1 is 10(a) and bodygroup 6 is 1, the rest are 0" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int nMaxGroups = pAnimating->GetNumBodyGroups();

    char *pBodyGroups = ( char * )_alloca( nMaxGroups + 1 );
    pBodyGroups[nMaxGroups] = 0;

    for ( int iGroup = 0; iGroup < nMaxGroups; ++iGroup )
    {
        int iValue = pAnimating->GetBodygroup( iGroup );

        if ( iValue >= 0 && iValue <= 9 )
        {
            pBodyGroups[iGroup] = '0' + iValue;
        }
        else if ( iValue >= 10 && iValue <= 35 )
        {
            pBodyGroups[iGroup] = 'a' + iValue - 10;
        }
        else
        {
            pBodyGroups[iGroup] = '0';
        }
    }

    lua_pushstring( L, pBodyGroups );

    return 1;
}
LUA_BINDING_END( "string", "The bodygroup values as a string of hexadecimal values" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroups, "class", "Get the bodygroup values as a table" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    int nMaxGroups = pAnimating->GetNumBodyGroups();
    CStudioHdr *pstudiohdr = pAnimating->GetModelPtr();

    if ( !pstudiohdr )
    {
        Warning( "CBaseAnimating::GetBodyGroups failed: no model\n" );
        return 0;
    }

    lua_newtable( L );

    for ( int iGroup = 0; iGroup < nMaxGroups; ++iGroup )
    {
        // TODO: Make a nice push struct function for this
        lua_newtable( L );

        lua_pushinteger( L, iGroup );
        lua_setfield( L, -2, "id" );

        lua_pushstring( L, pAnimating->GetBodygroupName( iGroup ) );
        lua_setfield( L, -2, "name" );

        int subGroupCount = pAnimating->GetBodygroupCount( iGroup );
        lua_pushinteger( L, subGroupCount );
        lua_setfield( L, -2, "num" );

        // zero indexed table of names in the smd mesh file indicating what valid subgroup values are
        lua_newtable( L );

        mstudiobodyparts_t *pbodypart = pstudiohdr->pBodypart( iGroup );

        for ( int iSubGroup = 0; iSubGroup < subGroupCount; ++iSubGroup )
        {
            mstudiomodel_t *submodel = pbodypart->pModel( iSubGroup );
            lua_pushstring( L, submodel->pszName() );
            lua_rawseti( L, -2, iSubGroup );
        }

        lua_setfield( L, -2, "submodels" );

        lua_rawseti( L, -2, iGroup );
    }

    return 1;
}
LUA_BINDING_END( "table", "The bodygroup values" )

LUA_BINDING_BEGIN( CBaseAnimating, SetPlaybackRate, "class", "Set the playback rate of the animation" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flPlaybackRate = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "rate" );

    pAnimating->SetPlaybackRate( flPlaybackRate );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, SetSkin, "class", "Set the skin of the entity" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSkin = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "skin" );

    pAnimating->m_nSkin = iSkin;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, GetFlexBounds, "class", "Returns the min and max values for the target flex controller" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int flexControllerIndex = ( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flexControllerIndex" );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )flexControllerIndex;
    CStudioHdr *pStudioHdr = pAnimating->GetModelPtr();

    Assert( pStudioHdr );

    mstudioflexcontroller_t *flex = pStudioHdr->pFlexcontroller( iFlexController );

    Assert( flex );

    lua_pushnumber( L, flex->min );
    lua_pushnumber( L, flex->max );

    return 2;
}
LUA_BINDING_END( "number", "The min value", "number", "The max value" )

LUA_BINDING_BEGIN( CBaseAnimating, GetFlexCount, "class", "Get the number of flex controllers" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetNumFlexControllers() );

    return 1;
}
LUA_BINDING_END( "integer", "The number of flex controllers" )

LUA_BINDING_BEGIN( CBaseAnimating, GetFlexName, "class", "Get the flex controller name by the flex controller index" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int flexControllerIndex = ( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flexControllerIndex" );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )flexControllerIndex;

    lua_pushstring( L, pAnimating->GetFlexControllerName( iFlexController ) );

    return 1;
}
LUA_BINDING_END( "string", "The flex controller name" )

LUA_BINDING_BEGIN( CBaseAnimating, SetMaterialOverride, "class", "Set the material override" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pszMaterialName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "materialName" );

    pAnimating->SetMaterialOverride( pszMaterialName );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, GetMaterialOverride, "class", "Get the material override" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    char pszMaterialName[MAX_PATH];

    pAnimating->GetMaterialOverride( pszMaterialName, sizeof( pszMaterialName ) );

    lua_pushstring( L, pszMaterialName );

    return 1;
}
LUA_BINDING_END( "string", "The material override" )

LUA_BINDING_BEGIN( CBaseAnimating, SetSubMaterialOverride, "class", "Set the sub material override by it's index (which starts at 0 and runs until at most 31). If called without arguments, it will reset all submaterials." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    if ( lua_gettop( L ) == 1 )
    {
        pAnimating->ClearSubMaterialOverrides();
        return 0;
    }

    int iSubMaterial = LUA_BINDING_ARGUMENT_NILLABLE( luaL_checknumber, 2, "subMaterialIndex" );
    const char *pszMaterialName = LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkstring, 3, "materialName" );

    pAnimating->SetSubMaterialOverride( iSubMaterial, pszMaterialName );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, GetSubMaterialOverride, "class", "Get the sub material override by it's index (which starts at 0 and runs until at most 31)" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSubMaterial = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "subMaterialIndex" );
    char pszMaterialName[MAX_PATH];

    pAnimating->GetSubMaterialOverride( iSubMaterial, pszMaterialName, sizeof( pszMaterialName ) );

    lua_pushstring( L, pszMaterialName );

    return 1;
}
LUA_BINDING_END( "string", "The sub material override" )

LUA_BINDING_BEGIN( CBaseAnimating, GetMaterials, "class", "Get the materials" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CStudioHdr *pStudioHdr = pAnimating->GetModelPtr();

    if ( !pStudioHdr )
    {
        lua_pushnil( L );
        return 1;
    }

    const studiohdr_t *pRenderHdr = pStudioHdr->GetRenderHdr();

    IMaterial *pMaterials[128];
    int iMaterialCount = g_pStudioRender->GetMaterialList( ( studiohdr_t * )pRenderHdr, ARRAYSIZE( pMaterials ), pMaterials );

    lua_newtable( L );

    for ( int i = 0; i < iMaterialCount; i++ )
    {
        IMaterial *pMaterial = pMaterials[i];

        if ( !pMaterial )
            continue;

        lua_pushnumber( L, i + 1 );  // 1 indexed
        lua_pushmaterial( L, pMaterial );
        lua_settable( L, -3 );
    }

    lua_pushinteger( L, iMaterialCount );
    return 2;
}
LUA_BINDING_END( "table", "The materials", "integer", "The number of materials" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSubModels, "class", "Get the submodels" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CStudioHdr *pStudioHdr = pAnimating->GetModelPtr();

    if ( !pStudioHdr )
    {
        lua_pushnil( L );
        return 1;
    }

    lua_newtable( L );

    for ( int i = 0; i < pStudioHdr->numbodyparts(); i++ )
    {
        mstudiobodyparts_t *pBodyPart = pStudioHdr->pBodypart( i );

        // TODO: Make a nice struct push function for this
        lua_newtable( L );

        lua_pushstring( L, pBodyPart->pszName() );
        lua_setfield( L, -2, "name" );

        lua_pushinteger( L, pBodyPart->modelindex );
        lua_setfield( L, -2, "id" );

        lua_rawseti( L, -2, i + 1 );  // 1 indexed
    }

    return 1;
}
LUA_BINDING_END( "table", "The submodels" )

LUA_BINDING_BEGIN( CBaseAnimating, GetSequences, "class", "Get all sequences the model has." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    CStudioHdr *pStudioHdr = pAnimating->GetModelPtr();

    if ( !pStudioHdr )
    {
        DevWarning( "CBaseAnimating::GetSequences failed: no model\n" );
        lua_newtable( L );
        return 1;
    }

    lua_newtable( L );

    for ( int i = 0; i < pStudioHdr->GetNumSeq(); i++ )
    {
        mstudioseqdesc_t &seqdesc = pStudioHdr->pSeqdesc( i );

        lua_pushnumber( L, i + 1 );  // 1 indexed
        lua_pushstring( L, seqdesc.pszLabel() );
        lua_settable( L, -3 );
    }

    return 1;
}
LUA_BINDING_END( "table", "The sequences" )

LUA_BINDING_BEGIN( CBaseAnimating, SetSequence, "class", "Set the sequence." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iSequence = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "sequence" );

    pAnimating->SetSequence( iSequence );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, StudioFrameAdvance, "class", "Advances the animation frame." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->StudioFrameAdvance();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, TransferDissolveFrom, "class", "Transfer the dissolve from." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    lua_CBaseAnimating *pAnimatingFrom = LUA_BINDING_ARGUMENT( luaL_checkanimating, 2, "entityFrom" );

    pAnimating->TransferDissolveFrom( pAnimatingFrom );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, UseClientSideAnimation, "class", "Use the client side animation." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->UseClientSideAnimation();

    return 0;
}
LUA_BINDING_END()

// static int CBaseAnimating___index( lua_State *L )
//{
//     CBaseAnimating *pEntity = lua_toanimating( L, 1 );
//
//     LUA_METATABLE_INDEX_CHECK_VALID( L, CBaseEntity_IsValid );
//     LUA_METATABLE_INDEX_CHECK( L, pEntity );
//
//     const char *field = luaL_checkstring( L, 2 );
//
//     if ( Q_strcmp( field, "m_nBody" ) == 0 )
//         lua_pushnumber( L, pEntity->m_nBody );
//     else if ( Q_strcmp( field, "m_nHitboxSet" ) == 0 )
//         lua_pushnumber( L, pEntity->m_nHitboxSet );
//     else if ( Q_strcmp( field, "m_nSkin" ) == 0 )
//         lua_pushnumber( L, pEntity->m_nSkin );
// #ifdef CLIENT_DLL
//     else if ( Q_strcmp( field, "m_bClientSideAnimation" ) == 0 )
//         lua_pushboolean( L, pEntity->m_bClientSideAnimation );
//     else if ( Q_strcmp( field, "m_bLastClientSideFrameReset" ) == 0 )
//         lua_pushboolean( L, pEntity->m_bLastClientSideFrameReset );
// #endif
//     else
//     {
//         LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, pEntity );
//
//         if ( lua_getmetatable( L, 1 ) )
//         {
//             LUA_METATABLE_INDEX_CHECK_TABLE( L );
//         }
//
//         luaL_getmetatable( L, LUA_BASEANIMATINGLIBNAME );
//         LUA_METATABLE_INDEX_CHECK_TABLE( L );
//
//         LUA_METATABLE_INDEX_DERIVE_INDEX( L, LUA_BASEENTITYMETANAME );
//
//         lua_pushnil( L );
//     }
//
//     return 1;
// }

// static int CBaseAnimating___newindex( lua_State *L )
//{
//     CBaseAnimating *pEntity = lua_toanimating( L, 1 );
//
//     if ( pEntity == NULL )
//     { /* avoid extra test when d is not 0 */
//         lua_Debug ar1;
//         lua_getstack( L, 1, &ar1 );
//         lua_getinfo( L, "fl", &ar1 );
//         lua_Debug ar2;
//         lua_getinfo( L, ">S", &ar2 );
//         lua_pushfstring( L, "%s:%d: attempt to index a NULL entity", ar2.short_src, ar1.currentline );
//         return lua_error( L );
//     }
//
//     const char *field = luaL_checkstring( L, 2 );
//
//     if ( Q_strcmp( field, "m_nBody" ) == 0 )
//         pEntity->m_nBody = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_nHitboxSet" ) == 0 )
//         pEntity->m_nHitboxSet = luaL_checknumber( L, 3 );
//     else if ( Q_strcmp( field, "m_nSkin" ) == 0 )
//         pEntity->m_nSkin = luaL_checknumber( L, 3 );
// #ifdef CLIENT_DLL
//     else if( Q_strcmp( field, "m_bClientSideAnimation" ) == 0 )
//         pEntity->m_bClientSideAnimation = luaL_checkboolean( L, 3 );
//     else if ( Q_strcmp( field, "m_bLastClientSideFrameReset" ) == 0 )
//         pEntity->m_bLastClientSideFrameReset = luaL_checkboolean( L, 3 );
// #endif
//     else
//     {
//         LUA_GET_REF_TABLE( L, pEntity );
//         lua_pushvalue( L, 3 );
//         lua_setfield( L, -2, field );
//         lua_pop( L, 1 );
//     }
//
//     return 0;
// }

// Experiment; We only let CBaseEntity determine equality, which should be fine since they're pointers to the same entity. Disabled:
// LUA_BINDING_BEGIN( CBaseAnimating, __eq, "class", "Metamethod to check if the two entities are the same" )
//{
//    lua_CBaseAnimating *pAnimating1 = LUA_BINDING_ARGUMENT( lua_toanimating, 1, "entity" );
//    lua_CBaseAnimating *pAnimating2 = LUA_BINDING_ARGUMENT( lua_toanimating, 2, "other" );
//
//    lua_pushboolean( L, pAnimating1 == pAnimating2 );
//
//    return 1;
//}
// LUA_BINDING_END( "boolean", "True if the entities are the same, false otherwise" )
//
// LUA_BINDING_BEGIN( CBaseAnimating, __tostring, "class", "Metamethod to get the string representation of the entity" )
//{
//    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( lua_toanimating, 1, "entity" );
//
//    if ( pAnimating == NULL )
//    {
//        lua_pushstring( L, "NULL" );
//    }
//    else
//    {
//        lua_pushfstring( L, "CBaseAnimating: %d \"%s\"", pAnimating->entindex(), pAnimating->GetClassname() );
//    }
//
//    return 1;
//}
// LUA_BINDING_END( "string", "The string representation of the entity" )

/*
** Open CBaseAnimating object
*/
LUALIB_API int luaopen_CBaseAnimating_shared( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_BASEANIMATINGLIBNAME );

    lua_pushstring( L, "Entity" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */

    LUA_REGISTRATION_COMMIT( CBaseAnimating );

    return 1;
}
