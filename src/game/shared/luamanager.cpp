#include "cbase.h"
#include "utlbuffer.h"
#include "UtlStringMap.h"
#include "activitylist.h"
#include "filesystem.h"
#ifndef CLIENT_DLL
#include "gameinterface.h"
#include "basescriptedtrigger.h"
#include "networkstringtable_gamedll.h"
#endif
#include "steam/isteamfriends.h"
#include "networkstringtabledefs.h"
#ifdef CLIENT_DLL
#include <scriptedclientluapanel.h>
#include <vgui_int.h>
#include "ienginevgui.h"
#endif
#include "basescripted.h"
#include "weapon_experimentbase_scriptedweapon.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "luacachefile.h"
#include "lconvar.h"
#include <lconcommand.h>
#include "licvar.h"
#include "lnetwork.h"
#include "inputsystem/ButtonCode.h"
#include <stack>
#include <cpng.h>
#include <mountaddons.h>
#include <steam/isteamhttp.h>
#include <lgameevents.h>
#include <mountsteamcontent.h>
#include <lresources.h>

#include <util/networkmanager.h>
#include <networksystem/inetworkgrouphandler.h>
#include <networksystem/networkserver.h>
#include "tier2/tier2.h"

#ifdef _WIN32
#include <winlite.h>
#endif
#include "zip/XUnzip.h"

extern "C"
{
#include "luasocket.h"
#include "mime.h"
}

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static FileHandle_t s_LuaLogFileHandle = FILESYSTEM_INVALID_HANDLE;

void Gamemode_ChangeCallback( IConVar *pConVar, char const *pOldString, float flOldValue );

ConVar gamemode( "gamemode", DEFAULT_GAMEMODE, FCVAR_ARCHIVE | FCVAR_REPLICATED, "The Lua gamemode to run" );

ConVar lua_log_cl( "lua_log_cl", "1", FCVAR_ARCHIVE );
ConVar lua_log_sv( "lua_log_sv", "1", FCVAR_ARCHIVE );
ConVar lua_log_loader( "lua_log_loader", "1", FCVAR_ARCHIVE );

static void LuaLogToFile( const char *format, ... )
{
    va_list argp;
    char msg[4096];

    va_start( argp, format );
    Q_vsnprintf( msg, sizeof( msg ), format, argp );
    va_end( argp );

#ifdef CLIENT_DLL
    if ( lua_log_cl.GetBool() )
#else
    if ( lua_log_sv.GetBool() )
#endif
    {
        if ( s_LuaLogFileHandle == FILESYSTEM_INVALID_HANDLE )
        {
            return;
        }

        filesystem->Write( msg, Q_strlen( msg ), s_LuaLogFileHandle );
        filesystem->Flush( s_LuaLogFileHandle );
    }
}

#define SHOW_LUA_ERROR( format, ... )                                  \
    ConColorMsg( REALM_COLOR, "\n[Lua] " format "\n", ##__VA_ARGS__ ); \
    LuaLogToFile( format, ##__VA_ARGS__ );

#define SHOW_LUA_MESSAGE( format, ... ) \
    Msg( "\n[" REALM_PREFIX "] " format "\n", ##__VA_ARGS__ );

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

/// <summary>
/// Lua Network Message Handler
///
/// This will pass the message to Lua for it to read the buffer.
/// </summary>
class CLuaNetworkGroupHandler : public INetworkGroupHandler
{
   public:
    CLuaNetworkGroupHandler( lua_State *L )
    {
        Assert( L );
        m_L = L;
    }

    void HandleReadingMessage( unsigned int messageTypeId, bf_read &buffer, IConnectedClient *client );

   private:
    lua_State *m_L;
};

void CLuaNetworkGroupHandler::HandleReadingMessage( unsigned int messageTypeId, bf_read &buffer, IConnectedClient *client )
{
    CBasePlayer *player = nullptr;

    if ( client )
    {
        player = g_pNetworkManager->FindConnectedPlayer( client );
        Assert( player != nullptr ); // TODO: What if there's no player for this client? How do we handle it?
    }

    lua_getglobal( m_L, LUA_NETWORKSLIBNAME );
    lua_getfield( m_L, -1, "HandleIncomingMessage" );

    lua_remove( m_L, -2 );  // Remove the Networks table
    Assert( lua_isfunction( L, -1 ) );

    lua_pushnumber( m_L, messageTypeId );
    lua_pushbf_read( m_L, &buffer );

    if ( player != nullptr )
    {
        CBasePlayer::PushLuaInstanceSafe( m_L, player );
    }
    else
    {
        lua_pushnil( m_L );
    }

    luasrc_pcall( m_L, 3, 0 );  // Call HandleIncomingMessage and pop it with 3 arguments
}

static CLuaNetworkGroupHandler *s_pLuaNetworkGroupHandler = nullptr;

#ifdef CLIENT_DLL
lua_State *LGameUI;
// This is where all scripted panels are parented to
// it will be created on the client when the L state is created
// and removed when the L state is destroyed.
CScriptedClientLuaPanel *g_pClientLuaPanel = NULL;

// This is where HUD parented panels are parented (which wont be shown
// when the GameUI Main Menu is shown)
CScriptedClientLuaPanel *g_pClientLuaPanelHUD = NULL;
#endif

// This is the state for the CLIENT (in case of CLIENT_DLL) or the
// SERVER (in case of GAME_DLL). It will persist for the entire
// play session.
lua_State *L;

// See luasrc_InitCustomLoader for more information
lua_State *loaderLuaState;
bool hasLoaderBeenActivated = false;

// Used to track the file that is currently being included, so that
// relative includes can be resolved correctly.
std::stack< std::string > fileIncludingStack;

// Lua system initialized for client or server
bool g_bLuaInitialized;
bool g_bGamemodeLoaded;

static void GetGamemodePath( const char *gamemodeName, char *gamemodePath, int gamemodePathLength )
{
    Q_snprintf( gamemodePath, gamemodePathLength, "gamemodes\\%s", gamemodeName );
}

#define LUA_GAMEMODE_INIT_FILE_SV "gamemode\\init.lua"
#define LUA_GAMEMODE_INIT_FILE_CL "gamemode\\cl_init.lua"

static bool TryFindGamemode( const char *gamemodeName, char *initialFileName, int initialFileNameLength, char *gamemodeRootPath = nullptr, int gamemodeRootPathLength = 0 )
{
    char gamemodePath[MAX_PATH];
    GetGamemodePath( gamemodeName, gamemodePath, sizeof( gamemodePath ) );

    // Load the cl_init.lua file client-side and init.lua server-side
#ifdef CLIENT_DLL
    Q_snprintf( initialFileName, initialFileNameLength, "%s\\" LUA_GAMEMODE_INIT_FILE_CL, gamemodePath );
#else
    Q_snprintf( initialFileName, initialFileNameLength, "%s\\" LUA_GAMEMODE_INIT_FILE_SV, gamemodePath );
#endif

    filesystem->RelativePathToFullPath( initialFileName, CONTENT_SEARCH_PATH, initialFileName, initialFileNameLength );

    if ( !filesystem->FileExists( initialFileName, CONTENT_SEARCH_PATH ) )
    {
        return false;
    }

    // Strips \\gamemode\\(cl_)init.lua from the initialFileName (we may have found it in a mounted game)
    if ( gamemodeRootPath )
    {
#ifdef CLIENT_DLL
        int partToStrip = Q_strlen( LUA_GAMEMODE_INIT_FILE_CL );
#else
        int partToStrip = Q_strlen( LUA_GAMEMODE_INIT_FILE_SV );
#endif

        Q_strncpy( gamemodeRootPath, initialFileName, gamemodeRootPathLength );
        gamemodeRootPath[Q_strlen( gamemodeRootPath ) - partToStrip] = '\0';
    }

    return true;
}

LUA_API int luasrc_print( lua_State *L )
{
    int n = lua_gettop( L ); /* number of arguments */
    int i;
    lua_getglobal( L, "tostring" );
    CUtlString stringToPrint = "";

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
            stringToPrint += "\t";
        stringToPrint += s;
        lua_pop( L, 1 ); /* pop result */
    }

    Msg( "%s\n", stringToPrint.Get() );
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

/// <summary>
/// Includes a file relative to the file that called this function.
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
static int luasrc_include( lua_State *L )
{
    int returnedValueCount = luasrc_dofile_leave_stack( L, luaL_checkstring( L, 1 ) );

    if ( returnedValueCount == -1 )
    {
        // An error occurred executing the script
        return 0;
    }

    return returnedValueCount;
}

/// <summary>
/// Lets users run Lua code from a string.
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
static int luasrc_RunString( lua_State *L )
{
    const char *code = luaL_checkstring( L, 1 );
    const char *identifier = luaL_optstring( L, 2, "RunString" );
    bool handleErrors = luaL_optboolean( L, 3, true );  // If false, will return the error message instead of throwing an error

    int iError = luaL_loadbuffer( L, code, strlen( code ), identifier );

    if ( iError != 0 )
    {
        if ( handleErrors )
        {
            luaL_error( L, "Error loading buffer: %s", lua_tostring( L, -1 ) );
        }
        else
        {
            return 1;
        }
    }

    iError = lua_pcall( L, 0, LUA_MULTRET, 0 );

    if ( iError != 0 )
    {
        if ( handleErrors )
        {
            luaL_error( L, "Error running buffer: %s", lua_tostring( L, -1 ) );
        }
        else
        {
            return 1;
        }
    }

    return 0;
}

static int PrintLuaMessage( lua_State *L, const char *append = nullptr )
{
    int n = lua_gettop( L );  // number of arguments
    int i;

    lua_getglobal( L, "tostring" );

    CUtlString stringToPrint = "";

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
            stringToPrint += "\t";
        stringToPrint += s;
        lua_pop( L, 1 );  // pop result
    }

    if ( append )
    {
        stringToPrint += append;
    }

    ConColorMsg( REALM_COLOR, "%s", stringToPrint.Get() );

    return 0;
}

// Prints blue text on the server, yellow text on the client
static int luasrc_PrintMessage( lua_State *L )
{
    return PrintLuaMessage( L );
}

static int luasrc_PrintMessageLine( lua_State *L )
{
    return PrintLuaMessage( L, "\n" );
}

static int luasrc_LuaLogToFile( lua_State *L )
{
    int n = lua_gettop( L );

    if ( n < 1 )
    {
        luaL_error( L, "at least one argument expected" );
        return 0;
    }

    const char *format = luaL_checkstring( L, 1 );
    CUtlVector< const char * > args;

    for ( int i = 2; i <= n; ++i )
    {
        args.AddToTail( lua_tostring( L, i ) );
    }

    LuaLogToFile( format, args.Base() );

    return 0;
}

static void luasrc_LoadEntityFromFile( char *fullPath, char *className )
{
    Q_strlower( className );

    lua_newtable( L );
    char entDir[MAX_PATH];
    Q_snprintf( entDir, sizeof( entDir ), "entities\\%s", className );
    lua_pushstring( L, entDir );
    lua_setfield( L, -2, "/*Folder*/" );
    lua_pushstring( L, LUA_BASE_ENTITY_CLASS );
    lua_setfield( L, -2, "Base" );
    lua_pushstring( L, LUA_BASE_ENTITY_FACTORY );
    lua_setfield( L, -2, "Factory" );
    lua_setglobal( L, "ENT" );

    if ( luasrc_dofile( L, fullPath ) == 0 )
    {
        lua_getglobal( L, LUA_SCRIPTEDENTITIESLIBNAME );
        if ( lua_istable( L, -1 ) )
        {
            lua_getfield( L, -1, "Register" );
            if ( lua_isfunction( L, -1 ) )
            {
                lua_remove( L, -2 );
                lua_getglobal( L, "ENT" );
                lua_pushstring( L, className );
                luasrc_pcall( L, 2, 0 );
                // Experiment; Commented to instead go ask Lua when we encounter a non-C-registered entities.
//                lua_getglobal( L, "ENT" );
//                if ( lua_istable( L, -1 ) )
//                {
//                    lua_getfield( L, -1, "Factory" );
//                    if ( lua_isstring( L, -1 ) )
//                    {
//                        const char *pszClassname =
//                            lua_tostring( L, -1 );
//                        if ( Q_strcmp( pszClassname,
//                                       "CBaseAnimating" ) == 0 )
//                            RegisterScriptedEntity( className );
//#ifndef CLIENT_DLL
//                        else if ( Q_strcmp( pszClassname,
//                                            "CBaseTrigger" ) == 0 )
//                            RegisterScriptedTrigger( className );
//#endif
//                    }
//                    lua_pop( L, 2 );
//                }
//                else
//                {
//                    lua_pop( L, 1 );
//                }
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

static void LoadEntitiesFromPath( const char *path = 0 )
{
    FileFindHandle_t fh;

    char searchPath[MAX_PATH] = { 0 };

    if ( !path )
    {
        path = LUA_PATH_ENTITIES;
    }

    Q_snprintf( searchPath, sizeof( searchPath ), "%s\\*", path );

    char fileName[MAX_PATH] = { 0 };
    char fullPath[MAX_PATH] = { 0 };
    char className[255] = { 0 };

    char const *fn = g_pFullFileSystem->FindFirstEx( searchPath, CONTENT_SEARCH_PATH, &fh );

    while ( fn )
    {
        if ( fn[0] == '.' )
        {
            fn = g_pFullFileSystem->FindNext( fh );
            continue;
        }

        if ( g_pFullFileSystem->FindIsDirectory( fh ) )
        {
            Q_strcpy( className, fn );

#ifdef CLIENT_DLL
            Q_snprintf( fileName, sizeof( fileName ), "%s\\%s\\cl_init.lua", path, className );
#else
            Q_snprintf( fileName, sizeof( fileName ), "%s\\%s\\init.lua", path, className );
#endif
            if ( !filesystem->FileExists( fileName, CONTENT_SEARCH_PATH ) )
            {
                // TODO: Move this to the gmod compatibility module
                // GMOD compatibility: Fallback to shared.lua if (cl_)init.lua doesn't exist
                Q_snprintf( fileName, sizeof( fileName ), "%s\\%s\\shared.lua", path, className );

                if ( !filesystem->FileExists( fileName, CONTENT_SEARCH_PATH ) )
                {
                    fn = g_pFullFileSystem->FindNext( fh );
                    continue;
                }
            }

            filesystem->RelativePathToFullPath(
                fileName, CONTENT_SEARCH_PATH, fullPath, sizeof( fullPath ) );

            luasrc_LoadEntityFromFile( fullPath, className );
        }
        else
        {
            Q_StripExtension( fn, className, sizeof( className ) );

            Q_snprintf( fileName, sizeof( fileName ), "%s\\%s", path, fn );

            filesystem->RelativePathToFullPath(
                fileName, CONTENT_SEARCH_PATH, fullPath, sizeof( fullPath ) );

            luasrc_LoadEntityFromFile( fullPath, className );
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }
    g_pFullFileSystem->FindClose( fh );
}

/// <summary>
/// Sets up the default values for the SWEP that is on the stack of L.
///
/// TODO: Copied from Garry's Mod Wiki, whilst not all values are used by
/// us. Copied for compatibility reasons.
/// TODO: Match with what CExperimentScriptedWeapon::InitScriptedWeapon expects
/// </summary>
static void luasrc_SetupDefaultWeapon( const char *className )
{
    lua_pushstring( L, "Other" );
    lua_setfield( L, -2, "Category" );

    lua_pushboolean( L, false );
    lua_setfield( L, -2, "Spawnable" );

    lua_pushboolean( L, false );
    lua_setfield( L, -2, "AdminOnly" );

    lua_pushstring( L, "Scripted Weapon" );
    lua_setfield( L, -2, "PrintName" );

    lua_pushstring( L, LUA_BASE_WEAPON );
    lua_setfield( L, -2, "Base" );

    lua_pushnumber( L, 1 );
    lua_setfield( L, -2, "m_WeaponDeploySpeed" );

    lua_pushstring( L, "" );
    lua_setfield( L, -2, "Author" );

    lua_pushstring( L, "" );
    lua_setfield( L, -2, "Contact" );

    lua_pushstring( L, "" );
    lua_setfield( L, -2, "Purpose" );

    lua_pushstring( L, "" );
    lua_setfield( L, -2, "Instructions" );

    lua_pushstring( L, "models/weapons/v_pistol.mdl" );
    lua_setfield( L, -2, "ViewModel" );

    lua_pushboolean( L, false );
    lua_setfield( L, -2, "ViewModelFlip" );

    lua_pushboolean( L, false );
    lua_setfield( L, -2, "ViewModelFlip1" );

    lua_pushboolean( L, false );
    lua_setfield( L, -2, "ViewModelFlip2" );

    lua_pushnumber( L, 62 );
    lua_setfield( L, -2, "ViewModelFOV" );

    lua_pushstring( L, "models/weapons/w_357.mdl" );
    lua_setfield( L, -2, "WorldModel" );

    lua_pushboolean( L, true );
    lua_setfield( L, -2, "AutoSwitchFrom" );

    lua_pushboolean( L, true );
    lua_setfield( L, -2, "AutoSwitchTo" );

    lua_pushnumber( L, 5 );
    lua_setfield( L, -2, "Weight" );

    lua_pushnumber( L, 1 );
    lua_setfield( L, -2, "BobScale" );

    lua_pushnumber( L, 1 );
    lua_setfield( L, -2, "SwayScale" );

    lua_pushboolean( L, true );
    lua_setfield( L, -2, "BounceWeaponIcon" );

    lua_pushboolean( L, true );
    lua_setfield( L, -2, "DrawWeaponInfoBox" );

    lua_pushboolean( L, true );
    lua_setfield( L, -2, "DrawAmmo" );

    lua_pushboolean( L, true );
    lua_setfield( L, -2, "DrawCrosshair" );

    lua_pushnumber( L, 0 );
    lua_setfield( L, -2, "Slot" );

    lua_pushnumber( L, 10 );
    lua_setfield( L, -2, "SlotPos" );

    lua_pushnumber( L, 0 );
    lua_setfield( L, -2, "SpeechBubbleLid" );

    lua_pushnumber( L, 0 );
    lua_setfield( L, -2, "WepSelectIcon" );

    lua_pushboolean( L, false );
    lua_setfield( L, -2, "CSMuzzleFlashes" );

    lua_pushboolean( L, false );
    lua_setfield( L, -2, "CSMuzzleX" );

    lua_newtable( L );
    lua_pushstring( L, "Pistol" );
    lua_setfield( L, -2, "Ammo" );
    lua_pushnumber( L, 0 );
    lua_setfield( L, -2, "ClipSize" );
    lua_pushnumber( L, 0 );
    lua_setfield( L, -2, "DefaultClip" );
    lua_pushboolean( L, false );
    lua_setfield( L, -2, "Automatic" );
    lua_setfield( L, -2, "Primary" );

    lua_newtable( L );
    lua_pushstring( L, "Pistol" );
    lua_setfield( L, -2, "Ammo" );
    lua_pushnumber( L, 0 );
    lua_setfield( L, -2, "ClipSize" );
    lua_pushnumber( L, 0 );
    lua_setfield( L, -2, "DefaultClip" );
    lua_pushboolean( L, false );
    lua_setfield( L, -2, "Automatic" );
    lua_setfield( L, -2, "Secondary" );

    lua_pushboolean( L, false );
    lua_setfield( L, -2, "UseHands" );

    lua_pushboolean( L, false );
    lua_setfield( L, -2, "AccurateCrosshair" );

    lua_pushboolean( L, true );
    lua_setfield( L, -2, "DisableDuplicator" );

    lua_pushstring( L, "weapon" );
    lua_setfield( L, -2, "ScriptedEntityType" );

    lua_pushboolean( L, true );
    lua_setfield( L, -2, "m_bPlayPickupSound" );

    lua_pushfstring( L, "materials/entities/%s.png", className );
    lua_setfield( L, -2, "IconOverride" );
}

static void luasrc_LoadWeaponFromFile( char *fullPath, char *className )
{
    Q_strlower( className );

    lua_newtable( L );
    char entDir[MAX_PATH];
    Q_snprintf( entDir, sizeof( entDir ), "weapons\\%s", className );

    lua_pushstring( L, entDir );
    lua_setfield( L, -2, "Folder" );
    lua_pushstring( L, className );
    lua_setfield( L, -2, "ClassName" );
    luasrc_SetupDefaultWeapon( className );
    lua_setglobal( L, "SWEP" );

    if ( luasrc_dofile( L, fullPath ) == 0 )
    {
        lua_getglobal( L, LUA_SCRIPTEDWEAPONSLIBNAME );

        if ( lua_istable( L, -1 ) )
        {
            lua_getfield( L, -1, "Register" );

            if ( lua_isfunction( L, -1 ) )
            {
                lua_remove( L, -2 );
                lua_getglobal( L, "SWEP" );
                lua_pushstring( L, className );
                luasrc_pcall( L, 2, 0 );
                // RegisterScriptedWeapon( className );
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
}

static void LoadWeaponsFromPath( const char *path = 0 )
{
    FileFindHandle_t fh;

    char searchPath[MAX_PATH] = { 0 };

    if ( !path )
    {
        path = LUA_PATH_WEAPONS;
    }

    Q_snprintf( searchPath, sizeof( searchPath ), "%s\\*", path );

    char fileName[MAX_PATH] = { 0 };
    char fullPath[MAX_PATH] = { 0 };
    char className[255] = { 0 };

    char const *fn = g_pFullFileSystem->FindFirstEx( searchPath, CONTENT_SEARCH_PATH, &fh );

    while ( fn )
    {
        if ( fn[0] == '.' )
        {
            fn = g_pFullFileSystem->FindNext( fh );
            continue;
        }

        if ( g_pFullFileSystem->FindIsDirectory( fh ) )
        {
            Q_strcpy( className, fn );

#ifdef CLIENT_DLL
            Q_snprintf( fileName, sizeof( fileName ), "%s\\%s\\cl_init.lua", path, className );
#else
            Q_snprintf( fileName, sizeof( fileName ), "%s\\%s\\init.lua", path, className );
#endif

            if ( !filesystem->FileExists( fileName, CONTENT_SEARCH_PATH ) )
            {
                // TODO: Move this to the gmod compatibility module
                // GMOD compatibility: Fallback to shared.lua if (cl_)init.lua doesn't exist
                Q_snprintf( fileName, sizeof( fileName ), "%s\\%s\\shared.lua", path, className );

                if ( !filesystem->FileExists( fileName, CONTENT_SEARCH_PATH ) )
                {
                    fn = g_pFullFileSystem->FindNext( fh );
                    continue;
                }
            }

            filesystem->RelativePathToFullPath(
                fileName, CONTENT_SEARCH_PATH, fullPath, sizeof( fullPath ) );

            luasrc_LoadWeaponFromFile( fullPath, className );
        }
        else
        {
            Q_StripExtension( fn, className, sizeof( className ) );

            Q_snprintf( fileName, sizeof( fileName ), "%s\\%s", path, fn );

            filesystem->RelativePathToFullPath(
                fileName, CONTENT_SEARCH_PATH, fullPath, sizeof( fullPath ) );

            luasrc_LoadWeaponFromFile( fullPath, className );
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }

    g_pFullFileSystem->FindClose( fh );
}

static int luasrc_LoadEntities( lua_State *L )
{
    LoadEntitiesFromPath( luaL_checkstring( L, -1 ) );
    return 0;
}

static int luasrc_LoadWeapons( lua_State *L )
{
    LoadWeaponsFromPath( luaL_checkstring( L, -1 ) );
    return 0;
}

/// <summary>
/// Sets the InheritsFrom field in the GM table. After the gamemode is loaded,
/// it will be read and the base gamemode will be loaded.
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
static int luasrc_InheritGamemode( lua_State *L )
{
    const char *baseGamemodeName = luaL_checkstring( L, 1 );
    lua_pop( L, 1 );  // Pop the base gamemode name

    lua_getglobal( L, "GM" );
    lua_pushstring( L, baseGamemodeName );
    lua_setfield( L, -2, "InheritsFrom" );

    // Leave the GM table on the stack so it isn't lost while we load this derived gamemode
    luasrc_LoadGamemode( baseGamemodeName );  // Overwrites the GM global table

    // Now that the base gamemode has loaded, restore the GM table we left on the stack as GM
    lua_setglobal( L, "GM" );

    return 0;
}

static const luaL_Reg base_funcs[] = {
    { "print", luasrc_print },
    { "PrintMessage", luasrc_PrintMessage },
    { "PrintMessageLine", luasrc_PrintMessageLine },
    { "type", luasrc_type },
    { "Include", luasrc_include },
    { "RunString", luasrc_RunString },
    { "InheritGamemode", luasrc_InheritGamemode },
    { "LuaLogToFile", luasrc_LuaLogToFile },
    { NULL, NULL } };

static CUtlStringMap< HMODULE > g_ModulesToUnload;

/// <summary>
/// Loads the specified module and calls the experiment_open function.
/// Has compatibility with GMod 13 modules and will look for gmod13_open
/// if experiment_open is not found.
/// </summary>
/// <param name="L"></param>
static int LoadAndInitModule( lua_State *L )
{
#ifdef _WIN32
    const char *moduleName = luaL_checkstring( L, 1 );
    const char *modulePath = luaL_checkstring( L, 2 );

    lua_settop( L, 0 );  // Clear the stack

    HMODULE hModule = LoadLibrary( modulePath );

    if ( !hModule )
    {
        DevWarning( "Failed to load module: %s\n", modulePath );
        return 0;
    }

    module_open_func open = ( module_open_func )GetProcAddress( hModule, "experiment_open" );
    int nReturnValues = 0;

    if ( open )
    {
        nReturnValues = open( L );
        DevMsg( "Loaded Experiment binary module: %s (%s)\n", moduleName, modulePath );
    }
    else
    {
        module_open_func_compat open = ( module_open_func_compat )GetProcAddress( hModule, "gmod13_open" );

        if ( !open )
        {
            DevWarning( "Failed to find experiment_open (or gmod13_open) in module: %s\n", modulePath );
            FreeLibrary( hModule );
            return 0;
        }

        // Create a compatibility Lua state for loading modules
        lua_StateWithCompat *LCompat = CreateLuaStateWithCompat( L );

        nReturnValues = open( LCompat );

        DevMsg( "Loaded GMOD13 binary module: %s (%s)\n", moduleName, modulePath );

        free( LCompat );  // can be freed as the CLuaBase remains. Compat will be recreated by PushCFunction
    }

    g_ModulesToUnload[modulePath] = hModule;
#else
    DevWarning( "Module loading is not yet supported on this platform.\n" );
#endif

    return nReturnValues;
}

static int luasrc_UnloadLoadedModules( lua_State *L )
{
    int nCount = g_ModulesToUnload.GetNumStrings();
    for ( int i = 0; i < nCount; ++i )
    {
        const char *modulePath = g_ModulesToUnload.String( i );
        HMODULE hModule = g_ModulesToUnload[i];

        module_close_func close = ( module_close_func )GetProcAddress( hModule, "experiment_close" );

        if ( close )
        {
            close( L );
            FreeLibrary( hModule );
        }
        else
        {
            module_close_func_compat close = ( module_close_func_compat )GetProcAddress( hModule, "gmod13_close" );

            // Create a compatibility Lua state for loading modules
            lua_StateWithCompat *LCompat = CreateLuaStateWithCompat( L );

            if ( close )
            {
                close( LCompat );
                FreeLibrary( hModule );

                DevMsg( "Unloaded GMOD13 binary module: %s\n", modulePath );
            }
            else
            {
                DevWarning( "Failed to find experiment_close (or gmod13_close) in module: %s\n", modulePath );
                FreeLibrary( hModule );
                free( LCompat );
                continue;
            }

            free( LCompat );  // can be freed as the CLuaBase remains. Compat will be recreated by PushCFunction
        }

        lua_settop( L, 0 );  // Reset the stack if close left anything
    }

    g_ModulesToUnload.Purge();
    return 0;
}

/// <summary>
/// Loader function to be added to the package.loaders table. This will
/// load LUA_BINARY_MODULES_GLOB files from the scripts/lua/bin directory.
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
static int luasrc_CustomModuleLoader( lua_State *L )
{
    const char *moduleName = luaL_checkstring( L, 1 );

    lua_settop( L, 0 );  // Clear the stack

    char modulePath[MAX_PATH];
    Q_snprintf( modulePath, sizeof( modulePath ), "%s\\" LUA_BINARY_MODULES_GLOB, LUA_PATH_BINARY_MODULES, moduleName );

    FileFindHandle_t findHandle;
    const char *fileName = filesystem->FindFirst( modulePath, &findHandle );

    while ( fileName )
    {
        if ( !filesystem->FindIsDirectory( findHandle ) )
        {
            Q_snprintf( modulePath, sizeof( modulePath ), "%s\\%s", LUA_PATH_BINARY_MODULES, fileName );

            filesystem->RelativePathToFullPath( modulePath, "GAME", modulePath, sizeof( modulePath ) );
            break;
        }

        fileName = filesystem->FindNext( findHandle );
    }

    filesystem->FindClose( findHandle );

    if ( !filesystem->FileExists( modulePath ) )
    {
        char error[MAX_PATH + 128];
        Q_snprintf( error, sizeof( error ), "no file: '(Experiment Game Directory)\\%s' (* can be anything like nothing, or 'gm')", modulePath );
        lua_pushstring( L, error );
        return 1;
    }

    lua_pushcfunction( L, LoadAndInitModule );
    lua_pushstring( L, modulePath );
    return 2;
}

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

    // Put luasocket into package.preload
    lua_pushglobaltable( L );
    lua_getfield( L, -1, "package" );
    lua_getfield( L, -1, "preload" );
    lua_pushcfunction( L, luaopen_socket_core );
    lua_setfield( L, -2, "luasocket" );
    lua_pushcfunction( L, luaopen_mime_core );
    lua_setfield( L, -2, "luasocket.mime" );

    lua_pop( L, 3 );  // Pop: preload field, package lib, global table
}

void luasrc_setmodulepaths( lua_State *L )
{
#ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
#else
    char gamePath[MAX_PATH];
    engine->GetGameDir( gamePath, MAX_PATH );
#endif

    // Expose the game path to Lua, so scripts can easily add paths relative to it
    lua_pushstring( L, gamePath );
    lua_setglobal( L, "GAME_PATH" );

    // Get the package table
    lua_getglobal( L, LUA_LOADLIBNAME );

    // Set the package.path by letting Lua format and set it
    lua_getfield( L, -1, "path" );

    // Get the current path and append the module paths to it
    const char *currentPath = lua_tostring( L, -1 );
    lua_pop( L, 1 );  // Pop the path string
    lua_pushfstring( L,
                     "%s\\%s\\?.lua;%s",
                     gamePath,
                     LUA_PATH_MODULES,
                     currentPath );
    lua_setfield( L, -2, "path" );

    // Set the package.cpath the same way
    lua_getfield( L, -1, "cpath" );

    const char *currentCPath = lua_tostring( L, -1 );
    lua_pop( L, 1 );  // Pop the cpath string
    lua_pushfstring( L,
#ifdef _WIN32
                     "%s\\%s\\?.dll;%s",
#elif _LINUX
                     "%s\\%s\\?.so;%s",
#endif
                     gamePath,
                     LUA_PATH_BINARY_MODULES,
                     currentCPath );
    lua_setfield( L, -2, "cpath" );

    // TODO: Repeat the above for all mounted games (replacing gamePath)

    // Experiment; we also abuse the package table to set a 'include' path
    // for the 'include' function. These paths will be attempted to be prefixed
    // before X.lua when include("X.lua") is called.
    lua_pushfstring( L,
                     ".\\;%s\\;%s",
                     LUA_ROOT,
                     LUA_PATH_GAMEMODES );
    lua_setfield( L, -2, "IncludePath" );

    // setup luasrc_CustomModuleLoader as a cpath loader for LUA_PATH_BINARY_MODULES
    lua_getfield( L, -1, "searchers" );
    lua_pushcfunction( L, luasrc_CustomModuleLoader );
    // Place it at the end for now
    lua_rawseti( L, -2, luaL_len( L, -2 ) + 1 );
    lua_pop( L, 1 );  // Pop the searchers table

    lua_pop( L, 1 );  // Pop the package table
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
    lua_setglobal( LGameUI, "GAMEUI" ); /* set global GAMEUI */

    luasrc_setmodulepaths( LGameUI );

    luasrc_openlibs( LGameUI, true );
    luasrc_dofile( LGameUI, LUA_PATH_INCLUDES_INIT_FILE );

    Msg( "Lua Menu initialized (" LUA_VERSION ")\n" );
}

void luasrc_shutdown_gameui( void )
{
    ResetGameUIConCommandDatabase();

    lua_close( LGameUI );
    LGameUI = nullptr;
}
#endif

/// <summary>
/// Called after the sh_init.lua has loaded all base modules and extensions into
/// the Lua environment.
/// This will setup further functions into those libraries.
/// </summary>
/// <param name="L"></param>
static void luasrc_init_post_includes( lua_State *L )
{
    lua_getglobal( L, LUA_SCRIPTEDENTITIESLIBNAME );
    lua_pushcfunction( L, luasrc_LoadEntities );
    lua_setfield( L, -2, "LoadFromDirectory" );
    lua_pop( L, 1 );  // Pop the entities library

    lua_getglobal( L, LUA_SCRIPTEDWEAPONSLIBNAME );
    lua_pushcfunction( L, luasrc_LoadWeapons );
    lua_setfield( L, -2, "LoadFromDirectory" );
    lua_pop( L, 1 );  // Pop the weapons library
}

void luasrc_init( void )
{
    if ( s_LuaLogFileHandle == FILESYSTEM_INVALID_HANDLE )
    {
#ifdef CLIENT_DLL
        const char *logFilePath = "lua_log_cl.log";
#else
        const char *logFilePath = "lua_log_sv.log";
#endif
        s_LuaLogFileHandle = filesystem->Open( logFilePath, "a", "GAME" );
    }

    if ( g_bLuaInitialized )
        return;

    g_bLuaInitialized = true;

    L = lua_open();
    s_pLuaNetworkGroupHandler = new CLuaNetworkGroupHandler( L );
    g_pNetworkSystem->RegisterGroupHandler( NETWORK_MESSAGE_GROUP::SCRIPT, s_pLuaNetworkGroupHandler );

#ifdef CLIENT_DLL
    g_pClientLuaPanel = new CScriptedClientLuaPanel( L );
    g_pClientLuaPanel->Start( gameuifuncs, gameeventmanager );

    g_pClientLuaPanelHUD = new CScriptedClientLuaPanel( L );
    g_pClientLuaPanelHUD->Start( gameuifuncs, gameeventmanager );

    luasrc_ui_enable();
#endif

    luaL_openlibs( L );
    base_open( L );
    lcf_open( L );

    // Andrew; Someone set us up the path for great justice
    luasrc_setmodulepaths( L );

    luasrc_openlibs( L );

    RegisterLuaUserMessages();

    InitializeLuaGameEventHandler( L );

    Msg( "Lua initialized (" LUA_VERSION ")\n" );

    SetupMountedAddons( L );

#ifdef CLIENT_DLL
    if ( gpGlobals->maxClients > 1 )
    {
        luasrc_dofolder( L, LUA_PATH_CACHE LUA_PATH_AUTO_LOAD_SHARED );
        luasrc_dofolder( L, LUA_PATH_CACHE LUA_PATH_AUTO_LOAD_CLIENT );
    }

    luasrc_dofile( L, LUA_PATH_INCLUDES_INIT_FILE );

    luasrc_init_post_includes( L );

    luasrc_dofolder( L, LUA_PATH_AUTO_LOAD_SHARED );
    luasrc_dofolder( L, LUA_PATH_AUTO_LOAD_CLIENT );

    LoadWeaponsFromPath();
    LoadEntitiesFromPath();
    // LoadEffectsFromPath();

    luasrc_LoadGamemode( LUA_BASE_GAMEMODE );
    luasrc_LoadGamemode( gamemode.GetString() );
    luasrc_SetGamemode( gamemode.GetString() );

#else   // CLIENT_DLL
    luasrc_dofile( L, LUA_PATH_INCLUDES_INIT_FILE );

    luasrc_init_post_includes( L );

    luasrc_dofolder( L, LUA_PATH_AUTO_LOAD_SHARED );
    luasrc_dofolder( L, LUA_PATH_AUTO_LOAD_SERVER );

    LoadWeaponsFromPath();
    LoadEntitiesFromPath();
    // LoadEffectsFromPath();

    luasrc_LoadGamemode( LUA_BASE_GAMEMODE );
    g_bGamemodeLoaded = luasrc_LoadGamemode( gamemode.GetString() );

    if ( !g_bGamemodeLoaded )
    {
        // Don't waste time setting up further, we will kick the host client in ClientConnect
        return;
    }

    luasrc_SetGamemode( gamemode.GetString() );

    if ( gpGlobals->maxClients > 1 )
    {
        // Adds files (like models, materials, sounds, etc) to the downloadables table
        // to be sent to clients when they connect.
        INetworkStringTable *downloadables =
            networkstringtable->FindTable( "downloadables" );
        int resourceFileCount = ResourcesGetFilesCount();

        for ( int i = 0; i < resourceFileCount; i++ )
        {
            const char *resourceFile = ResourcesGetFile( i );
            downloadables->AddString( true, resourceFile, -1 );
        }

        ResourcesFreeFiles();

        // Load the Lua cache files (zipped) into the downloadables table
        lcf_preparecachefile();
    }
#endif  // else CLIENT_DLL
}

#ifdef CLIENT_DLL
void luasrc_ui_enable( void )
{
    if ( !g_pClientLuaPanel )
        return;

    vgui::VPANEL pRoot = enginevgui->GetPanel( PANEL_ROOT );
    g_pClientLuaPanel->SetParent( pRoot );
    g_pClientLuaPanel->SetProportional( false );
    // g_pClientLuaPanel->SetCursor( vgui::dc_none );
    g_pClientLuaPanel->SetVisible( true );

    if ( g_pClientLuaPanel->IsKeyBoardInputEnabled() )
    {
        g_pClientLuaPanel->RequestFocus();
    }

    pRoot = VGui_GetClientDLLRootPanel();
    g_pClientLuaPanelHUD->SetParent( pRoot );
    g_pClientLuaPanelHUD->SetProportional( false );
    g_pClientLuaPanelHUD->SetCursor( vgui::dc_none );
    g_pClientLuaPanelHUD->SetVisible( true );

    int wide, tall;
    vgui::ipanel()->GetSize( pRoot, wide, tall );
    luasrc_ui_layout( wide, tall );
}

void luasrc_ui_layout( int wide, int tall )
{
    if ( !g_pClientLuaPanel )
        return;

    g_pClientLuaPanel->SetBounds( 0, 0, wide, tall );
    g_pClientLuaPanelHUD->SetBounds( 0, 0, wide, tall );
}

void luasrc_ui_disable( void )
{
    if ( !g_pClientLuaPanel )
        return;

    g_pClientLuaPanel->SetParent( ( vgui::VPANEL )NULL );
    g_pClientLuaPanel->SetVisible( false );
}
#endif

void luasrc_shutdown( void )
{
#ifdef CLIENT_DLL
    delete g_pClientLuaPanel;
    g_pClientLuaPanel = NULL;

    delete g_pClientLuaPanelHUD;
    g_pClientLuaPanelHUD = NULL;
#endif

    if ( s_LuaLogFileHandle != FILESYSTEM_INVALID_HANDLE )
    {
        filesystem->Close( s_LuaLogFileHandle );
        s_LuaLogFileHandle = FILESYSTEM_INVALID_HANDLE;
    }

    if ( !g_bLuaInitialized )
        return;

    LUA_CALL_HOOK_BEGIN( "ShutDown" );
    LUA_CALL_HOOK_END( 0, 0 );

#ifdef CLIENT_DLL
    Msg( "Lua shutdown - Client\n" );
#else
    Msg( "Lua shutdown - Server\n" );
#endif

    ShutdownLuaGameEventHandler( L );

    luasrc_UnloadLoadedModules( L );

    g_pNetworkSystem->UnregisterNetworkHandler( NETWORK_MESSAGE_GROUP::SCRIPT, s_pLuaNetworkGroupHandler );
    delete s_pLuaNetworkGroupHandler;

    g_bLuaInitialized = false;

    // Remove the content searchpath for any load gamemode
    lua_getglobal( L, "GAMEMODE" );

    if ( lua_istable( L, -1 ) )
    {
        lua_getfield( L, -1, "Folder" );
        const char *gamemodeFolder = lua_tostring( L, -1 );
        lua_pop( L, 1 );  // Remove the folder name

        char contentSearchPath[MAX_PATH];
        Q_strcpy( contentSearchPath, gamemodeFolder );
        Q_strncat( contentSearchPath, "\\content", sizeof( contentSearchPath ) );

        RemoveRootSearchPaths( contentSearchPath, L );
    }

    lua_pop( L, 1 );  // Remove the GAMEMODE/non-table value

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
    L = nullptr;

    if ( hasLoaderBeenActivated )
    {
        lua_close( loaderLuaState );
        hasLoaderBeenActivated = false;
    }

    ActivityList_Free();
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
/// Provides a useful stack traceback for errors that occur.
/// It is called before the stack is unwound by the error.
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
static int luasrc_traceback( lua_State *L )
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
/// Uses the provided file path (relative) to find the file. Uses all search
/// paths to find the file.
/// </summary>
static bool luasrc_find_file( lua_State *L, const char *fileName, char *fullPath, size_t fullPathSize )
{
    // If the find is already an absolute path, just copy it
    if ( Q_IsAbsolutePath( fileName ) )
    {
        Q_strncpy( fullPath, fileName, fullPathSize );
        return true;
    }

    // Check relative to the current file being loaded if it is set
    if ( !fileIncludingStack.empty() )
    {
        const char *currentFile = fileIncludingStack.top().c_str();
        char currentFileDir[MAX_PATH];
        Q_ExtractFilePath( currentFile, currentFileDir, sizeof( currentFileDir ) );
        char relativeFileName[MAX_PATH];
        Q_snprintf( relativeFileName, sizeof( relativeFileName ), "%s\\%s", currentFileDir, fileName );

        if ( filesystem->FileExists( relativeFileName, CONTENT_SEARCH_PATH ) )
        {
            Q_strncpy( fullPath, relativeFileName, fullPathSize );
            return true;
        }
    }

    // Next search relative to where the Lua script is
    // TODO: This fails when called from the console with lua_run include("file.lua")
    // because the console is not a file and it would try load 'nclude("file.lua")'
    lua_Debug ar1;

    if ( lua_getstack( L, 1, &ar1 ) != 0 )
    {
        lua_getinfo( L, "f", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        int iLength = Q_strlen( ar2.source );
        char source[MAX_PATH];
        Q_StrRight( ar2.source, iLength - 1, source, sizeof( source ) );
        Q_StripFilename( source );
        char relativeFileName[MAX_PATH];
        Q_snprintf( relativeFileName, sizeof( relativeFileName ), "%s\\%s", source, fileName );

        if ( filesystem->FileExists( relativeFileName, CONTENT_SEARCH_PATH ) )
        {
            Q_strncpy( fullPath, relativeFileName, fullPathSize );
            return true;
        }
    }

    // Otherwise, search for the file in all search paths

    // Get all IncludePath search paths from the package table
    lua_getglobal( L, LUA_LOADLIBNAME );
    lua_getfield( L, -1, "IncludePath" );
    const char *includePathsRaw = lua_tostring( L, -1 );
    lua_pop( L, 2 );  // Pop the IncludePath string and the package table

    // Make a copy of the includePaths because strtok modifies the string
    char *includePaths = strdup( includePathsRaw );

    char *includeContext;
    char *includeToken = strtok_s( includePaths, ";", &includeContext );

    while ( includeToken != NULL )
    {
        // Go through all include paths that are configured in package.IncludePath
        char includePath[MAX_PATH];
        Q_snprintf( includePath, sizeof( includePath ), "%s\\%s", includeToken, fileName );
        V_FixDoubleSlashes( includePath );
        Q_RemoveDotSlashes( includePath );

        // Trim .\\ from the beginning of the path
        if ( Q_strncmp( includePath, ".\\", 2 ) == 0 )
        {
            Q_strncpy( includePath, includePath + 2, sizeof( includePath ) - 2 );
        }

        char searchPath[MAX_PATH * 25];  // TODO: How do we know how many paths there are? This is a guess.
        filesystem->GetSearchPath_safe( "GAME", true, searchPath );

        char *searchContext;
        char *token = strtok_s( searchPath, ";", &searchContext );
        while ( token != NULL )
        {
            char path[MAX_PATH];
            Q_snprintf( path, sizeof( path ), "%s\\%s", token, includePath );
            V_FixDoubleSlashes( path );
            Q_FixSlashes( path );

            if ( filesystem->FileExists( path, CONTENT_SEARCH_PATH ) )
            {
                Q_strncpy( fullPath, path, fullPathSize );
                return true;
            }

            token = strtok_s( NULL, ";", &searchContext );
        }

        includeToken = strtok_s( NULL, ";", &includeContext );
    }

    free( includePaths );

    // If the file was not found, just copy the file name
    Q_strncpy( fullPath, fileName, fullPathSize );
    return false;
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
    char fullPath[MAX_PATH];
    luasrc_find_file( L, filePath, fullPath, sizeof( fullPath ) );

    // Useful for tracking relative includes
    fileIncludingStack.push( fullPath );

    // If the loader is not activated, default to simple dofile loading
    if ( !hasLoaderBeenActivated )
    {
        lua_pushcfunction( L, luasrc_traceback );
        int traceBackPosition = lua_gettop( L );

        // Load the file, leaving the chunk on the stack. Then pcall it with
        // the traceback function (which will be below the chunk on the stack)
        int result = ( luaL_loadfile( L, fullPath ) || lua_pcall( L, 0, LUA_MULTRET, traceBackPosition ) );

        lua_remove( L, traceBackPosition );

        fileIncludingStack.pop();

        return result;
    }

    // Handle the file manually, preprocessing it with the loader

    // Read the file contents
    CUtlBuffer fileContentsBuffer( 0, 0, CUtlBuffer::TEXT_BUFFER );
    if ( !filesystem->ReadFile( fullPath, CONTENT_SEARCH_PATH, fileContentsBuffer ) )
    {
        lua_pushfstring( L, "Failed to read file: %s", fullPath );
        fileIncludingStack.pop();
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

        SHOW_LUA_ERROR( "Loader Error: Failed to preprocess file %s because of error: '%s' (returning file unmodified!)\n", fullPath, error );

        int result = luaL_loadbuffer( L, fileContents, fileSize, bufferFileName );

        fileIncludingStack.pop();

        return result;
    }

    if ( !lua_isstring( loaderLuaState, -1 ) )
    {
        lua_pop( loaderLuaState, 1 );  // Pop the invalid preprocessed file contents

        SHOW_LUA_ERROR( "Loader Error: PreProcessFile did not return a string for file %s (returning file unmodified!)\n", fullPath );

        int result = luaL_loadbuffer( L, fileContents, fileSize, bufferFileName );

        fileIncludingStack.pop();

        return result;
    }

    const char *preprocessedFileContents = lua_tostring( loaderLuaState, -1 );
    CUtlBuffer buffer( 0, strlen( preprocessedFileContents ) * sizeof( char ), CUtlBuffer::TEXT_BUFFER );
    lua_pop( loaderLuaState, 1 );  // Pop the preprocessed file contents now that we have them

    if ( lua_log_loader.GetBool() )
    {
        // Write the file to the logs folder
        char relativePath[MAX_PATH];

        // Go through all search paths and find one that this file is in. Then trim that path from the file name.
        char searchPath[MAX_PATH * 25];  // TODO: How do we know how many paths there are? This is a guess.
        filesystem->GetSearchPath_safe( "GAME", true, searchPath );

        char *searchContext;
        char *token = strtok_s( searchPath, ";", &searchContext );
        while ( token != NULL )
        {
            if ( Q_strncmp( fullPath, token, Q_strlen( token ) ) == 0 )
            {
                Q_strncpy( relativePath, fullPath + Q_strlen( token ), sizeof( relativePath ) );
                break;
            }

            token = strtok_s( NULL, ";", &searchContext );
        }

        if ( relativePath[0] == '\0' )
        {
            DevWarning( "Failed to find include anywhere in mounted paths. Alert a dev!" );
            Q_strncmp( relativePath, fullPath, Q_strlen( "?:/" ) );  // trim any drive letter
        }

        char fileLogPath[MAX_PATH];
        Q_snprintf( fileLogPath, sizeof( fileLogPath ), "logs\\_loader\\%s", relativePath );
        V_RemoveDotSlashes( fileLogPath );

        char fileLogPathDirectory[MAX_PATH];
        Q_ExtractFilePath( fileLogPath, fileLogPathDirectory, sizeof( fileLogPathDirectory ) );
        filesystem->CreateDirHierarchy( fileLogPathDirectory, "GAME" );

        buffer.PutString( preprocessedFileContents );
        filesystem->WriteFile( fileLogPath, "GAME", buffer );
    }

    lua_pushcfunction( L, luasrc_traceback );
    int traceBackPosition = lua_gettop( L );

    // Load the preprocessed file contents as a Lua chunk
    // and execute it with the traceback function below it
    int errorCode = luaL_loadbuffer(
                        L,
                        preprocessedFileContents,
                        strlen( preprocessedFileContents ),
                        bufferFileName ) ||
                    lua_pcall( L, 0, LUA_MULTRET, traceBackPosition );

    lua_remove( L, traceBackPosition );

    fileIncludingStack.pop();

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
        SHOW_LUA_ERROR( "error loading file '%s': %s\n", fileName, lua_tostring( L, -1 ) );
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
        SHOW_LUA_ERROR( "error loading file '%s': %s\n", fileName, lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
        return -1;
    }

    return lua_gettop( L ) - stackBefore;
}

bool luasrc_checkfolder( const char *path )
{
    return g_pFullFileSystem->IsDirectory( path, CONTENT_SEARCH_PATH );
}

LUA_API void luasrc_dofolder( lua_State *L, const char *path )
{
    if ( !luasrc_checkfolder( path ) )
    {
        DevMsg( "Lua dofolder failed! Folder does not exist: %s\n", path );
        return;
    }

    FileFindHandle_t fh;

    char searchPath[MAX_PATH];
    Q_snprintf( searchPath, sizeof( searchPath ), "%s\\*.lua", path );

    char const *fn = g_pFullFileSystem->FindFirstEx( searchPath, CONTENT_SEARCH_PATH, &fh );

    while ( fn )
    {
        if ( fn[0] != '.' )
        {
            char ext[10];
            Q_ExtractFileExtension( fn, ext, sizeof( ext ) );

            if ( !Q_stricmp( ext, "lua" ) )
            {
                char relative[MAX_PATH];
                char loadname[MAX_PATH];
                Q_snprintf( relative, sizeof( relative ), "%s\\%s", path, fn );
                filesystem->RelativePathToFullPath( relative, CONTENT_SEARCH_PATH, loadname, sizeof( loadname ) );
                luasrc_dofile( L, loadname );
            }
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }
    g_pFullFileSystem->FindClose( fh );
}

/// <summary>
/// Calls with error handling, providing own traceback function if none is provided.
/// </summary>
/// <param name="L"></param>
/// <param name="amountOfArguments"></param>
/// <param name="amountOfResults"></param>
/// <param name="errorFunctionStackPos"></param>
/// <returns></returns>
LUA_API int luasrc_pcall( lua_State *L, int amountOfArguments, int amountOfResults, int errorFunctionStackPos )
{
    bool injectedOwnErrorFunction = false;

    if ( errorFunctionStackPos == LUA_DEFAULT_ERROR_HANDLER )
    {
        // insert luasrc_traceback function below everything
        lua_pushcfunction( L, luasrc_traceback );
        lua_insert( L, 1 );
        errorFunctionStackPos = 1;
        injectedOwnErrorFunction = true;
    }

    int iError = lua_pcall( L, amountOfArguments, amountOfResults, errorFunctionStackPos );

    if ( injectedOwnErrorFunction )
    {
        lua_remove( L, errorFunctionStackPos );
    }

    if ( iError != 0 )
    {
        SHOW_LUA_ERROR( "pcall error: %s\n", lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
    }

    return iError;
}

LUA_API void luasrc_dumpstack( lua_State *L )
{
    int n = lua_gettop( L ); /* number of objects */
    int i;
    lua_getglobal( L, "tostring" );

    CUtlString dumpStack = "";

    for ( i = 1; i <= n; i++ )
    {
        const char *s;
        lua_pushvalue( L, -1 ); /* function to be called */
        lua_pushvalue( L, i );  /* value to print */
        lua_call( L, 1, 1 );
        s = lua_tostring( L, -1 ); /* get result */

        dumpStack += i;
        dumpStack += ": ";
        dumpStack += s;
        dumpStack += "\n";

        lua_pop( L, 1 ); /* pop result */
    }
    lua_pop( L, 1 ); /* pop function */

    Msg( "%s", dumpStack.Get() );
}

/// <summary>
/// As soon as a `{gamemode name}.lua` file exists in the scripts/lua/loaders
/// folder for the gamemode being loaded, it and all its dependencies will be
/// loaded using this loader lua state.
/// Each file can be preprocessed by the loader, which can modify the file
/// contents before they are loaded into the main Lua state.
/// </summary>
/// <param name="gamemodePath"></param>
void luasrc_InitCustomLoader( const char *gamemode )
{
    // Check if the scripts/lua/loaders/{gamemode}.lua file exists, if it does,
    // load it into the  loader Lua state
    char loaderFilePath[MAX_PATH];
    Q_snprintf( loaderFilePath, sizeof( loaderFilePath ), LUA_ROOT "\\loaders\\%s.lua", gamemode );

    if ( !filesystem->FileExists( loaderFilePath, CONTENT_SEARCH_PATH ) )
    {
        return;
    }

    filesystem->RelativePathToFullPath( loaderFilePath, CONTENT_SEARCH_PATH, loaderFilePath, sizeof( loaderFilePath ) );

#ifndef CLIENT_DLL
    // Always send the _loader.lua to the client
    luasrc_sendfile( L, loaderFilePath );
#endif

    loaderLuaState = luaL_newstate();

    luaopen_base_minimal( loaderLuaState );

    lua_pushglobaltable( loaderLuaState );

    static const luaL_Reg baseLoaderFuncs[] = {
        { "print", luasrc_print },
        { "Include", luasrc_include },
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
        { LUA_DBLIBNAME, luaopen_debug },
        { LUA_LOADLIBNAME, luaopen_package },
        { NULL, NULL } };

    const luaL_Reg *lib;

    for ( lib = availableLibraries; lib->func; lib++ )
    {
        luaL_requiref( loaderLuaState, lib->name, lib->func, 1 );
        lua_pop( loaderLuaState, 1 );
    }

    lua_pushstring( loaderLuaState, LUA_VERSION );
    lua_setfield( loaderLuaState, -2, "_VERSION" );

    lua_pushboolean( loaderLuaState, 1 );
    lua_setglobal( loaderLuaState, "LOADER" ); /* set global LOADER to mark this environment */

    luasrc_setmodulepaths( loaderLuaState );

    // Remove the global table from the stack
    lua_pop( loaderLuaState, 1 );

    // Execute the loader file, leaving the table at the top of the stack
    if ( luasrc_dofile_leave_stack( loaderLuaState, loaderFilePath ) == -1 || !lua_istable( loaderLuaState, -1 ) )
    {
        lua_close( loaderLuaState );
        SHOW_LUA_ERROR(
            "Gamemode '%s' had invalid loader (%s failed to load)!\n",
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
        SHOW_LUA_ERROR(
            "Gamemode '%s' had invalid loader (%s has no PreProcessFile function)!\n",
            gamemode,
            loaderFilePath );

        return;
    }

    lua_pop( loaderLuaState, 1 );  // Pop the PreProcessFile function for now

    // The loader file was loaded successfully. The PreProcessFile loader
    // function is now at the top of the stack.
    hasLoaderBeenActivated = true;

    SHOW_LUA_MESSAGE( "Activated custom loader for gamemode '%s' (%s)!\n\tUsing custom loader for subsequent file loading in all (derived) gamemodes.\n", gamemode, loaderFilePath );
}

static void cleanUpGamemodeLoading( bool shouldCleanLoader )
{
    // Unset the GM table
    lua_pushnil( L );
    lua_setglobal( L, "GM" );

    if ( hasLoaderBeenActivated && shouldCleanLoader )
    {
        // Clean up the loader if we initialized it
        lua_close( loaderLuaState );
        hasLoaderBeenActivated = false;
    }
}

/// <summary>
/// Append the new path to the existing package.path so that requires/
/// includes can find files in the given search path.
/// </summary>
/// <param name="L"></param>
/// <param name="searchPath"></param>
/// <param name="isPathC"></param>
void luasrc_add_to_package_path( lua_State *L, const char *searchPath, bool isPathC /* = false */ )
{
    const char *pathField = isPathC ? "cpath" : "path";

    lua_getglobal( L, LUA_LOADLIBNAME );
    lua_getfield( L, -1, pathField );

    const char *currentPath = lua_tostring( L, -1 );
    lua_pop( L, 1 );  // Remove the current path from the stack

    lua_pushfstring( L, "%s;%s", currentPath, searchPath );
    lua_setfield( L, -2, pathField );
    lua_pop( L, 1 );  // Remove the package table from the stack
}

/// <summary>
/// Removes the given search path from the package.path so that requires/
/// includes can no longer find files in the given search path.
/// </summary>
/// <param name="L"></param>
/// <param name="searchPath"></param>
/// <param name="isPathC"></param>
void luasrc_remove_from_package_path( lua_State *L, const char *searchPath, bool isPathC /* = false */ )
{
    const char *pathField = isPathC ? "cpath" : "path";

    lua_getglobal( L, LUA_LOADLIBNAME );
    lua_getfield( L, -1, pathField );

    const char *currentPath = lua_tostring( L, -1 );
    lua_pop( L, 1 );  // Remove the current path from the stack

    char newPath[MAX_PATH];
    Q_strncpy( newPath, currentPath, sizeof( newPath ) );

    char *searchPathInCurrentPath = Q_strstr( newPath, searchPath );

    if ( searchPathInCurrentPath )
    {
        // Remove the search path from the current path
        Q_strcpy( searchPathInCurrentPath, searchPathInCurrentPath + Q_strlen( searchPath ) );

        lua_pushstring( L, newPath );
        lua_setfield( L, -2, pathField );
    }

    lua_pop( L, 1 );  // Remove the package table from the stack
}

/// <summary>
/// Loads the gamemode with the given name from the gamemodes folder.
///
/// Calls gamemodes.Register(gamemodeTable, gamemodeName, baseGameMode)
/// after loading all the relevant gamemodes.
/// </summary>
/// <param name="gamemodeName"></param>
/// <returns></returns>
bool luasrc_LoadGamemode( const char *gamemodeName )
{
    DevMsg( "Loading gamemode %s\n", gamemodeName );

    // Start loading the loader for anything that isn't the base gamemode
    if ( Q_strcmp( gamemodeName, LUA_BASE_GAMEMODE ) != 0 && !hasLoaderBeenActivated )
    {
        luasrc_InitCustomLoader( gamemodeName );
    }

    char initialFileName[MAX_PATH];
    char gamemodePath[MAX_PATH];

    if ( !TryFindGamemode( gamemodeName, initialFileName, sizeof( initialFileName ), gamemodePath, sizeof( gamemodePath ) ) )
    {
        cleanUpGamemodeLoading( true );

        SHOW_LUA_ERROR( "Failed to load invalid gamemode %s! Required initial file '%s' does not exist.\n", gamemodeName, initialFileName );

        return false;
    }

    // Make sure lua files can find lua files inside the gamemode folder
    CUtlString gamemodeSearchPath;
    gamemodeSearchPath.Format( "%s?.lua;%sgamemode\\?.lua",
                               gamemodePath,
                               gamemodePath );
    const char *gamemodeSearchPathC = gamemodeSearchPath.String();
    luasrc_add_to_package_path( L, gamemodeSearchPathC );

    // Add the gamemode content folder to the search path
    char contentSearchPath[MAX_PATH];
    Q_strcpy( contentSearchPath, gamemodePath );
    Q_strncat( contentSearchPath, "content", sizeof( contentSearchPath ) );

    SetupRootSearchPaths( contentSearchPath, L );

    // Set the GM table as a global variable
    lua_newtable( L );
    lua_setglobal( L, "GM" );

    // Let scripts know what folder the gamemode is in
    lua_getglobal( L, "GM" );
    lua_pushstring( L, "Folder" );
    lua_pushstring( L, gamemodePath );
    lua_settable( L, -3 );

    // lua_pop( L, 1 );  // We leave the GM table on the stack for now

    // Load the initial file, letting it print errors if it fails
    luasrc_dofile( L, initialFileName );

    // Call gamemodes.Register with this gamemode
    lua_getglobal( L, LUA_GAMEMODESLIBNAME );
    lua_getfield( L, -1, "Register" );
    lua_remove( L, -2 );  // Remove gamemodes now that we have Register

    // Sanity check in case someone messes with the gamemodes module
    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L, 1 );  // Pop Register
        lua_pop( L, 1 );  // Pop the GM table

        SHOW_LUA_ERROR( "Error loading gamemode: %s! The function gamemodes.Register isn't valid.\n", gamemodeName );

        return false;
    }

    // Bring the GM table to the top of the stack
    lua_pushvalue( L, -2 );
    lua_remove( L, -3 );  // Remove the GM table that was below the Register function

    // Get GM.InheritsFrom for the registration
    lua_getfield( L, -1, "InheritsFrom" );

    // (gamemodes.)Register and the original GM table are on the stack
    // Call gamemodes.Register(gamemodeTable, gamemodeName, baseGameMode)
    lua_pushstring( L, gamemodeName );
    lua_insert( L, -2 );  // Move the gamemode name to the 2nd parameter
    luasrc_pcall( L, 3, 0 );

    // Unset the GM table, users should use the GAMEMODE table instead
    // We keep the loader active for any later includes. It will be cleaned up
    // when the Lua state is closed.
    cleanUpGamemodeLoading( false );

    DevMsg( "Loaded gamemode %s\n", gamemodeName );

    return true;
}

bool luasrc_SetGamemode( const char *gamemodeName )
{
    // Even if an include failed, they should still have popped the file
    // from the stack
    assert( fileIncludingStack.empty() );

    lua_getglobal( L, LUA_GAMEMODESLIBNAME );

    if ( !lua_istable( L, -1 ) )
    {
        lua_pop( L, 1 );  // Remove gamemode table
        SHOW_LUA_ERROR( "Failed to set gamemode. The gamemodes module doesn't exist!\n" );
        return false;
    }

    lua_getfield( L, -1, "Get" );

    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L, 2 );  // Remove gamemode table and Get function
        SHOW_LUA_ERROR( "Failed to set gamemode. The gamemodes.Get function doesn't exist!\n" );
        return false;
    }

    lua_remove( L, -2 );                // Remove gamemode table
    lua_pushstring( L, gamemodeName );  // Push gamemode name
    luasrc_pcall( L, 1, 1 );            // Call gamemodes.Get(gamemode)
    lua_setglobal( L, "GAMEMODE" );     // Set GAMEMODE to the active gamemode table

    lua_getglobal( L, LUA_GAMEMODESLIBNAME );
    lua_getfield( L, -1, "InternalSetActiveName" );

    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L,
                 2 );  // Remove gamemode table and InternalSetActiveName function
        SHOW_LUA_ERROR( "Failed to set gamemode. gamemodes.InternalSetActiveName didn't exist!\n" );
        return false;
    }

    lua_remove( L, -2 );                // Remove gamemode table
    lua_pushstring( L, gamemodeName );  // Push gamemode name
    luasrc_pcall( L, 1, 0 );            // Call gamemodes.InternalSetActiveName(gamemode)

    // Get the folder name of the gamemode
    lua_getglobal( L, "GAMEMODE" );
    lua_getfield( L, -1, "Folder" );
    const char *gamemodeFolder = lua_tostring( L, -1 );
    lua_pop( L, 2 );  // Remove the folder name and GAMEMODE table

    char contentSearchPath[MAX_PATH];
    Q_strcpy( contentSearchPath, gamemodeFolder );
    Q_strncat( contentSearchPath, "content", sizeof( contentSearchPath ) );

    char loadPath[MAX_PATH];
    Q_snprintf( loadPath, sizeof( loadPath ), "%s\\" LUA_PATH_WEAPONS, contentSearchPath );
    LoadWeaponsFromPath( loadPath );
    Q_snprintf( loadPath, sizeof( loadPath ), "%s\\" LUA_PATH_ENTITIES, contentSearchPath );
    LoadEntitiesFromPath( loadPath );
    // Q_snprintf( loadPath, sizeof( loadPath ), "%s\\" LUA_PATH_EFFECTS, contentSearchPath );
    // LoadEffectsFromPath( loadPath );

    LUA_CALL_HOOK_BEGIN( "Initialize" );
    LUA_CALL_HOOK_END( 0, 0 );

    return true;
}

static void CommandLuaRun( lua_State *L, const CCommand &args )
{
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

#ifdef CLIENT_DLL
CON_COMMAND( lua_run_cl, "Run a Lua string on the client" )
{
    if ( !g_bLuaInitialized )
        return;

    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_run_cl <string>\n" );
        return;
    }

    CommandLuaRun( L, args );
}
CON_COMMAND( lua_run_menu, "Run a Lua string in the menu" )
{
    if ( !LGameUI )
        return;

    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_run_menu <string>\n" );
        return;
    }

    CommandLuaRun( LGameUI, args );
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

    CommandLuaRun( L, args );
}
#endif

static void ProcessLuaFile( lua_State *L, const char *arg )
{
    char fullpath[MAX_PATH] = { 0 };
    char fileName[256] = { 0 };
    Q_snprintf( fileName, sizeof( fileName ), LUA_ROOT "\\%s", arg );
    Q_strlower( fileName );
    Q_FixSlashes( fileName );

    if ( filesystem->FileExists( fileName, CONTENT_SEARCH_PATH ) )
    {
        filesystem->RelativePathToFullPath( fileName, CONTENT_SEARCH_PATH, fullpath, sizeof( fullpath ) );
    }
    else
    {
#ifdef CLIENT_DLL
        const char *gamePath = engine->GetGameDirectory();
#else
        char gamePath[MAX_PATH];
        engine->GetGameDir( gamePath, MAX_PATH );
        Q_StripTrailingSlash( gamePath );
#endif
        Q_snprintf( fullpath, sizeof( fullpath ), "%s\\" LUA_ROOT "\\%s", gamePath, arg );
        Q_strlower( fullpath );
        Q_FixSlashes( fullpath );
    }

    if ( Q_strstr( fullpath, ".." ) == NULL )
    {
        Msg( "Running file %s...\n", arg );
        luasrc_dofile( L, fullpath );
    }
    else
    {
        Msg( "Error: Invalid file path %s\n", fullpath );
    }
}

static int DoFileCompletion( const char *partial,
                             char commands[COMMAND_COMPLETION_MAXITEMS]
                                          [COMMAND_COMPLETION_ITEM_LENGTH] )
{
    int current = 0;

#ifdef CLIENT_DLL
    const char *cmdname = "lua_openscript_cl";

    if ( Q_strstr( partial, "lua_openscript_menu" ) )
    {
        cmdname = "lua_openscript_menu";
    }
#else
    const char *cmdname = "lua_openscript";
#endif
    char *substring = NULL;
    if ( Q_strstr( partial, cmdname ) && strlen( partial ) > strlen( cmdname ) + 1 )
    {
        substring = ( char * )partial + strlen( cmdname ) + 1;
    }

    FileFindHandle_t fh;

    char WildCard[MAX_PATH] = { 0 };
    if ( substring == NULL )
        substring = "";

    Q_snprintf( WildCard, sizeof( WildCard ), LUA_ROOT "\\%s*", substring );
    Q_FixSlashes( WildCard );

    char const *fn = g_pFullFileSystem->FindFirstEx( WildCard, CONTENT_SEARCH_PATH, &fh );

    char searchFileDirectory[MAX_PATH] = { 0 };

    char searchFileName[MAX_PATH] = { 0 };
    Q_snprintf( searchFileName, sizeof( searchFileName ), LUA_ROOT "\\%s", substring );
    Q_FixSlashes( searchFileName );

    if ( !filesystem->IsDirectory( searchFileName, CONTENT_SEARCH_PATH ) )
        Q_ExtractFilePath( substring, searchFileDirectory, sizeof( searchFileDirectory ) );
    else
        V_strcpy_safe( searchFileDirectory, substring );

    while ( fn && current < COMMAND_COMPLETION_MAXITEMS )
    {
        if ( fn[0] != '.' )
        {
            char fileName[MAX_PATH] = { 0 };
            Q_snprintf( fileName, sizeof( fileName ), LUA_ROOT "\\%s\\%s", searchFileDirectory, fn );
            Q_FixSlashes( fileName );

            if ( filesystem->FileExists( fileName, CONTENT_SEARCH_PATH ) || filesystem->IsDirectory( fileName, CONTENT_SEARCH_PATH ) )
            {
                Q_snprintf( commands[current], sizeof( commands[current] ), "%s %s%s", cmdname, searchFileDirectory, fn );
                current++;
            }
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }
    g_pFullFileSystem->FindClose( fh );

    return current;
}

#ifdef CLIENT_DLL
CON_COMMAND_F_COMPLETION( lua_openscript_cl, "Load and run a Lua file", 0, DoFileCompletion )
{
    if ( !g_bLuaInitialized )
        return;

    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_openscript_cl <fileName>\n" );
        return;
    }

    ProcessLuaFile( L, args.ArgS() );
}
CON_COMMAND_F_COMPLETION( lua_openscript_menu, "Load and run a Lua file", 0, DoFileCompletion )
{
    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_openscript_menu <fileName>\n" );
        return;
    }

    ProcessLuaFile( LGameUI, args.ArgS() );
}

#else
CON_COMMAND_F_COMPLETION( lua_openscript, "Load and run a Lua file", 0, DoFileCompletion )
{
    if ( !g_bLuaInitialized )
        return;

    if ( !UTIL_IsCommandIssuedByServerAdmin() )
        return;

    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_openscript <fileName>\n" );
        return;
    }

    ProcessLuaFile( L, args.ArgS() );
}
#endif

#if DEBUG
#define MAX_STACK_DUMP 100
static void DumpLuaStack( lua_State *L )
{
    int n = lua_gettop( L ); /* number of objects */
    int i;

    CUtlString dumpStack = "Lua Stack:\n";

    for ( i = 1; i <= min( n, MAX_STACK_DUMP ); i++ )
    {
        if ( lua_istable( L, -1 ) )
        {
            lua_getglobal( L, LUA_TABLIBNAME );
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
            dumpStack += " ";
            dumpStack += i;
            dumpStack += ": ";
            dumpStack += s;
            lua_pop( L, 1 ); /* pop result */
        }
    }

    if ( n > 0 )
    {
        dumpStack += "\nWarning: ";
        dumpStack += n;
        dumpStack += " object(s) left on the stack!";

        if ( n > MAX_STACK_DUMP )
        {
            dumpStack += " (only the first ";
            dumpStack += MAX_STACK_DUMP;
            dumpStack += " objects are shown)";
        }
    }
    else
    {
        dumpStack += "Stack is empty, that's great!";
    }

    Warning( "%s\n", dumpStack.Get() );
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
