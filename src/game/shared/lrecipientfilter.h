#ifndef LRECIPIENTFILTER_H
#define LRECIPIENTFILTER_H
#ifdef _WIN32
#pragma once
#endif

/* type for CRecipientFilter functions */
#ifdef CLIENT_DLL
typedef C_RecipientFilter lua_CRecipientFilter;
#else
typedef CRecipientFilter lua_CRecipientFilter;
#endif

/*
** access functions (stack -> C)
*/

LUA_API lua_CRecipientFilter &( lua_torecipientfilter )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushrecipientfilter )( lua_State *L, lua_CRecipientFilter &filter );

LUALIB_API lua_CRecipientFilter &( luaL_checkrecipientfilter )( lua_State *L, int narg );
LUALIB_API lua_CRecipientFilter &( luaL_optrecipientfilter )( lua_State *L, int narg, lua_CRecipientFilter &def );
LUA_API bool( lua_isrecipientfilter )( lua_State *L, int idx );

#endif  // LRECIPIENTFILTER_H
