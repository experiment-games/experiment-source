#include "cbase.h"
#include "lua/lua.hpp"
#include "lresources.h"
#include "luasrclib.h"
#include "utlvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CUtlVector< const char * > g_pLuaResources;

// Get amount of registered resources registerd with resources.AddFile
int resources_GetFilesCount()
{
    return g_pLuaResources.Count();
}

// Get iterator for resources registered with resources.AddFile
const char *resources_GetFile( int i )
{
    return g_pLuaResources[i];
}

// Add a file to the list of resources
static int resources_AddFile( lua_State *L )
{
    const char *pFile = luaL_checkstring( L, 1 );

    // We duplicate the string, because the memory will get overwritten
    g_pLuaResources.AddToTail( strdup(pFile) );

    lua_pop( L, 1 ); // Pop the file name off the stack

    return 0;
}

void resources_Free()
{
    for ( int i = 0; i < g_pLuaResources.Count(); ++i )
    {
        free( ( void * )g_pLuaResources[i] );
    }
    g_pLuaResources.RemoveAll();
}

static const luaL_Reg resourcesLib[] = {
    { "AddFile", resources_AddFile },
    { NULL, NULL } };

/*
** Open resources library
*/
LUALIB_API int luaopen_resources( lua_State *L )
{
    luaL_register( L, LUA_RESOURCESLIBNAME, resourcesLib );
    return 1;
}
