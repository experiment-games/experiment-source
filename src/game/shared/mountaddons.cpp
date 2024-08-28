//========== Copyleft © 2011, Team Sandbox, Some rights reserved. ===========//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "filesystem.h"
#include "luamanager.h"
#include <mountsteamcontent.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CUtlMap< CUtlString, CUtlString > g_MountedAddons;

void MountAddon( const char *addonDirectoryName, const char *addonPath )
{
    char addonFullPath[MAX_PATH];
    Q_snprintf( addonFullPath, sizeof( addonFullPath ), "%s\\%s", addonPath, addonDirectoryName );
    CUtlString addonFullPathString( addonFullPath );

    // Check if the addon is already mounted by their name
    FOR_EACH_MAP( g_MountedAddons, i )
    {
        const char *mountedAddonName = g_MountedAddons.Key(i).Get();

        if ( Q_strcmp( mountedAddonName, addonDirectoryName ) == 0 )
        {
            return;
        }
    }

    // Ensure the engine can find content in the addon
    filesystem->AddSearchPath( addonFullPath, CONTENT_SEARCH_PATH, PATH_ADD_TO_TAIL );
    filesystem->AddSearchPath( addonFullPath, "thirdparty", PATH_ADD_TO_TAIL );

#ifdef GAME_DLL // Prevent the message from printing twice in clients
    ConColorMsg( Color( 200, 255, 0, 255 ), "Mounting addon \"%s\"...\n", addonDirectoryName );
#endif

    g_MountedAddons.Insert( addonDirectoryName, addonFullPathString );
}

void MountAddons()
{
    g_MountedAddons.SetLessFunc( DefLessFunc( CUtlString ) );

    // We want the absolute paths so we can use them to set them as mount roots
    // and find their gamemodes, lua paths, etc get added to the search paths
    // We'll split the search at each ; and loop through each path, adding LUA_PATH_ADDONS "\\*"
    // and finding the addons in each path
    char mountPaths[MAX_PATH * 50]; // TODO: Make this dynamic
    filesystem->GetSearchPath( "GAME", false, mountPaths, sizeof( mountPaths ) );

    char *pPath = mountPaths;
    char *pEnd = pPath + strlen( pPath );

    while ( pPath < pEnd )
    {
        char *pSemiColon = strchr( pPath, ';' );

        if ( pSemiColon )
        {
            *pSemiColon = '\0';
        }

        char addonPath[MAX_PATH];
        Q_snprintf( addonPath, sizeof( addonPath ), "%s" LUA_PATH_ADDONS, pPath );

        char addonSearchPath[MAX_PATH];
        Q_snprintf( addonSearchPath, sizeof( addonSearchPath ), "%s\\*", addonPath );

        FileFindHandle_t fh;
        char addonName[255];
        const char *fn = filesystem->FindFirstEx( addonSearchPath, CONTENT_SEARCH_PATH, &fh );

        while ( fn )
        {
            Q_strcpy( addonName, fn );
            if ( fn[0] != '.' )
            {
                if ( filesystem->FindIsDirectory( fh ) )
                {
                    MountAddon( addonName, addonPath );
                }
            }

            fn = filesystem->FindNext( fh );
        }

        filesystem->FindClose( fh );

        if ( pSemiColon )
        {
            pPath = pSemiColon + 1;
        }
        else
        {
            break;
        }
    }
}

int GetMountedAddons( CUtlVector< CUtlString > &addons )
{
    FOR_EACH_MAP( g_MountedAddons, i )
    {
        addons.AddToTail( g_MountedAddons[i] );
    }

    return g_MountedAddons.Count();
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
    FOR_EACH_MAP( g_MountedAddons, i )
    {
        const char *addonFullPath = g_MountedAddons[i].Get();
        SetupRootSearchPaths( addonFullPath, L );
    }
}
