//========== Copyleft © 2011, Team Sandbox, Some rights reserved. ===========//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "filesystem.h"
#include "luamanager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CUtlVector< CUtlString > g_MountedAddons;

void MountAddon( const char *addonName )
{
    char relativeAddonPath[MAX_PATH];
    Q_snprintf( relativeAddonPath, sizeof( relativeAddonPath ), LUA_PATH_ADDONS "\\%s", addonName );

    char currentDirectoryPath[MAX_PATH];
    bool bGetCurrentDirectory = V_GetCurrentDirectory( currentDirectoryPath, sizeof( currentDirectoryPath ) );

    if ( bGetCurrentDirectory )
    {
#ifdef CLIENT_DLL
        const char *gamePath = engine->GetGameDirectory();
#else
        char gamePath[MAX_PATH];
        engine->GetGameDir( gamePath, sizeof( gamePath ) );
#endif

        V_SetCurrentDirectory( gamePath );
    }

    // Ensure the engine can find content in the addon
    filesystem->AddSearchPath( relativeAddonPath, CONTENT_SEARCH_PATH, PATH_ADD_TO_TAIL );

    if ( bGetCurrentDirectory )
        V_SetCurrentDirectory( currentDirectoryPath );

#ifdef GAME_DLL // Prevent the message from printing twice in clients
    ConColorMsg( Color( 200, 255, 0, 255 ), "Mounting addon \"%s\"...\n", addonName );
#endif

    g_MountedAddons.AddToTail( CUtlString( addonName ) );
}

void MountAddons()
{
    FileFindHandle_t fh;
    char addonName[255];
    char const *fn = g_pFullFileSystem->FindFirstEx( LUA_PATH_ADDONS "\\*", CONTENT_SEARCH_PATH, &fh );

    g_MountedAddons.Purge();

    while ( fn )
    {
        Q_strcpy( addonName, fn );
        if ( fn[0] != '.' )
        {
            if ( g_pFullFileSystem->FindIsDirectory( fh ) )
            {
                MountAddon( addonName );
            }
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }

    g_pFullFileSystem->FindClose( fh );
}

int GetMountedAddons( CUtlVector< CUtlString > &addons )
{
    int nAddonCount = g_MountedAddons.Count();

    addons.CopyArray( g_MountedAddons.Base(), nAddonCount );

    return nAddonCount;
}

/// <summary>
/// Sets up the package.path and package.cpath to include the lua
/// directories and gamemode directories of mounted addons.
///
/// This makes 'Include' and 'require' work with addon content.
/// </summary>
/// <param name="L"></param>
void SetupMountedAddons( lua_State *L )
{
    int nAddonCount = g_MountedAddons.Count();

#ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
#else
    char gamePath[MAX_PATH];
    engine->GetGameDir( gamePath, MAX_PATH );
#endif

    for ( int i = 0; i < nAddonCount; i++ )
    {
        const char *addonName = g_MountedAddons[i].Get();

        char fullAddonPath[MAX_PATH];
        Q_snprintf( fullAddonPath, sizeof( fullAddonPath ), "%s\\" LUA_PATH_ADDONS "\\%s", gamePath, addonName );

        // <full game dir>/addons/<addon name>/scripts/lua/?.lua
        char addonLuaRootPath[MAX_PATH];
        Q_snprintf( addonLuaRootPath, sizeof( addonLuaRootPath ), "%s\\%s\\?.lua", fullAddonPath, LUA_ROOT );

        luasrc_add_to_package_path( L, addonLuaRootPath );

        // <full game dir>/addons/<addon name>/scripts/lua/modules/?.lua
        char addonLuaModulesPath[MAX_PATH];
        Q_snprintf( addonLuaModulesPath, sizeof( addonLuaModulesPath ), "%s\\%s\\?.lua", fullAddonPath, LUA_PATH_MODULES );

        luasrc_add_to_package_path( L, addonLuaModulesPath );

        // <full game dir>/addons/<addon name>/scripts/lua/bin/?.[dll|so] (cpath)
        char addonLuaBinaryModulesPath[MAX_PATH];

        Q_snprintf(
            addonLuaBinaryModulesPath,
            sizeof( addonLuaBinaryModulesPath ),
#ifdef _WIN32
            "%s\\%s\\?.dll",
#elif _LINUX
            "%s\\%s\\?.so",
#endif
            fullAddonPath,
            LUA_PATH_BINARY_MODULES );

        luasrc_add_to_package_path( L, addonLuaBinaryModulesPath, /* isPathC = */ true );

        // <full game dir>/addons/<addon name>/gamemodes/?.lua
        char addonLuaGamemodesPath[MAX_PATH];
        Q_snprintf( addonLuaGamemodesPath, sizeof( addonLuaGamemodesPath ), "%s%s\\?.lua", fullAddonPath, LUA_PATH_GAMEMODES );

        luasrc_add_to_package_path( L, addonLuaGamemodesPath );
    }
}
