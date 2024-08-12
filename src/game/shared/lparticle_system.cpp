#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "particle_parse.h"
#include "particles/particles.h"

#ifdef CLIENT_DLL
#include "cdll_client_int.h"
#else
#include "gameinterface.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int ParticleSystem_Precache( lua_State *L )
{
    PrecacheParticleSystem( luaL_checkstring( L, 1 ) );
    return 0;
}

static int ParticleSystem_ReadConfigFile( lua_State *L )
{
    const char *filePath = luaL_checkstring( L, 1 );

    if ( !g_pParticleSystemMgr->ReadParticleConfigFile( filePath, true, true ) )
    {
        // ReadParticleConfigFile already prints a warning message itself
        // Warning( "Error reading particle config file: %s\n", filePath );
    }

    return 0;
}

static const luaL_Reg ParticleSystem_funcs[] = {
    { "Precache", ParticleSystem_Precache },
    { "ReadConfigFile", ParticleSystem_ReadConfigFile },
    { NULL, NULL } };

/*
** Open ParticleSystem library
*/
LUALIB_API int luaopen_ParticleSystem( lua_State *L )
{
    luaL_register( L, LUA_PARTICLESYSTEMLIBNAME, ParticleSystem_funcs );
    return 1;
}
