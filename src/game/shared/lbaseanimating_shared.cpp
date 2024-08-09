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

LUA_REGISTRATION_INIT()

LUA_BINDING_BEGIN( CBaseAnimating, GetModelName, "class", "Get the model path of the entity" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushstring( L, pAnimating->GetModelPtr()->pszName() );

    return 1;
}
LUA_BINDING_END( "string", "The model name" )

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

LUA_BINDING_BEGIN( CBaseAnimating, FindBodyGroupByName, "class", "Find the bodygroup id by the bodygroup name" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    const char *pBodyGroupName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "bodyGroupName" );

    lua_pushinteger( L, pAnimating->FindBodygroupByName( pBodyGroupName ) );

    return 1;
}
LUA_BINDING_END( "number", "The bodygroup id" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroup, "class", "Get the bodygroup value by the bodygroup id" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBodyGroup = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bodyGroupId" );

    lua_pushinteger( L, pAnimating->GetBodygroup( iBodyGroup ) );

    return 1;
}
LUA_BINDING_END( "number", "The bodygroup value" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroupCount, "class", "Get the number of bodygroups" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBodyGroup = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bodyGroupId" );

    lua_pushinteger( L, pAnimating->GetBodygroupCount( iBodyGroup ) );

    return 1;
}
LUA_BINDING_END( "number", "The number of bodygroups" )

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroupName, "class", "Get the bodygroup name by the bodygroup id" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int iBodyGroup = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "bodyGroupId" );

    lua_pushstring( L, pAnimating->GetBodygroupName( iBodyGroup ) );

    return 1;
}
LUA_BINDING_END( "string", "The bodygroup name" )

LUA_BINDING_BEGIN( CBaseAnimating, GetNumBodyGroups, "class", "Get the number of bodygroups" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    lua_pushinteger( L, pAnimating->GetNumBodyGroups() );

    return 1;
}
LUA_BINDING_END( "number", "The number of bodygroups" )

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

LUA_BINDING_BEGIN( CBaseAnimating, GetBodyGroups, "class", "Get the bodygroup values as a string of hexadecimal values. Each hexadecimal character represents the bodygroup at its index, e.g: 0a00001 means bodygroup 1 is 10(a) and bodygroup 6 is 1, the rest are 0" )
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
LUA_BINDING_END( "number", "The number of flex controllers" )

LUA_BINDING_BEGIN( CBaseAnimating, GetFlexName, "class", "Get the flex controller name by the flex controller index" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    int flexControllerIndex = ( int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flexControllerIndex" );
    LocalFlexController_t iFlexController = ( LocalFlexController_t )flexControllerIndex;

    lua_pushstring( L, pAnimating->GetFlexControllerName( iFlexController ) );

    return 1;
}
LUA_BINDING_END( "string", "The flex controller name" )

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

LUA_BINDING_BEGIN( CBaseAnimating, __eq, "class", "Metamethod to check if the two entities are the same" )
{
    lua_CBaseAnimating *pAnimating1 = LUA_BINDING_ARGUMENT( lua_toanimating, 1, "entity1" );
    lua_CBaseAnimating *pAnimating2 = LUA_BINDING_ARGUMENT( lua_toanimating, 2, "entity2" );

    lua_pushboolean( L, pAnimating1 == pAnimating2 );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the entities are the same, false otherwise" )

LUA_BINDING_BEGIN( CBaseAnimating, __tostring, "class", "Metamethod to get the string representation of the entity" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( lua_toanimating, 1, "entity" );

    if ( pAnimating == NULL )
    {
        lua_pushstring( L, "NULL" );
    }
    else
    {
        lua_pushfstring( L, "CBaseAnimating: %d \"%s\"", pAnimating->entindex(), pAnimating->GetClassname() );
    }

    return 1;
}
LUA_BINDING_END( "string", "The string representation of the entity" )

/*
** Open CBaseAnimating object
*/
LUALIB_API int luaopen_CBaseAnimating_shared( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_BASEANIMATINGLIBNAME );

    LUA_REGISTRATION_COMMIT();

    return 1;
}
