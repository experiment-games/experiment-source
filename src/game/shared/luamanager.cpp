//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Contains the implementation of Lua for scripting.
//
//===========================================================================//

#include "cbase.h"
#include "utlbuffer.h"
#include "activitylist.h"
#include "filesystem.h"
#ifndef CLIENT_DLL
#include "gameinterface.h"
#include "basescriptedtrigger.h"
#endif
#include "steam/isteamfriends.h"
#include "networkstringtabledefs.h"
#ifdef CLIENT_DLL
#include <scriptedclientluapanel.h>
#include <vgui_int.h>
#endif
#include "basescripted.h"
#include "weapon_hl2mpbase_scriptedweapon.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "luacachefile.h"
#include "lconvar.h"
#include "licvar.h"
#include "lnetwork.h"
#include "inputsystem/ButtonCode.h"
#include <stack>
#include <cpng.h>

extern "C"
{
#include "luasocket.h"
#include "mime.h"
}

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static FileHandle_t g_LuaLogFileHandle = FILESYSTEM_INVALID_HANDLE;

ConVar gamemode( "gamemode", "hl2sb", FCVAR_ARCHIVE | FCVAR_REPLICATED );
ConVar lua_log_cl( "lua_log_cl", "1", FCVAR_ARCHIVE );
ConVar lua_log_sv( "lua_log_sv", "1", FCVAR_ARCHIVE );
ConVar lua_log_loader( "lua_log_loader", "1", FCVAR_ARCHIVE );

#ifdef CLIENT_DLL
#define REALM_COLOR Color( 255, 202, 58, 255 )
#else
#define REALM_COLOR Color( 120, 205, 215, 255 )
#endif

void LuaLogToFile( const char *format, ... )
{
    va_list argp;
    char msg[4096];

    va_start( argp, format );
    Q_vsnprintf( msg, sizeof( msg ), format, argp );
    va_end(argp);

#ifdef CLIENT_DLL
    if ( lua_log_cl.GetBool() )
#else
    if ( lua_log_sv.GetBool() )
#endif
    {
        if ( g_LuaLogFileHandle == FILESYSTEM_INVALID_HANDLE )
        {
            return;
        }

        filesystem->Write( msg, Q_strlen( msg ), g_LuaLogFileHandle );
        filesystem->Flush( g_LuaLogFileHandle );
    }
}

#define SHOW_LUA_ERROR( format, ... )                                  \
    ConColorMsg( REALM_COLOR, "\n[Lua] " format "\n", ##__VA_ARGS__ ); \
    LuaLogToFile( format, ##__VA_ARGS__ );

#define SHOW_LUA_MESSAGE( format, ... ) \
    Msg( "\n[Lua] " format "\n", ##__VA_ARGS__ );

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
// This is where all scripted panels are parented to
// it will be created on the client when the L state is created
// and removed when the L state is destroyed
CScriptedClientLuaPanel *g_pClientLuaPanel = NULL;
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

LUA_API int luasrc_print( lua_State *L )
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
        Msg( "%s", s ); // By providing the string as second parameter, we prevent formatting from user input
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

// Prints blue text on the server, yellow text on the client
static int luasrc_Msg( lua_State *L )
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
            ConColorMsg( REALM_COLOR, "\t" );

        ConColorMsg( REALM_COLOR, "%s", s );
        lua_pop( L, 1 );  // pop result
    }

    return 0;
}

static int luasrc_MsgN( lua_State *L )
{
    luasrc_Msg( L );

    ConColorMsg( REALM_COLOR, "\n" );

    return 0;
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
    lua_pop( L, 1 );  // Pop the GM table

    return 0;
}

static const luaL_Reg base_funcs[] = {
    { "print", luasrc_print },
    { "Msg", luasrc_Msg },
    { "MsgN", luasrc_MsgN },
    { "type", luasrc_type },
    { "Include", luasrc_include },
    { "RunString", luasrc_RunString },
    { "InheritGamemode", luasrc_InheritGamemode },
    { "LuaLogToFile", luasrc_LuaLogToFile },
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
    char gamePath[256];
    engine->GetGameDir( gamePath, 256 );
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
                     LUA_PATH_MODULES,
                     currentCPath );
    lua_setfield( L, -2, "cpath" );

    // Experiment; we also abuse the package table to set a 'include' path
    // for the 'include' function. These paths will be attempted to be prefixed
    // before X.lua when include("X.lua") is called.
    lua_pushfstring( L,
                     ".\\;%s\\;%s\\%s;%s",
                     LUA_ROOT,
                     LUA_PATH_ADDONS,
                     LUA_ROOT,
                     LUA_PATH_GAMEMODES );
    lua_setfield( L, -2, "IncludePath" );

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

    // int leftOnStack = 0;
    // leftOnStack += luaopen_ConCommand( LGameUI );
    // leftOnStack += luaopen_dbg( LGameUI );
    // leftOnStack += luaopen_system( LGameUI );
    // leftOnStack += luaopen_engine( LGameUI );
    // leftOnStack += luaopen_enginevgui( LGameUI );
    // leftOnStack += luaopen_FCVAR( LGameUI );
    // leftOnStack += luaopen_KeyValues( LGameUI );
    // leftOnStack += luaopen_surface( LGameUI );
    // leftOnStack += luaopen_vgui( LGameUI );
    // leftOnStack += luaopen_scheme( LGameUI );
    // leftOnStack += luaopen_Color( LGameUI );
    // lua_pop( LGameUI, leftOnStack );
    //  Experiment; The above is unmaintainable. Find another way, for now lets open all
    //  at the risk of opening libraries not functional in the menu.
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

int buttonCodeStringNext = -1;

static void JumpToButtonCodeIndex( int jumpTo )
{
    buttonCodeStringNext = jumpTo;
}

static void RegisterButtonCodeString( const char *name, ButtonCode_t code, bool dontIncrement )
{
    // Prevent mistakes by ensure we register all button codes
    Assert( code >= BUTTON_CODE_INVALID && code <= BUTTON_CODE_COUNT );
    Assert( buttonCodeStringNext == code );

    lua_pushenum( L, code, name );

    if ( !dontIncrement )
        JumpToButtonCodeIndex( code + 1 );
}

#define REGISTER_BUTTON_CODE_STRING( name ) \
    RegisterButtonCodeString( #name, name, false );

#define REGISTER_BUTTON_CODE_ALIAS2( name1, name2 )  \
    RegisterButtonCodeString( #name1, name1, true ); \
    RegisterButtonCodeString( #name2, name1, false );

#define REGISTER_BUTTON_CODE_ALIAS3( name1, name2, name3 ) \
    RegisterButtonCodeString( #name1, name1, true );       \
    RegisterButtonCodeString( #name2, name1, true );       \
    RegisterButtonCodeString( #name3, name1, false );

#define JUMP_TO_BUTTON_CODE_INDEX( index ) \
    JumpToButtonCodeIndex( index );

void luasrc_init( void )
{
    if ( g_LuaLogFileHandle == FILESYSTEM_INVALID_HANDLE )
    {
#ifdef CLIENT_DLL
        const char *logFilePath = "lua_log_cl.log";
#else
        const char *logFilePath = "lua_log_sv.log";
#endif
        g_LuaLogFileHandle = filesystem->Open( logFilePath, "a", "GAME" );
    }

    if ( g_bLuaInitialized )
        return;

    g_bLuaInitialized = true;

    L = lua_open();

#ifdef CLIENT_DLL
    g_pClientLuaPanel = new CScriptedClientLuaPanel( L );
    g_pClientLuaPanel->Start( gameuifuncs, gameeventmanager );
    luasrc_ui_enable();
#endif

    luaL_openlibs( L );
    base_open( L );
    lcf_open( L );

    // Andrew; Someone set us up the path for great justice
    luasrc_setmodulepaths( L );

    luasrc_openlibs( L );

    RegisterLuaUserMessages();

    Msg( "Lua initialized (" LUA_VERSION ")\n" );

    // Experiment; In order to get all ACT_* enums available in time,
    // we moved this from CWorld and C_World to here.
    ActivityList_Init();

    // Andrew; There's a big issue with including the Activity enumeration
    // library, and that issue is that it's massive. While we clean up _G by
    // placing it in it's own library and increase lookup times across nearly
    // all of our resources, it may be best that we make it a standard practice
    // for developers to look up the enumerations that they need on an
    // as-needed basis, and store them as locals in their relative files, or
    // simply use the raw value of that enumeration in scripts, which is the
    // most performance efficient option.

    // Experiment; Thanks Andrew, but for GMod compatibility we have to ignore
    // the above advice and registers all enumerations.
    BEGIN_LUA_SET_ENUM_LIB( L, "Activity" );

    ActivityList_RegisterSharedActivities();

    END_LUA_SET_ENUM_LIB( L );

    // Expose ButtonCode_t as enums
    BEGIN_LUA_SET_ENUM_LIB_CONTINUED( L, "BUTTON" );
    REGISTER_BUTTON_CODE_STRING( BUTTON_CODE_INVALID );
    REGISTER_BUTTON_CODE_ALIAS3( BUTTON_CODE_NONE, KEY_FIRST, KEY_NONE );
    REGISTER_BUTTON_CODE_STRING( KEY_0 );
    REGISTER_BUTTON_CODE_STRING( KEY_1 );
    REGISTER_BUTTON_CODE_STRING( KEY_2 );
    REGISTER_BUTTON_CODE_STRING( KEY_3 );
    REGISTER_BUTTON_CODE_STRING( KEY_4 );
    REGISTER_BUTTON_CODE_STRING( KEY_5 );
    REGISTER_BUTTON_CODE_STRING( KEY_6 );
    REGISTER_BUTTON_CODE_STRING( KEY_7 );
    REGISTER_BUTTON_CODE_STRING( KEY_8 );
    REGISTER_BUTTON_CODE_STRING( KEY_9 );
    REGISTER_BUTTON_CODE_STRING( KEY_A );
    REGISTER_BUTTON_CODE_STRING( KEY_B );
    REGISTER_BUTTON_CODE_STRING( KEY_C );
    REGISTER_BUTTON_CODE_STRING( KEY_D );
    REGISTER_BUTTON_CODE_STRING( KEY_E );
    REGISTER_BUTTON_CODE_STRING( KEY_F );
    REGISTER_BUTTON_CODE_STRING( KEY_G );
    REGISTER_BUTTON_CODE_STRING( KEY_H );
    REGISTER_BUTTON_CODE_STRING( KEY_I );
    REGISTER_BUTTON_CODE_STRING( KEY_J );
    REGISTER_BUTTON_CODE_STRING( KEY_K );
    REGISTER_BUTTON_CODE_STRING( KEY_L );
    REGISTER_BUTTON_CODE_STRING( KEY_M );
    REGISTER_BUTTON_CODE_STRING( KEY_N );
    REGISTER_BUTTON_CODE_STRING( KEY_O );
    REGISTER_BUTTON_CODE_STRING( KEY_P );
    REGISTER_BUTTON_CODE_STRING( KEY_Q );
    REGISTER_BUTTON_CODE_STRING( KEY_R );
    REGISTER_BUTTON_CODE_STRING( KEY_S );
    REGISTER_BUTTON_CODE_STRING( KEY_T );
    REGISTER_BUTTON_CODE_STRING( KEY_U );
    REGISTER_BUTTON_CODE_STRING( KEY_V );
    REGISTER_BUTTON_CODE_STRING( KEY_W );
    REGISTER_BUTTON_CODE_STRING( KEY_X );
    REGISTER_BUTTON_CODE_STRING( KEY_Y );
    REGISTER_BUTTON_CODE_STRING( KEY_Z );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_0 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_1 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_2 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_3 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_4 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_5 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_6 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_7 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_8 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_9 );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_DIVIDE );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_MULTIPLY );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_MINUS );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_PLUS );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_ENTER );
    REGISTER_BUTTON_CODE_STRING( KEY_PAD_DECIMAL );
    REGISTER_BUTTON_CODE_STRING( KEY_LBRACKET );
    REGISTER_BUTTON_CODE_STRING( KEY_RBRACKET );
    REGISTER_BUTTON_CODE_STRING( KEY_SEMICOLON );
    REGISTER_BUTTON_CODE_STRING( KEY_APOSTROPHE );
    REGISTER_BUTTON_CODE_STRING( KEY_BACKQUOTE );
    REGISTER_BUTTON_CODE_STRING( KEY_COMMA );
    REGISTER_BUTTON_CODE_STRING( KEY_PERIOD );
    REGISTER_BUTTON_CODE_STRING( KEY_SLASH );
    REGISTER_BUTTON_CODE_STRING( KEY_BACKSLASH );
    REGISTER_BUTTON_CODE_STRING( KEY_MINUS );
    REGISTER_BUTTON_CODE_STRING( KEY_EQUAL );
    REGISTER_BUTTON_CODE_STRING( KEY_ENTER );
    REGISTER_BUTTON_CODE_STRING( KEY_SPACE );
    REGISTER_BUTTON_CODE_STRING( KEY_BACKSPACE );
    REGISTER_BUTTON_CODE_STRING( KEY_TAB );
    REGISTER_BUTTON_CODE_STRING( KEY_CAPSLOCK );
    REGISTER_BUTTON_CODE_STRING( KEY_NUMLOCK );
    REGISTER_BUTTON_CODE_STRING( KEY_ESCAPE );
    REGISTER_BUTTON_CODE_STRING( KEY_SCROLLLOCK );
    REGISTER_BUTTON_CODE_STRING( KEY_INSERT );
    REGISTER_BUTTON_CODE_STRING( KEY_DELETE );
    REGISTER_BUTTON_CODE_STRING( KEY_HOME );
    REGISTER_BUTTON_CODE_STRING( KEY_END );
    REGISTER_BUTTON_CODE_STRING( KEY_PAGEUP );
    REGISTER_BUTTON_CODE_STRING( KEY_PAGEDOWN );
    REGISTER_BUTTON_CODE_STRING( KEY_BREAK );
    REGISTER_BUTTON_CODE_STRING( KEY_LSHIFT );
    REGISTER_BUTTON_CODE_STRING( KEY_RSHIFT );
    REGISTER_BUTTON_CODE_STRING( KEY_LALT );
    REGISTER_BUTTON_CODE_STRING( KEY_RALT );
    REGISTER_BUTTON_CODE_STRING( KEY_LCONTROL );
    REGISTER_BUTTON_CODE_STRING( KEY_RCONTROL );
    REGISTER_BUTTON_CODE_STRING( KEY_LWIN );
    REGISTER_BUTTON_CODE_STRING( KEY_RWIN );
    REGISTER_BUTTON_CODE_STRING( KEY_APP );
    REGISTER_BUTTON_CODE_STRING( KEY_UP );
    REGISTER_BUTTON_CODE_STRING( KEY_LEFT );
    REGISTER_BUTTON_CODE_STRING( KEY_DOWN );
    REGISTER_BUTTON_CODE_STRING( KEY_RIGHT );
    REGISTER_BUTTON_CODE_STRING( KEY_F1 );
    REGISTER_BUTTON_CODE_STRING( KEY_F2 );
    REGISTER_BUTTON_CODE_STRING( KEY_F3 );
    REGISTER_BUTTON_CODE_STRING( KEY_F4 );
    REGISTER_BUTTON_CODE_STRING( KEY_F5 );
    REGISTER_BUTTON_CODE_STRING( KEY_F6 );
    REGISTER_BUTTON_CODE_STRING( KEY_F7 );
    REGISTER_BUTTON_CODE_STRING( KEY_F8 );
    REGISTER_BUTTON_CODE_STRING( KEY_F9 );
    REGISTER_BUTTON_CODE_STRING( KEY_F10 );
    REGISTER_BUTTON_CODE_STRING( KEY_F11 );
    REGISTER_BUTTON_CODE_STRING( KEY_F12 );
    REGISTER_BUTTON_CODE_STRING( KEY_CAPSLOCKTOGGLE );
    REGISTER_BUTTON_CODE_STRING( KEY_NUMLOCKTOGGLE );
    REGISTER_BUTTON_CODE_ALIAS2( KEY_SCROLLLOCKTOGGLE, KEY_LAST );

    REGISTER_BUTTON_CODE_ALIAS2( MOUSE_FIRST, MOUSE_LEFT );
    REGISTER_BUTTON_CODE_STRING( MOUSE_RIGHT );
    REGISTER_BUTTON_CODE_STRING( MOUSE_MIDDLE );
    REGISTER_BUTTON_CODE_STRING( MOUSE_4 );
    REGISTER_BUTTON_CODE_STRING( MOUSE_5 );
    REGISTER_BUTTON_CODE_STRING( MOUSE_WHEEL_UP );
    REGISTER_BUTTON_CODE_ALIAS2( MOUSE_WHEEL_DOWN, MOUSE_LAST );

    REGISTER_BUTTON_CODE_ALIAS2( JOYSTICK_FIRST, JOYSTICK_FIRST_BUTTON );

    JUMP_TO_BUTTON_CODE_INDEX( JOYSTICK_LAST_BUTTON );
    REGISTER_BUTTON_CODE_STRING( JOYSTICK_LAST_BUTTON );

    REGISTER_BUTTON_CODE_STRING( JOYSTICK_FIRST_POV_BUTTON );

    JUMP_TO_BUTTON_CODE_INDEX( JOYSTICK_LAST_POV_BUTTON );
    REGISTER_BUTTON_CODE_STRING( JOYSTICK_LAST_POV_BUTTON );

    REGISTER_BUTTON_CODE_STRING( JOYSTICK_FIRST_AXIS_BUTTON );

    JUMP_TO_BUTTON_CODE_INDEX( JOYSTICK_LAST_AXIS_BUTTON );
    REGISTER_BUTTON_CODE_ALIAS2( JOYSTICK_LAST_AXIS_BUTTON, JOYSTICK_LAST );

#if !defined( _X360 )
    JUMP_TO_BUTTON_CODE_INDEX( NOVINT_FIRST );
    REGISTER_BUTTON_CODE_ALIAS2( NOVINT_FIRST, NOVINT_LOGO_0 );
    REGISTER_BUTTON_CODE_STRING( NOVINT_TRIANGLE_0 );
    REGISTER_BUTTON_CODE_STRING( NOVINT_BOLT_0 );
    REGISTER_BUTTON_CODE_STRING( NOVINT_PLUS_0 );
    REGISTER_BUTTON_CODE_STRING( NOVINT_LOGO_1 );
    REGISTER_BUTTON_CODE_STRING( NOVINT_TRIANGLE_1 );
    REGISTER_BUTTON_CODE_STRING( NOVINT_BOLT_1 );
    REGISTER_BUTTON_CODE_ALIAS2( NOVINT_PLUS_1, NOVINT_LAST );
#endif

    REGISTER_BUTTON_CODE_STRING( BUTTON_CODE_LAST );
    REGISTER_BUTTON_CODE_STRING( BUTTON_CODE_COUNT );
    END_LUA_SET_ENUM_LIB( L );
    buttonCodeStringNext = -1;  // Clean up for next time
}

#ifdef CLIENT_DLL
void luasrc_ui_enable( void )
{
    if ( !g_pClientLuaPanel )
        return;

    vgui::VPANEL pRoot = VGui_GetClientDLLRootPanel();

    if ( pRoot != 0 )
    {
        g_pClientLuaPanel->SetParent( pRoot );
    }

    g_pClientLuaPanel->SetProportional( false );
    g_pClientLuaPanel->SetCursor( vgui::dc_none );
    g_pClientLuaPanel->SetVisible( true );

    if ( g_pClientLuaPanel->IsKeyBoardInputEnabled() )
    {
        g_pClientLuaPanel->RequestFocus();
    }

    if ( pRoot != 0 )
    {
        int wide, tall;
        vgui::ipanel()->GetSize( pRoot, wide, tall );
        luasrc_ui_layout( wide, tall );
    }
}

void luasrc_ui_layout( int wide, int tall )
{
    if ( !g_pClientLuaPanel )
        return;

    g_pClientLuaPanel->SetBounds( 0, 0, wide, tall );
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
    Msg( "Lua shutdown - Client\n" );

    delete g_pClientLuaPanel;
    g_pClientLuaPanel = NULL;
#else
    Msg( "Lua shutdown - Server\n" );
#endif

    if ( g_LuaLogFileHandle != FILESYSTEM_INVALID_HANDLE )
    {
        filesystem->Close( g_LuaLogFileHandle );
        g_LuaLogFileHandle = FILESYSTEM_INVALID_HANDLE;
    }

    if ( !g_bLuaInitialized )
        return;

    BEGIN_LUA_CALL_HOOK( "ShutDown" );
    END_LUA_CALL_HOOK( 0, 0 );

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

        if ( filesystem->FileExists( relativeFileName, "MOD" ) )
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

        if ( filesystem->FileExists( relativeFileName, "MOD" ) )
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

            if ( filesystem->FileExists( path, "MOD" ) )
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
    if ( !filesystem->ReadFile( fullPath, "MOD", fileContentsBuffer ) )
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
    return g_pFullFileSystem->IsDirectory( path, "MOD" );
}

LUA_API void luasrc_dofolder( lua_State *L, const char *path )
{
    if ( !luasrc_checkfolder( path ) )
    {
        DevMsg( "Lua dofolder failed! Folder does not exist: %s\n", path );
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

static int luasrc_LoadEntities( lua_State *L )
{
    luasrc_LoadEntities( luaL_checkstring( L, -1 ) );
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
        lua_getglobal( L, LUA_ENTITIESLIBNAME );
        if ( lua_istable( L, -1 ) )
        {
            lua_getfield( L, -1, "Register" );
            if ( lua_isfunction( L, -1 ) )
            {
                lua_remove( L, -2 );
                lua_getglobal( L, "ENT" );
                lua_pushstring( L, className );
                luasrc_pcall( L, 2, 0 );
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

void luasrc_LoadEntities( const char *path )
{
    // First register this function so other directories can be loaded from Lua
    lua_getglobal( L, LUA_ENTITIESLIBNAME );
    lua_pushcfunction( L, luasrc_LoadEntities );
    lua_setfield( L, -2, "LoadFromDirectory" );
    lua_pop( L, 1 );  // Pop the entities library

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

    char const *fn = g_pFullFileSystem->FindFirstEx( searchPath, "MOD", &fh );

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
            if ( !filesystem->FileExists( fileName, "MOD" ) )
            {
                fn = g_pFullFileSystem->FindNext( fh );
                continue;
            }

            filesystem->RelativePathToFullPath(
                fileName, "MOD", fullPath, sizeof( fullPath ) );

            luasrc_LoadEntityFromFile( fullPath, className );
        }
        else
        {
            Q_StripExtension( fn, className, sizeof( className ) );

            Q_snprintf( fileName, sizeof( fileName ), "%s\\%s", path, fn );

            filesystem->RelativePathToFullPath(
                fileName, "MOD", fullPath, sizeof( fullPath ) );

            luasrc_LoadEntityFromFile( fullPath, className );
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }
    g_pFullFileSystem->FindClose( fh );
}

static int luasrc_LoadWeapons( lua_State *L )
{
    luasrc_LoadWeapons( luaL_checkstring( L, -1 ) );
    return 0;
}

/// <summary>
/// Sets up the default values for the SWEP that is on the stack of L.
///
/// TODO: Copied from Garry's Mod Wiki, whilst not all values are used by
/// us. Copied for compatibility reasons.
/// </summary>
static void luasrc_SetupDefaultWeapon()
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

    lua_pushstring( L, "materials/entities/<ClassName>.png" );
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
    luasrc_SetupDefaultWeapon();
    lua_setglobal( L, "SWEP" );

    if ( luasrc_dofile( L, fullPath ) == 0 )
    {
        lua_getglobal( L, LUA_WEAPONSLIBNAME );

        if ( lua_istable( L, -1 ) )
        {
            lua_getfield( L, -1, "Register" );

            if ( lua_isfunction( L, -1 ) )
            {
                lua_remove( L, -2 );
                lua_getglobal( L, "SWEP" );
                lua_pushstring( L, className );
                luasrc_pcall( L, 2, 0 );
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
}

void luasrc_LoadWeapons( const char *path )
{
    // First register this function so other directories can be loaded from Lua
    lua_getglobal( L, LUA_WEAPONSLIBNAME );
    lua_pushcfunction( L, luasrc_LoadWeapons );
    lua_setfield( L, -2, "LoadFromDirectory" );
    lua_pop( L, 1 );  // Pop the weapons library

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

    char const *fn = g_pFullFileSystem->FindFirstEx( searchPath, "MOD", &fh );

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

            if ( !filesystem->FileExists( fileName, "MOD" ) )
            {
                fn = g_pFullFileSystem->FindNext( fh );
                continue;
            }

            filesystem->RelativePathToFullPath(
                fileName, "MOD", fullPath, sizeof( fullPath ) );

            luasrc_LoadWeaponFromFile( fullPath, className );
        }
        else
        {
            Q_StripExtension( fn, className, sizeof( className ) );

            Q_snprintf( fileName, sizeof( fileName ), "%s\\%s", path, fn );

            filesystem->RelativePathToFullPath(
                fileName, "MOD", fullPath, sizeof( fullPath ) );

            luasrc_LoadWeaponFromFile( fullPath, className );
        }

        fn = g_pFullFileSystem->FindNext( fh );
    }

    g_pFullFileSystem->FindClose( fh );
}

/// <summary>
/// As soon as a `{gamemode name}.lua` file exists in the lua/loaders folder
/// for the gamemode being loaded, it and all its dependencies will be loaded
/// using this loader lua state.
/// Each file can be preprocessed by the loader, which can modify the file
/// contents before they are loaded into the main Lua state.
/// </summary>
/// <param name="gamemodePath"></param>
void luasrc_InitCustomLoader( const char *gamemode )
{
    // Check if the lua/loaders/{gamemode}.lua file exists, if it does,
    // load it into the  loader Lua state
    char loaderFilePath[MAX_PATH];
    Q_snprintf( loaderFilePath, sizeof( loaderFilePath ), "lua\\loaders\\%s.lua", gamemode );

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
/// Append the new path to the existing package.path
/// </summary>
/// <param name="L"></param>
/// <param name="searchPath"></param>
static void luasrc_add_to_package_path( lua_State *L, const char *searchPath )
{
    lua_getglobal( L, LUA_LOADLIBNAME );
    lua_getfield( L, -1, "path" );  // Get the current package.path
    const char *currentPath = lua_tostring( L, -1 );
    lua_pop( L, 1 );  // Remove the current path from the stack

    lua_pushfstring( L, "%s;%s", currentPath, searchPath );
    lua_setfield( L, -2, "path" );
    lua_pop( L, 1 );  // Remove the package table from the stack
}

/// <summary>
/// Loads the gamemode with the given name from the gamemodes folder.
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
    if ( Q_strcmp( gamemode, LUA_BASE_GAMEMODE ) != 0 && !hasLoaderBeenActivated )
    {
        luasrc_InitCustomLoader( gamemode );
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

    // lua_pop( L, 1 );  // Pop the GM table, the stack should be empty now

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
        lua_pop( L, 1 );  // Pop the GM table
        cleanUpGamemodeLoading( true );

        SHOW_LUA_ERROR( "Failed to load invalid gamemode %s (%s does not exist)!\n", gamemode, initialFileName );

        return false;
    }

    filesystem->RelativePathToFullPath( initialFileName, "MOD", initialFilePath, sizeof( initialFilePath ) );

    if ( luasrc_dofile( L, initialFilePath ) != LUA_OK )
    {
        lua_pop( L, 1 );  // Pop the GM table
        cleanUpGamemodeLoading( true );

        SHOW_LUA_ERROR( "Failed to load invalid gamemode %s (%s had errors)!\n", gamemode, initialFilePath );

        return false;
    }

    // Call gamemodes.Register with this gamemode
    lua_getglobal( L, LUA_GAMEMODESLIBNAME );
    lua_getfield( L, -1, "Register" );
    lua_remove( L, -2 );  // Remove gamemodes now that we have Register

    // Sanity check in case someone messes with the gamemodes module
    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L, 1 );  // Pop Register
        lua_pop( L, 1 );  // Pop the GM table

        SHOW_LUA_ERROR( "Error loading gamemode: %s (gamemodes.Register is not a function)!\n", gamemode );

        return false;
    }

    // Bring the GM table to the top of the stack
    lua_pushvalue( L, -2 );
    lua_remove( L, -3 );  // Remove the GM table that was below the Register function

    // If InheritGamemode has been called through the loaded scripts above,
    // the GM.InheritsFrom field will be set. Lets get it from the GM table
    // and load the base gamemode if it exists.
    lua_getfield( L, -1, "InheritsFrom" );
    const char *baseGamemodeName;

    if ( lua_isstring( L, -1 ) )
    {
        baseGamemodeName = lua_tostring( L, -1 );

        luasrc_LoadGamemode( baseGamemodeName );  // Overwrites the GM global table
    }
    else
    {
        baseGamemodeName = LUA_BASE_GAMEMODE;
    }

    // (gamemodes.)Register and the original GM table are on the stack
    // Call gamemodes.Register(gamemodeTable, gamemodeName, baseGameMode)
    lua_pushstring( L, gamemode );
    lua_insert( L, -2 );  // Move the gamemode name to the 2nd parameter
    luasrc_pcall( L, 3, 0 );

    // Unset the GM table, users should use the GAMEMODE table instead
    // We keep the loader active for any later includes. It will be cleaned up
    // when the Lua state is closed.
    cleanUpGamemodeLoading( false );

    DevMsg( "Loaded gamemode %s\n", gamemode );

    return true;
}

bool luasrc_SetGamemode( const char *gamemode )
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

    lua_remove( L, -2 );             // Remove gamemode table
    lua_pushstring( L, gamemode );   // Push gamemode name
    luasrc_pcall( L, 1, 1 );      // Call gamemodes.Get(gamemode)
    lua_setglobal( L, "GAMEMODE" );  // Set GAMEMODE to the active gamemode table

    lua_getglobal( L, LUA_GAMEMODESLIBNAME );
    lua_getfield( L, -1, "InternalSetActiveName" );

    if ( !lua_isfunction( L, -1 ) )
    {
        lua_pop( L,
                 2 );  // Remove gamemode table and InternalSetActiveName function
        SHOW_LUA_ERROR( "Failed to set gamemode. gamemodes.InternalSetActiveName didn't exist!\n" );
        return false;
    }

    lua_remove( L, -2 );            // Remove gamemode table
    lua_pushstring( L, gamemode );  // Push gamemode name
    luasrc_pcall( L, 1, 0 );     // Call gamemodes.InternalSetActiveName(gamemode)

#ifdef CLIENT_DLL
    const char *gamePath = engine->GetGameDirectory();
#else
    char gamePath[256];
    engine->GetGameDir( gamePath, 256 );
#endif
    Q_strncpy( contentSearchPath, gamePath, sizeof( contentSearchPath ) );
    Q_strncat( contentSearchPath, LUA_PATH_GAMEMODES "\\", sizeof( contentSearchPath ) );
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
    char fullpath[512] = { 0 };
    char fileName[256] = { 0 };
    Q_snprintf( fileName, sizeof( fileName ), LUA_ROOT "\\%s", arg );
    Q_strlower( fileName );
    Q_FixSlashes( fileName );

    if ( filesystem->FileExists( fileName, "MOD" ) )
    {
        filesystem->RelativePathToFullPath( fileName, "MOD", fullpath, sizeof( fullpath ) );
    }
    else
    {
#ifdef CLIENT_DLL
        const char *gamePath = engine->GetGameDirectory();
#else
        char gamePath[256];
        engine->GetGameDir( gamePath, 256 );
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

    ProcessLuaFile( L, args.ArgS() );
}
CON_COMMAND_F_COMPLETION( lua_dofile_menu, "Load and run a Lua file", 0, DoFileCompletion )
{
    if ( args.ArgC() == 1 )
    {
        Msg( "Usage: lua_dofile_menu <fileName>\n" );
        return;
    }

    ProcessLuaFile( LGameUI, args.ArgS() );
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

    ProcessLuaFile( L, args.ArgS() );
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
