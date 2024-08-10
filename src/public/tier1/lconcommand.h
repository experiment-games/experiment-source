#ifndef LCONCOMMAND_H
#define LCONCOMMAND_H

#if _WIN32
#pragma once
#endif

/* type for ConCommand functions */
typedef ConCommand lua_ConCommand;

/*
** access functions (stack -> C)
*/

LUA_API lua_ConCommand     *(lua_toconcommand) (lua_State *L, int idx);

/*
** push functions (C -> stack)
*/
LUA_API void  (lua_pushconcommand) (lua_State *L, lua_ConCommand *pConCommand);

LUALIB_API lua_ConCommand *(luaL_checkconcommand) (lua_State *L, int narg);

#ifdef CLIENT_DLL
void ResetGameUIConCommandDatabase( void );
#endif
void ResetConCommandDatabase( void );

bool TryRunConsoleCommand( const char *pszCommandString );

#endif // LCONCOMMAND_H
