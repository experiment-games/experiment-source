#include "cbase.h"
#include "lua.hpp"

#include "luasrclib.h"
#include "lauxlib.h"

enum Realm
{
    REALM_CLIENT = 1,
    REALM_SERVER = 2,
    REALM_GAMEUI = 4
};

#define REALM_SHARED REALM_CLIENT | REALM_SERVER

typedef struct luaL_RegForState
{
    const char *name;
    lua_CFunction func;
    int availableRealms;
} luaL_RegForState;

static const luaL_RegForState luasrclibs[] = {
    //
    // Shared
    //
    { LUA_SHAREDENUMNAME, luaopen_SharedEnumerations, REALM_SHARED },
    { LUA_MESSAGEREADERMETANAME, luaopen_bf_read, REALM_SHARED },
    { LUA_MESSAGEWRITERMETANAME, luaopen_bf_write, REALM_SHARED },
    { LUA_BUTTONENUMNAME, luaopen_BUTTON, REALM_SHARED },
    { LUA_ACTIVITYENUMNAME, luaopen_ACTIVITY, REALM_SHARED },
    { LUA_BASEANIMATINGLIBNAME, luaopen_CBaseAnimating_shared, REALM_SHARED },
    { LUA_BASEANIMATINGLIBNAME, luaopen_CBaseAnimating, REALM_SHARED },
    { LUA_BASECOMBATWEAPONLIBNAME, luaopen_CBaseCombatWeapon, REALM_SHARED },
    { LUA_BASEENTITYMETANAME, luaopen_CBaseEntity_shared, REALM_SHARED },
    { LUA_BASEENTITYMETANAME, luaopen_CBaseEntity, REALM_SHARED },
    { LUA_BASEPLAYERMETANAME, luaopen_CBasePlayer_shared, REALM_SHARED },
    { LUA_BASEPLAYERMETANAME, luaopen_CBasePlayer, REALM_SHARED },
    { LUA_PLAYERSLIBNAME, luaopen_Players, REALM_SHARED },
    { LUA_CBASEFLEXLIBNAME, luaopen_CBaseFlex_shared, REALM_SHARED },
    { LUA_COLORMETANAME, luaopen_Color, REALM_SHARED | REALM_GAMEUI },
    { LUA_CONCOMMANDLIBNAME, luaopen_ConsoleCommandMeta, REALM_SHARED | REALM_GAMEUI },
    { LUA_CONTENTSLIBNAME, luaopen_CONTENTS, REALM_SHARED | REALM_GAMEUI },
    { LUA_CONVARLIBNAME, luaopen_ConsoleVariableMeta, REALM_SHARED | REALM_GAMEUI },
    { LUA_CVARLIBNAME, luaopen_ConsoleVariablesLib, REALM_SHARED },
    { LUA_DBGLIBNAME, luaopen_dbg, REALM_SHARED | REALM_GAMEUI },
    { LUA_DEBUGOVERLAYLIBNAME, luaopen_debugoverlay, REALM_SHARED },
    { LUA_EFFECTDATAMETANAME, luaopen_CEffectData, REALM_SHARED },
    { LUA_EFLIBNAME, luaopen_EF, REALM_SHARED | REALM_GAMEUI },
    { LUA_ENGINEFLAGSENUMLIBNAME, luaopen_FL, REALM_SHARED | REALM_GAMEUI },
    { LUA_ENGINELIBNAME, luaopen_engine_shared, REALM_SHARED | REALM_GAMEUI },
    { LUA_ENGINELIBNAME, luaopen_engine, REALM_SHARED | REALM_GAMEUI },
    { LUA_ENTITIESLIBNAME, luaopen_Entities, REALM_SHARED },
    { LUA_EXPERIMENTPLAYERLIBNAME, luaopen_CExperiment_Player_shared, REALM_SHARED },
    { LUA_FCVARENUMLIBNAME, luaopen_FCVAR, REALM_SHARED | REALM_GAMEUI },
    { LUA_FILEHANDLEMETANAME, luaopen_FileHandle, REALM_SHARED | REALM_GAMEUI },
    { LUA_FILESYSTEMLIBNAME, luaopen_Files, REALM_SHARED | REALM_GAMEUI },
    { LUA_FLEDICTLIBNAME, luaopen_FL_EDICT, REALM_SHARED | REALM_GAMEUI },
    { LUA_GAMETRACEMETANAME, luaopen_CGameTrace, REALM_SHARED },
    { LUA_GAMEEVENTSLIBNAME, luaopen_GameEvents, REALM_SHARED },
    { LUA_GESTURESLOTLIBNAME, luaopen_GESTURE_SLOT, REALM_SHARED | REALM_GAMEUI },
    { LUA_INETWORKSTRINGTABLELIBNAME, luaopen_INetworkStringTable, REALM_SHARED },
    { LUA_INENUMLIBNAME, luaopen_IN, REALM_SHARED | REALM_GAMEUI },
    { LUA_ITEXTUREMETANAME, luaopen_ITexture, REALM_SHARED | REALM_GAMEUI },
    { LUA_KEYVALUESLIBNAME, luaopen_KeyValues, REALM_SHARED | REALM_GAMEUI },
    { LUA_LIFELIBNAME, luaopen_LIFE, REALM_SHARED | REALM_GAMEUI },
    { LUA_LOCALIZATIONLIBNAME, luaopen_Localizations, REALM_SHARED | REALM_GAMEUI },
    { LUA_MASKLIBNAME, luaopen_MASK, REALM_SHARED | REALM_GAMEUI },
    { LUA_MATERIALMETANAME, luaopen_IMaterial, REALM_SHARED | REALM_GAMEUI },
    { LUA_MATHLIBLIBNAME, luaopen_mathlib, REALM_SHARED | REALM_GAMEUI },
    { LUA_MATRIXLIBNAME, luaopen_Matrix3x4, REALM_SHARED | REALM_GAMEUI },
    { LUA_MOVECOLLIDELIBNAME, luaopen_MOVECOLLIDE, REALM_SHARED | REALM_GAMEUI },
    { LUA_MOVEDATAMETANAME, luaopen_CMoveData, REALM_SHARED },
    { LUA_MOVEHELPERMETANAME, luaopen_IMoveHelper, REALM_SHARED },
    { LUA_MOVETYPELIBNAME, luaopen_MOVETYPE, REALM_SHARED | REALM_GAMEUI },
    { LUA_NETWORKSLIBNAME, luaopen_Networks, REALM_SHARED },
    { LUA_NETWORKSTRINGTABLELIBNAME, luaopen_NetworkStringTables, REALM_SHARED | REALM_GAMEUI },
    { LUA_OBSMODELIBNAME, luaopen_OBS_MODE, REALM_SHARED | REALM_GAMEUI },
    { LUA_PARTICLESYSTEMLIBNAME, luaopen_ParticleSystem, REALM_SHARED },
    { LUA_PHYSENVLIBNAME, luaopen_PhysicsEnvironments, REALM_SHARED },
    { LUA_PHYSICSOBJECTMETANAME, luaopen_IPhysicsObject, REALM_SHARED },
    { LUA_PHYSICSSURFACEPROPSMETANAME, luaopen_IPhysicsSurfaceProps, REALM_SHARED },
    { LUA_PREDICTIONSYSTEMLIBNAME, luaopen_PredictionSystems, REALM_SHARED },
    { LUA_PREDICTIONSLIBNAME, luaopen_PredictionShared, REALM_SHARED },
    { LUA_QANGLEMETANAME, luaopen_QAngle, REALM_SHARED | REALM_GAMEUI },
    { LUA_RANDOMLIBNAME, luaopen_random, REALM_SHARED | REALM_GAMEUI },
    { LUA_RECIPIENTFILTERMETANAME, luaopen_CRecipientFilter, REALM_SHARED },
    { LUA_RENDERLIBNAME, luaopen_render, REALM_SHARED | REALM_GAMEUI },
    { LUA_SERIALIZERSLIBNAME, luaopen_Serializers, REALM_SHARED | REALM_GAMEUI },
    { LUA_SOLIDFLAGLIBNAME, luaopen_SOLIDFLAG, REALM_SHARED | REALM_GAMEUI },
    { LUA_SOUNDSLIBNAME, luaopen_Sounds, REALM_SHARED | REALM_GAMEUI },
    { LUA_SOLIDLIBNAME, luaopen_SOLID, REALM_SHARED | REALM_GAMEUI },
    { LUA_STEAMAPICONTEXTLIBNAME, luaopen_SteamApiContexts, REALM_SHARED | REALM_GAMEUI },
    { LUA_STEAMFRIENDSMETANAME, luaopen_ISteamFriends, REALM_SHARED | REALM_GAMEUI },
    { LUA_SURFENUMNAME, luaopen_SURF, REALM_SHARED | REALM_GAMEUI },
    { LUA_SYSTEMSLIBNAME, luaopen_Systems, REALM_SHARED | REALM_GAMEUI },
    { LUA_TAKEDAMAGEINFOMETANAME, luaopen_CTakeDamageInfo, REALM_SHARED },
    { LUA_UMSGLIBNAME, luaopen_UserMessages, REALM_SHARED },
    { LUA_USERCMDMETANAME, luaopen_CUserCmd, REALM_SHARED },
    { LUA_UTILLIBNAME, luaopen_Util_shared, REALM_SHARED },
    { LUA_UTILLIBNAME, luaopen_Util, REALM_SHARED },
    { LUA_VECTORMETANAME, luaopen_Vector, REALM_SHARED | REALM_GAMEUI },
    { LUA_VMATRIXLIBNAME, luaopen_VMatrix, REALM_SHARED | REALM_GAMEUI },
    { LUA_WEBCONNECTIONSLIBNAME, luaopen_WebConnections, REALM_SHARED | REALM_GAMEUI },

#ifndef CLIENT_DLL
    //
    // Server only
    //
    { LUA_SERVERENUMNAME, luaopen_ServerEnumerations, REALM_SERVER },
    { LUA_NETCHANNELINFOMETANAME, luaopen_INetChannelInfo, REALM_SERVER },
    { LUA_RESOURCESLIBNAME, luaopen_resources, REALM_SERVER },
    { LUA_PREDICTIONSLIBNAME, luaopen_Predictions, REALM_SERVER },
#endif

#ifdef CLIENT_DLL
    //
    // Client
    //
    { LUA_CLIENTENUMNAME, luaopen_ClientEnumerations, REALM_CLIENT },
    { LUA_CHATSLIBNAME, luaopen_Chats, REALM_CLIENT },
    { LUA_CLIENTSHADOWMGRLIBNAME, luaopen_g_pClientShadowMgr, REALM_CLIENT },

    // Experiment; TODO: Is this comment correct, test:
    // HL2SB; FIXME: obsolete? should be passing VPANELs, but passes Panel instead,
    // which always ends up being invalid (we can't access them by pointer)
    { LUA_ENGINEVGUILIBNAME, luaopen_enginevgui, REALM_CLIENT | REALM_GAMEUI },

    { LUA_FONTFLAGLIBNAME, luaopen_FONTFLAG, REALM_CLIENT },
    { LUA_FONTLIBNAME, luaopen_HFont, REALM_CLIENT },
    { LUA_INPUTLIBNAME, luaopen_input, REALM_CLIENT | REALM_GAMEUI },
    { LUA_ISCHEMELIBNAME, luaopen_IScheme, REALM_CLIENT | REALM_GAMEUI },
    { LUA_PREDICTIONSLIBNAME, luaopen_Predictions, REALM_CLIENT },
    { LUA_PROJECTEDTEXTUREMETANAME, luaopen_ProjectedTexture, REALM_CLIENT },
    { LUA_SCHEMELIBNAME, luaopen_Schemes, REALM_CLIENT | REALM_GAMEUI },
    { LUA_AUDIOCHANNELMETANAME, luaopen_AudioChannel, REALM_CLIENT | REALM_GAMEUI },
    { LUA_SURFACELIBNAME, luaopen_Surfaces, REALM_CLIENT | REALM_GAMEUI },
    { LUA_VGUILIBNAME, luaopen_vgui, REALM_CLIENT | REALM_GAMEUI },
#endif

    { NULL, NULL, NULL } };

LUALIB_API void luasrc_openlibs( lua_State *L, bool isStateGameUI )
{
    const luaL_RegForState *lib;

    for ( lib = luasrclibs; lib->func; lib++ )
    {
        if ( isStateGameUI && ( lib->availableRealms & REALM_GAMEUI ) == 0 )
            continue;

        if ( lib->availableRealms & REALM_CLIENT )
        {
#ifndef CLIENT_DLL
            // Do not load client-exclusive libraries on the server
            if ( ( lib->availableRealms & REALM_SERVER ) == 0 )
                continue;
#endif
        }
        else if ( lib->availableRealms & REALM_SERVER )
        {
#ifdef CLIENT_DLL
            // Do not load server-exclusive libraries on the client
            if ( ( lib->availableRealms & REALM_CLIENT ) == 0 )
                continue;
#endif
        }

        lua_pushcfunction( L, lib->func );
        lua_pushstring( L, lib->name );
        lua_call( L, 1, 0 );
    }
}

void luaL_register( lua_State *L, const char *libname, CUtlVector< LuaRegEntry > &luaRegistry )
{
    if ( libname )
    {
        luaL_getsubtable( L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE );
        lua_getfield( L, -1, libname );  // get package.loaded[libname]
        if ( !lua_istable( L, -1 ) )
        {
            lua_pop( L, 1 );  // remove previous result
            lua_newtable( L );
            lua_pushvalue( L, -1 );
            lua_setfield( L, -3,
                          libname );  // package.loaded[libname] = new table
        }
        lua_remove( L, -2 );  // remove package.loaded
        lua_pushvalue( L, -1 );
        lua_setglobal( L, libname );  // _G[libname] = new table
    }

    for ( int i = 0; i < luaRegistry.Count(); i++ )
    {
        lua_pushcfunction( L, luaRegistry[i].function );
        lua_setfield( L, -2, luaRegistry[i].name );
    }
}
