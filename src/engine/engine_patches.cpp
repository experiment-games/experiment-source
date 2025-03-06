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

DWORD_PTR FindPattern( HMODULE module, const char *pattern, const char *mask )
{
    if ( !module || !pattern || !mask )
        return 0;

    PIMAGE_DOS_HEADER dos = ( PIMAGE_DOS_HEADER )module;
    PIMAGE_NT_HEADERS nt = ( PIMAGE_NT_HEADERS )( ( BYTE * )module + dos->e_lfanew );
    DWORD size = nt->OptionalHeader.SizeOfImage;
    BYTE *data = ( BYTE * )module;
    DWORD length = ( DWORD )strlen( mask );

    for ( DWORD i = 0; i <= size - length; i++ )
    {
        bool found = true;
        for ( DWORD j = 0; j < length; j++ )
        {
            if ( mask[j] != '?' && pattern[j] != ( char )data[i + j] )
            {
                found = false;
                break;
            }
        }
        if ( found )
            return ( DWORD_PTR )&data[i];
    }
    return 0;
}

void ApplyDetoursOnDllStartup()
{
    if ( MH_Initialize() != MH_OK )
    {
        // DevWarning( "Failed to initialize MinHook.\n" );
        return;
    }

#if defined( CLIENT_DLL )

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
