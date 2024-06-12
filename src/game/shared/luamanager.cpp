//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Contains the implementation of Lua for scripting.
//
//===========================================================================//

#include "cbase.h"
#include "utlbuffer.h"
#include "filesystem.h"
#ifndef CLIENT_DLL
#include "gameinterface.h"
#endif
#include "steam/isteamfriends.h"
#include "networkstringtabledefs.h"
#ifndef CLIENT_DLL
#include "basescriptedtrigger.h"
#endif
#include "basescripted.h"
#include "weapon_hl2mpbase_scriptedweapon.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "luacachefile.h"
#include "lconvar.h"
#include "licvar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar gamemode( "gamemode", "sandbox", FCVAR_ARCHIVE | FCVAR_REPLICATED );
ConVar lua_log_loader( "lua_log_loader", "1", FCVAR_ARCHIVE );

static char contentSearchPath[MAX_PATH];

static void tag_error( lua_State *L, int narg, int tag )
{
    luaL_typerror( L, narg, lua_typename( L, tag ) );
}

LUALIB_API int luaL_checkboolean( lua_State *L, int narg )
{
    int d = lua_toboolean( L, narg );
    if ( d == 0 &&
         !lua_isboolean( L, narg ) ) /* avoid extra test when d is not 0 */
        tag_error( L, narg, LUA_TBOOLEAN );
    return d;
}

LUALIB_API int luaL_optboolean( lua_State *L, int narg, int def )
{
    return luaL_opt( L, luaL_checkboolean, narg, def );
}

#ifdef CLIENT_DLL
lua_State *LGameUI;
#endif

// This is the state for the CLIENT (in case of CLIENT_DLL) or the
// SERVER (in case of GAME_DLL). It will persist for the entire
// play session.
lua_State *L;

// See luasrc_InitCustomLoader for more information
lua_State *loaderLuaState;
bool hasLoaderBeenActivated = false;

// Lua system initialized for client or server
bool g_bLuaInitialized;

static int luasrc_print( lua_State *L )
{
    int n = lua_gettop( L ); /* number of arguments */
    int i;
    lua_getglobal( L, "tostring" );
    for ( i = 1; i <= n; i++ )
    {
        const char *s;
        lua_pushvalue( L, -1 ); /* function to be called */
        lua_pushvalue( L, i );  /* value to print */
        lua_call( L, 1, 1 );
        s = lua_tostring( L, -1 ); /* get result */
        if ( s == NULL )
            return luaL_error(
                L,
                LUA_QL( "tostring" ) " must return a string to " LUA_QL( "print" ) );
        if ( i > 1 )
            Msg( "\t" );
        Msg( s );
        lua_pop( L, 1 ); /* pop result */
    }
    Msg( "\n" );
    return 0;
}

static int luasrc_type( lua_State *L )
{
    luaL_checkany( L, 1 );
    if ( lua_getmetatable( L, 1 ) )
    {
        lua_pushstring( L, "__type" );
        lua_rawget( L, -2 );
        lua_remove( L, -2 );
        if ( !lua_isstring( L, -1 ) )
            lua_pop( L, 1 );
        else
            return 1;
    }
    lua_pushstring( L, luaL_typename( L, 1 ) );
    return 1;
}

static int luasrc_include( lua_State *L )
{
    lua_Debug ar1;
    lua_getstack( L, 1, &ar1 );
    lua_getinfo( L, "f", &ar1 );
    lua_Debug ar2;
    lua_getinfo( L, ">S", &ar2 );
    int iLength = Q_strlen( ar2.source );
    char source[MAX_PATH];
    Q_StrRight( ar2.source, iLength - 1, source, sizeof( source ) );
    Q_StripFilename( source );
    char fileName[MAX_PATH];
    Q_snprintf( fileName, sizeof( fileName ), "%s\\%s", source, luaL_checkstring( L, 1 ) );

    int returnedValueCount = luasrc_dofile_leave_stack( L, fileName );

    if ( returnedValueCount == -1 )
    {
        // An error occurred executing the script
        return 0;
    }

    return returnedValueCount;
}

// Prints blue text on the server, yellow text on the client
static int luasrc_InternalMsg( lua_State *L, Color color )
{
    int n = lua_gettop( L );  // number of arguments
    int i;

    lua_getglobal( L, "tostring" );

    for ( i = 1; i <= n; i++ )
    {
        const char *s;
        lua_pushvalue( L, -1 );  // function to be called
        lua_pushvalue( L, i );   // value to print
        lua_call( L, 1, 1 );
        s = lua_tostring( L, -1 );  // get result
        if ( s == NULL )
            return luaL_error( L, LUA_QL( "tostring" ) " must return a string to " LUA_QL( "print" ) );
        if ( i > 1 )
            Msg( "\t" );
        Msg( s );
        lua_pop( L, 1 );  // pop result
    }

    Msg( "\n" );

    return 0;
}
static int luasrc_Msg( lua_State *L )
{
#ifdef CLIENT_DLL
    luasrc_InternalMsg( L, Color( 255, 255, 0, 255 ) );
#else
    luasrc_InternalMsg( L, Color( 0, 0, 255, 255 ) );
#endif

    return 0;
}

static int luasrc_MsgN( lua_State *L )
{
#ifdef CLIENT_DLL
    luasrc_InternalMsg( L, Color( 255, 255, 0, 255 ) );
#else
    luasrc_InternalMsg( L, Color( 0, 0, 255, 255 ) );
#endif

    Msg( "\n" );

    return 0;
}

static const luaL_Reg base_funcs[] = {
    { "print", luasrc_print },
    { "Msg", luasrc_Msg },
    { "MsgN", luasrc_MsgN },
    { "type", luasrc_type },
    { "include", luasrc_include },
    { NULL, NULL } };

static void base_open( lua_State *L )
{
    /* set global _R */
    lua_pushvalue( L, LUA_REGISTRYINDEX );
    lua_setglobal( L, LUA_RNAME );
    /* open lib into global table */
    luaL_register( L, LUA_GNAME, base_funcs );
    lua_pop( L, 1 );
    /* set global enum table */
    lua_newtable( L );
    lua_setglobal( L, LUA_ENAME );
#ifdef CLIENT_DLL
    lua_pushboolean( L, 1 );
    lua_setglobal( L, "CLIENT" );
#else
    lua_pushboolean( L, 1 );
    lua_setglobal( L, "SERVER" );
#endif
}

void luasrc_setmodulepaths( lua_State *L )
{
    lua_getglobal( L, LUA_LOADLIBNAME );
#ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
#else
    char gamePath[256];
    engine->GetGameDir( gamePath, 256 );
#endif

    // Andrew; set package.cpath.
    lua_getfield( L, -1, "cpath" );
    // MAX_PATH + package.cpath:len();
    char lookupCPath[MAX_PATH + 99];
    Q_snprintf( lookupCPath, sizeof( lookupCPath ), "%s\\%s;%s", gamePath,
#ifdef _WIN32
                LUA_PATH_MODULES "\\?.dll",
#elif _LINUX
                LUA_PATH_MODULES "\\?.so",
#endif
                luaL_checkstring( L, -1 ) );
    Q_strlower( lookupCPath );
    Q_FixSlashes( lookupCPath );
    lua_pop( L, 1 ); /* pop result */
    lua_pushstring( L, lookupCPath );
    lua_setfield( L, -2, "cpath" );

    // Andrew; set package.path.
    lua_getfield( L, -1, "path" );
    // MAX_PATH + package.path:len();
    char lookupPath[MAX_PATH + 197];
    Q_snprintf( lookupPath, sizeof( lookupPath ), "%s\\%s;%s", gamePath, LUA_PATH_MODULES "\\?.lua", luaL_checkstring( L, -1 ) );
    Q_strlower( lookupPath );
    Q_FixSlashes( lookupPath );
    lua_pop( L, 1 ); /* pop result */
    lua_pushstring( L, lookupPath );
    lua_setfield( L, -2, "path" );

    lua_pop( L, 1 ); /* pop result */
}

static int printTableValues( lua_State *L )
{
    // Get the table argument from the stack (assuming it's at index 1)
    lua_gettop( L );  // Get the number of arguments on the stack
    if ( lua_type( L, 1 ) != LUA_TTABLE )
    {
        luaL_error( L, "Expected table argument" );
        return 0;
    }

    // Iterate through key-value pairs using `lua_next`
    lua_pushnil( L );  // Push a nil key to start iteration
    while ( lua_next( L, 1 ) != 0 )
    {
        // Get the key (can be a string or a number)
        int keyType = lua_type( L, -2 );
        if ( keyType == LUA_TSTRING )
        {
            const char *key = lua_tostring( L, -2 );
            Msg( "Key: %s\n", key );
        }
        else if ( keyType == LUA_TNUMBER )
        {
            lua_Number numKey = lua_tonumber( L, -2 );
            Msg( "Key (number): %f\n", numKey );
        }
        else
        {
            Msg( "Unsupported key type\n" );
        }

        // Get the value and print it based on its type
        int valueType = lua_type( L, -1 );
        switch ( valueType )
        {
            case LUA_TNIL:
                Msg( "Value: nil\n" );
                break;
            case LUA_TBOOLEAN:
                Msg( "Value: %s\n", lua_toboolean( L, -1 ) ? "true" : "false" );
                break;
            case LUA_TNUMBER:
                Msg( "Value: %f\n", lua_tonumber( L, -1 ) );
                break;
            case LUA_TSTRING:
                Msg( "Value: %s\n", lua_tostring( L, -1 ) );
                break;
            case LUA_TTABLE:
                Msg( "Value: (nested table)\n" );
                // You could recursively call this function here to print nested
                // values but be cautious of potential infinite recursion
                break;
            default:
                Msg( "Value: (unknown type)\n" );
        }

        // Pop the value and the key from the stack
        lua_pop( L, 2 );
    }

    return 0;
}

#ifdef CLIENT_DLL
void luasrc_init_gameui( void )
{
    LGameUI = luaL_newstate();

    luaL_openlibs( LGameUI );
    base_open( LGameUI );

    lua_pushboolean( LGameUI, 1 );
    lua_setglobal( LGameUI, "_GAMEUI" ); /* set global _GAMEUI */

    luasrc_setmodulepaths( LGameUI );

    int leftOnStack = 0;
    leftOnStack += luaopen_ConCommand( LGameUI );
    leftOnStack += luaopen_dbg( LGameUI );
    leftOnStack += luaopen_engine( LGameUI );
    leftOnStack += luaopen_enginevgui( LGameUI );
    leftOnStack += luaopen_FCVAR( LGameUI );
    leftOnStack += luaopen_KeyValues( LGameUI );
    leftOnStack += luaopen_Panel( LGameUI );
    leftOnStack += luaopen_surface( LGameUI );
    leftOnStack += luaopen_vgui( LGameUI );
    lua_pop( LGameUI, leftOnStack );

    Msg( "Lua Menu initialized (" LUA_VERSION ")\n" );
}

void luasrc_shutdown_gameui( void )
{
    ResetGameUIConCommandDatabase();

    lua_close( LGameUI );
}
#endif

void luasrc_init( void )
{
    if ( g_bLuaInitialized )
        return;
    g_bLuaInitialized = true;

    L = lua_open();

    luaL_openlibs( L );
    base_open( L );
    lcf_open( L );

    // Andrew; Someone set us up the path for great justice
    luasrc_setmodulepaths( L );

    luasrc_openlibs( L );

    Msg( "Lua initialized (" LUA_VERSION ")\n" );
}

void luasrc_shutdown( void )
{
    if ( !g_bLuaInitialized )
        return;

    g_bLuaInitialized = false;

    filesystem->RemoveSearchPath( contentSearchPath, "MOD" );

    ResetConCommandDatabase();

    RemoveGlobalChangeCallbacks();
    ResetConVarDatabase();

#ifndef CLIENT_DLL
    ResetTriggerFactoryDatabase();
#endif
    ResetEntityFactoryDatabase();
    ResetWeaponFactoryDatabase();

    lcf_close( L );
    lua_close( L );
}

LUA_API int luasrc_dostring( lua_State *L, const char *string )
{
    int iError = luaL_dostring( L, string );

    if ( iError != 0 )
    {
        Warning( "%s\n", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
    }

    return iError;
}

/// <summary>
/// Provides a useful stack traceback for errors that occur in the loader.
/// It is called before the stack is unwound by the error.
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
static int luasrc_loader_traceback( lua_State *L )
{
    const char *msg = lua_tostring( L, 1 );
    if ( msg == NULL )
    {                                               /* is error object not a string? */
        if ( luaL_callmeta( L, 1, "__tostring" ) && /* does it have a metamethod */
             lua_type( L, -1 ) == LUA_TSTRING )     /* that produces a string? */
            return 1;                               /* that is the message */
        else
            msg = lua_pushfstring( L, "(error object is a %s value)", luaL_typename( L, 1 ) );
    }
    luaL_traceback( L, L, msg, 1 ); /* append a standard traceback */
    return 1;                       /* return the traceback */
}

/// <summary>
/// If no custom loader has been activated, this function will simply
/// load the given file using luaL_dofile. If a custom loader has been
/// activated, the file will be preprocessed by the loader before being
/// loaded.
/// </summary>
/// <param name="L"></param>
/// <param name="fileName"></param>
/// <returns></returns>
LUA_API int luasrc_dofile_with_loader( lua_State *L, const char *filePath )
{
    // If the loader is not activated, default to simple dofile loading
    if ( !hasLoaderBeenActivated )
    {
        return luaL_dofile( L, filePath );
    }

    // Handle the file manually, preprocessing it with the loader
    char fullPath[MAX_PATH];
    filesystem->RelativePathToFullPath( filePath, "MOD", fullPath, sizeof( fullPath ) );

    if ( !filesystem->FileExists( fullPath, "MOD" ) )
    {
        lua_pushfstring( L, "File does not exist: %s", fullPath );
        return LUA_ERRFILE;
    }

    // Read the file contents
    CUtlBuffer fileContentsBuffer( 0, 0, CUtlBuffer::TEXT_BUFFER );
    if ( !filesystem->ReadFile( fullPath, "MOD", fileContentsBuffer ) )
    {
        lua_pushfstring( L, "Failed to read file: %s", fullPath );
        return LUA_ERRFILE;
    }

    const char *fileContents = fileContentsBuffer.String();
    size_t fileSize = fileContentsBuffer.TellPut();

    // Let the PreProcessFile function in the loader Lua state
    // process the file contents
    lua_getfield( loaderLuaState, -1, "PreProcessFile" );
    lua_pushlstring( loaderLuaState, fileContents, fileContentsBuffer.TellPut() );
    lua_pushstring( loaderLuaState, fullPath );
    int result = lua_pcall( loaderLuaState, 2, 1, 0 );

    // From here on we prefix @ to the fileName, just like luaL_dofile does
    char bufferFileName[MAX_PATH];
    Q_snprintf( bufferFileName, MAX_PATH, "@%s", fullPath );

    if ( result != LUA_OK )
    {
        const char *error = lua_tostring( loaderLuaState, -1 );
        lua_pop( loaderLuaState, 1 );  // Pop the error message

        Warning( "[Lua] Loader Error: Failed to preprocess file %s because of error: '%s' (returning file unmodified!)\n", fullPath, error );

        return luaL_loadbuffer( L, fileContents, fileSize, bufferFileName );
    }

    if ( !lua_isstring( loaderLuaState, -1 ) )
    {
        lua_pop( loaderLuaState, 1 );  // Pop the invalid preprocessed file contents

        Warning( "[Lua] Loader Error: PreProcessFile did not return a string for file %s (returning file unmodified!)\n", fullPath );

        return luaL_loadbuffer( L, fileContents, fileSize, bufferFileName );
    }

    const char *preprocessedFileContents = lua_tostring( loaderLuaState, -1 );

    if ( lua_log_loader.GetBool() )
    {
        // Write the file to the logs folder
#ifdef CLIENT_DLL
        const char *gamePath = engine->GetGameDirectory();
#else
        char gamePath[256];
        engine->GetGameDir( gamePath, 256 );
#endif
        char relativePath[MAX_PATH];
        Q_StrRight( fullPath, Q_strlen( fullPath ) - Q_strlen( gamePath ), relativePath, sizeof( relativePath ) );
        char fileLogPath[MAX_PATH];
        Q_snprintf( fileLogPath, sizeof( fileLogPath ), "logs\\_loader%s", relativePath );
        V_RemoveDotSlashes( fileLogPath );

        CUtlBuffer buffer( 0, 0, CUtlBuffer::TEXT_BUFFER );
        buffer.PutString( preprocessedFileContents );

        char fileLogPathDirectory[MAX_PATH];
        Q_ExtractFilePath( fileLogPath, fileLogPathDirectory, sizeof( fileLogPathDirectory ) );
        filesystem->CreateDirHierarchy( fileLogPathDirectory, "GAME" );

        filesystem->WriteFile( fileLogPath, "GAME", buffer );
    }

    lua_pop( loaderLuaState, 1 );  // Pop the preprocessed file contents now that we have them

    int errorCode = luaL_loadbuffer(
                        L,
                        preprocessedFileContents,
                        strlen( preprocessedFileContents ),
                        bufferFileName ) ||
                    lua_pcall( L, 0, LUA_MULTRET, 0 );

    return errorCode;
}

/// <summary>
/// Executes the given file, clearing the stack of values that
/// were returned by the file afterwards.
/// </summary>
/// <param name="L"></param>
/// <param name="string"></param>
/// <returns></returns>
LUA_API int luasrc_dofile( lua_State *L, const char *fileName )
{
    int stackBefore = lua_gettop( L );
    int iError = luasrc_dofile_with_loader( L, fileName );

    if ( iError != 0 )
    {
        Warning( "[Lua] Error: %s\n", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
    }

    lua_settop( L, stackBefore );

    return iError;
}

/// <summary>
/// Executes the given file, not resetting the stack afterwards.
/// It returns the amount of new values on the stack (which the file
/// may have returned).
/// </summary>
/// <param name="L"></param>
/// <param name="string"></param>
/// <returns>The amount of new values on the stack or -1 on error</returns>
LUA_API int luasrc_dofile_leave_stack( lua_State *L, const char *fileName )
{
    int stackBefore = lua_gettop( L );
    int iError = luasrc_dofile_with_loader( L, fileName );

    if ( iError != 0 )
    {
        Warning( "[Lua] Error: %s\n", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
        return -1;
    }

    return lua_gettop( L ) - stackBefore;
}

bool luasrc_checkfolder( const char *path )
{
    return g_pFullFileSystem->IsDirectory( path, "MOD" );
}

LUA_API void luasrc_dofolder( lua_State *L, const char *path )
{
    if ( !luasrc_checkfolder( path ) )
    {
        Warning( "[Lua] (dofolder) Folder does not exist: %s\n", path );
        return;
    }

    FileFindHandle_t fh;

    char searchPath[512];
    Q_snprintf( searchPath, sizeof( searchPath ), "%s\\*.lua", path );

    char const *fn = g_pFullFileSystem->FindFirstEx( searchPath, "MOD", &fh );

    while ( fn )
    {
        if ( fn[0] != '.' )
        {
            char ext[10];
            Q_ExtractFileExtension( fn, ext, sizeof( ext ) );

            if ( !Q_stricmp( ext, "lua" ) )
            {
                char relative[512];
                char loadname[512];
                Q_snprintf( relative, sizeof( relative ), "%s\\%s", path, fn );
                filesystem->RelativePathToFullPath( relative, "MOD", loadname, sizeof( loadname ) );
                luasrc_dofile( L, loadname );
            }
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }
    g_pFullFileSystem->FindClose( fh );
}

LUA_API int luasrc_pcall( lua_State *L, int nargs, int nresults, int errfunc )
{
    int iError = lua_pcall( L, nargs, nresults, errfunc );

    if ( iError != 0 )
    {
        Warning( "[Lua] %s\n", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
    }

    return iError;
}

LUA_API void luasrc_print( lua_State *L, int narg )
{
    lua_getglobal( L, "tostring" );
    const char *s;
    lua_pushvalue( L, -1 );   /* function to be called */
    lua_pushvalue( L, narg ); /* value to print */
    lua_call( L, 1, 1 );
    s = lua_tostring( L, -1 ); /* get result */
    Msg( " %d:\t%s\n", narg, s );
    lua_pop( L, 1 ); /* pop result */
    lua_pop( L, 1 ); /* pop function */
}

LUA_API void luasrc_dumpstack( lua_State *L )
{
    int n = lua_gettop( L ); /* number of objects */
    int i;
    lua_getglobal( L, "tostring" );

    for ( i = 1; i <= n; i++ )
    {
        const char *s;
        lua_pushvalue( L, -1 ); /* function to be called */
        lua_pushvalue( L, i );  /* value to print */
        lua_call( L, 1, 1 );
        s = lua_tostring( L, -1 ); /* get result */
        Msg( " %d:\t%s\n", i, s );
        lua_pop( L, 1 ); /* pop result */
    }
    lua_pop( L, 1 ); /* pop function */
}

void luasrc_LoadEntities( const char *path )
{
    FileFindHandle_t fh;

    if ( !path )
    {
        path = "";
    }

    char root[MAX_PATH] = { 0 };

    char fileName[MAX_PATH] = { 0 };
    char fullPath[MAX_PATH] = { 0 };
    char className[255] = { 0 };

    Q_snprintf( root, sizeof( root ), "%s" LUA_PATH_ENTITIES "\\*", path );

    char const *fn = g_pFullFileSystem->FindFirstEx( root, "MOD", &fh );
    while ( fn )
    {
        Q_strcpy( className, fn );
        Q_strlower( className );
        if ( fn[0] != '.' )
        {
            if ( g_pFullFileSystem->FindIsDirectory( fh ) )
            {
#ifdef CLIENT_DLL
                Q_snprintf( fileName, sizeof( fileName ), "%s" LUA_PATH_ENTITIES "\\%s\\cl_init.lua", path, className );
#else
                Q_snprintf( fileName, sizeof( fileName ), "%s" LUA_PATH_ENTITIES "\\%s\\init.lua", path, className );
#endif
                if ( filesystem->FileExists( fileName, "MOD" ) )
                {
                    filesystem->RelativePathToFullPath(
                        fileName, "MOD", fullPath, sizeof( fullPath ) );
                    lua_newtable( L );
                    char entDir[MAX_PATH];
                    Q_snprintf( entDir, sizeof( entDir ), "entities\\%s", className );
                    lua_pushstring( L, entDir );
                    lua_setfield( L, -2, "Folder" );
                    lua_pushstring( L, LUA_BASE_ENTITY_CLASS );
                    lua_setfield( L, -2, "Base" );
                    lua_pushstring( L, LUA_BASE_ENTITY_FACTORY );
                    lua_setfield( L, -2, "Factory" );
                    lua_setglobal( L, "ENT" );
                    if ( luasrc_dofile( L, fullPath ) == 0 )
                    {
                        lua_getglobal( L, "entities" );
                        if ( lua_istable( L, -1 ) )
                        {
                            lua_getfield( L, -1, "Register" );
                            if ( lua_isfunction( L, -1 ) )
                            {
                                lua_remove( L, -2 );
                                lua_getglobal( L, "ENT" );
                                lua_pushstring( L, className );
                                luasrc_pcall( L, 2, 0, 0 );
                                lua_getglobal( L, "ENT" );
                                if ( lua_istable( L, -1 ) )
                                {
                                    lua_getfield( L, -1, "Factory" );
                                    if ( lua_isstring( L, -1 ) )
                                    {
                                        const char *pszClassname =
                                            lua_tostring( L, -1 );
                                        if ( Q_strcmp( pszClassname,
                                                       "CBaseAnimating" ) == 0 )
                                            RegisterScriptedEntity( className );
#ifndef CLIENT_DLL
                                        else if ( Q_strcmp( pszClassname,
                                                            "CBaseTrigger" ) == 0 )
                                            RegisterScriptedTrigger( className );
#endif
                                    }
                                    lua_pop( L, 2 );
                                }
                                else
                                {
                                    lua_pop( L, 1 );
                                }
                            }
                            else
                            {
                                lua_pop( L, 2 );
                            }
                        }
                        else
                        {
                            lua_pop( L, 1 );
                        }
                    }
                    lua_pushnil( L );
                    lua_setglobal( L, "ENT" );
                }
            }
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }
    g_pFullFileSystem->FindClose( fh );
}

void luasrc_LoadWeapons( const char *path )
{
    FileFindHandle_t fh;

    if ( !path )
    {
        path = "";
    }

    char root[MAX_PATH] = { 0 };

    char fileName[MAX_PATH] = { 0 };
    char fullpath[MAX_PATH] = { 0 };
    char className[MAX_WEAPON_STRING] = { 0 };

    Q_snprintf( root, sizeof( root ), "%s" LUA_PATH_WEAPONS "\\*", path );

    char const *fn = g_pFullFileSystem->FindFirstEx( root, "MOD", &fh );
    while ( fn )
    {
        Q_strcpy( className, fn );
        Q_strlower( className );

        if ( fn[0] == '.' )
        {
            fn = g_pFullFileSystem->FindNext( fh );
            continue;
        }

        if ( !g_pFullFileSystem->FindIsDirectory( fh ) )
        {
            fn = g_pFullFileSystem->FindNext( fh );
            continue;
        }

#ifdef CLIENT_DLL
        Q_snprintf( fileName, sizeof( fileName ), "%s" LUA_PATH_WEAPONS "\\%s\\cl_init.lua", path, className );
#else
        Q_snprintf( fileName, sizeof( fileName ), "%s" LUA_PATH_WEAPONS "\\%s\\init.lua", path, className );
#endif
        if ( !filesystem->FileExists( fileName, "MOD" ) )
        {
            fn = g_pFullFileSystem->FindNext( fh );
            continue;
        }

        filesystem->RelativePathToFullPath(
            fileName, "MOD", fullpath, sizeof( fullpath ) );

        lua_newtable( L );
        char entDir[MAX_PATH];
        Q_snprintf( entDir, sizeof( entDir ), "weapons\\%s", className );

        lua_pushstring( L, entDir );
        lua_setfield( L, -2, "Folder" );
        lua_pushstring( L, LUA_BASE_WEAPON );
        lua_setfield( L, -2, "Base" );
        lua_setglobal( L, "SWEP" );
        if ( luasrc_dofile( L, fullpath ) == 0 )
        {
            lua_getglobal( L, "weapons" );

            if ( lua_istable( L, -1 ) )
            {
                lua_getfield( L, -1, "Register" );

                if ( lua_isfunction( L, -1 ) )
                {
                    lua_remove( L, -2 );
                    lua_getglobal( L, "SWEP" );
                    lua_pushstring( L, className );
                    luasrc_pcall( L, 2, 0, 0 );
                    RegisterScriptedWeapon( className );
                }
                else
                {
                    lua_pop( L, 2 );
                }
            }
            else
            {
                lua_pop( L, 1 );
            }
        }

        lua_pushnil( L );
        lua_setglobal( L, "SWEP" );

        fn = g_pFullFileSystem->FindNext( fh );
    }

    g_pFullFileSystem->FindClose( fh );
}

/// <summary>
/// The loader Lua state will be used to load files for the first gamemode
/// that has a `_loader.lua` file in its folder. This file will be used to
/// preprocess all files that are loaded for the gamemode and the gamemodes
/// it inherits from.
///
/// Only one loader will be used for loading gamemodes. The base gamemode
/// will not be allowed to have a loader.
///
/// See `_loader.lua` inside `gmod_base` for an example of how to use this.
/// </summary>
/// <param name="gamemodePath"></param>
void luasrc_InitCustomLoader( const char *gamemode, const char *gamemodePath )
{
    // Check if the _loader.lua file exists, if it does, load it into the
    // loader Lua state
    char loaderFilePath[MAX_PATH];
    Q_snprintf( loaderFilePath, sizeof( loaderFilePath ), "%s\\_loader.lua", gamemodePath );

    if ( !filesystem->FileExists( loaderFilePath, "MOD" ) )
    {
        return;
    }

    filesystem->RelativePathToFullPath( loaderFilePath, "MOD", loaderFilePath, sizeof( loaderFilePath ) );

#ifndef CLIENT_DLL
    // Always send the _loader.lua to the client
    luasrc_sendfile( L, loaderFilePath );
#endif

    loaderLuaState = luaL_newstate();

    luaopen_base_minimal( loaderLuaState );

    lua_pushglobaltable( loaderLuaState );

    static const luaL_Reg baseLoaderFuncs[] = {
        { "print", luasrc_print },
        { NULL, NULL } };
    luaL_setfuncs( loaderLuaState, baseLoaderFuncs, 0 );
    lua_pushvalue( loaderLuaState, -1 );

    // Set the global _G table
    lua_setfield( loaderLuaState, -2, LUA_GNAME );

    static const luaL_Reg availableLibraries[] = {
        { LUA_TABLIBNAME, luaopen_table },
        { LUA_STRLIBNAME, luaopen_string },
        { LUA_UTF8LIBNAME, luaopen_utf8 },
        { LUA_MATHLIBNAME, luaopen_math },
        { NULL, NULL } };

    const luaL_Reg *lib;

    for ( lib = availableLibraries; lib->func; lib++ )
    {
        luaL_requiref( loaderLuaState, lib->name, lib->func, 1 );
        lua_pop( loaderLuaState, 1 );
    }

    lua_pushstring( loaderLuaState, LUA_VERSION );
    lua_setfield( loaderLuaState, -2, "_VERSION" );

    // Remove the global table from the stack
    lua_pop( loaderLuaState, 1 );

    // Before preprocessing, make a cfunction available to pcall for error handling
    // keep it at the bottom of the stack
    lua_pushcfunction( loaderLuaState, luasrc_loader_traceback );

    // Execute the loader file, leaving the table at the top of the stack
    if ( luasrc_dofile_leave_stack( loaderLuaState, loaderFilePath ) == -1 )
    {
        lua_close( loaderLuaState );
        Warning(
            "[Lua] Gamemode '%s' had invalid loader (%s failed to load)!\n",
            gamemode,
            loaderFilePath );

        return;
    }

    // Bring the PreProcessFile function to the top of the stack
    lua_getfield( loaderLuaState, -1, "PreProcessFile" );

    if ( !lua_isfunction( loaderLuaState, -1 ) )
    {
        lua_pop( loaderLuaState, 2 );
        lua_close( loaderLuaState );
        Warning(
            "[Lua] Gamemode '%s' had invalid loader (%s has no PreProcessFile function)!\n",
            gamemode,
            loaderFilePath );

        return;
    }

    lua_pop( loaderLuaState, 1 );  // Pop the PreProcessFile function for now

    // The loader file was loaded successfully. The PreProcessFile loader
    // function is now at the top of the stack.
    hasLoaderBeenActivated = true;

    DevMsg( "[Lua] Activated custom loader for gamemode '%s' (%s)!\n\tUsing custom loader for subsequent file loading in all (derived) gamemodes.\n", gamemode, loaderFilePath );
}

static void cleanUpGamemodeLoading( bool cleanLoader )
{
    // Unset the GM table
    lua_pushnil( L );
    lua_setglobal( L, "GM" );

    if ( cleanLoader && hasLoaderBeenActivated )
    {
        // Clean up the loader if we initialized it
        lua_close( loaderLuaState );
        hasLoaderBeenActivated = false;
    }
}

// too much maintenance
// static int luasrc_require_with_loader( lua_State *L )
//{
//    const char *name = luaL_checkstring( L, 1 );
//
//    if ( !hasLoaderBeenActivated )
//    {
//        lua_pushfstring( L, "Module '%s' not found by custom loader (not activated) ", name );
//
//        return 1;  // Returning 1 because we are pushing an error message
//    }
//
//    // Get the gamemode folder from GM.Folder or GAMEMODE.Folder
//    lua_getglobal( L, "GM" )
//
//    // TODO: Finish from here, make sure the loader func is used to have modules also use the preprocessor
//
//
// #ifdef CLIENT_DLL
//    const char *gamePath = engine->GetGameDirectory();
// #else
//    char gamePath[256];
//    engine->GetGameDir( gamePath, 256 );
// #endif
//
//    char gamemodeSearchPath[MAX_PATH];
//    Q_snprintf(
//        gamemodeSearchPath,
//        sizeof( gamemodeSearchPath ),
//        "%s\\%s\\?.lua;%s\\%s\\gamemode\\?.lua",
//        gamePath,
//        gamemodePath,
//        gamePath,
//        gamemodePath );
//    // luasrc_add_to_package_path( L, gamemodeSearchPath );
//
//    // Here we would load the module 'name' in some manner
//    // For simplicity, let's just push an error message to the stack
//    lua_pushfstring( L, "Module '%s' not found by luasrc_require_with_loader", name );
//
//    return 1;  // Returning 1 because we are pushing an error message
//}
//
// static void luasrc_add_loader_searcher( lua_State *L )
//{
//    lua_getglobal( L, "package" );
//    lua_getfield( L, -1, "searchers" );  // Get the current package.searchers
//    int searchersIndex = lua_gettop( L );
//
//    // Get the current length of package.searchers
//    lua_len( L, searchersIndex );
//    int length = lua_tointeger( L, -1 );
//    lua_pop( L, 1 );
//
//    // Push the new searcher function
//    lua_pushcfunction( L, luasrc_require_with_loader );
//    lua_rawseti( L, searchersIndex, length + 1 );
//}

/// <summary>
/// Append the new path to the existing package.path
/// </summary>
/// <param name="L"></param>
/// <param name="searchPath"></param>
static void luasrc_add_to_package_path( lua_State *L, const char *searchPath )
{
    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" );  // Get the current package.path
    const char *currentPath = lua_tostring( L, -1 );
    lua_pop( L, 1 );  // Remove the current path from the stack

    lua_pushfstring( L, "%s;%s", currentPath, searchPath );
    lua_setfield( L, -2, "path" );
    lua_pop( L, 1 );  // Remove the package table from the stack
}

/// <summary>
/// Loads the gamemode with the given name from the gamemodes folder.
/// Recursively loads the base gamemode if the GM.Base field is set.
///
/// Calls gamemodes.Register(gamemodeTable, gamemodeName, baseGameMode)
/// after loading all the relevant gamemodes.
/// </summary>
/// <param name="gamemode"></param>
/// <returns></returns>
bool luasrc_LoadGamemode( const char *gamemode )
{
    DevMsg( "Loading gamemode %s\n", gamemode );

    // Build the gamemode path
    char gamemodePath[MAX_PATH];
    Q_snprintf( gamemodePath, sizeof( gamemodePath ), "gamemodes\\%s", gamemode );

    // Start loading the loader for anything that isn't the base gamemode
    bool didWeInitLoader = false;
    if ( Q_strcmp( gamemode, LUA_BASE_GAMEMODE ) != 0 && !hasLoaderBeenActivated )
    {
        luasrc_InitCustomLoader( gamemode, gamemodePath );
        didWeInitLoader = true;
    }

#ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
#else
    char gamePath[256];
    engine->GetGameDir( gamePath, 256 );
#endif

    char gamemodeSearchPath[MAX_PATH];
    Q_snprintf(
        gamemodeSearchPath,
        sizeof( gamemodeSearchPath ),
        "%s\\%s\\?.lua;%s\\%s\\gamemode\\?.lua",
        gamePath,
        gamemodePath,
        gamePath,
        gamemodePath );
    luasrc_add_to_package_path( L, gamemodeSearchPath );

    // Set the GM table as a global variable
    lua_newtable( L );
    lua_setglobal( L, "GM" );

    // Let scripts know what folder the gamemode is in
    lua_getglobal( L, "GM" );
    lua_pushstring( L, "Folder" );
    lua_pushstring( L, gamemodePath );
    lua_settable( L, -3 );

    lua_pop( L, 1 );  // Pop the GM table, the stack should be empty now

    // Load the cl_init.lua file client-side and init.lua server-side
    char initialFileName[MAX_PATH];
    char initialFilePath[MAX_PATH];
#ifdef CLIENT_DLL
    Q_snprintf( initialFileName, sizeof( initialFileName ), "%s\\gamemode\\cl_init.lua", gamemodePath );
#else
    Q_snprintf( initialFileName, sizeof( initialFileName ), "%s\\gamemode\\init.lua", gamemodePath );
#endif

    if ( !filesystem->FileExists( initialFileName, "MOD" ) )
    {
        cleanUpGamemodeLoading( didWeInitLoader );

        Warning( "[Lua] Error loading gamemode: Attempted to load an invalid gamemode %s (%s does not exist)!\n", gamemode, initialFileName );

        return false;
    }

    filesystem->RelativePathToFullPath( initialFileName, "MOD", initialFilePath, sizeof( initialFilePath ) );

    if ( luasrc_dofile( L, initialFilePath ) != LUA_OK )
    {
        cleanUpGamemodeLoading( didWeInitLoader );

        Warning( "[Lua] Error loading gamemode: Attempted to load an invalid gamemode %s (failed to load %s)!\n", gamemode, initialFilePath );

        return false;
    }

    // Checks if the GM.Base field is set, if it is, we call luasrc_LoadGamemode
    // to load the relevant base gamemode
    lua_getglobal( L, "GM" );
    lua_getfield( L, -1, "Base" );
    lua_remove( L, -2 );  // Remove GM table

    const char *baseGamemode = LUA_BASE_GAMEMODE;

    // Get the base gamemode string
    if ( lua_isstring( L, -1 ) )
    {
        baseGamemode = lua_tostring( L, -1 );
    }

    lua_pop( L, 1 );  // Pop the base gamemode string or nil

    // Prepare gamemodes.Register with this gamemode to be called later
    // once all base gamemodes are loaded
    lua_getglobal( L, "gamemodes" );
    lua_getfield( L, -1, "Register" );
    lua_remove( L, -2 );  // Remove gamemodes now that we have Register

    // Sanity check in case someone messes with the gamemodes module
    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L, 1 );  // Pop Register

        Warning( "[Lua] Error loading gamemode: %s (gamemodes.Register is not a function)!\n", gamemode );

        return false;
    }

    // Leave the GM table on the stack to be used by gamemodes.Register
    // after all base gamemodes are loaded
    lua_getglobal( L, "GM" );

    if ( Q_strcmp( baseGamemode, LUA_BASE_GAMEMODE ) != 0 )
    {
        // Load the base gamemode for this gamemode only if it's
        // different from the default base gamemode
        if ( !luasrc_LoadGamemode( baseGamemode ) )
        {
            cleanUpGamemodeLoading( didWeInitLoader );

            // Pop gamemodes.Register and the GM table that were left on the stack
            lua_pop( L, 2 );

            Warning( "[Lua] Error loading gamemode: Attempted to load an invalid gamemode %s (failed to load base gamemode %s)!\n", gamemode, baseGamemode );

            return false;
        }
    }

    // (gamemodes.)Register and the original GM table are on the stack
    // Call gamemodes.Register(gamemodeTable, gamemodeName, baseGameMode)
    lua_pushstring( L, gamemode );
    lua_pushstring( L, baseGamemode );
    luasrc_pcall( L, 3, 0, 0 );

    // Unset the GM table, users should use the GAMEMODE table instead
    // Also clean up the loader if we initialized it
    cleanUpGamemodeLoading( didWeInitLoader );

    DevMsg( "Loaded gamemode %s\n", gamemode );

    return true;
}

bool luasrc_SetGamemode( const char *gamemode )
{
    lua_getglobal( L, "gamemodes" );

    if ( !lua_istable( L, -1 ) )
    {
        lua_pop( L, 1 );  // Remove gamemode table
        Warning( "[Lua] Error loading gamemode: Failed to load gamemode module!\n" );
        return false;
    }

    lua_getfield( L, -1, "Get" );

    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L, 2 );  // Remove gamemode table and Get function
        Warning( "[Lua] Error loading gamemode: Failed to set gamemode!\n" );
        return false;
    }

    lua_remove( L, -2 );             // Remove gamemode table
    lua_pushstring( L, gamemode );   // Push gamemode name
    luasrc_pcall( L, 1, 1, 0 );      // Call gamemodes.Get(gamemode)
    lua_setglobal( L, "GAMEMODE" );  // Set GAMEMODE to the active gamemode table

    lua_getglobal( L, "gamemodes" );
    lua_getfield( L, -1, "InternalSetActiveName" );

    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L,
                 2 );  // Remove gamemode table and InternalSetActiveName function
        Warning( "[Lua] Error loading gamemode: Failed to set gamemode (gamemodes.InternalSetActiveName didn't exist)!\n" );
        return false;
    }

    lua_remove( L, -2 );            // Remove gamemode table
    lua_pushstring( L, gamemode );  // Push gamemode name
    luasrc_pcall( L, 1, 0, 0 );     // Call gamemodes.InternalSetActiveName(gamemode)

#ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
#else
    char gamePath[256];
    engine->GetGameDir( gamePath, 256 );
#endif
    Q_strncpy( contentSearchPath, gamePath, sizeof( contentSearchPath ) );
    Q_strncat( contentSearchPath, "\\gamemodes\\", sizeof( contentSearchPath ) );
    Q_strncat( contentSearchPath, gamemode, sizeof( contentSearchPath ) );
    Q_strncat( contentSearchPath, "\\content", sizeof( contentSearchPath ) );

    filesystem->AddSearchPath( contentSearchPath, "GAME" );

    char loadPath[MAX_PATH];
    Q_snprintf( loadPath, sizeof( loadPath ), "%s\\", contentSearchPath );

    luasrc_LoadWeapons( loadPath );
    luasrc_LoadEntities( loadPath );
    // luasrc_LoadEffects( loadPath );

    BEGIN_LUA_CALL_HOOK( "Initialize" );
    END_LUA_CALL_HOOK( 0, 0 );

    return true;
}

#ifdef LUA_SDK
#ifdef CLIENT_DLL
CON_COMMAND( lua_run_cl, "Run a Lua string" )
{
    if ( !g_bLuaInitialized )
        return;

    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_run_cl <string>\n" );
        return;
    }

    int status = luasrc_dostring( L, args.ArgS() );
    if ( status == 0 && lua_gettop( L ) > 0 )
    { /* any result to print? */
        lua_getglobal( L, "print" );
        lua_insert( L, 1 );
        if ( lua_pcall( L, lua_gettop( L ) - 1, 0, 0 ) != 0 )
            Warning( "%s",
                     lua_pushfstring( L, "error calling " LUA_QL( "print" ) " (%s)", lua_tostring( L, -1 ) ) );
    }
    lua_settop( L, 0 ); /* clear stack */
}
#else
CON_COMMAND( lua_run, "Run a Lua string" )
{
    if ( !g_bLuaInitialized )
        return;

    if ( !UTIL_IsCommandIssuedByServerAdmin() )
        return;

    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_run <string>\n" );
        return;
    }

    int status = luasrc_dostring( L, args.ArgS() );
    if ( status == 0 && lua_gettop( L ) > 0 )
    { /* any result to print? */
        lua_getglobal( L, "print" );
        lua_insert( L, 1 );
        if ( lua_pcall( L, lua_gettop( L ) - 1, 0, 0 ) != 0 )
            Warning( "%s",
                     lua_pushfstring( L, "error calling " LUA_QL( "print" ) " (%s)", lua_tostring( L, -1 ) ) );
    }
    lua_settop( L, 0 ); /* clear stack */
}
#endif

static int DoFileCompletion( const char *partial,
                             char commands[COMMAND_COMPLETION_MAXITEMS]
                                          [COMMAND_COMPLETION_ITEM_LENGTH] )
{
    int current = 0;

#ifdef CLIENT_DLL
    const char *cmdname = "lua_dofile_cl";
#else
    const char *cmdname = "lua_dofile";
#endif
    char *substring = NULL;
    int substringLen = 0;
    if ( Q_strstr( partial, cmdname ) && strlen( partial ) > strlen( cmdname ) + 1 )
    {
        substring = ( char * )partial + strlen( cmdname ) + 1;
        substringLen = strlen( substring );
    }

    FileFindHandle_t fh;

    char WildCard[MAX_PATH] = { 0 };
    if ( substring == NULL )
        substring = "";
    Q_snprintf( WildCard, sizeof( WildCard ), LUA_ROOT "\\%s*", substring );
    Q_FixSlashes( WildCard );
    char const *fn = g_pFullFileSystem->FindFirstEx( WildCard, "MOD", &fh );
    while ( fn && current < COMMAND_COMPLETION_MAXITEMS )
    {
        if ( fn[0] != '.' )
        {
            char fileName[MAX_PATH] = { 0 };
            Q_snprintf( fileName, sizeof( fileName ), LUA_ROOT "\\%s\\%s", substring, fn );
            Q_FixSlashes( fileName );
            if ( filesystem->FileExists( fileName, "MOD" ) )
            {
                Q_snprintf( commands[current], sizeof( commands[current] ), "%s %s%s", cmdname, substring, fn );
                current++;
            }
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }
    g_pFullFileSystem->FindClose( fh );

    return current;
}

#ifdef CLIENT_DLL
CON_COMMAND_F_COMPLETION( lua_dofile_cl, "Load and run a Lua file", 0, DoFileCompletion )
{
    if ( !g_bLuaInitialized )
        return;

    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_dofile_cl <fileName>\n" );
        return;
    }

    char fullpath[512] = { 0 };
    char fileName[256] = { 0 };
    Q_snprintf( fileName, sizeof( fileName ), LUA_ROOT "\\%s", args.ArgS() );
    Q_strlower( fileName );
    Q_FixSlashes( fileName );
    if ( filesystem->FileExists( fileName, "MOD" ) )
    {
        filesystem->RelativePathToFullPath( fileName, "MOD", fullpath, sizeof( fullpath ) );
    }
    else
    {
        Q_snprintf( fullpath, sizeof( fullpath ), "%s\\" LUA_ROOT "\\%s", engine->GetGameDirectory(), args.ArgS() );
        Q_strlower( fullpath );
        Q_FixSlashes( fullpath );
    }

    if ( Q_strstr( fullpath, ".." ) )
    {
        return;
    }
    Msg( "Running file %s...\n", args.ArgS() );
    luasrc_dofile( L, fullpath );
}
#else
CON_COMMAND_F_COMPLETION( lua_dofile, "Load and run a Lua file", 0, DoFileCompletion )
{
    if ( !g_bLuaInitialized )
        return;

    if ( !UTIL_IsCommandIssuedByServerAdmin() )
        return;

    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_dofile <fileName>\n" );
        return;
    }

    char fullpath[512] = { 0 };
    char fileName[256] = { 0 };
    Q_snprintf( fileName, sizeof( fileName ), LUA_ROOT "lua\\%s", args.ArgS() );
    Q_strlower( fileName );
    Q_FixSlashes( fileName );
    if ( filesystem->FileExists( fileName, "MOD" ) )
    {
        filesystem->RelativePathToFullPath( fileName, "MOD", fullpath, sizeof( fullpath ) );
    }
    else
    {
        // fileName is local to game dir for Steam, so we need to prepend game
        // dir for regular file load
        char gamePath[256];
        engine->GetGameDir( gamePath, 256 );
        Q_StripTrailingSlash( gamePath );
        Q_snprintf( fullpath, sizeof( fullpath ), "%s\\" LUA_ROOT "\\%s", gamePath, args.ArgS() );
        Q_strlower( fullpath );
        Q_FixSlashes( fullpath );
    }

    if ( Q_strstr( fullpath, ".." ) )
    {
        return;
    }
    Msg( "Running file %s...\n", args.ArgS() );
    luasrc_dofile( L, fullpath );
}
#endif

#if DEBUG
static void DumpLuaStack( lua_State *L )
{
    int n = lua_gettop( L ); /* number of objects */
    int i;

    for ( i = 1; i <= n; i++ )
    {
        if ( lua_istable( L, -1 ) )
        {
            lua_getglobal( L, "table" );
            lua_getfield( L, -1, "Print" );
            lua_remove( L, -2 );
            lua_pushvalue( L, i );
            lua_call( L, 1, 0 );
        }
        else
        {
            const char *s;
            lua_getglobal( L, "tostring" );
            lua_pushvalue( L, -1 ); /* function to be called */
            lua_pushvalue( L, i );  /* value to print */
            lua_call( L, 1, 1 );

            s = lua_tostring( L, -1 ); /* get result */
            Warning( " %d:\t%s\n", i, s );
            lua_pop( L, 1 ); /* pop result */
        }
    }

    if ( n > 0 )
        Warning( "Warning: %d object(s) left on the stack!\n", n );
}

#ifdef CLIENT_DLL
CON_COMMAND( lua_dumpstack_cl, "Prints the Lua stack" )
{
    if ( !g_bLuaInitialized )
        return;

    DumpLuaStack( L );
}

CON_COMMAND( lua_dumpstack_menu, "Prints the Lua stack" )
{
    DumpLuaStack( LGameUI );
}
#else
CON_COMMAND( lua_dumpstack, "Prints the Lua stack" )
{
    if ( !g_bLuaInitialized )
        return;

    DumpLuaStack( L );
}
#endif
#endif
#endif
