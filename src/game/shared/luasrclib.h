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

#define LUA_CLIENTENUMNAME ""
LUALIB_API int luaopen_ClientEnumerations( lua_State *L );
#define LUA_SHAREDENUMNAME ""
LUALIB_API int( luaopen_SharedEnumerations )( lua_State *L );
#define LUA_SERVERENUMNAME ""
LUALIB_API int( luaopen_ServerEnumerations )( lua_State *L );

#define LUA_ACTIVITYENUMNAME "ACTIVITY"
LUALIB_API int( luaopen_ACTIVITY )( lua_State *L );

#define LUA_BUTTONENUMNAME "BUTTON"
LUALIB_API int( luaopen_BUTTON )( lua_State *L );

#define LUA_BASEANIMATINGLIBNAME "CBaseAnimating"
LUALIB_API int( luaopen_CBaseAnimating )( lua_State *L );
LUALIB_API int( luaopen_CBaseAnimating_shared )( lua_State *L );

#define LUA_CBASEFLEXLIBNAME "CBaseFlex"
LUALIB_API int( luaopen_CBaseFlex_shared )( lua_State *L );

#define LUA_BASECOMBATWEAPONLIBNAME "Weapon"
LUALIB_API int( luaopen_CBaseCombatWeapon )( lua_State *L );

#define LUA_BASEENTITYMETANAME "Entity"
LUALIB_API int( luaopen_CBaseEntity )( lua_State *L );
LUALIB_API int( luaopen_CBaseEntity_shared )( lua_State *L );

#define LUA_PLAYERSLIBNAME "Players"
#define LUA_BASEPLAYERMETANAME "Player"
LUALIB_API int( luaopen_CBasePlayer )( lua_State *L );
LUALIB_API int( luaopen_CBasePlayer_shared )( lua_State *L );
LUALIB_API int( luaopen_Players )( lua_State *L );

#define LUA_EFFECTDATAMETANAME "EffectData"
LUALIB_API int( luaopen_CEffectData )( lua_State *L );

#define LUA_GAMETRACEMETANAME "Trace"
#define LUA_GAMETRACESLIBNAME "Traces"
LUALIB_API int( luaopen_CGameTrace )( lua_State *L );

#define LUA_EXPERIMENTPLAYERLIBNAME "CExperimentPlayer"
LUALIB_API int( luaopen_CExperiment_Player_shared )( lua_State *L );

#define LUA_COLORMETANAME "Color"
LUALIB_API int( luaopen_Color )( lua_State *L );

#define LUA_CONCOMMANDLIBNAME "ConsoleCommands"
#define LUA_CONCOMMANDMETANAME "ConsoleCommand"
LUALIB_API int( luaopen_ConsoleCommandMeta )( lua_State *L );

#define LUA_CONTENTSLIBNAME "CONTENTS"
LUALIB_API int( luaopen_CONTENTS )( lua_State *L );

#define LUA_CONVARLIBNAME "ConsoleVariables"
#define LUA_CONVARMETANAME "ConsoleVariable"
LUALIB_API int( luaopen_ConsoleVariableMeta )( lua_State *L );

#define LUA_RECIPIENTFILTERMETANAME "RecipientFilter"
LUALIB_API int( luaopen_CRecipientFilter )( lua_State *L );

#define LUA_TAKEDAMAGEINFOMETANAME "TakeDamageInfo"
LUALIB_API int( luaopen_CTakeDamageInfo )( lua_State *L );

#define LUA_CVARLIBNAME "ConsoleVariables"
LUALIB_API int( luaopen_ConsoleVariablesLib )( lua_State *L );

#define LUA_DBGLIBNAME "Debug"  // Unused, since we extend the default debug library
LUALIB_API int( luaopen_dbg )( lua_State *L );

#define LUA_DEBUGOVERLAYLIBNAME "DebugOverlays"
LUALIB_API int( luaopen_debugoverlay )( lua_State *L );

#define LUA_EFLIBNAME "ENTITY_EFFECT"
LUALIB_API int( luaopen_EF )( lua_State *L );

#define LUA_ENGINELIBNAME "Engines"
LUALIB_API int( luaopen_engine_shared )( lua_State *L );
LUALIB_API int( luaopen_engine )( lua_State *L );

#define LUA_ENTITIESLIBNAME "Entities"
LUALIB_API int( luaopen_Entities )( lua_State *L );

#define LUA_ENGINEFLAGSENUMLIBNAME "ENGINE_FLAG"
LUALIB_API int( luaopen_FL )( lua_State *L );

#define LUA_FLEDICTLIBNAME "EDICT_FLAG"
LUALIB_API int( luaopen_FL_EDICT )( lua_State *L );

#define LUA_ENGINEVGUILIBNAME "EngineGui"
LUALIB_API int( luaopen_enginevgui )( lua_State *L );

#define LUA_FCVARENUMLIBNAME "FCVAR"
LUALIB_API int( luaopen_FCVAR )( lua_State *L );

#define LUA_FILESYSTEMLIBNAME "Files"
#define LUA_FILEHANDLEMETANAME "FileHandle"
LUALIB_API int( luaopen_Files )( lua_State *L );
LUALIB_API int( luaopen_FileHandle )( lua_State *L );

#define LUA_FONTFLAGLIBNAME "FONT_FLAG"
LUALIB_API int( luaopen_FONTFLAG )( lua_State *L );

#define LUA_GESTURESLOTLIBNAME "GESTURE_SLOT"
LUALIB_API int( luaopen_GESTURE_SLOT )( lua_State *L );

#define LUA_CLIENTSHADOWMGRLIBNAME "ClientShadowManager"
LUALIB_API int( luaopen_g_pClientShadowMgr )( lua_State *L );

#define LUA_FONTLIBNAME "FontHandle"
LUALIB_API int( luaopen_HFont )( lua_State *L );

#define LUA_GAMEEVENTSLIBNAME "GameEvents"
LUALIB_API int( luaopen_GameEvents )( lua_State *L );

#define LUA_MATERIALMETANAME "Material"
LUALIB_API int( luaopen_IMaterial )( lua_State *L );

#define LUA_MOVEHELPERMETANAME "MoveHelper"
LUALIB_API int( luaopen_IMoveHelper )( lua_State *L );

#define LUA_INENUMLIBNAME "INPUT"
LUALIB_API int( luaopen_IN )( lua_State *L );

#define LUA_NETCHANNELINFOMETANAME "NetChannelInfo"
LUALIB_API int( luaopen_INetChannelInfo )( lua_State *L );

#define LUA_INETWORKSTRINGTABLELIBNAME "NetworkStringTable"
LUALIB_API int( luaopen_INetworkStringTable )( lua_State *L );

#define LUA_INPUTLIBNAME "Input"
LUALIB_API int( luaopen_input )( lua_State *L );

#define LUA_PHYSICSOBJECTMETANAME "PhysicsObject"
LUALIB_API int( luaopen_IPhysicsObject )( lua_State *L );

#define LUA_PHYSICSSURFACEPROPSMETANAME "PhysicsSurfacePropertiesHandle"
#define LUA_PHYSICSSURFACEPROPSLIBNAME "PhysicsSurfaceProperties"
LUALIB_API int( luaopen_IPhysicsSurfaceProps )( lua_State *L );

#define LUA_PREDICTIONSYSTEMLIBNAME "PredictionSystems"
LUALIB_API int( luaopen_PredictionSystems )( lua_State *L );

#define LUA_ISCHEMELIBNAME "Scheme"
LUALIB_API int( luaopen_IScheme )( lua_State *L );

#define LUA_STEAMFRIENDSMETANAME "SteamFriendsHandle"
LUALIB_API int( luaopen_ISteamFriends )( lua_State *L );

#define LUA_KEYVALUESLIBNAME "KeyValues"
#define LUA_KEYVALUESMETANAME "KeyValuesHandle"
LUALIB_API int( luaopen_KeyValues )( lua_State *L );

#define LUA_LIFELIBNAME "LIFE"
LUALIB_API int( luaopen_LIFE )( lua_State *L );

#define LUA_MASKLIBNAME "MASK"
LUALIB_API int( luaopen_MASK )( lua_State *L );

#define LUA_MATHLIBLIBNAME "Math"  // Unused, since we extend the default math library
LUALIB_API int( luaopen_mathlib )( lua_State *L );

#define LUA_MATRIXLIBNAME "Matrix3x4"
LUALIB_API int( luaopen_Matrix3x4 )( lua_State *L );

#define LUA_MOVECOLLIDELIBNAME "MOVE_COLLIDE"
LUALIB_API int( luaopen_MOVECOLLIDE )( lua_State *L );

#define LUA_MOVETYPELIBNAME "MOVE_TYPE"
LUALIB_API int( luaopen_MOVETYPE )( lua_State *L );

#define LUA_NETWORKSTRINGTABLELIBNAME "NetworkStringTable"
LUALIB_API int( luaopen_NetworkStringTables )( lua_State *L );

#define LUA_OBSMODELIBNAME "OBSERVER_MODE"
LUALIB_API int( luaopen_OBS_MODE )( lua_State *L );

#define LUA_PARTICLESYSTEMLIBNAME "ParticleSystems"
LUALIB_API int( luaopen_ParticleSystem )( lua_State *L );

#define LUA_PHYSENVLIBNAME "PhysicsEnvironments"
LUALIB_API int( luaopen_PhysicsEnvironments )( lua_State *L );

#define LUA_PREDICTIONSLIBNAME "Predictions"
LUALIB_API int( luaopen_Predictions )( lua_State *L );
LUALIB_API int( luaopen_PredictionShared )( lua_State *L );

#define LUA_QANGLEMETANAME "Angle"
LUALIB_API int( luaopen_QAngle )( lua_State *L );

#define LUA_RANDOMLIBNAME "Random"
LUALIB_API int( luaopen_random )( lua_State *L );

#define LUA_RESOURCESLIBNAME "Resources"
LUALIB_API int( luaopen_resources )( lua_State *L );

#define LUA_SCHEMELIBNAME "Schemes"
LUALIB_API int( luaopen_Schemes )( lua_State *L );

#define LUA_SERIALIZERSLIBNAME "Serializers"
LUALIB_API int( luaopen_Serializers )( lua_State *L );

#define LUA_SOLIDLIBNAME "SOLID"
LUALIB_API int( luaopen_SOLID )( lua_State *L );

#define LUA_SOLIDFLAGLIBNAME "SOLID_FLAG"
LUALIB_API int( luaopen_SOLIDFLAG )( lua_State *L );

#define LUA_SOUNDSLIBNAME "Sounds"
LUALIB_API int( luaopen_Sounds )( lua_State *L );

#define LUA_STEAMAPICONTEXTLIBNAME "SteamApiContexts"
LUALIB_API int( luaopen_SteamApiContexts )( lua_State *L );

#define LUA_SURFENUMNAME "SURFACE"
LUALIB_API int( luaopen_SURF )( lua_State *L );

#define LUA_SURFACELIBNAME "Surfaces"
LUALIB_API int( luaopen_Surfaces )( lua_State *L );

#define LUA_UTILLIBNAME "Utilities"
LUALIB_API int( luaopen_Util )( lua_State *L );
LUALIB_API int( luaopen_Util_shared )( lua_State *L );

#define LUA_VECTORMETANAME "Vector"
#define LUA_VECTORSLIBNAME "Vectors"
LUALIB_API int( luaopen_Vector )( lua_State *L );

#define LUA_PANELMETANAME "Panel"
#define LUA_VGUILIBNAME "Panels"
LUALIB_API int( luaopen_vgui )( lua_State *L );

#define LUA_VMATRIXLIBNAME "Matrix"
LUALIB_API int( luaopen_VMatrix )( lua_State *L );

#ifdef CLIENT_DLL
#define LUA_BFREADLIBNAME "UserMessageReader"
LUALIB_API int( luaopen_bf_read )( lua_State *L );
#endif

#define LUA_UMSGLIBNAME "UserMessages"
LUALIB_API int( luaopen_UserMessages )( lua_State *L );

#define LUA_SYSTEMSLIBNAME "Systems"
LUALIB_API int( luaopen_Systems )( lua_State *L );

#define LUA_RENDERLIBNAME "Renders"
#define LUA_ITEXTUREMETANAME "Texture"
LUALIB_API int( luaopen_render )( lua_State *L );
LUALIB_API int( luaopen_ITexture )( lua_State *L );

#define LUA_LOCALIZATIONLIBNAME "Localizations"
LUALIB_API int luaopen_Localizations( lua_State *L );

#define LUA_MOVEDATAMETANAME "MoveData"
LUALIB_API int( luaopen_CMoveData )( lua_State *L );

#define LUA_USERCMDMETANAME "UserCommand"
LUALIB_API int( luaopen_CUserCmd )( lua_State *L );

/* open all Source Engine libraries */
LUALIB_API void( luasrc_openlibs )( lua_State *L, bool isStateGameUI = false );

// Lua defined libraries
#define LUA_SCRIPTEDENTITIESLIBNAME "ScriptedEntities"
#define LUA_SCRIPTEDWEAPONSLIBNAME "ScriptedWeapons"
#define LUA_HOOKSLIBNAME "Hooks"
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

#define LUA_REGISTRATION_COMMIT_LIBRARY( ClassName ) \
    luaL_register( L, #ClassName, ClassName##_luaRegistry );

#define LUA_REGISTER_METHOD( Registry, name, func ) \
    static struct RegHelper_##func                  \
    {                                               \
        RegHelper_##func()                          \
        {                                           \
            Registry.AddToTail( { name, func } );   \
        }                                           \
    } regHelper_##func;

#define LUA_BINDING_BEGIN( ClassName, FunctionName, Concept, DocumentationDescription, ... )  \
    static int ClassName##_##FunctionName( lua_State *L );                                    \
    LUA_REGISTER_METHOD( ClassName##_luaRegistry, #FunctionName, ClassName##_##FunctionName ) \
    static int ClassName##_##FunctionName( lua_State *L )                                     \
    {
#define LUA_BINDING_ARGUMENT( CheckFunction, ArgIndex, DocumentationName ) \
    CheckFunction( L, ArgIndex )

#define LUA_BINDING_ARGUMENT_NILLABLE( CheckFunction, ArgIndex, DocumentationName ) \
    CheckFunction( L, ArgIndex )

#define LUA_BINDING_ARGUMENT_WITH_EXTRA( CheckFunction, ArgIndex, Extra, DocumentationName ) \
    CheckFunction( L, ArgIndex, Extra )

#define LUA_BINDING_ARGUMENT_WITH_DEFAULT( OptCheckFunction, ArgIndex, OptValue, DocumentationName ) \
    OptCheckFunction( L, ArgIndex, OptValue )

#define LUA_BINDING_END( ... ) \
    }

// We use checknumber for enums, so users can safely do maths to get their enum values
// since they are then cut down to integers
#define LUA_BINDING_ARGUMENT_ENUM( EnumType, ArgIndex, DocumentationName ) \
    ( EnumType )( int ) luaL_checknumber( L, ArgIndex )

#define LUA_BINDING_ARGUMENT_ENUM_WITH_DEFAULT( EnumType, ArgIndex, Default, DocumentationName ) \
    ( EnumType )( int ) luaL_optnumber( L, ArgIndex, Default )

// For enumerations that are really just #defines
#define LUA_BINDING_ARGUMENT_ENUM_DEFINE( EnumType, ArgIndex, DocumentationName ) \
    ( int ) luaL_checknumber( L, ArgIndex )

// TODO
//#define LUA_BINDING_ENUM_CHECK( LuaState, ArgIndex, EnumType ) \
//    ( EnumType )( int ) luaL_checknumber( LuaState, ArgIndex )
//
//#define LUA_BINDING_ENUM_WITH_DEFAULT( LuaState, ArgIndex, EnumType, Default ) \
//    ( EnumType )( int ) luaL_optnumber( LuaState, ArgIndex, Default )
//
//#define LUA_BINDING_ENUM_PUSH( LuaState, EnumValue, EnumType ) \
//    ( EnumType ) lua_pushinteger( LuaState, EnumValue );

void luaL_register( lua_State *L, const char *libname, CUtlVector< LuaRegEntry > &luaRegistry );

#endif  // LUASRCLIB_H
