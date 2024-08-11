//========= Copyright © 1996-2005, Valve Corporation, All rights reserved.
//============//
//
// Purpose: Base class for all animating characters and objects.
//
//=============================================================================//

#include "cbase.h"
#include "luamanager.h"
#include "lbaseanimating.h"
#include "mathlib/lvector.h"
#include "lbaseentity_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( CBaseAnimating )

LUA_BINDING_BEGIN( CBaseAnimating, Ignite, "class", "Ignite the entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );
    float flFlameLifetime = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "lifetimeInSeconds" );
    float flFlameSize = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0.0f, "spreadRadius" );

    pAnimating->Ignite( flFlameLifetime, false, flFlameSize );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( CBaseAnimating, Extinguish, "class", "Extinguish the entity." )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->Extinguish();

    return 0;
}
LUA_BINDING_END()

/*
** Open CBaseAnimating object
*/
LUALIB_API int luaopen_CBaseAnimating( lua_State *L )
{
    LUA_PUSH_METATABLE_TO_EXTEND( L, LUA_BASEANIMATINGLIBNAME );

    // luaL_register( L, NULL, CBaseAnimatingmeta );
    // lua_pushstring( L, "Entity" );
    // lua_setfield( L, -2, "__type" ); /* metatable.__type = "Entity" */ // moved to lbaseanimating_shared
    LUA_REGISTRATION_COMMIT( CBaseAnimating );

    return 1;
}
