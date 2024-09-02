#ifndef LNETWORK_H
#define LNETWORK_H
#ifdef _WIN32
#pragma once
#endif

void RegisterLuaUserMessages();

// bf_read
LUA_API bf_read &( lua_tobf_read )( lua_State *L, int idx );
LUA_API void lua_pushbf_read( lua_State *L, bf_read *message );
LUALIB_API bf_read *luaL_checkbf_read( lua_State *L, int narg );

// bf_write
LUA_API bf_write &( lua_tobf_write )( lua_State *L, int idx );
LUA_API void lua_pushbf_write( lua_State *L, bf_write *message );
LUALIB_API bf_write *luaL_checkbf_write( lua_State *L, int narg );

#endif  // LNETWORK_H
