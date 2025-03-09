#include "cbase.h"
#ifdef CLIENT_DLL
#include "c_recipientfilter.h"
#define CRecipientFilter C_RecipientFilter
#else
#include "recipientfilter.h"
#endif
#include "luamanager.h"
#include "luasrclib.h"
#include "lrecipientfilter.h"
#include "lbaseplayer_shared.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static lua_CRecipientFilter *s_pFilter = new CRecipientFilter();

/*
** access functions (stack -> C)
*/

LUA_API lua_CRecipientFilter &lua_torecipientfilter( lua_State *L, int idx )
{
    lua_CRecipientFilter **ppFilter = ( lua_CRecipientFilter ** )lua_touserdata( L, idx );
    return **ppFilter;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushrecipientfilter( lua_State *L, lua_CRecipientFilter &filter )
{
    lua_CRecipientFilter **ppFilter = ( lua_CRecipientFilter ** )lua_newuserdata( L, sizeof( lua_CRecipientFilter ) );
    s_pFilter->Reset();
    s_pFilter->CopyFrom( filter );
    *ppFilter = s_pFilter;
    LUA_SAFE_SET_METATABLE( L, LUA_RECIPIENTFILTERMETANAME );
}

LUALIB_API lua_CRecipientFilter &luaL_checkrecipientfilter( lua_State *L, int idx )
{
    lua_CRecipientFilter *d = ( lua_CRecipientFilter * )lua_touserdata( L, idx );

    if ( !d )
        luaL_typerror( L, idx, LUA_RECIPIENTFILTERMETANAME );

    return **( lua_CRecipientFilter ** )d;
}

LUALIB_API lua_CRecipientFilter &luaL_optrecipientfilter( lua_State *L, int narg, lua_CRecipientFilter &def )
{
    if ( lua_isnoneornil( L, narg ) )
        return def;
    return luaL_checkrecipientfilter( L, narg );
}

LUALIB_API bool lua_isrecipientfilter( lua_State *L, int idx )
{
    void *p = luaL_testudata( L, idx, LUA_RECIPIENTFILTERMETANAME );
    return p != NULL;
}

LUA_REGISTRATION_INIT( RecipientFilter );

LUA_BINDING_BEGIN( RecipientFilter, AddAllPlayers, "class", "Adds all players to the filter" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).AddAllPlayers();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, AddRecipient, "class", "Adds a single recipient" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).AddRecipient( LUA_BINDING_ARGUMENT( luaL_checkplayer, 2, "player" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, AddRecipientsByPas, "class", "Adds recipients by potentially audible set" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).AddRecipientsByPAS( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, AddRecipientsByPVS, "class", "Adds recipients by potentially visible set" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).AddRecipientsByPVS( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, CopyFrom, "class", "Copies from another filter" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "destFilter" ).CopyFrom( LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 2, "srcFilter" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, GetRecipientCount, "class", "Gets the count of recipients" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).GetRecipientCount() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, GetRecipientIndex, "class", "Gets recipient index" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).GetRecipientIndex( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" ) ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, IgnorePredictionCull, "class", "Checks if prediction culling is ignored" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).IgnorePredictionCull() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, IsInitMessage, "class", "Checks if it is an init message" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).IsInitMessage() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, IsReliable, "class", "Checks if the filter is reliable" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).IsReliable() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, IsUsingPredictionRules, "class", "Checks if using prediction rules" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).IsUsingPredictionRules() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, MakeReliable, "class", "Makes the filter reliable" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).MakeReliable();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, RemoveRecipient, "class", "Removes a recipient" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).RemoveRecipient( LUA_BINDING_ARGUMENT( luaL_checkplayer, 2, "player" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, Reset, "class", "Resets the filter" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).Reset();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, SetIgnorePredictionCull, "class", "Sets whether to ignore prediction cull" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).SetIgnorePredictionCull( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "ignoreCull" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, UsePredictionRules, "class", "Applies prediction rules" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).UsePredictionRules();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, __tostring, "class", "Returns a string representation of the filter" )
{
    lua_pushfstring( L, "RecipientFilter: %p", LUA_BINDING_ARGUMENT( lua_torecipientfilter, 1, "filter" ) );
    return 1;
}
LUA_BINDING_END()

#ifndef CLIENT_DLL
LUA_BINDING_BEGIN( RecipientFilter, MakeInitMessage, "class", "Makes the filter an init message", "client" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).MakeInitMessage();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, RemoveAllRecipients, "class", "Removes all recipients", "client" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).RemoveAllRecipients();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, RemoveRecipientByPlayerIndex, "class", "Removes a recipient by player index", "client" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).RemoveRecipientByPlayerIndex( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, RemoveRecipientsByPvs, "class", "Removes recipients by potentially visible set", "client" )
{
    LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" ).RemoveRecipientsByPVS( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "position" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( RecipientFilter, GetRecipients, "class", "Gets all recipients in the filter" )
{
    // returns a table of all recipeitns
    lua_CRecipientFilter &filter = LUA_BINDING_ARGUMENT( luaL_checkrecipientfilter, 1, "filter" );

    lua_newtable( L );

    for ( int i = 0; i < filter.m_Recipients.Count(); i++ )
    {
        int entIndex = filter.m_Recipients[i];
        CBasePlayer *pPlayer = UTIL_PlayerByIndex( entIndex );

        lua_pushinteger( L, i + 1 );  // Lua tables are 1-based
        CBasePlayer::PushLuaInstanceSafe( L, pPlayer );
        lua_settable( L, -3 );  // Set table[i+1] = pPlayer
    }

    return 1;
}
LUA_BINDING_END( "table", "Table of all recipient players" )
#endif

static int luasrc_CRecipientFilter( lua_State *L )
{
    CRecipientFilter filter;
    lua_pushrecipientfilter( L, filter );
    return 1;
}

LUA_REGISTRATION_INIT( RecipientFilters )

LUA_BINDING_BEGIN( RecipientFilters, Create, "library", "Creates a new recipient filter" )
{
    bool isUnreliable = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 1, false, "isUnreliable" );

    CRecipientFilter filter;

    if ( !isUnreliable )
        filter.MakeReliable();

    lua_pushrecipientfilter( L, filter );

    return 1;
}
LUA_BINDING_END( "RecipientFilter", "The new recipient filter" )

/*
** Open CRecipientFilter object
*/
LUALIB_API int luaopen_CRecipientFilter( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_RECIPIENTFILTERMETANAME );

    LUA_REGISTRATION_COMMIT( RecipientFilter );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_RECIPIENTFILTERMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "RecipientFilter" */

    LUA_REGISTRATION_COMMIT_LIBRARY( RecipientFilters );

    lua_pop( L, 1 );
    return 1;
}
