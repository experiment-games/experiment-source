#ifndef ENGINE_PATCHES_H
#define ENGINE_PATCHES_H

#ifdef _WIN32
#pragma once
#endif

#include "MinHook.h"
#undef CreateEvent  // WinBase.h defines this, but it messes with out gameeventmanager->CreateEvent

DWORD_PTR GetModuleBaseAddress( const char *moduleName );
void ApplyDetoursOnDllStartup();
void NotifyDetoursOnLevelShutdown();

/*
** Specific detours
*/

#ifdef GAME_DLL // Server detours

void HandleCheckPasswordDetour( void *serverObject, const char *steamIdAsString );
void UpdateCheckPasswordDetourOnLevelShutdown();

void ApplyClientConnectDetour();

#endif

#ifdef CLIENT_DLL // Client detours

// ...

#endif

#endif // ENGINE_PATCHES_H
