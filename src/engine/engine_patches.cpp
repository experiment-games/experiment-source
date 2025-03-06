#include "engine_patches.h"

// Experiment;
// These detours are experimental and give us access to engine features we
// wouldn't have access to otherwise. It's a shame that we have to be this
// hacky, but it's the only way to get access to things like CheckPassword and
// ClientConnect.

DWORD_PTR GetModuleBaseAddress( HMODULE module )
{
    return reinterpret_cast< DWORD_PTR >( module );
}

void ApplyDetoursOnDllStartup()
{
    if ( MH_Initialize() != MH_OK )
    {
        //DevWarning( "Failed to initialize MinHook.\n" );
        return;
    }

#if defined( CLIENT_DLL)

#elif defined( GAME_DLL )
    // HandleCheckPasswordDetour();
    ApplyClientConnectDetour();
#endif
}

void NotifyDetoursOnLevelShutdown()
{
#if defined( CLIENT_DLL )

#elif defined( GAME_DLL )
    // UpdateCheckPasswordDetourOnLevelShutdown();
#endif
}
