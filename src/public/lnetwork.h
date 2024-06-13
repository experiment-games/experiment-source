//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef LNETWORK_H
#define LNETWORK_H
#ifdef _WIN32
#pragma once
#endif

void RegisterLuaUserMessages();

#ifdef CLIENT_DLL
LUA_API void lua_pushbf_read( lua_State *L, bf_read *message );

LUALIB_API bf_read *luaL_checkbf_read( lua_State *L, int narg );
#endif

#endif  // LNETWORK_H
