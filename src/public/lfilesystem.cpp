#include "cbase.h"
#include "filesystem.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lfilesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API FileHandle_t &lua_tofilehandle( lua_State *L, int idx )
{
    FileHandle_t *phFile = ( FileHandle_t * )luaL_checkudata( L, idx, LUA_FILEHANDLEMETANAME );
    return *phFile;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushfilehandle( lua_State *L, FileHandle_t hFile )
{
    FileHandle_t *phFile = ( FileHandle_t * )lua_newuserdata( L, sizeof( FileHandle_t ) );
    *phFile = hFile;
    LUA_SAFE_SET_METATABLE( L, LUA_FILEHANDLEMETANAME );
}

LUALIB_API FileHandle_t &luaL_checkfilehandle( lua_State *L, int narg )
{
    FileHandle_t *d = ( FileHandle_t * )luaL_checkudata( L, narg, LUA_FILEHANDLEMETANAME );

    if ( *d == FILESYSTEM_INVALID_HANDLE ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "FileHandle_t expected, got FILESYSTEM_INVALID_HANDLE" );

    return *d;
}

LUA_REGISTRATION_INIT( Files );

LUA_BINDING_BEGIN( Files, AddPackFile, "library", "Add a pack file to the search path." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "pathId" );

    lua_pushboolean( L, filesystem->AddPackFile( filePath, pathId ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the pack file was added, false otherwise." )

LUA_BINDING_BEGIN( Files, AddSearchPath, "library", "Add a search path to the filesystem." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "pathId" );
    SearchPathAdd_t addType = LUA_BINDING_ARGUMENT_ENUM_WITH_DEFAULT( SearchPathAdd_t, 3, PATH_ADD_TO_TAIL, "addType" );

    char fullpath[512] = { 0 };
    bool bGetCurrentDirectory = V_GetCurrentDirectory( fullpath, sizeof( fullpath ) );
    if ( bGetCurrentDirectory )
    {
#ifdef CLIENT_DLL
        const char *gamePath = engine->GetGameDirectory();
#else
        char gamePath[256];
        engine->GetGameDir( gamePath, 256 );
#endif
        V_SetCurrentDirectory( gamePath );
    }
    filesystem->AddSearchPath( filePath, pathId, addType );
    if ( bGetCurrentDirectory )
    {
        V_SetCurrentDirectory( fullpath );
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, BeginMapAccess, "library", "Begin map access." )
{
    filesystem->BeginMapAccess();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, CancelWaitForResources, "library", "Cancel waiting for resources." )
{
    filesystem->CancelWaitForResources( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "id" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, Close, "library", "Close a file." )
{
    FileHandle_t &file = LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 1, "file" );
    filesystem->Close( file );

    // Andrew; this isn't standard behavior or usage, but we do this for the sake
    // of things being safe in Lua
    file = FILESYSTEM_INVALID_HANDLE;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, CreateDirectoryHierarchy, "library", "Create a directory hierarchy." )
{
    const char *path = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "pathId" );

    filesystem->CreateDirHierarchy( path, pathId );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, DiscardPreloadData, "library", "Discard preload data." )
{
    filesystem->DiscardPreloadData();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, Disconnect, "library", "Disconnect." )
{
    filesystem->Disconnect();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, EnableAllowListFileTracking, "library", "Enable allowlist file tracking." )
{
    bool enable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "enable" );
    bool cacheAllVPKHashes = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "cacheAllVPKHashes" );
    bool recalculateAndCheckHashes = LUA_BINDING_ARGUMENT( luaL_checkboolean, 3, "recalculateAndCheckHashes" );

    filesystem->EnableWhitelistFileTracking( enable, cacheAllVPKHashes, recalculateAndCheckHashes );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, EndMapAccess, "library", "End map access." )
{
    filesystem->EndMapAccess();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, EndOfFile, "library", "Check if the end of a file has been reached." )
{
    lua_pushboolean( L, filesystem->EndOfFile( LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 1, "file" ) ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the end of the file has been reached, false otherwise." )

LUA_BINDING_BEGIN( Files, FileExists, "library", "Check if the file or directory exists." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "pathId" );

    lua_pushboolean( L,
        filesystem->FileExists( filePath, pathId )
        || filesystem->IsDirectory( filePath, pathId ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the file or directory exists, false otherwise." )

LUA_BINDING_BEGIN( Files, Flush, "library", "Flush a file." )
{
    filesystem->Flush( LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 1, "file" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, GetDvdMode, "library", "Get the DVD mode." )
{
    lua_pushinteger( L, filesystem->GetDVDMode() );

    return 1;
}
LUA_BINDING_END( "integer", "The DVD mode." )

LUA_BINDING_BEGIN( Files, GetLocalCopy, "library", "Get a local copy of a file." )
{
    filesystem->GetLocalCopy( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, GetSearchPath, "library", "Get the search path." )
{
    char searchPath[MAX_PATH * 25];  // TODO: How do we know how many paths there are? This is a guess.
    filesystem->GetSearchPath_safe( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "bGetPackFiles" ), searchPath );
    lua_pushstring( L, searchPath );

    return 1;
}
LUA_BINDING_END( "string", "The search path." )

LUA_BINDING_BEGIN( Files, GetWhitelistSpewFlags, "library", "Get the whitelist spew flags." )
{
    lua_pushinteger( L, filesystem->GetWhitelistSpewFlags() );

    return 1;
}
LUA_BINDING_END( "integer", "The whitelist spew flags." )

LUA_BINDING_BEGIN( Files, HintResourceNeed, "library", "Hint that a resource is needed." )
{
    const char *pFilename = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "filename" );
    int iCharacterCount = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "characterCount" );

    lua_pushinteger( L, filesystem->HintResourceNeed( pFilename, iCharacterCount ) );

    return 1;
}
LUA_BINDING_END( "integer", "The hint resource need." )

LUA_BINDING_BEGIN( Files, IsDirectory, "library", "Check if a path is a directory." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "pathId" );

    lua_pushboolean( L, filesystem->IsDirectory( filePath, pathId ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the path is a directory, false otherwise." )

LUA_BINDING_BEGIN( Files, IsFileImmediatelyAvailable, "library", "Check if a file is immediately available." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );

    lua_pushboolean( L, filesystem->IsFileImmediatelyAvailable( filePath ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the file is immediately available, false otherwise." )

LUA_BINDING_BEGIN( Files, IsFileWritable, "library", "Check if a file is writable." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "pathId" );

    lua_pushboolean( L, filesystem->IsFileWritable( filePath, pathId ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the file is writable, false otherwise." )

LUA_BINDING_BEGIN( Files, IsOk, "library", "Check if a file handle is valid." )
{
    lua_pushboolean( L, filesystem->IsOk( LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 1, "file" ) ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the file handle is valid, false otherwise." )

LUA_BINDING_BEGIN( Files, IsSteam, "library", "Check if Steam is running." )
{
    lua_pushboolean( L, filesystem->IsSteam() );

    return 1;
}
LUA_BINDING_END( "boolean", "true if Steam is running, false otherwise." )

LUA_BINDING_BEGIN( Files, LoadCompiledKeyValues, "library", "Load compiled key values." )
{
    filesystem->LoadCompiledKeyValues( LUA_BINDING_ARGUMENT_ENUM( IFileSystem::KeyValuesPreloadType_t, 1, "preloadType" ), LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "filename" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, MarkAllCrcsUnverified, "library", "Mark all CRCs as unverified." )
{
    filesystem->MarkAllCRCsUnverified();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, MarkPathIdByRequestOnly, "library", "Mark a path ID as request only." )
{
    const char *pathId = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "pathId" );
    bool bRequestOnly = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "requestOnly" );

    filesystem->MarkPathIDByRequestOnly( pathId, bRequestOnly );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, MountSteamContent, "library", "Mount Steam content." )
{
    lua_pushinteger( L, filesystem->MountSteamContent( LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 1, -1, "appId" ) ) );

    return 1;
}
LUA_BINDING_END( "integer", "The mount Steam content." )

LUA_BINDING_BEGIN( Files, Open, "library", "Open a file." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "filePath" );
    char const *readMode = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "readMode" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, "DATA", "pathId" );

    // Ensure that the read mode is valid
    while ( *readMode )
    {
        if ( *readMode != 'r' && *readMode != 'w' && *readMode != 'a' && *readMode != '+' && *readMode != 'b' )
        {
            luaL_argerror( L, 2, "Invalid read mode" );
        }
        readMode++;
    }

    readMode = luaL_checkstring( L, 2 );

    // For now We will support writing only to DATA, we will allow reading in any search path for now
    // Experiment; TODO: Is it risky to allow reading from any search path?
    if ( readMode[0] == 'w' )
    {
        if ( !V_stristr( pathId, "data" ) )
        {
            char message[256];
            V_snprintf( message, sizeof( message ), "Invalid pathId for writing (DATA expected, got %s)", pathId );
            luaL_argerror( L, 3, message );
        }
    }

    lua_pushfilehandle( L, filesystem->Open( filePath, readMode, pathId ) );
    return 1;
}
LUA_BINDING_END( "FileHandle", "The file handle." )

LUA_BINDING_BEGIN( Files, Precache, "library", "Precache a file." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "pathId" );

    lua_pushboolean( L, filesystem->Precache( filePath, pathId ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the file was precached, false otherwise." )

LUA_BINDING_BEGIN( Files, PrintOpenedFiles, "library", "Print opened files." )
{
    filesystem->PrintOpenedFiles();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, PrintSearchPaths, "library", "Print search paths." )
{
    filesystem->PrintSearchPaths();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, Read, "library", "Read from a file." )
{
    FileHandle_t &file = LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 2, "file" );
    int size = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "size" );

    if ( size < 0 )
    {
        lua_pushstring( L, "Invalid size parameter" );
        return lua_error( L );
    }

    char *buffer = new char[size + 1];
    if ( !buffer )
    {
        lua_pushstring( L, "Memory allocation failed" );
        return lua_error( L );
    }

    int bytesRead = filesystem->Read( buffer, size, file );
    if ( bytesRead < 0 )
    {
        delete[] buffer;
        lua_pushstring( L, "File read error" );
        return lua_error( L );
    }

    buffer[bytesRead] = '\0';  // Ensure the buffer is null-terminated

    lua_pushinteger( L, bytesRead );
    lua_pushlstring( L, buffer, bytesRead );
    delete[] buffer;

    return 2;
}
LUA_BINDING_END( "integer", "The number of bytes read.", "string", "The data read." )

LUA_BINDING_BEGIN( Files, RemoveAllSearchPaths, "library", "Remove all search paths." )
{
    filesystem->RemoveAllSearchPaths();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, RemoveFile, "library", "Remove a file." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "pathId" );

    filesystem->RemoveFile( filePath, pathId );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, RemoveSearchPath, "library", "Remove a search path." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "pathId" );

    lua_pushboolean( L, filesystem->RemoveSearchPath( filePath, pathId ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the search path was removed, false otherwise." )

LUA_BINDING_BEGIN( Files, RemoveSearchPaths, "library", "Remove search paths." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );

    filesystem->RemoveSearchPaths( filePath );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, RenameFile, "library", "Rename a file." )
{
    const char *pOldPath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "oldPath" );
    const char *pNewPath = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "newPath" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, 0, "pathId" );

    lua_pushboolean( L, filesystem->RenameFile( pOldPath, pNewPath, pathId ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the file was renamed, false otherwise." )

LUA_BINDING_BEGIN( Files, SetFileWritable, "library", "Set a file as writable." )
{
    const char *filePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    bool bWritable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "writable" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, 0, "pathId" );

    lua_pushboolean( L, filesystem->SetFileWritable( filePath, bWritable, pathId ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the file was set as writable, false otherwise." )

LUA_BINDING_BEGIN( Files, SetupPreloadData, "library", "Setup preload data." )
{
    filesystem->SetupPreloadData();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, SetWarningLevel, "library", "Set the warning level." )
{
    filesystem->SetWarningLevel( LUA_BINDING_ARGUMENT_ENUM( FileWarningLevel_t, 1, "level" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, SetWhitelistSpewFlags, "library", "Set the whitelist spew flags." )
{
    filesystem->SetWhitelistSpewFlags( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "flags" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, Shutdown, "library", "Shutdown the filesystem." )
{
    filesystem->Shutdown();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Files, Size, "library", "Get the size of a file." )
{
    switch ( lua_type( L, 1 ) )
    {
        case LUA_TSTRING:
            lua_pushinteger( L, filesystem->Size( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "pathId" ) ) );
            break;
        case LUA_TUSERDATA:
        default:
            lua_pushinteger( L, filesystem->Size( LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 1, "file" ) ) );
            break;
    }

    return 1;
}
LUA_BINDING_END( "integer", "The size of the file." )

LUA_BINDING_BEGIN( Files, UnzipFile, "library", "Unzip a file." )
{
    const char *pSource = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "source" );
    const char *pDestination = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "destination" );
    const char *pathId = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "pathId" );

    lua_pushboolean( L, filesystem->UnzipFile( pSource, pDestination, pathId ) );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the file was unzipped, false otherwise." )

LUA_BINDING_BEGIN( Files, WaitForResources, "library", "Wait for resources." )
{
    lua_pushinteger( L, filesystem->WaitForResources( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" ) ) );

    return 1;
}
LUA_BINDING_END( "integer", "The wait for resources." )

LUA_BINDING_BEGIN( Files, Write, "library", "Write to a file." )
{
    size_t l;
    const char *pInput = LUA_BINDING_ARGUMENT_WITH_EXTRA( luaL_checklstring, 1, &l, "input" );
    lua_pushinteger( L, filesystem->Write( pInput, l, LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 2, "file" ) ) );

    return 1;
}
LUA_BINDING_END( "integer", "The number of bytes written." )

LUA_BINDING_BEGIN( Files, Find, "library", "Find files." )
{
    const char *path = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "path" );
    const char *pathId = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, "GAME", "pathId" );

    FileFindHandle_t fh;

    char const *fn = filesystem->FindFirstEx( path, pathId, &fh );

    lua_createtable( L, 0, 0 );  // files
    lua_createtable( L, 0, 0 );  // directories

    while ( fn )
    {
        if ( fn[0] == '.' )
        {
            fn = filesystem->FindNext( fh );
            continue;
        }

        if ( filesystem->FindIsDirectory( fh ) )
        {
            lua_pushstring( L, fn );
            lua_rawseti( L, -2, luaL_len( L, -2 ) + 1 );
        }
        else
        {
            lua_pushstring( L, fn );
            lua_rawseti( L, -3, luaL_len( L, -3 ) + 1 );
        }

        fn = filesystem->FindNext( fh );
    }

    filesystem->FindClose( fh );

    return 2;
}
LUA_BINDING_END( "table", "The files and directories." )

/*
** FileHandle Meta
*/

LUA_REGISTRATION_INIT( FileHandle )

#define GET_FILE_SIZE -1

LUA_BINDING_BEGIN( FileHandle, Read, "class", "Read from a file." )
{
    FileHandle_t &file = LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 1, "file" );
    int size = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, GET_FILE_SIZE, "size" );

    if ( size == GET_FILE_SIZE )
    {
        size = filesystem->Size( file );
    }

    if ( size < 0 )
    {
        lua_pushstring( L, "Invalid size parameter" );
        return lua_error( L );
    }

    char *buffer = new char[size + 1];
    if ( !buffer )
    {
        lua_pushstring( L, "Memory allocation failed" );
        return lua_error( L );
    }

    int bytesRead = filesystem->Read( buffer, size, file );

    if ( bytesRead < 0 )
    {
        delete[] buffer;
        lua_pushstring( L, "File read error" );
        return lua_error( L );
    }

    buffer[bytesRead] = '\0';  // Ensure the buffer is null-terminated

    lua_pushlstring( L, buffer, bytesRead );
    lua_pushinteger( L, bytesRead );

    delete[] buffer;

    return 2;
}
LUA_BINDING_END( "integer", "The number of bytes read.", "string", "The data read." )

LUA_BINDING_BEGIN( FileHandle, Size, "class", "Get the size of a file." )
{
    lua_pushinteger( L, filesystem->Size( LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 1, "file" ) ) );

    return 1;
}
LUA_BINDING_END( "integer", "The size of the file." )

LUA_BINDING_BEGIN( FileHandle, Write, "class", "Write to a file." )
{
    size_t l;
    const char *pInput = LUA_BINDING_ARGUMENT_WITH_EXTRA( luaL_checklstring, 1, &l, "input" );
    lua_pushinteger( L, filesystem->Write( pInput, l, LUA_BINDING_ARGUMENT( luaL_checkfilehandle, 2, "file" ) ) );

    return 1;
}
LUA_BINDING_END( "integer", "The number of bytes written." )

LUA_BINDING_BEGIN( FileHandle, __gc, "class", "Close a file." )
{
    FileHandle_t hFile = LUA_BINDING_ARGUMENT( lua_tofilehandle, 1, "file" );
    if ( hFile != FILESYSTEM_INVALID_HANDLE )
        filesystem->Close( hFile );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( FileHandle, __tostring, "class", "Get the string representation of a file handle." )
{
    FileHandle_t hFile = LUA_BINDING_ARGUMENT( lua_tofilehandle, 1, "file" );
    if ( hFile == FILESYSTEM_INVALID_HANDLE )
        lua_pushstring( L, "FILESYSTEM_INVALID_HANDLE" );
    else
        lua_pushfstring( L, "FileHandle_t: %p", lua_tofilehandle( L, 1 ) );

    return 1;
}
LUA_BINDING_END( "string", "The string representation of the file handle." )

LUA_BINDING_BEGIN( FileHandle, __eq, "class", "Check if two file handles are equal." )
{
    FileHandle_t hFile1 = LUA_BINDING_ARGUMENT( lua_tofilehandle, 1, "file1" );
    FileHandle_t hFile2 = LUA_BINDING_ARGUMENT( lua_tofilehandle, 2, "file2" );
    lua_pushboolean( L, hFile1 == hFile2 );

    return 1;
}
LUA_BINDING_END( "boolean", "true if the file handles are equal, false otherwise." )

LUA_BINDING_BEGIN( FileHandle, __len, "class", "Get the length of a file." )
{
    FileHandle_t hFile = LUA_BINDING_ARGUMENT( lua_tofilehandle, 1, "file" );
    if ( hFile == FILESYSTEM_INVALID_HANDLE )
        lua_pushnil( L );
    else
        lua_pushinteger( L, filesystem->Size( hFile ) );

    return 1;
}
LUA_BINDING_END( "integer", "The length of the file or nil if the file handle is invalid." )

/*
** Open filesystem library and meta table
*/
LUALIB_API int luaopen_Files( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Files );

    lua_pushfilehandle( L, FILESYSTEM_INVALID_HANDLE );
    lua_setglobal( L, "FILESYSTEM_INVALID_HANDLE" );

    return 1;
}

LUALIB_API int luaopen_FileHandle( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_FILEHANDLEMETANAME );

    LUA_REGISTRATION_COMMIT( FileHandle );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_FILEHANDLEMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "FileHandle" */

    return 1;
}
