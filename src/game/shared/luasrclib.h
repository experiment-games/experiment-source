/*
    Original code by Team Sandbox:
        Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

    Modified for Experiment.
*/

#ifndef LUASRCLIB_H
#define LUASRCLIB_H
#ifdef _WIN32
#pragma once
#endif

#define LUA_BASEANIMATINGLIBNAME "CBaseAnimating"
LUALIB_API int( luaopen_CBaseAnimating )( lua_State *L );
LUALIB_API int( luaopen_CBaseAnimating_shared )( lua_State *L );

#define LUA_CBASEFLEXLIBNAME "CBaseFlex"
LUALIB_API int( luaopen_CBaseFlex_shared )( lua_State *L );

#define LUA_BASECOMBATWEAPONLIBNAME "CBaseCombatWeapon"
LUALIB_API int( luaopen_CBaseCombatWeapon )( lua_State *L );

#define LUA_BASEENTITYLIBNAME "CBaseEntity"
LUALIB_API int( luaopen_CBaseEntity )( lua_State *L );
LUALIB_API int( luaopen_CBaseEntity_shared )( lua_State *L );

#define LUA_BASEPLAYERLIBNAME "CBasePlayer"
LUALIB_API int( luaopen_CBasePlayer )( lua_State *L );
LUALIB_API int( luaopen_CBasePlayer_shared )( lua_State *L );

#define LUA_EFFECTDATALIBNAME "CEffectData"
LUALIB_API int( luaopen_CEffectData )( lua_State *L );

#define LUA_GAMETRACELIBNAME "CGameTrace"
LUALIB_API int( luaopen_CGameTrace )( lua_State *L );

#define LUA_EXPERIMENTPLAYERLIBNAME "CExperimentPlayer"
LUALIB_API int( luaopen_CExperiment_Player_shared )( lua_State *L );

#define LUA_COLORLIBNAME "Color"
LUALIB_API int( luaopen_Color )( lua_State *L );

#define LUA_CONCOMMANDLIBNAME "ConsoleCommands"
#define LUA_CONCOMMANDMETANAME "ConsoleCommand"
LUALIB_API int( luaopen_ConCommand )( lua_State *L );

#define LUA_CONTENTSLIBNAME "CONTENTS"
LUALIB_API int( luaopen_CONTENTS )( lua_State *L );

#define LUA_CONVARLIBNAME "ConsoleVariables"
#define LUA_CONVARMETANAME "ConsoleVariable"
LUALIB_API int( luaopen_ConVar )( lua_State *L );

#define LUA_PASFILTERLIBNAME "CPASFilter"
LUALIB_API int( luaopen_CPASFilter )( lua_State *L );

#define LUA_RECIPIENTFILTERLIBNAME "CRecipientFilter"
LUALIB_API int( luaopen_CRecipientFilter )( lua_State *L );

#define LUA_TAKEDAMAGEINFOLIBNAME "CTakeDamageInfo"
LUALIB_API int( luaopen_CTakeDamageInfo )( lua_State *L );

#define LUA_CVARLIBNAME "ConsoleVariables"
LUALIB_API int( luaopen_cvars )( lua_State *L );

#define LUA_DBGLIBNAME "Debug"  // Unused, since we extend the default debug library
LUALIB_API int( luaopen_dbg )( lua_State *L );

#define LUA_DEBUGOVERLAYLIBNAME "DebugOverlay"
LUALIB_API int( luaopen_debugoverlay )( lua_State *L );

#define LUA_EFLIBNAME "EF"
LUALIB_API int( luaopen_EF )( lua_State *L );

#define LUA_ENGINELIBNAME "Engine"
LUALIB_API int( luaopen_engine_shared )( lua_State *L );
LUALIB_API int( luaopen_engine )( lua_State *L );

#define LUA_ENTLISTLIBNAME "EntityList"
LUALIB_API int( luaopen_EntityList )( lua_State *L );

#define LUA_ENGINEFLAGSLIBNAME "FL"
LUALIB_API int( luaopen_FL )( lua_State *L );

#define LUA_FLEDICTLIBNAME "FL_EDICT"
LUALIB_API int( luaopen_FL_EDICT )( lua_State *L );

#define LUA_ENGINEVGUILIBNAME "EngineGui"
LUALIB_API int( luaopen_enginevgui )( lua_State *L );

#define LUA_FCVARLIBNAME "FCVAR"
LUALIB_API int( luaopen_FCVAR )( lua_State *L );

#define LUA_FILESYSTEMLIBNAME "FileSystem"
LUALIB_API int( luaopen_filesystem )( lua_State *L );

#define LUA_FONTFLAGLIBNAME "FONTFLAG"
LUALIB_API int( luaopen_FONTFLAG )( lua_State *L );

#define LUA_GESTURESLOTLIBNAME "GESTURE_SLOT"
LUALIB_API int( luaopen_GESTURE_SLOT )( lua_State *L );

#define LUA_GLOBALSLIBNAME "Globals"
LUALIB_API int( luaopen_gpGlobals )( lua_State *L );

#define LUA_CLIENTSHADOWMGRLIBNAME "ClientShadowManager"
LUALIB_API int( luaopen_g_pClientShadowMgr )( lua_State *L );

#define LUA_FONTLIBNAME "FontHandle"
LUALIB_API int( luaopen_HFont )( lua_State *L );

#define LUA_HSCHEMELIBNAME "SchemeHandle"
LUALIB_API int( luaopen_HScheme )( lua_State *L );

#define LUA_MATERIALMETANAME "Material"
LUALIB_API int( luaopen_IMaterial )( lua_State *L );

#define LUA_MOVEHELPERLIBNAME "IMoveHelper"
LUALIB_API int( luaopen_IMoveHelper )( lua_State *L );

#define LUA_INLIBNAME "IN"
LUALIB_API int( luaopen_IN )( lua_State *L );

#define LUA_NETCHANNELINFOLIBNAME "INetChannelInfo"
LUALIB_API int( luaopen_INetChannelInfo )( lua_State *L );

#define LUA_INETWORKSTRINGTABLELIBNAME "INetworkStringTable"
LUALIB_API int( luaopen_INetworkStringTable )( lua_State *L );

#define LUA_INPUTLIBNAME "Input"
LUALIB_API int( luaopen_input )( lua_State *L );

#define LUA_PHYSICSOBJECTLIBNAME "IPhysicsObject"
LUALIB_API int( luaopen_IPhysicsObject )( lua_State *L );

#define LUA_PHYSICSSURFACEPROPSLIBNAME "IPhysicsSurfaceProps"
LUALIB_API int( luaopen_IPhysicsSurfaceProps )( lua_State *L );

#define LUA_PREDICTIONSYSTEMLIBNAME "IPredictionSystem"
LUALIB_API int( luaopen_IPredictionSystem )( lua_State *L );

#define LUA_ISCHEMELIBNAME "IScheme"
LUALIB_API int( luaopen_IScheme )( lua_State *L );

#define LUA_STEAMFRIENDSLIBNAME "SteamFriends"
LUALIB_API int( luaopen_ISteamFriends )( lua_State *L );

#define LUA_KEYVALUESLIBNAME "KeyValues"
LUALIB_API int( luaopen_KeyValues )( lua_State *L );

#define LUA_LIFELIBNAME "LIFE"
LUALIB_API int( luaopen_LIFE )( lua_State *L );

#define LUA_MASKLIBNAME "MASK"
LUALIB_API int( luaopen_MASK )( lua_State *L );

#define LUA_MATHLIBLIBNAME "Math"  // Unused, since we extend the default math library
LUALIB_API int( luaopen_mathlib )( lua_State *L );

#define LUA_MATRIXLIBNAME "Matrix3x4"
LUALIB_API int( luaopen_matrix3x4_t )( lua_State *L );

#define LUA_MOVECOLLIDELIBNAME "MOVECOLLIDE"
LUALIB_API int( luaopen_MOVECOLLIDE )( lua_State *L );

#define LUA_MOVETYPELIBNAME "MOVETYPE"
LUALIB_API int( luaopen_MOVETYPE )( lua_State *L );

#define LUA_NETWORKSTRINGTABLELIBNAME "NetworkStringTable"
LUALIB_API int( luaopen_networkstringtable )( lua_State *L );

#define LUA_OBSMODELIBNAME "OBS_MODE"
LUALIB_API int( luaopen_OBS_MODE )( lua_State *L );

#define LUA_PARTICLESYSTEMLIBNAME "ParticleSystem"
LUALIB_API int( luaopen_ParticleSystem )( lua_State *L );

#define LUA_PHYSENVLIBNAME "PhysicsEnvironment"
LUALIB_API int( luaopen_physenv )( lua_State *L );

#define LUA_PREDICTIONLIBNAME "Prediction"
LUALIB_API int( luaopen_prediction )( lua_State *L );

#define LUA_QANGLELIBNAME "Angle"
LUALIB_API int( luaopen_QAngle )( lua_State *L );

#define LUA_RANDOMLIBNAME "Random"
LUALIB_API int( luaopen_random )( lua_State *L );

#define LUA_RESOURCESLIBNAME "Resources"
LUALIB_API int( luaopen_resources )( lua_State *L );

#define LUA_SCHEMELIBNAME "Scheme"
LUALIB_API int( luaopen_scheme )( lua_State *L );

#define LUA_SOLIDLIBNAME "SOLID"
LUALIB_API int( luaopen_SOLID )( lua_State *L );

#define LUA_SOLIDFLAGLIBNAME "FSOLID"
LUALIB_API int( luaopen_SOLIDFLAG )( lua_State *L );

#define LUA_STEAMAPICONTEXTLIBNAME "SteamApiContext"
LUALIB_API int( luaopen_steamapicontext )( lua_State *L );

#define LUA_SURFLIBNAME "SURF"
LUALIB_API int( luaopen_SURF )( lua_State *L );

#define LUA_SURFACELIBNAME "Surface"
LUALIB_API int( luaopen_surface )( lua_State *L );

#define LUA_UTILLIBNAME "Util"
LUALIB_API int( luaopen_Util )( lua_State *L );
LUALIB_API int( luaopen_Util_shared )( lua_State *L );

#define LUA_VECTORLIBNAME "Vector"
LUALIB_API int( luaopen_Vector )( lua_State *L );

#define LUA_VGUILIBNAME "Gui"
#define LUA_PANELLIBNAME "Panel"
LUALIB_API int( luaopen_vgui )( lua_State *L );

#define LUA_VMATRIXLIBNAME "Matrix"
LUALIB_API int( luaopen_VMatrix )( lua_State *L );

#ifdef CLIENT_DLL
#define LUA_BFREADLIBNAME "bf_read"
LUALIB_API int( luaopen_bf_read )( lua_State *L );
#endif

#define LUA_UMSGLIBNAME "UserMessages"
LUALIB_API int( luaopen_umsg )( lua_State *L );

#define LUA_SYSTEMLIBNAME "System"
LUALIB_API int( luaopen_system )( lua_State *L );

#define LUA_RENDERLIBNAME "Renders"
#define LUA_ITEXTUREMETANAME "Texture"
LUALIB_API int( luaopen_render )( lua_State *L );
LUALIB_API int( luaopen_ITexture )( lua_State *L );

#define LUA_LOCALIZATIONLIBNAME "Localization"
LUALIB_API int luaopen_localization( lua_State *L );

#define LUA_MOVEDATALIBNAME "CMoveData"
LUALIB_API int( luaopen_CMoveData )( lua_State *L );

#define LUA_USERCMDLIBNAME "CUserCmd"
LUALIB_API int( luaopen_CUserCmd )( lua_State *L );

/* open all Source Engine libraries */
LUALIB_API void( luasrc_openlibs )( lua_State *L, bool isStateGameUI = false );

// Lua defined libraries
#define LUA_ENTITIESLIBNAME "Entities"
#define LUA_HOOKSLIBNAME "Hooks"
#define LUA_WEAPONSLIBNAME "Weapons"
#define LUA_GAMEMODESLIBNAME "Gamemodes"

struct LuaRegEntry
{
    const char *name;
    lua_CFunction function;
};

#define LUA_REGISTRATION_INIT( ClassName ) \
    static CUtlVector< LuaRegEntry > ClassName##_luaRegistry;

#define LUA_REGISTRATION_COMMIT( ClassName ) \
    luaL_register( L, NULL, ClassName##_luaRegistry );

#define LUA_REGISTRATION_COMMIT_LIBRARY( ClassName, LibraryTableName ) \
    luaL_register( L, LibraryTableName, ClassName##_luaRegistry );

#define LUA_REGISTER_METHOD( Registry, name, func )            \
    static struct RegHelper_##func                   \
    {                                                \
        RegHelper_##func()                           \
        {                                            \
            Registry.AddToTail( { name, func } ); \
        }                                            \
    } regHelper_##func;

#define LUA_BINDING_BEGIN( ClassName, FunctionName, Concept, DocumentationDescription, ... ) \
    static int ClassName##_##FunctionName( lua_State *L );                              \
    LUA_REGISTER_METHOD( ClassName##_luaRegistry, #FunctionName, ClassName##_##FunctionName )                    \
    static int ClassName##_##FunctionName( lua_State *L )                               \
    {

#define LUA_BINDING_ARGUMENT( CheckFunction, ArgIndex, DocumentationName ) \
    CheckFunction( L, ArgIndex )

#define LUA_BINDING_ARGUMENT_NILLABLE( CheckFunction, ArgIndex, DocumentationName ) \
    CheckFunction( L, ArgIndex )

#define LUA_BINDING_ARGUMENT_WITH_DEFAULT( OptCheckFunction, ArgIndex, OptValue, DocumentationName ) \
    OptCheckFunction( L, ArgIndex, OptValue )

#define LUA_BINDING_END( ... ) \
    }

void luaL_register( lua_State *L, const char *libname, CUtlVector< LuaRegEntry > &luaRegistry );

#endif  // LUASRCLIB_H
