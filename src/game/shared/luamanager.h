//========= Copyright © 1996-2005, Valve Corporation, All rights reserved.
//============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef LUAMANAGER_H
#define LUAMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "lua.hpp"
#include "luasrclib.h"
#include "lsingleluainstance.h"
#include <gmod_luabase.h>

#if CLIENT_DLL
#include <scriptedclientluapanel.h>
#endif

#define LUA_ROOT "lua"  // Can't be "LUA_PATH" because luaconf.h uses it.
#define LUA_PATH_CACHE "lua_cache\\"
#define LUA_PATH_ADDONS "addons"
#define LUA_PATH_GAMEMODES "\\gamemodes"
#define LUA_PATH_ENUM LUA_ROOT "\\includes\\enum"
#define LUA_PATH_EXTENSIONS LUA_ROOT "\\includes\\extensions"
#define LUA_PATH_MODULES LUA_ROOT "\\includes\\modules"
#define LUA_PATH_BINARY_MODULES LUA_ROOT "\\bin"

#ifdef _WIN32
#ifdef CLIENT_DLL
#define LUA_BINARY_MODULES_GLOB "*cl_%s_win32.dll" // * allows gm prefix (gmcl_helloworld_win32.dll)
#else
#define LUA_BINARY_MODULES_GLOB "*sv_%s_win32.dll"
#endif
#else
// TODO: other platforms
#endif

#define LUA_PATH_INCLUDES LUA_ROOT "\\includes"
#define LUA_PATH_INCLUDES_INIT_FILE LUA_PATH_INCLUDES "\\sh_init.lua"
#define LUA_PATH_GAME_CLIENT LUA_ROOT "\\game\\client"
#define LUA_PATH_GAME_SERVER LUA_ROOT "\\game\\server"
#define LUA_PATH_GAME_SHARED LUA_ROOT "\\game\\shared"
#define LUA_PATH_EFFECTS LUA_ROOT "\\effects"
#define LUA_PATH_ENTITIES LUA_ROOT "\\entities"
#define LUA_PATH_GAMEUI LUA_ROOT "\\gameui"
#define LUA_PATH_WEAPONS LUA_ROOT "\\weapons"

#define LUA_BASE_ENTITY_CLASS "prop_scripted"
#define LUA_BASE_ENTITY_FACTORY "CBaseAnimating"
#define LUA_BASE_WEAPON "weapon_experimentbase_scriptedweapon"
#define LUA_BASE_GAMEMODE "super"

#define LUA_MAX_WEAPON_ACTIVITIES 32

#ifdef CLIENT_DLL
#define REALM_COLOR Color( 255, 202, 58, 255 )
#else
#define REALM_COLOR Color( 120, 205, 215, 255 )
#endif

#pragma warning( disable : 4800 )  // forcing value to bool 'true' or 'false'
                                   // (performance warning)

#define BEGIN_LUA_SET_ENUM_LIB( L, libraryName ) \
    const char *lib = libraryName;               \
    lua_getglobal( L, LUA_ENAME );               \
    lua_newtable( L );

#define BEGIN_LUA_SET_ENUM_LIB_CONTINUED( L, libraryName ) \
    lib = libraryName;                                     \
    lua_getglobal( L, LUA_ENAME );                         \
    lua_newtable( L );

#define lua_pushenum( L, enum, shortname ) \
    lua_pushinteger( L, enum );            \
    lua_setfield( L, -2, shortname );

#define END_LUA_SET_ENUM_LIB( L ) \
    lua_setfield( L, -2, lib );   \
    lua_pop( L, 1 );

#define BEGIN_LUA_CALL_HOOK_FOR_STATE( L, functionName ) \
    lua_getglobal( L, LUA_HOOKSLIBNAME );                \
    if ( lua_istable( L, -1 ) )                          \
    {                                                    \
        lua_getfield( L, -1, "Call" );                   \
        if ( lua_isfunction( L, -1 ) )                   \
        {                                                \
            lua_remove( L, -2 );                         \
            int args = 0;                                \
            lua_pushstring( L, functionName );           \
            lua_getglobal( L, "GAMEMODE" );              \
            args = 2;

#define BEGIN_LUA_CALL_HOOK( functionName ) \
    BEGIN_LUA_CALL_HOOK_FOR_STATE( L, functionName )

#define END_LUA_CALL_HOOK_FOR_STATE( L, nArgs, nresults ) \
    args += nArgs;                                        \
    luasrc_pcall( L, args, nresults );                    \
    }                                                     \
    else lua_pop( L, 2 );                                 \
    }                                                     \
    else lua_pop( L, 1 );

#define END_LUA_CALL_HOOK( nArgs, nresults ) \
    END_LUA_CALL_HOOK_FOR_STATE( L, nArgs, nresults )

#define BEGIN_LUA_CALL_WEAPON_METHOD( functionName ) \
    lua_getref( L, m_nTableReference );              \
    lua_getfield( L, -1, functionName );             \
    lua_remove( L, -2 );                             \
    if ( lua_isfunction( L, -1 ) )                   \
    {                                                \
        int args = 0;                                \
        CBaseEntity::PushLuaInstanceSafe( L, this ); \
        ++args;

#define END_LUA_CALL_WEAPON_METHOD( nArgs, nresults ) \
    args += nArgs;                                    \
    luasrc_pcall( L, args, nresults );                \
    }                                                 \
    else lua_pop( L, 1 );

#define BEGIN_LUA_CALL_WEAPON_HOOK( functionName, pWeapon ) \
    if ( pWeapon->IsScripted() )                            \
    {                                                       \
        lua_getref( L, pWeapon->m_nTableReference );        \
        lua_getfield( L, -1, functionName );                \
        lua_remove( L, -2 );                                \
        int args = 0;                                       \
        CBaseEntity::PushLuaInstanceSafe( L, pWeapon );     \
        ++args;

#define END_LUA_CALL_WEAPON_HOOK( nArgs, nresults ) \
    args += nArgs;                                  \
    luasrc_pcall( L, args, nresults );              \
    }

#define BEGIN_LUA_CALL_ENTITY_METHOD( functionName ) \
    lua_getref( L, m_nTableReference );              \
    lua_getfield( L, -1, functionName );             \
    lua_remove( L, -2 );                             \
    if ( lua_isfunction( L, -1 ) )                   \
    {                                                \
        int args = 0;                                \
        CBaseEntity::PushLuaInstanceSafe( L, this ); \
        ++args;

#define END_LUA_CALL_ENTITY_METHOD( nArgs, nresults ) \
    args += nArgs;                                    \
    luasrc_pcall( L, args, nresults );                \
    }                                                 \
    else lua_pop( L, 1 );

#define BEGIN_LUA_CALL_TRIGGER_METHOD( functionName ) \
    lua_getref( L, m_nTableReference );               \
    lua_getfield( L, -1, functionName );              \
    lua_remove( L, -2 );                              \
    if ( lua_isfunction( L, -1 ) )                    \
    {                                                 \
        int args = 0;                                 \
        CBaseEntity::PushLuaInstanceSafe( L, this );  \
        ++args;

#define END_LUA_CALL_TRIGGER_METHOD( nArgs, nresults ) \
    args += nArgs;                                     \
    luasrc_pcall( L, args, nresults );                 \
    }                                                  \
    else lua_pop( L, 1 );

#define BEGIN_LUA_CALL_PANEL_METHOD_FOR( Target, functionName ) \
    if ( m_lua_State && Target->m_nTableReference >= 0 )        \
    {                                                           \
        lua_getref( m_lua_State, Target->m_nTableReference );   \
        lua_getfield( m_lua_State, -1, functionName );          \
        lua_remove( m_lua_State, -2 );                          \
        if ( lua_isfunction( m_lua_State, -1 ) )                \
        {                                                       \
            int args = 0;                                       \
            Target->PushLuaInstance( m_lua_State );             \
            ++args;

#define BEGIN_LUA_CALL_PANEL_METHOD( functionName ) \
    BEGIN_LUA_CALL_PANEL_METHOD_FOR( this, functionName )

// Will call the function if it exists, leaving the specified amount of
// return values on the stack (or nils if the function doesn't exist).
#define END_LUA_CALL_PANEL_METHOD( nArgs, nresults )       \
    args += nArgs;                                         \
    if ( luasrc_pcall( m_lua_State, args, nresults ) > 0 ) \
    {                                                      \
        for ( int i = 0; i < nresults; i++ )               \
        {                                                  \
            lua_pushnil( m_lua_State );                    \
        }                                                  \
    }                                                      \
    }                                                      \
    else                                                   \
    {                                                      \
        lua_pop( m_lua_State, 1 );                         \
        for ( int i = 0; i < nresults; i++ )               \
        {                                                  \
            lua_pushnil( m_lua_State );                    \
        }                                                  \
    }                                                      \
    }

#define RETURN_LUA_NONE_IF_FALSE()                         \
    if ( lua_gettop( L ) == 1 )                            \
    {                                                      \
        if ( lua_isboolean( L, -1 ) )                      \
        {                                                  \
            bool res = ( bool )luaL_checkboolean( L, -1 ); \
            lua_pop( L, 1 );                               \
            if ( !res )                                    \
                return;                                    \
        }                                                  \
        else                                               \
            lua_pop( L, 1 );                               \
    }

#define RETURN_LUA_NONE_IF_TRUE()                          \
    if ( lua_gettop( L ) == 1 )                            \
    {                                                      \
        if ( lua_isboolean( L, -1 ) )                      \
        {                                                  \
            bool res = ( bool )luaL_checkboolean( L, -1 ); \
            lua_pop( L, 1 );                               \
            if ( res )                                     \
                return;                                    \
        }                                                  \
        else                                               \
            lua_pop( L, 1 );                               \
    }

#define RETURN_LUA_PANEL_NONE()                                      \
    if ( m_lua_State && lua_gettop( m_lua_State ) == 1 )             \
    {                                                                \
        if ( lua_isboolean( m_lua_State, -1 ) )                      \
        {                                                            \
            bool res = ( bool )luaL_checkboolean( m_lua_State, -1 ); \
            lua_pop( m_lua_State, 1 );                               \
            if ( !res )                                              \
                return;                                              \
        }                                                            \
        else                                                         \
            lua_pop( m_lua_State, 1 );                               \
    }

#define RETURN_LUA_BOOLEAN()                               \
    if ( lua_gettop( L ) == 1 )                            \
    {                                                      \
        if ( lua_isboolean( L, -1 ) )                      \
        {                                                  \
            bool res = ( bool )luaL_checkboolean( L, -1 ); \
            lua_pop( L, 1 );                               \
            return res;                                    \
        }                                                  \
        else                                               \
            lua_pop( L, 1 );                               \
    }

#define RETURN_LUA_PANEL_BOOLEAN()                                   \
    if ( m_lua_State && lua_gettop( m_lua_State ) == 1 )             \
    {                                                                \
        if ( lua_isboolean( m_lua_State, -1 ) )                      \
        {                                                            \
            bool res = ( bool )luaL_checkboolean( m_lua_State, -1 ); \
            lua_pop( m_lua_State, 1 );                               \
            return res;                                              \
        }                                                            \
        else                                                         \
            lua_pop( m_lua_State, 1 );                               \
    }

#define RETURN_LUA_NUMBER()                        \
    if ( lua_gettop( L ) == 1 )                    \
    {                                              \
        if ( lua_isnumber( L, -1 ) )               \
        {                                          \
            float res = luaL_checknumber( L, -1 ); \
            lua_pop( L, 1 );                       \
            return res;                            \
        }                                          \
        else                                       \
            lua_pop( L, 1 );                       \
    }

#define RETURN_LUA_INTEGER()                     \
    if ( lua_gettop( L ) == 1 )                  \
    {                                            \
        if ( lua_isnumber( L, -1 ) )             \
        {                                        \
            int res = luaL_checknumber( L, -1 ); \
            lua_pop( L, 1 );                     \
            return res;                          \
        }                                        \
        else                                     \
            lua_pop( L, 1 );                     \
    }

#define RETURN_LUA_ACTIVITY()                    \
    if ( lua_gettop( L ) == 1 )                  \
    {                                            \
        if ( lua_isnumber( L, -1 ) )             \
        {                                        \
            int res = luaL_checknumber( L, -1 ); \
            lua_pop( L, 1 );                     \
            return ( Activity )res;              \
        }                                        \
        else                                     \
            lua_pop( L, 1 );                     \
    }

#define RETURN_LUA_STRING()                              \
    if ( lua_gettop( L ) == 1 )                          \
    {                                                    \
        if ( lua_isstring( L, -1 ) )                     \
        {                                                \
            const char *res = luaL_checkstring( L, -1 ); \
            lua_pop( L, 1 );                             \
            return res;                                  \
        }                                                \
        else                                             \
            lua_pop( L, 1 );                             \
    }

#define RETURN_LUA_WEAPON()                                          \
    if ( lua_gettop( L ) == 1 )                                      \
    {                                                                \
        if ( lua_isuserdata( L, -1 ) &&                              \
             luaL_checkudata( L, -1, LUA_BASECOMBATWEAPONLIBNAME ) ) \
        {                                                            \
            CBaseCombatWeapon *res = luaL_checkweapon( L, -1 );      \
            lua_pop( L, 1 );                                         \
            return res;                                              \
        }                                                            \
        else                                                         \
            lua_pop( L, 1 );                                         \
    }

#define RETURN_LUA_ENTITY()                                                               \
    if ( lua_gettop( L ) == 1 )                                                           \
    {                                                                                     \
        if ( lua_isuserdata( L, -1 ) && luaL_checkudata( L, -1, LUA_BASEENTITYLIBNAME ) ) \
        {                                                                                 \
            CBaseEntity *res = luaL_checkentity( L, -1 );                                 \
            lua_pop( L, 1 );                                                              \
            return res;                                                                   \
        }                                                                                 \
        else                                                                              \
            lua_pop( L, 1 );                                                              \
    }

#define RETURN_LUA_PLAYER()                                                               \
    if ( lua_gettop( L ) == 1 )                                                           \
    {                                                                                     \
        if ( lua_isuserdata( L, -1 ) && luaL_checkudata( L, -1, LUA_BASEPLAYERLIBNAME ) ) \
        {                                                                                 \
            CBasePlayer *res = luaL_checkplayer( L, -1 );                                 \
            lua_pop( L, 1 );                                                              \
            return res;                                                                   \
        }                                                                                 \
        else                                                                              \
            lua_pop( L, 1 );                                                              \
    }

#define RETURN_LUA_VECTOR()                                                           \
    if ( lua_gettop( L ) == 1 )                                                       \
    {                                                                                 \
        if ( lua_isuserdata( L, -1 ) && luaL_checkudata( L, -1, LUA_VECTORLIBNAME ) ) \
        {                                                                             \
            Vector res = luaL_checkvector( L, -1 );                                   \
            lua_pop( L, 1 );                                                          \
            return res;                                                               \
        }                                                                             \
        else                                                                          \
            lua_pop( L, 1 );                                                          \
    }

#define RETURN_LUA_ANGLE()                                                            \
    if ( lua_gettop( L ) == 1 )                                                       \
    {                                                                                 \
        if ( lua_isuserdata( L, -1 ) && luaL_checkudata( L, -1, LUA_QANGLELIBNAME ) ) \
        {                                                                             \
            QAngle res = luaL_checkangle( L, -1 );                                    \
            lua_pop( L, 1 );                                                          \
            return res;                                                               \
        }                                                                             \
        else                                                                          \
            lua_pop( L, 1 );                                                          \
    }

#define RETURN_LUA_VALUE_IF_TRUE( value )          \
    if ( lua_gettop( L ) == 1 )                    \
    {                                              \
        if ( lua_isboolean( L, -1 ) )              \
        {                                          \
            bool res = luaL_checkboolean( L, -1 ); \
            lua_pop( L, 1 );                       \
            if ( res )                             \
                return value;                      \
        }                                          \
        else                                       \
            lua_pop( L, 1 );                       \
    }

// Merge any methods from the metatable with the given name into the metatable at the top of the stack.
#define LUA_MERGE_METATABLE( L, DerivedFrom )                                        \
    luaL_getmetatable( L, DerivedFrom );                                             \
                                                                                     \
    lua_pushnil( L );                                                                \
    while ( lua_next( L, -2 ) != 0 )                                                 \
    {                                                                                \
        lua_getfield( L, -4, lua_tostring( L, -2 ) );                                \
                                                                                     \
        /* Only copy the metamethod if it doesn't already exist in the metatable */  \
        if ( !lua_isnil( L, -1 ) )                                                   \
        {                                                                            \
            lua_pop( L, 1 ); /* Pop the field value from the Button metatable */     \
                                                                                     \
            lua_pop( L, 1 ); /* Pop the field value from the Label metatable */      \
            continue;                                                                \
        }                                                                            \
                                                                                     \
        lua_pop( L, 1 ); /* Pop the field value from the Button metatable */         \
                                                                                     \
        lua_pushvalue( L, -2 ); /* Copy the key */                                   \
        lua_pushvalue( L, -2 ); /* Copy the value */                                 \
        lua_settable( L, -6 );  /* Set the key-value pair in the Button metatable */ \
                                                                                     \
        /* Pop the value, leaving the key on the stack for the next iteration */     \
        lua_pop( L, 1 );                                                             \
    }                                                                                \
                                                                                     \
    lua_pop( L, 1 ); /* Pop the Label metatable */

#define LUA_GET_REF_TABLE( L, Target )                     \
    if ( !lua_isrefvalid( L, Target->m_nTableReference ) ) \
    {                                                      \
        Target->SetupRefTable( L );                        \
    }                                                      \
    lua_getref( L, Target->m_nTableReference );

#define LUA_METATABLE_INDEX_CHECK_VALID( L, IsValidFunc )         \
    /* IsValid checks before we find out if the target is NULL */ \
    if ( Q_strcmp( luaL_checkstring( L, 2 ), "IsValid" ) == 0 )   \
    {                                                             \
        lua_pushcfunction( L, IsValidFunc );                      \
        return 1;                                                 \
    }

#define LUA_METATABLE_INDEX_CHECK( L, Target )                                                                                       \
    /* Invalid panels fail all checks */                                                                                             \
    if ( Target == NULL )                                                                                                            \
    {                                                                                                                                \
        lua_Debug ar1;                                                                                                               \
        lua_getstack( L, 1, &ar1 );                                                                                                  \
        lua_getinfo( L, "fl", &ar1 );                                                                                                \
        lua_Debug ar2;                                                                                                               \
        lua_getinfo( L, ">S", &ar2 );                                                                                                \
        if ( lua_getmetatable( L, 1 ) )                                                                                              \
        {                                                                                                                            \
            luaL_getmetafield( L, -1, "__name" );                                                                                    \
            const char *__metatableName = lua_tostring( L, -1 );                                                                     \
            lua_pop( L, 2 ); /* Pop the metatable name and the metatable */                                                          \
            lua_pushfstring( L, "%s:%d: attempt to index an invalid %s", ar2.short_src, ar1.currentline, __metatableName );          \
        }                                                                                                                            \
        else                                                                                                                         \
        {                                                                                                                            \
            lua_pushfstring( L, "%s:%d: attempt to index an unknown type (that has no metatable)", ar2.short_src, ar1.currentline ); \
        }                                                                                                                            \
                                                                                                                                     \
        return lua_error( L );                                                                                                       \
    }

// Helper macro to check table on top of the stack for __index
#define LUA_METATABLE_INDEX_CHECK_TABLE( L ) \
    lua_pushvalue( L, 2 );                   \
    lua_gettable( L, -2 );                   \
                                             \
    if ( !lua_isnil( L, -1 ) )               \
    {                                        \
        return 1;                            \
    }                                        \
                                             \
    lua_pop( L, 2 ); /* Pop the table and the nil value */

#define LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, Target )                                    \
    /* We follow by checking if the target has any properties set in its reference table */ \
    if ( Target && L )                                                                      \
    {                                                                                       \
        LUA_GET_REF_TABLE( L, Target );                                                     \
        LUA_METATABLE_INDEX_CHECK_TABLE( L );                                               \
    }

#define LUA_METATABLE_INDEX_DERIVE_INDEX( L, DerivedFrom )         \
    if ( luaL_getmetatable( L, DerivedFrom ) )                     \
    {                                                              \
        lua_getfield( L, -1, "__index" );                          \
        if ( lua_isfunction( L, -1 ) )                             \
        {                                                          \
            lua_pushvalue( L, 1 );                                 \
            lua_pushvalue( L, 2 );                                 \
            lua_call( L, 2, 1 );                                   \
            return 1;                                              \
        }                                                          \
        else if ( lua_istable( L, -1 ) )                           \
        {                                                          \
            lua_pushvalue( L, 2 );                                 \
            lua_gettable( L, -2 );                                 \
            return 1;                                              \
        }                                                          \
                                                                   \
        lua_pop( L, 1 ); /* Pop the result of luaL_getmetatable */ \
    }

#define LUA_PUSH_PANEL_USERDATA_METATABLE( L, MetaTableName ) \
    luaL_getmetatable( L, MetaTableName );                    \
    lua_setmetatable( L, -2 );

#define LUA_PUSH_PANEL_USERDATA( L, Target, TargetType, MetaTableName )            \
    TargetType *_pPanel = dynamic_cast< TargetType * >( Target );                  \
    if ( _pPanel )                                                                 \
        ++_pPanel->m_nRefCount;                                                    \
    PHandle *_pPanelHandle = ( PHandle * )lua_newuserdata( L, sizeof( PHandle ) ); \
    _pPanelHandle->Set( Target );                                                  \
    LUA_PUSH_PANEL_USERDATA_METATABLE( L, MetaTableName );

#define LUA_PUSH_NEW_METATABLE( L, MetaTableName )                      \
    luaL_getmetatable( L, MetaTableName );                              \
    AssertMsg( lua_isnoneornil( L, -1 ), "Metatable already exists!" ); \
    lua_pop( L, 1 );                                                    \
    luaL_newmetatable( L, MetaTableName );

#define LUA_PUSH_METATABLE_TO_EXTEND( L, MetaTableName ) \
    luaL_getmetatable( L, MetaTableName );               \
    AssertMsg( lua_istable( L, -1 ), "Metatable doesn't exist!" );

#define LUA_SAFE_SET_METATABLE( L, MetaTableName )                 \
    luaL_getmetatable( L, MetaTableName );                         \
    AssertMsg( lua_istable( L, -1 ), "Metatable doesn't exist!" ); \
    lua_setmetatable( L, -2 );

extern ConVar gamemode;

LUALIB_API int luaL_checkboolean( lua_State *L, int narg );
LUALIB_API int luaL_optboolean( lua_State *L, int narg, int def );

#ifdef CLIENT_DLL
extern lua_State *LGameUI;  // gameui state
extern CScriptedClientLuaPanel *g_pClientLuaPanel;
extern CScriptedClientLuaPanel *g_pClientLuaPanelHUD;
#endif

extern lua_State *L;

// Set to true between LevelInit and LevelShutdown.
extern bool g_bLuaInitialized;

#ifdef CLIENT_DLL
void luasrc_init_gameui( void );
void luasrc_shutdown_gameui( void );
#endif

void luasrc_init( void );
#ifdef CLIENT_DLL
void luasrc_ui_enable( void );
void luasrc_ui_layout( int wide, int tall );
void luasrc_ui_disable( void );
#endif
void luasrc_shutdown( void );

LUA_API int( luasrc_dostring )( lua_State *L, const char *string );
LUA_API int( luasrc_dofile )( lua_State *L, const char *filename );
LUA_API int( luasrc_dofile_leave_stack )( lua_State *L, const char *filename );
LUA_API void( luasrc_dofolder )( lua_State *L, const char *path );

#define LUA_DEFAULT_ERROR_HANDLER 0
LUA_API int( luasrc_pcall )( lua_State *L, int amountOfArguments, int amountOfResults, int errorFunctionStackPos = LUA_DEFAULT_ERROR_HANDLER );
LUA_API int( luasrc_print )( lua_State *L );
LUA_API void( luasrc_dumpstack )( lua_State *L );

// void    luasrc_LoadEffects (const char *path = 0);
void luasrc_LoadEntities( const char *path = 0 );
void luasrc_LoadWeapons( const char *path = 0 );

bool luasrc_LoadGamemode( const char *gamemode );
bool luasrc_SetGamemode( const char *gamemode );

struct lua_StateWithCompat; // forward declaration

typedef int ( *module_open_func )( lua_State * );
typedef int ( *module_open_func_compat )( lua_StateWithCompat * );
typedef int ( *module_close_func )( lua_State * );
typedef int ( *module_close_func_compat )( lua_StateWithCompat * );

#endif  // LUAMANAGER_H
