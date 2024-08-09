#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "mathlib/lvector.h"
#include "lbaseentity_shared.h"
#ifdef CLIENT_DLL
#include "lc_baseanimating.h"
#else
#include "lbaseanimating.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CUtlVector< LuaRegEntry > luaRegistry;

LUA_BINDING_BEGIN( CBaseAnimating, GetModelName, "class", "Get the model path of the entity" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->GetModelPtr()->pszName();

    return 1;
}
LUA_BINDING_END( "string", "The model name" )

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

static int CBaseAnimating_FindBodygroupByName( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->FindBodygroupByName( luaL_checkstring( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_GetBodygroup( lua_State *L )
{
    lua_pushinteger(
        L, luaL_checkanimating( L, 1 )->GetBodygroup( luaL_checknumber( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_GetBodygroupCount( lua_State *L )
{
    lua_pushinteger(
        L, luaL_checkanimating( L, 1 )->GetBodygroupCount( luaL_checknumber( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_GetBodygroupName( lua_State *L )
{
    lua_pushstring(
        L, luaL_checkanimating( L, 1 )->GetBodygroupName( luaL_checknumber( L, 2 ) ) );
    return 1;
}

static int CBaseAnimating_GetNumBodyGroups( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->GetNumBodyGroups() );
    return 1;
}

static int CBaseAnimating_SetBodygroup( lua_State *L )
{
    luaL_checkanimating( L, 1 )->SetBodygroup( luaL_checknumber( L, 2 ),
                                               luaL_checknumber( L, 3 ) );
    return 0;
}

static int CBaseAnimating_SetBodyGroups( lua_State *L )
{
    const char *pBodyGroups = luaL_checkstring( L, 2 );
    int iNumGroups = Q_strlen( pBodyGroups );

    if ( iNumGroups > 0 )
    {
        lua_CBaseAnimating *pAnimating = luaL_checkanimating( L, 1 );
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
    }
    return 0;
}

static int CBaseAnimating_GetBodyGroups( lua_State *L )
{
    lua_CBaseAnimating *pAnimating = luaL_checkanimating( L, 1 );
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

static int CBaseAnimating_SetPlaybackRate( lua_State *L )
{
    luaL_checkanimating( L, 1 )->SetPlaybackRate( luaL_checknumber( L, 2 ) );
    return 0;
}

static int CBaseAnimating_SetSkin( lua_State *L )
{
    // TODO: SetSkin
    // TODO: There's also logic in lbaseanimating.__newindex for this
    // TODO: Placed here because its easily shared, but we should really create a common place for baseanimating shared functions
    lua_toanimating( L, 1 )->m_nSkin = luaL_checknumber( L, 2 );
    return 0;
}

// returns the min and max values for the flex controller
static int CBaseAnimating_GetFlexBounds( lua_State *L )
{
    lua_CBaseAnimating *pAnimating = luaL_checkanimating( L, 1 );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )( int )luaL_checknumber( L, 2 );
    CStudioHdr *pStudioHdr = pAnimating->GetModelPtr();

    Assert( pStudioHdr );

    mstudioflexcontroller_t *flex = pStudioHdr->pFlexcontroller( iFlexController );

    Assert( flex );

    lua_pushnumber( L, flex->min );
    lua_pushnumber( L, flex->max );

    return 2;
}

static int CBaseAnimating_GetFlexCount( lua_State *L )
{
    lua_pushinteger( L, luaL_checkanimating( L, 1 )->GetNumFlexControllers() );
    return 1;
}

static int CBaseAnimating_GetFlexName( lua_State *L )
{
    lua_CBaseAnimating *pAnimating = luaL_checkanimating( L, 1 );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )( int )luaL_checknumber( L, 2 );

    lua_pushstring( L, pAnimating->GetFlexControllerName( iFlexController ) );
    return 1;
}

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
//         LUA_METATABLE_INDEX_DERIVE_INDEX( L, LUA_BASEENTITYLIBNAME );
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

static int CBaseAnimating___eq( lua_State *L )
{
    lua_pushboolean( L, lua_toanimating( L, 1 ) == lua_toanimating( L, 2 ) );
    return 1;
}

static int CBaseAnimating___tostring( lua_State *L )
{
    CBaseAnimating *pEntity = lua_toanimating( L, 1 );
    if ( pEntity == NULL )
        lua_pushstring( L, "NULL" );
    else
        lua_pushfstring( L, "CBaseAnimating: %d \"%s\"", pEntity->entindex(), pEntity->GetClassname() );
    return 1;
}

static const luaL_Reg CBaseAnimatingmeta[] = {
    //{ "GetModelName", CBaseAnimating_GetModelName },
    //{ "GetAttachment", CBaseAnimating_GetAttachment },

    // Wonky naming conventions, lets just support both out-of-the-box
    { "FindBodygroupByName", CBaseAnimating_FindBodygroupByName },
    { "FindBodyGroupByName", CBaseAnimating_FindBodygroupByName },

    { "GetBodygroup", CBaseAnimating_GetBodygroup },
    { "GetBodyGroup", CBaseAnimating_GetBodygroup },
    { "GetBodygroupCount", CBaseAnimating_GetBodygroupCount },
    { "GetBodyGroupCount", CBaseAnimating_GetBodygroupCount },
    { "GetBodygroupName", CBaseAnimating_GetBodygroupName },
    { "GetBodyGroupName", CBaseAnimating_GetBodygroupName },
    { "GetBodygroups", CBaseAnimating_GetBodyGroups },
    { "GetBodyGroups", CBaseAnimating_GetBodyGroups },

    { "SetBodygroup", CBaseAnimating_SetBodygroup },
    { "SetBodyGroup", CBaseAnimating_SetBodygroup },
    { "SetBodygroups", CBaseAnimating_SetBodyGroups },
    { "SetBodyGroups", CBaseAnimating_SetBodyGroups },

    { "GetNumBodygroups", CBaseAnimating_GetNumBodyGroups },
    { "GetNumBodyGroups", CBaseAnimating_GetNumBodyGroups },

    { "SetPlaybackRate", CBaseAnimating_SetPlaybackRate },
    { "SetSkin", CBaseAnimating_SetSkin },

    { "GetFlexBounds", CBaseAnimating_GetFlexBounds },
    { "GetFlexCount", CBaseAnimating_GetFlexCount },
    { "GetFlexName", CBaseAnimating_GetFlexName },
    // { "GetFlexScale", CBaseAnimating_GetFlexScale }, // TODO: How is this implemented?

    // { "__index", CBaseAnimating___index }, // In Lua now
    // { "__newindex", CBaseAnimating___newindex }, // Conflicts when storing with CBaseEntity ref table
    { "__eq", CBaseAnimating___eq },
    { "__tostring", CBaseAnimating___tostring },

    { NULL, NULL } };

/*
** Open CBaseAnimating object
*/
LUALIB_API int luaopen_CBaseAnimating_shared( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_BASEANIMATINGLIBNAME );
    luaL_register( L, NULL, CBaseAnimatingmeta );

    luaL_register( L, NULL, luaRegistry );

    return 1;
}
