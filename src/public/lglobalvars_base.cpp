//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "filesystem.h"
#include "utlbuffer.h"
#include "materialsystem/itexture.h"
#include <materialsystem/imaterial.h>
#include <materialsystem/imaterialsystem.h>
#include "materialsystem/imaterialvar.h"
#include <bitmap/bitmap.h>
#include <cpng.h>
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "vgui_controls/Controls.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int gpGlobals_absoluteframetime( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->absoluteframetime );
    return 1;
}

static int gpGlobals_curtime( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->curtime );
    return 1;
}

static int gpGlobals_SystemTime( lua_State *L )
{
    lua_pushnumber( L, Plat_FloatTime() );
    return 1;
}

static int gpGlobals_framecount( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->framecount );
    return 1;
}

static int gpGlobals_frametime( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->frametime );
    return 1;
}

static int gpGlobals_interval_per_tick( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->interval_per_tick );
    return 1;
}

static int gpGlobals_IsClient( lua_State *L )
{
    lua_pushboolean( L, gpGlobals->IsClient() );
    return 1;
}

static int gpGlobals_maxClients( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->maxClients );
    return 1;
}

static int gpGlobals_network_protocol( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->network_protocol );
    return 1;
}

static int gpGlobals_realtime( lua_State *L )
{
    lua_pushnumber( L, gpGlobals->realtime );
    return 1;
}

static int gpGlobals_simTicksThisFrame( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->simTicksThisFrame );
    return 1;
}

static int gpGlobals_tickcount( lua_State *L )
{
    lua_pushinteger( L, gpGlobals->tickcount );
    return 1;
}

extern IMaterialSystem *g_pMaterialSystem;

static int gpGlobals_FindMaterial( lua_State *L )
{
    const char *name = luaL_checkstring( L, 1 );

    IMaterial *pMaterial = g_pMaterialSystem->FindMaterial( name, 0, false );

    if ( IsErrorMaterial( pMaterial ) )
    {
        char ext[4];

        Q_ExtractFileExtension( name, ext, sizeof( ext ) );

        if ( Q_stricmp( ext, "png" ) == 0 )
        {
            // Get a name for png materials (prefixed with ! and no png extension)
            char nameWithoutExtension[MAX_PATH];
            Q_StripExtension( name, nameWithoutExtension, sizeof( nameWithoutExtension ) );
            char materialName[MAX_PATH];
            Q_snprintf( materialName, sizeof( materialName ), "!%s", nameWithoutExtension );

            pMaterial = CPngTextureRegen::GetOrCreateProceduralMaterial( materialName, name );

            // We need to assign a TextureID to the material, or else the game will crash in MaterialSystem.pdb
            // when shutting down the game, after having created (but never assigning a CreateNewTextureID)
            // a material with this FindMaterial call.
            g_pMatSystemSurface->DrawSetTextureMaterial( vgui::surface()->CreateNewTextureID( true ), pMaterial );
        }
    }

    IMaterial **pUserData = ( IMaterial ** )lua_newuserdata( L, sizeof( IMaterial * ) );
    *pUserData = pMaterial;
    luaL_getmetatable( L, LUA_MATERIALLIBNAME );
    lua_setmetatable( L, -2 );

    return 1;
}

static int gpGlobals_DoesMaterialExist( lua_State *L )
{
    const char *name = luaL_checkstring( L, 1 );
    IMaterial *pMaterial = g_pMaterialSystem->FindMaterial( name, 0, false );

    if ( !IsErrorMaterial( pMaterial ) )
    {
        lua_pushboolean( L, 1 );
        return 1;
    }

    char ext[4];

    Q_ExtractFileExtension( name, ext, sizeof( ext ) );

    if ( Q_stricmp( ext, "png" ) == 0 )
    {
        // Get a name for png materials (prefixed with ! and no png extension)
        char nameWithoutExtension[MAX_PATH];
        Q_StripExtension( name, nameWithoutExtension, sizeof( nameWithoutExtension ) );
        char materialName[MAX_PATH];
        Q_snprintf( materialName, sizeof( materialName ), "!%s", nameWithoutExtension );

        if ( g_pMaterialSystem->IsMaterialLoaded( materialName ) )
        {
            lua_pushboolean( L, 1 );
            return 1;
        }

        char fullFilePath[MAX_PATH];
        Q_snprintf( fullFilePath, sizeof( fullFilePath ), "materials/%s", name );
        Q_FixSlashes( fullFilePath );

#ifdef CLIENT_DLL
        if ( filesystem->FileExists( fullFilePath, "GAME" ) )
        {
            lua_pushboolean( L, 1 );
            return 1;
        }
#endif
    }

    lua_pushboolean( L, 0 );
    return 1;
}

static int gpGlobals_CreateMaterial( lua_State *L )
{
    const char *name = luaL_checkstring( L, 1 );
    const char *shaderName = luaL_checkstring( L, 2 );

    KeyValues *keys = new KeyValues( shaderName );

    // Get the table to fill in the key values
    lua_pushvalue( L, 3 );  // Push the table to the top of the stack

    lua_pushnil( L );  // Push the first key
    while ( lua_next( L, -2 ) != 0 )
    {
        const char *key = luaL_checkstring( L, -2 );
        const char *value = luaL_checkstring( L, -1 );

        keys->SetString( key, value );

        lua_pop( L, 1 );  // Pop the value, but leave the key
    }

    lua_pop( L, 1 );  // Pop the table

    IMaterial *pMaterial = g_pMaterialSystem->CreateMaterial( name, keys );

    if ( pMaterial )
    {
        IMaterial **pUserData = ( IMaterial ** )lua_newuserdata( L, sizeof( IMaterial * ) );
        *pUserData = pMaterial;
        luaL_getmetatable( L, LUA_MATERIALLIBNAME );
        lua_setmetatable( L, -2 );
    }
    else
    {
        lua_pushnil( L );
    }

    return 1;
}

static const luaL_Reg gpGlobalslib[] = {
    { "absoluteframetime", gpGlobals_absoluteframetime },
    { "curtime", gpGlobals_curtime },
    { "SystemTime", gpGlobals_SystemTime },
    { "framecount", gpGlobals_framecount },
    { "frametime", gpGlobals_frametime },
    { "interval_per_tick", gpGlobals_interval_per_tick },
    { "IsClient", gpGlobals_IsClient },
    { "maxClients", gpGlobals_maxClients },
    { "network_protocol", gpGlobals_network_protocol },
    { "realtime", gpGlobals_realtime },
    { "simTicksThisFrame", gpGlobals_simTicksThisFrame },
    { "tickcount", gpGlobals_tickcount },
    { "FindMaterial", gpGlobals_FindMaterial },
    { "DoesMaterialExist", gpGlobals_DoesMaterialExist },
    { "CreateMaterial", gpGlobals_CreateMaterial },
    { NULL, NULL } };

/*
** Open gpGlobals library
*/
LUALIB_API int luaopen_gpGlobals( lua_State *L )
{
    luaL_register( L, LUA_GLOBALSLIBNAME, gpGlobalslib );
    return 1;
}
