#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "mathlib/lvector.h"
#ifdef CLIENT_DLL
#include "lc_baseanimating.h"
#else
#include "lbaseanimating.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int CBaseAnimating_GetModelName( lua_State *L )
{
    // lua_pushstring( L, STRING( luaL_checkentity( L, 1 )->GetModelName() ) ); // Must be explicitly set?
    lua_pushstring( L, luaL_checkanimating( L, 1 )->GetModelPtr()->pszName() );

    return 1;
}

static int CBaseAnimating_GetAttachment( lua_State *L )
{
    CBaseAnimating *pAnimating = luaL_checkanimating( L, 1 );
    int iArg2Type = lua_type( L, 2 );
    Vector pVecOrigin;
    QAngle pVecAngles;
    int boneID = -1;

    if ( iArg2Type == LUA_TNUMBER )
    {
        boneID = luaL_checknumber( L, 2 );

        if ( !pAnimating->GetAttachment( boneID, pVecOrigin, pVecAngles ) )
        {
            lua_pushnil( L );
            return 1;
        }
    }
    else if ( iArg2Type == LUA_TSTRING )
    {
        boneID = pAnimating->LookupAttachment( luaL_checkstring( L, 2 ) );

        if ( !pAnimating->GetAttachment( boneID, pVecOrigin, pVecAngles ) )
        {
            lua_pushnil( L );
            return 1;
        }
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

static int CBaseEntity_SetSkin( lua_State *L )
{
    // TODO: SetSkin
    // TODO: There's also logic in lbaseanimating.__newindex for this
    // TODO: Placed here because its easily shared, but we should really create a common place for baseanimating shared functions
    lua_toanimating( L, 1 )->m_nSkin = luaL_checknumber( L, 2 );
    return 0;
}

static const luaL_Reg CBaseAnimatingmeta[] = {
    { "GetModelName", CBaseAnimating_GetModelName },
    { "GetAttachment", CBaseAnimating_GetAttachment },

    // Wonky naming conventions, lets just support both out-of-the-box
    { "FindBodygroupByName", CBaseAnimating_FindBodygroupByName },
    { "FindBodyGroupByName", CBaseAnimating_FindBodygroupByName },
    { "GetBodygroup", CBaseAnimating_GetBodygroup },
    { "GetBodyGroup", CBaseAnimating_GetBodygroup },
    { "GetBodygroupCount", CBaseAnimating_GetBodygroupCount },
    { "GetBodyGroupCount", CBaseAnimating_GetBodygroupCount },
    { "GetBodygroupName", CBaseAnimating_GetBodygroupName },
    { "GetBodyGroupName", CBaseAnimating_GetBodygroupName },

    { "GetBodygroup", CBaseAnimating_GetBodyGroups },
    { "GetBodyGroup", CBaseAnimating_GetBodyGroups },
    { "SetBodygroup", CBaseAnimating_SetBodygroup },
    { "SetBodyGroup", CBaseAnimating_SetBodygroup },
    { "SetBodygroups", CBaseAnimating_SetBodyGroups },
    { "SetBodyGroups", CBaseAnimating_SetBodyGroups },

    { "GetNumBodygroups", CBaseAnimating_GetNumBodyGroups },
    { "GetNumBodyGroups", CBaseAnimating_GetNumBodyGroups },

    { "SetPlaybackRate", CBaseAnimating_SetPlaybackRate },
    { "SetSkin", CBaseEntity_SetSkin },

    { NULL, NULL } };

/*
** Open CBaseAnimating object
*/
LUALIB_API int luaopen_CBaseAnimating_shared( lua_State *L )
{
    // Extends the CBaseEntity metatable with every method part of CBaseAnimating
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEENTITYLIBNAME );
    luaL_register( L, NULL, CBaseAnimatingmeta );
    return 1;
}
