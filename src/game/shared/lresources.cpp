#include "cbase.h"
#include "lua.hpp"
#include "lresources.h"
#include "luasrclib.h"
#include "utlvector.h"
#include "filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Resources );

#ifndef CLIENT_DLL

static CUtlVector< const char * > g_pLuaResources;

/// <summary>
/// Get amount of registered resources registerd with Resources.AddFile
/// </summary>
/// <returns></returns>
int ResourcesGetFilesCount()
{
    return g_pLuaResources.Count();
}

/// <summary>
/// Get iterator for resources registered with Resources.AddFile
/// </summary>
/// <param name="i"></param>
/// <returns></returns>
const char *ResourcesGetFile( int i )
{
    return g_pLuaResources[i];
}

/// <summary>
/// Free all resources registered with Resources.AddFile
/// </summary>
void ResourcesFreeFiles()
{
    for ( int i = 0; i < g_pLuaResources.Count(); ++i )
    {
        free( ( void * )g_pLuaResources[i] );
    }
    g_pLuaResources.RemoveAll();
}

/// <summary>
/// Strips the extension from the file path, attaching the given extension.
/// It will check if the files exists and warn if not.
/// </summary>
/// <param name="filePath"></param>
/// <param name="relatedExtension"></param>
static void AddRelatedFileOrWarn( const char *filePath, const char *relatedExtension )
{
    char relatedFile[MAX_PATH];
    V_StripExtension( filePath, relatedFile, sizeof( relatedFile ) );
    V_strcat( relatedFile, relatedExtension, sizeof( relatedFile ) );

    if ( !filesystem->FileExists( relatedFile ) )
    {
        DevWarning( "Resources.AddFile couldn't find: %s\n", relatedFile );
    }

    g_pLuaResources.AddToTail( strdup( relatedFile ) );
}

LUA_BINDING_BEGIN( Resources, AddFile, "library", "Add a file to the list of resources so it's downloaded when players connect to the server. For convenience .vmt and .mdl will also add the related .vtf/.png/.jpg and model files (.vvd, .ani, .dx80.vtx, .dx90.vtx, .sw.vtx, .phy) with the same name. It will warn for each missing file.", "server" )
{
    const char *pFile = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "filePath" );
    const char *extension = V_GetFileExtension( pFile );

    if ( Q_stricmp( extension, "vmt" ) == 0 )
    {
        AddRelatedFileOrWarn( pFile, ".vtf" );
        AddRelatedFileOrWarn( pFile, ".png" );
        AddRelatedFileOrWarn( pFile, ".jpg" );
    }
    else if ( Q_stricmp( extension, "mdl" ) == 0 )
    {
        AddRelatedFileOrWarn( pFile, ".vvd" );
        AddRelatedFileOrWarn( pFile, ".ani" );
        AddRelatedFileOrWarn( pFile, ".dx80.vtx" );
        AddRelatedFileOrWarn( pFile, ".dx90.vtx" );
        AddRelatedFileOrWarn( pFile, ".sw.vtx" );
        AddRelatedFileOrWarn( pFile, ".phy" );
    }
    else
    {
        g_pLuaResources.AddToTail( strdup( pFile ) );
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Resources, AddSingleFile, "library", "Add a file to the list of resources so it's downloaded when players connect to the server.", "server" )
{
    const char *pFile = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "filePath" );

    // We duplicate the string, because the memory will get overwritten
    g_pLuaResources.AddToTail( strdup( pFile ) );

    lua_pop( L, 1 );  // Pop the file name off the stack

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Resources, GetFiles, "library", "Get the list of files added with Resources.AddFile.", "server" )
{
    lua_newtable( L );

    for ( int i = 0; i < g_pLuaResources.Count(); ++i )
    {
        lua_pushstring( L, g_pLuaResources[i] );
        lua_rawseti( L, -2, i + 1 );
    }

    return 1;
}
LUA_BINDING_END( "table", "List of files added with Resources.AddFile." )

#endif

/*
** Open resources library
*/
LUALIB_API int luaopen_resources( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Resources );
    return 1;
}
