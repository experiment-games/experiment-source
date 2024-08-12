#include "cbase.h"
#include "lua.hpp"
#include "lresources.h"
#include "luasrclib.h"
#include "utlvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

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

LUA_REGISTRATION_INIT( Resources );

LUA_BINDING_BEGIN( Resources, AddFile, "library", "Add a file to the list of resources so it's downloaded when players connect to the server." )
{
    const char *pFile = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "filePath" );

    // We duplicate the string, because the memory will get overwritten
    g_pLuaResources.AddToTail( strdup( pFile ) );

    lua_pop( L, 1 );  // Pop the file name off the stack

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Resources, GetFiles, "library", "Get the list of files added with Resources.AddFile." )
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

/*
** Open resources library
*/
LUALIB_API int luaopen_resources( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Resources, LUA_RESOURCESLIBNAME );
    return 1;
}
