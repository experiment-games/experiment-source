#ifndef LGAMEEVENTS_H
#define LGAMEEVENTS_H
#ifdef _WIN32
#pragma once
#endif

void InitializeLuaGameEventHandler( lua_State *L );
void ShutdownLuaGameEventHandler( lua_State *L );

#endif  // LGAMEEVENTS_H
