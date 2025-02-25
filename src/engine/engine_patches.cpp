#include "cbase.h"
#include "engine_patches.h"

// Experiment;
// These detours are experimental and give us access to engine features we
// wouldn't have access to otherwise. It's a shame that we have to be this
// hacky, but it's the only way to get access to things like CheckPassword and
// ClientConnect.

DWORD_PTR GetModuleBaseAddress( const char *moduleName )
{
    return reinterpret_cast< DWORD_PTR >( GetModuleHandle( moduleName ) );
}

void ApplyDetoursOnDllStartup()
{
    if ( MH_Initialize() != MH_OK )
    {
        DevWarning( "Failed to initialize MinHook.\n" );
        return;
    }

#ifdef CLIENT_DLL

#else
    // HandleCheckPasswordDetour();
    ApplyClientConnectDetour();
#endif
}

void NotifyDetoursOnLevelShutdown()
{
#ifdef CLIENT_DLL

#else
    UpdateCheckPasswordDetourOnLevelShutdown();
#endif
}
