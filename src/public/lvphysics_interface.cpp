#include "cbase.h"
#include "vphysics_interface.h"
#include "vphysics/performance.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lvphysics_interface.h"
#include "vphysics/lperformance.h"
#include "mathlib/lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_IPhysicsObject *lua_tophysicsobject( lua_State *L, int idx )
{
    lua_IPhysicsObject **ppPhysicsObject = ( lua_IPhysicsObject ** )lua_touserdata( L, idx );
    return *ppPhysicsObject;
}

LUA_API lua_surfacephysicsparams_t lua_tosurfacephysicsparams( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TTABLE );
    surfacephysicsparams_t physics;
    lua_getfield( L, idx, "friction" );
    if ( !lua_isnil( L, -1 ) )
        physics.friction = luaL_checknumber( L, -1 );
    else
        physics.friction = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "elasticity" );
    if ( !lua_isnil( L, -1 ) )
        physics.elasticity = luaL_checknumber( L, -1 );
    else
        physics.elasticity = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "density" );
    if ( !lua_isnil( L, -1 ) )
        physics.density = luaL_checknumber( L, -1 );
    else
        physics.density = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "thickness" );
    if ( !lua_isnil( L, -1 ) )
        physics.thickness = luaL_checknumber( L, -1 );
    else
        physics.thickness = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "dampening" );
    if ( !lua_isnil( L, -1 ) )
        physics.dampening = luaL_checknumber( L, -1 );
    else
        physics.dampening = 0;
    lua_pop( L, 1 );
    return physics;
}

LUA_API lua_surfaceaudioparams_t lua_tosurfaceaudioparams( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TTABLE );
    surfaceaudioparams_t audio;
    lua_getfield( L, idx, "reflectivity" );
    if ( !lua_isnil( L, -1 ) )
        audio.reflectivity = luaL_checknumber( L, -1 );
    else
        audio.reflectivity = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "hardnessFactor" );
    if ( !lua_isnil( L, -1 ) )
        audio.hardnessFactor = luaL_checknumber( L, -1 );
    else
        audio.hardnessFactor = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "roughnessFactor" );
    if ( !lua_isnil( L, -1 ) )
        audio.roughnessFactor = luaL_checknumber( L, -1 );
    else
        audio.roughnessFactor = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "roughThreshold" );
    if ( !lua_isnil( L, -1 ) )
        audio.roughThreshold = luaL_checknumber( L, -1 );
    else
        audio.roughThreshold = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "hardThreshold" );
    if ( !lua_isnil( L, -1 ) )
        audio.hardThreshold = luaL_checknumber( L, -1 );
    else
        audio.hardThreshold = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "hardVelocityThreshold" );
    if ( !lua_isnil( L, -1 ) )
        audio.hardVelocityThreshold = luaL_checknumber( L, -1 );
    else
        audio.hardVelocityThreshold = 0;
    lua_pop( L, 1 );
    return audio;
}

LUA_API lua_surfacesoundnames_t lua_tosurfacesoundnames( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TTABLE );
    surfacesoundnames_t sounds;
    lua_getfield( L, idx, "stepleft" );
    if ( !lua_isnil( L, -1 ) )
        sounds.stepleft = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.stepleft = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "stepright" );
    if ( !lua_isnil( L, -1 ) )
        sounds.stepright = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.stepright = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "impactSoft" );
    if ( !lua_isnil( L, -1 ) )
        sounds.impactSoft = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.impactSoft = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "impactHard" );
    if ( !lua_isnil( L, -1 ) )
        sounds.impactHard = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.impactHard = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "scrapeSmooth" );
    if ( !lua_isnil( L, -1 ) )
        sounds.scrapeSmooth = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.scrapeSmooth = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "scrapeRough" );
    if ( !lua_isnil( L, -1 ) )
        sounds.scrapeRough = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.scrapeRough = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "bulletImpact" );
    if ( !lua_isnil( L, -1 ) )
        sounds.bulletImpact = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.bulletImpact = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "rolling" );
    if ( !lua_isnil( L, -1 ) )
        sounds.rolling = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.rolling = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "breakSound" );
    if ( !lua_isnil( L, -1 ) )
        sounds.breakSound = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.breakSound = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "strainSound" );
    if ( !lua_isnil( L, -1 ) )
        sounds.strainSound = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        sounds.strainSound = 0;
    lua_pop( L, 1 );
    return sounds;
}

LUA_API lua_surfacesoundhandles_t lua_tosurfacesoundhandles( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TTABLE );
    surfacesoundhandles_t soundhandles;
    lua_getfield( L, idx, "stepleft" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.stepleft = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.stepleft = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "stepright" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.stepright = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.stepright = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "impactSoft" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.impactSoft = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.impactSoft = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "impactHard" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.impactHard = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.impactHard = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "scrapeSmooth" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.scrapeSmooth = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.scrapeSmooth = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "scrapeRough" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.scrapeRough = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.scrapeRough = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "bulletImpact" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.bulletImpact = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.bulletImpact = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "rolling" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.rolling = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.rolling = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "breakSound" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.breakSound = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.breakSound = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "strainSound" );
    if ( !lua_isnil( L, -1 ) )
        soundhandles.strainSound = ( short )luaL_checkinteger( L, -1 );
    else
        soundhandles.strainSound = 0;
    lua_pop( L, 1 );
    return soundhandles;
}

LUA_API lua_surfacegameprops_t lua_tosurfacegameprops( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TTABLE );
    surfacegameprops_t game;
    lua_getfield( L, idx, "maxSpeedFactor" );
    if ( !lua_isnil( L, -1 ) )
        game.maxSpeedFactor = luaL_checknumber( L, -1 );
    else
        game.maxSpeedFactor = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "jumpFactor" );
    if ( !lua_isnil( L, -1 ) )
        game.jumpFactor = luaL_checknumber( L, -1 );
    else
        game.jumpFactor = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "material" );
    if ( !lua_isnil( L, -1 ) )
        game.material = ( unsigned short )luaL_checkinteger( L, -1 );
    else
        game.material = 0;
    lua_pop( L, 1 );

    lua_getfield( L, idx, "climbable" );
    if ( !lua_isnil( L, -1 ) )
        game.climbable = ( unsigned char )luaL_checkinteger( L, -1 );
    else
        game.climbable = 0;
    lua_pop( L, 1 );
    lua_getfield( L, idx, "pad" );
    if ( !lua_isnil( L, -1 ) )
        game.pad = ( unsigned char )luaL_checkinteger( L, -1 );
    else
        game.pad = 0;
    lua_pop( L, 1 );
    return game;
}

#pragma warning( disable : 4701 )

LUA_API lua_surfacedata_t lua_tosurfacedata( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TTABLE );
    surfacedata_t surface;
    lua_getfield( L, idx, "physics" );
    if ( !lua_isnil( L, -1 ) )
        surface.physics = lua_tosurfacephysicsparams( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "audio" );
    if ( !lua_isnil( L, -1 ) )
        surface.audio = lua_tosurfaceaudioparams( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "sounds" );
    if ( !lua_isnil( L, -1 ) )
        surface.sounds = lua_tosurfacesoundnames( L, -1 );
    lua_pop( L, 1 );
    lua_getfield( L, idx, "game" );
    if ( !lua_isnil( L, -1 ) )
        surface.game = lua_tosurfacegameprops( L, -1 );
    lua_pop( L, 1 );

    lua_getfield( L, idx, "soundhandles" );
    if ( !lua_isnil( L, -1 ) )
        surface.soundhandles = lua_tosurfacesoundhandles( L, -1 );
    lua_pop( L, 1 );
    return surface;
}

LUA_API lua_IPhysicsSurfaceProps *lua_tophysicssurfaceprops( lua_State *L, int idx )
{
    lua_IPhysicsSurfaceProps **ppProps = ( lua_IPhysicsSurfaceProps ** )lua_touserdata( L, idx );
    return *ppProps;
}

/*
** push functions (C -> stack)
*/

// Experiment; TODO: Fix this:
// FIXME?: So here's the deal folks; in the Source SDK storing physics objects
// long-term is not done frequently. This is because physics objects are, for
// the majority of the time, tied to plain physics props or other similar
// movable entities.
//
// In the Lau SDK, we want to promote the storage of the entity in variables in
// Lua instead, which mirrors internal code storing entities in handles. There
// are no handles for IPhysicsObjects in the Source SDK, so here we use a
// pointer to a pointer.
//
// This means this object is unsafe in Lua, since the entity which this physics
// object is tied to could be gone when you attempt to use it.
//
// Henry wrote up a handle system for this, so if we need to end up using that
// due to unforeseen logical issues, we will.
LUA_API void lua_pushphysicsobject( lua_State *L, lua_IPhysicsObject *pPhysicsObject )
{
    lua_IPhysicsObject **ppPhysicsObject = ( lua_IPhysicsObject ** )lua_newuserdata( L, sizeof( pPhysicsObject ) );
    *ppPhysicsObject = pPhysicsObject;
    LUA_SAFE_SET_METATABLE( L, LUA_PHYSICSOBJECTMETANAME );
}

LUA_API void lua_pushsurfacephysicsparams( lua_State *L, lua_surfacephysicsparams_t *pphysics )
{
    lua_newtable( L );
    lua_pushnumber( L, pphysics->friction );
    lua_setfield( L, -2, "friction" );
    lua_pushnumber( L, pphysics->elasticity );
    lua_setfield( L, -2, "elasticity" );
    lua_pushnumber( L, pphysics->density );
    lua_setfield( L, -2, "density" );
    lua_pushnumber( L, pphysics->thickness );
    lua_setfield( L, -2, "thickness" );
    lua_pushnumber( L, pphysics->dampening );
    lua_setfield( L, -2, "dampening" );
}

LUA_API void lua_pushsurfaceaudioparams( lua_State *L, lua_surfaceaudioparams_t *paudio )
{
    lua_newtable( L );
    lua_pushnumber( L, paudio->reflectivity );
    lua_setfield( L, -2, "reflectivity" );
    lua_pushnumber( L, paudio->hardnessFactor );
    lua_setfield( L, -2, "hardnessFactor" );
    lua_pushnumber( L, paudio->roughnessFactor );
    lua_setfield( L, -2, "roughnessFactor" );

    lua_pushnumber( L, paudio->roughThreshold );
    lua_setfield( L, -2, "roughThreshold" );
    lua_pushnumber( L, paudio->hardThreshold );
    lua_setfield( L, -2, "hardThreshold" );
    lua_pushnumber( L, paudio->hardVelocityThreshold );
    lua_setfield( L, -2, "hardVelocityThreshold" );
}

LUA_API void lua_pushsurfacesoundnames( lua_State *L, lua_surfacesoundnames_t *psounds )
{
    lua_newtable( L );
    lua_pushinteger( L, psounds->stepleft );
    lua_setfield( L, -2, "stepleft" );
    lua_pushinteger( L, psounds->stepright );
    lua_setfield( L, -2, "stepright" );

    lua_pushinteger( L, psounds->impactSoft );
    lua_setfield( L, -2, "impactSoft" );
    lua_pushinteger( L, psounds->impactHard );
    lua_setfield( L, -2, "impactHard" );

    lua_pushinteger( L, psounds->scrapeSmooth );
    lua_setfield( L, -2, "scrapeSmooth" );
    lua_pushinteger( L, psounds->scrapeRough );
    lua_setfield( L, -2, "scrapeRough" );

    lua_pushinteger( L, psounds->bulletImpact );
    lua_setfield( L, -2, "bulletImpact" );
    lua_pushinteger( L, psounds->rolling );
    lua_setfield( L, -2, "rolling" );

    lua_pushinteger( L, psounds->breakSound );
    lua_setfield( L, -2, "breakSound" );
    lua_pushinteger( L, psounds->strainSound );
    lua_setfield( L, -2, "strainSound" );
}

LUA_API void lua_pushsurfacesoundhandles( lua_State *L, lua_surfacesoundhandles_t *psoundhandles )
{
    lua_newtable( L );
    lua_pushinteger( L, psoundhandles->stepleft );
    lua_setfield( L, -2, "stepleft" );
    lua_pushinteger( L, psoundhandles->stepright );
    lua_setfield( L, -2, "stepright" );

    lua_pushinteger( L, psoundhandles->impactSoft );
    lua_setfield( L, -2, "impactSoft" );
    lua_pushinteger( L, psoundhandles->impactHard );
    lua_setfield( L, -2, "impactHard" );

    lua_pushinteger( L, psoundhandles->scrapeSmooth );
    lua_setfield( L, -2, "scrapeSmooth" );
    lua_pushinteger( L, psoundhandles->scrapeRough );
    lua_setfield( L, -2, "scrapeRough" );

    lua_pushinteger( L, psoundhandles->bulletImpact );
    lua_setfield( L, -2, "bulletImpact" );
    lua_pushinteger( L, psoundhandles->rolling );
    lua_setfield( L, -2, "rolling" );

    lua_pushinteger( L, psoundhandles->breakSound );
    lua_setfield( L, -2, "breakSound" );
    lua_pushinteger( L, psoundhandles->strainSound );
    lua_setfield( L, -2, "strainSound" );
}

LUA_API void lua_pushsurfacegameprops( lua_State *L, lua_surfacegameprops_t *pgame )
{
    lua_newtable( L );
    lua_pushnumber( L, pgame->maxSpeedFactor );
    lua_setfield( L, -2, "maxSpeedFactor" );
    lua_pushnumber( L, pgame->jumpFactor );
    lua_setfield( L, -2, "jumpFactor" );

    lua_pushinteger( L, pgame->material );
    lua_setfield( L, -2, "material" );

    lua_pushinteger( L, pgame->climbable );
    lua_setfield( L, -2, "climbable" );
    lua_pushinteger( L, pgame->pad );
    lua_setfield( L, -2, "pad" );
}

LUA_API void lua_pushsurfacedata( lua_State *L, lua_surfacedata_t *psurface )
{
    lua_newtable( L );
    lua_pushsurfacephysicsparams( L, &psurface->physics );
    lua_setfield( L, -2, "physics" );
    lua_pushsurfaceaudioparams( L, &psurface->audio );
    lua_setfield( L, -2, "audio" );
    lua_pushsurfacesoundnames( L, &psurface->sounds );
    lua_setfield( L, -2, "sounds" );
    lua_pushsurfacegameprops( L, &psurface->game );
    lua_setfield( L, -2, "game" );

    lua_pushsurfacesoundhandles( L, &psurface->soundhandles );
    lua_setfield( L, -2, "soundhandles" );
}

LUA_API void lua_pushphysicssurfaceprops( lua_State *L, lua_IPhysicsSurfaceProps *pProps )
{
    lua_IPhysicsSurfaceProps **ppProps = ( lua_IPhysicsSurfaceProps ** )lua_newuserdata( L, sizeof( pProps ) );
    *ppProps = pProps;
    LUA_SAFE_SET_METATABLE( L, LUA_PHYSICSSURFACEPROPSMETANAME );
}

LUALIB_API lua_IPhysicsObject *luaL_checkphysicsobject( lua_State *L, int narg )
{
    lua_IPhysicsObject *d = lua_tophysicsobject( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "PhysicsObject expected, got NULL physicsobject" );
    return d;
}

LUALIB_API lua_IPhysicsSurfaceProps *luaL_checkphysicssurfaceprops( lua_State *L, int narg )
{
    lua_IPhysicsSurfaceProps **d = ( lua_IPhysicsSurfaceProps ** )luaL_checkudata( L, narg, LUA_PHYSICSSURFACEPROPSMETANAME );

    if ( *d == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "PhysicsSurfaceProperties expected, got NULL" );

    return *d;
}

/*
** PhysicsEnvironments library
*/

LUA_REGISTRATION_INIT( PhysicsEnvironments )

LUA_BINDING_BEGIN( PhysicsEnvironments, CleanupDeleteList, "library", "Cleanup delete list." )
{
    physenv->CleanupDeleteList();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, ClearStats, "library", "Clears the physics stats." )
{
    physenv->ClearStats();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, DebugCheckContacts, "library", "Debug check contacts." )
{
    physenv->DebugCheckContacts();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, GetActiveObjectCount, "library", "Get active object count." )
{
    lua_pushnumber( L, physenv->GetActiveObjectCount() );
    return 1;
}
LUA_BINDING_END( "integer", "The active object count." )

LUA_BINDING_BEGIN( PhysicsEnvironments, GetAirDensity, "library", "Get air density." )
{
    lua_pushnumber( L, physenv->GetAirDensity() );
    return 1;
}
LUA_BINDING_END( "number", "The air density value." )

LUA_BINDING_BEGIN( PhysicsEnvironments, GetGravity, "library", "Get gravity vector." )
{
    Vector pGravityVector;
    physenv->GetGravity( &pGravityVector );
    lua_pushvector( L, pGravityVector );
    return 1;
}
LUA_BINDING_END( "Vector", "The gravity vector." )

LUA_BINDING_BEGIN( PhysicsEnvironments, GetNextFrameTime, "library", "Get next frame time." )
{
    lua_pushnumber( L, physenv->GetNextFrameTime() );
    return 1;
}
LUA_BINDING_END( "number", "The next frame time." )

LUA_BINDING_BEGIN( PhysicsEnvironments, GetPerformanceSettings, "library", "Get performance settings." )
{
    physics_performanceparams_t pOutput;
    physenv->GetPerformanceSettings( &pOutput );
    lua_pushperformanceparams( L, &pOutput );
    return 1;
}
LUA_BINDING_END( "physics_performanceparams_t", "The physics performance parameters." )

LUA_BINDING_BEGIN( PhysicsEnvironments, GetSimulationTime, "library", "Get simulation time." )
{
    lua_pushnumber( L, physenv->GetSimulationTime() );
    return 1;
}
LUA_BINDING_END( "number", "The simulation time." )

LUA_BINDING_BEGIN( PhysicsEnvironments, GetSimulationTimestep, "library", "Get simulation timestep." )
{
    lua_pushnumber( L, physenv->GetSimulationTimestep() );
    return 1;
}
LUA_BINDING_END( "number", "The simulation timestep." )

LUA_BINDING_BEGIN( PhysicsEnvironments, IsInSimulation, "library", "Check if in simulation." )
{
    lua_pushboolean( L, physenv->IsInSimulation() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether in simulation or not." )

LUA_BINDING_BEGIN( PhysicsEnvironments, PostRestore, "library", "Post restore operations." )
{
    physenv->PostRestore();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, ResetSimulationClock, "library", "Reset the simulation clock." )
{
    physenv->ResetSimulationClock();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, SetAirDensity, "library", "Set air density." )
{
    float density = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "density" );
    physenv->SetAirDensity( density );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, SetGravity, "library", "Set gravity." )
{
    Vector gravity = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "gravity" );
    physenv->SetGravity( gravity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, SetPerformanceSettings, "library", "Set performance settings." )
{
    physics_performanceparams_t params = LUA_BINDING_ARGUMENT( lua_toperformanceparams, 1, "params" );
    physenv->SetPerformanceSettings( &params );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, SetQuickDelete, "library", "Set quick delete." )
{
    bool quickDelete = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "quickDelete" );
    physenv->SetQuickDelete( quickDelete );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, SetSimulationTimestep, "library", "Set simulation timestep." )
{
    float timestep = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "timestep" );
    physenv->SetSimulationTimestep( timestep );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsEnvironments, Simulate, "library", "Simulate for specified time." )
{
    float time = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "time" );
    physenv->Simulate( time );
    return 0;
}
LUA_BINDING_END()

/*
** Open physenv library
*/
LUALIB_API int luaopen_PhysicsEnvironments( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( PhysicsEnvironments );

    return 1;
}

/*
** IPhysicsObject Meta
*/

LUA_REGISTRATION_INIT( PhysicsObject );

LUA_BINDING_BEGIN( PhysicsObject, AddAngleVelocity, "class", "Adds angular velocity" )
{
    AngularImpulse angularVelocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "angularVelocity" );
    Vector velocity( 0, 0, 0 );
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->AddVelocity( &velocity, &angularVelocity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, AddVelocity, "class", "Adds velocity and optionally angular velocity" )
{
    Vector velocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "velocity" );
    AngularImpulse angularVelocity( 0, 0, 0 );
    angularVelocity = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optvector, 3, &angularVelocity, "angularVelocity" );
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->AddVelocity( &velocity, &angularVelocity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, ApplyForceCenter, "class", "Applies force at center" )
{
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->ApplyForceCenter( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "force" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, ApplyForceOffset, "class", "Applies force at an offset" )
{
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->ApplyForceOffset( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "force" ), LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "offset" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, ApplyTorqueCenter, "class", "Applies torque at center" )
{
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->ApplyTorqueCenter( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "torque" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, BecomeHinged, "class", "Becomes hinged at a specified point" )
{
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->BecomeHinged( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "localAxis" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, BecomeTrigger, "class", "Becomes a trigger" )
{
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->BecomeTrigger();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, CalculateAngularDrag, "class", "Calculates angular drag" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->CalculateAngularDrag( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "objectSpaceRotationAxis" ) ) );
    return 1;
}
LUA_BINDING_END( "number", "The angular drag value." )

LUA_BINDING_BEGIN( PhysicsObject, CalculateForceOffset, "class", "Calculates force and torque offset" )
{
    Vector centerForce, centerTorque;
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->CalculateForceOffset( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "force" ), LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "worldPosition" ), &centerForce, &centerTorque );
    lua_pushvector( L, centerForce );
    lua_pushvector( L, centerTorque );
    return 2;
}
LUA_BINDING_END( "Vector", "The center force.", "Vector", "The center torque." )

LUA_BINDING_BEGIN( PhysicsObject, CalculateLinearDrag, "class", "Calculates linear drag" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->CalculateLinearDrag( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "unitDirection" ) ) );
    return 1;
}
LUA_BINDING_END( "number", "The linear drag value." )

LUA_BINDING_BEGIN( PhysicsObject, CalculateVelocityOffset, "class", "Calculates velocity and angular velocity offsets" )
{
    Vector centerVelocity, centerAngularVelocity;
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->CalculateVelocityOffset( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "velocity" ), LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "offset" ), &centerVelocity, &centerAngularVelocity );
    lua_pushvector( L, centerVelocity );
    lua_pushvector( L, centerAngularVelocity );
    return 2;
}
LUA_BINDING_END( "Vector", "The center velocity.", "Vector", "The center angular velocity." )

LUA_BINDING_BEGIN( PhysicsObject, EnableCollisions, "class", "Enables or disables collisions" )
{
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->EnableCollisions( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, EnableDrag, "class", "Enables or disables drag" )
{
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->EnableDrag( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, EnableGravity, "class", "Enables or disables gravity" )
{
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->EnableGravity( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, EnableMotion, "class", "Enables or disables motion" )
{
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->EnableMotion( LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, GetAabb, "class", "Gets the axis-aligned bounding box" )
{
    Vector mins, maxs;
    physcollision->CollideGetAABB( &mins, &maxs, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "collide" )->GetCollide(), vec3_origin, vec3_angle );
    lua_pushvector( L, mins );
    lua_pushvector( L, maxs );
    return 2;
}
LUA_BINDING_END( "Vector", "The minimum vector.", "Vector", "The maximum vector." )

LUA_BINDING_BEGIN( PhysicsObject, GetCallbackFlags, "class", "Gets callback flags" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetCallbackFlags() );
    return 1;
}
LUA_BINDING_END( "integer", "The callback flags." )

LUA_BINDING_BEGIN( PhysicsObject, GetContactPoint, "class", "Gets the contact point and contact object" )
{
    Vector contactPoint;
    IPhysicsObject *contactObject;
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetContactPoint( &contactPoint, &contactObject ) );
    lua_pushvector( L, contactPoint );
    lua_pushphysicsobject( L, contactObject );
    return 3;
}
LUA_BINDING_END( "boolean", "Whether there is a contact point or not.", "Vector", "The contact point.", "physicsObject", "The contact object." )

LUA_BINDING_BEGIN( PhysicsObject, GetContents, "class", "Gets contents" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetContents() );
    return 1;
}
LUA_BINDING_END( "integer", "The contents." )

LUA_BINDING_BEGIN( PhysicsObject, GetDamping, "class", "Gets damping rates" )
{
    float speed, rot;
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetDamping( &speed, &rot );
    lua_pushnumber( L, speed );
    lua_pushnumber( L, rot );
    return 2;
}
LUA_BINDING_END( "number", "The speed damping rate.", "number", "The rotation damping rate." )

LUA_BINDING_BEGIN( PhysicsObject, GetEnergy, "class", "Gets energy" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetEnergy() );
    return 1;
}
LUA_BINDING_END( "number", "The energy." )

LUA_BINDING_BEGIN( PhysicsObject, GetGameFlags, "class", "Gets game flags" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetGameFlags() );
    return 1;
}
LUA_BINDING_END( "integer", "The game flags." )

LUA_BINDING_BEGIN( PhysicsObject, GetGameIndex, "class", "Gets game index" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetGameIndex() );
    return 1;
}
LUA_BINDING_END( "integer", "The game index." )

LUA_BINDING_BEGIN( PhysicsObject, GetImplicitVelocity, "class", "Gets implicit velocity and angular velocity" )
{
    Vector velocity, angularVelocity;
    LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetImplicitVelocity( &velocity, &angularVelocity );
    lua_pushvector( L, velocity );
    lua_pushvector( L, angularVelocity );
    return 2;
}
LUA_BINDING_END( "Vector", "The implicit velocity.", "Vector", "The implicit angular velocity." )

LUA_BINDING_BEGIN( PhysicsObject, GetInertia, "class", "Gets inertia" )
{
    lua_pushvector( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetInertia() );
    return 1;
}
LUA_BINDING_END( "Vector", "The inertia." )

LUA_BINDING_BEGIN( PhysicsObject, GetInverseInertia, "class", "Gets inverse inertia" )
{
    lua_pushvector( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetInvInertia() );
    return 1;
}
LUA_BINDING_END( "Vector", "The inverse inertia." )

LUA_BINDING_BEGIN( PhysicsObject, GetInverseMass, "class", "Gets inverse mass" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetInvMass() );
    return 1;
}
LUA_BINDING_END( "number", "The inverse mass." )

LUA_BINDING_BEGIN( PhysicsObject, GetMass, "class", "Gets mass" )
{
    lua_pushnumber( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetMass() );
    return 1;
}
LUA_BINDING_END( "number", "The mass." )

LUA_BINDING_BEGIN( PhysicsObject, GetMassCenterLocalSpace, "class", "Gets mass center in local space" )
{
    lua_pushvector( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetMassCenterLocalSpace() );
    return 1;
}
LUA_BINDING_END( "Vector", "The mass center in local space." )

LUA_BINDING_BEGIN( PhysicsObject, GetMaterialIndex, "class", "Gets material index" )
{
    lua_pushinteger( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetMaterialIndex() );
    return 1;
}
LUA_BINDING_END( "integer", "The material index." )

LUA_BINDING_BEGIN( PhysicsObject, GetName, "class", "Gets name" )
{
    lua_pushstring( L, LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" )->GetName() );
    return 1;
}
LUA_BINDING_END( "string", "The name." )

LUA_BINDING_BEGIN( PhysicsObject, GetPosition, "class", "Gets the world position and angles" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector worldPosition;
    QAngle angles;
    physicsObject->GetPosition( &worldPosition, &angles );
    lua_pushvector( L, worldPosition );
    lua_pushangle( L, angles );
    return 2;
}
LUA_BINDING_END( "Vector", "The world position.", "QAngle", "The angles." )

LUA_BINDING_BEGIN( PhysicsObject, GetShadowPosition, "class", "Gets the shadow position and angles" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector position;
    QAngle angles;
    lua_pushinteger( L, physicsObject->GetShadowPosition( &position, &angles ) );
    lua_pushvector( L, position );
    lua_pushangle( L, angles );
    return 3;
}
LUA_BINDING_END( "boolean", "Whether there is a shadow position or not.", "Vector", "The shadow position.", "QAngle", "The angles." )

LUA_BINDING_BEGIN( PhysicsObject, GetSphereRadius, "class", "Gets the sphere radius" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushnumber( L, physicsObject->GetSphereRadius() );
    return 1;
}
LUA_BINDING_END( "number", "The sphere radius." )

LUA_BINDING_BEGIN( PhysicsObject, GetVelocity, "class", "Gets the linear and angular velocity" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector velocity, angularVelocity;
    physicsObject->GetVelocity( &velocity, &angularVelocity );
    lua_pushvector( L, velocity );
    lua_pushvector( L, angularVelocity );
    return 2;
}
LUA_BINDING_END( "Vector", "The linear velocity.", "Vector", "The angular velocity." )

LUA_BINDING_BEGIN( PhysicsObject, GetVelocityAtPoint, "class", "Gets velocity at a specified point" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector pVelocity;
    Vector point = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "worldPosition" );
    physicsObject->GetVelocityAtPoint( point, &pVelocity );
    lua_pushvector( L, pVelocity );
    return 1;
}
LUA_BINDING_END( "Vector", "The velocity at the point." )

LUA_BINDING_BEGIN( PhysicsObject, IsAsleep, "class", "Checks if the object is asleep" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsAsleep() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the object is asleep or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsAttachedToConstraint, "class", "Checks if the object is attached to a constraint" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    bool externalOnly = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "externalOnly" );
    lua_pushboolean( L, physicsObject->IsAttachedToConstraint( externalOnly ) );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the object is attached to a constraint or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsCollisionEnabled, "class", "Checks if collision is enabled" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsCollisionEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether collision is enabled or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsDragEnabled, "class", "Checks if drag is enabled" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsDragEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether drag is enabled or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsFluid, "class", "Checks if the object is a fluid" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsFluid() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the object is a fluid or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsGravityEnabled, "class", "Checks if gravity is enabled" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsGravityEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether gravity is enabled or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsHinged, "class", "Checks if the object is hinged" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsHinged() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the object is hinged or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsMotionEnabled, "class", "Checks if motion is enabled" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsMotionEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether motion is enabled or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsMoveable, "class", "Checks if the object is moveable" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsMoveable() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the object is moveable or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsStatic, "class", "Checks if the object is static" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsStatic() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the object is static or not." )

LUA_BINDING_BEGIN( PhysicsObject, IsTrigger, "class", "Checks if the object is a trigger" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    lua_pushboolean( L, physicsObject->IsTrigger() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the object is a trigger or not." )

LUA_BINDING_BEGIN( PhysicsObject, LocalToWorld, "class", "Converts local position to world position" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector localPosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "localPosition" );
    Vector worldPosition;
    physicsObject->LocalToWorld( &worldPosition, localPosition );
    lua_pushvector( L, worldPosition );
    return 1;
}
LUA_BINDING_END( "Vector", "The world position." )

LUA_BINDING_BEGIN( PhysicsObject, LocalToWorldVector, "class", "Converts local vector to world vector" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector localVector = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "localVector" );
    Vector worldVector;
    physicsObject->LocalToWorldVector( &worldVector, localVector );
    lua_pushvector( L, worldVector );
    return 1;
}
LUA_BINDING_END( "Vector", "The world vector." )

LUA_BINDING_BEGIN( PhysicsObject, OutputDebugInfo, "class", "Outputs debug information" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    physicsObject->OutputDebugInfo();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, RecheckCollisionFilter, "class", "Rechecks the collision filter" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    physicsObject->RecheckCollisionFilter();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, RecheckContactPoints, "class", "Rechecks contact points" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    physicsObject->RecheckContactPoints();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, RemoveHinged, "class", "Removes hinged state" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    physicsObject->RemoveHinged();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, RemoveShadowController, "class", "Removes shadow controller" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    physicsObject->RemoveShadowController();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, RemoveTrigger, "class", "Removes trigger state" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    physicsObject->RemoveTrigger();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetBuoyancyRatio, "class", "Sets the buoyancy ratio" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    float ratio = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "ratio" );
    physicsObject->SetBuoyancyRatio( ratio );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetCallbackFlags, "class", "Sets callback flags" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    unsigned flags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );
    physicsObject->SetCallbackFlags( flags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetContents, "class", "Sets the contents" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    unsigned int contents = ( unsigned int )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "contents" );
    physicsObject->SetContents( contents );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetDragCoefficient, "class", "Sets drag coefficients" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    float pDrag = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "pDrag" );
    float pAngularDrag = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "pAngularDrag" );
    physicsObject->SetDragCoefficient( &pDrag, &pAngularDrag );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetGameFlags, "class", "Sets game-specific flags" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    unsigned flags = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "flags" );
    physicsObject->SetGameFlags( flags );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetGameIndex, "class", "Sets the game index" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    int index = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" );
    physicsObject->SetGameIndex( index );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetMass, "class", "Sets the mass" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    float mass = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "mass" );
    physicsObject->SetMass( mass );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetMaterialIndex, "class", "Sets the material index" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    int materialIndex = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "materialIndex" );
    physicsObject->SetMaterialIndex( materialIndex );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetShadow, "class", "Sets the shadow parameters" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    float maxSpeed = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "maxSpeed" );
    float maxAngularSpeed = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "maxAngularSpeed" );
    bool allowTranslation = LUA_BINDING_ARGUMENT( luaL_checkboolean, 4, "allowTranslation" );
    bool allowRotation = LUA_BINDING_ARGUMENT( luaL_checkboolean, 5, "allowRotation" );
    physicsObject->SetShadow( maxSpeed, maxAngularSpeed, allowTranslation, allowRotation );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetVelocity, "class", "Sets the velocity" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector linearVelocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "linearVelocity" );
    Vector angularVelocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "angularVelocity" );
    physicsObject->SetVelocity( &linearVelocity, &angularVelocity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, SetVelocityInstantaneous, "class", "Sets the velocity instantaneously" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector linearVelocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "linearVelocity" );
    Vector angularVelocity = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "angularVelocity" );
    physicsObject->SetVelocityInstantaneous( &linearVelocity, &angularVelocity );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, Sleep, "class", "Puts the object to sleep" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    physicsObject->Sleep();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, Wake, "class", "Wakes the object up" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    physicsObject->Wake();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( PhysicsObject, WorldToLocal, "class", "Converts world position to local position" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector localPosition;
    Vector worldPosition = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "worldPosition" );
    physicsObject->WorldToLocal( &localPosition, worldPosition );
    lua_pushvector( L, localPosition );
    return 1;
}
LUA_BINDING_END( "Vector", "The local position." )

LUA_BINDING_BEGIN( PhysicsObject, WorldToLocalVector, "class", "Converts world vector to local vector" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( luaL_checkphysicsobject, 1, "physicsObject" );
    Vector localVector;
    Vector worldVector = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "worldVector" );
    physicsObject->WorldToLocalVector( &localVector, worldVector );
    lua_pushvector( L, localVector );
    return 1;
}
LUA_BINDING_END( "Vector", "The local vector." )

LUA_BINDING_BEGIN( PhysicsObject, __tostring, "class", "Provides a string representation of the object" )
{
    lua_IPhysicsObject *physicsObject = LUA_BINDING_ARGUMENT( lua_tophysicsobject, 1, "physicsObject" );
    lua_pushfstring( L, "PhysicsObject: %p", physicsObject );
    return 1;
}
LUA_BINDING_END( "string", "The string representation." )

LUA_BINDING_BEGIN( PhysicsObject, __eq, "class", "Checks if two objects are equal" )
{
    lua_IPhysicsObject *physicsObject1 = LUA_BINDING_ARGUMENT( lua_tophysicsobject, 1, "physicsObject1" );
    lua_IPhysicsObject *physicsObject2 = LUA_BINDING_ARGUMENT( lua_tophysicsobject, 2, "physicsObject2" );
    lua_pushboolean( L, physicsObject1 == physicsObject2 );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the objects are equal or not." )

/*
** Open IPhysicsObject object
*/
LUALIB_API int luaopen_IPhysicsObject( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_PHYSICSOBJECTMETANAME );

    LUA_REGISTRATION_COMMIT( PhysicsObject );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_PHYSICSOBJECTMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "PhysicsObject" */

    return 1;
}

/*
** IPhysicsSurfaceProps Meta
*/

LUA_REGISTRATION_INIT( PhysicsSurfacePropertiesHandle );

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, GetPhysicsParameters, "class", "Gets physics parameters" )
{
    surfacephysicsparams_t pParamsOut;
    physprops->GetPhysicsParameters( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "surfaceData" ), &pParamsOut );
    lua_pushsurfacephysicsparams( L, &pParamsOut );
    return 1;
}
LUA_BINDING_END( "SurfacePhysicsParams", "The physics parameters." )

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, GetPhysicsProperties, "class", "Gets physics properties" )
{
    float density, thickness, friction, elasticity;
    physprops->GetPhysicsProperties( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "surfaceData" ), &density, &thickness, &friction, &elasticity );
    lua_pushnumber( L, density );
    lua_pushnumber( L, thickness );
    lua_pushnumber( L, friction );
    lua_pushnumber( L, elasticity );
    return 4;
}
LUA_BINDING_END( "number", "The density.", "number", "The thickness.", "number", "The friction.", "number", "The elasticity." )

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, GetPropName, "class", "Gets the property name" )
{
    lua_pushstring( L, physprops->GetPropName( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "surfaceData" ) ) );
    return 1;
}
LUA_BINDING_END( "string", "The property name." )

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, GetString, "class", "Gets the string" )
{
    lua_pushstring( L, physprops->GetString( ( unsigned short )LUA_BINDING_ARGUMENT( luaL_checkinteger, 1, "stringTableIndex" ) ) );
    return 1;
}
LUA_BINDING_END( "string", "The string." )

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, GetSurfaceData, "class", "Gets the surface data" )
{
    lua_pushsurfacedata( L, physprops->GetSurfaceData( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "surfaceData" ) ) );
    return 1;
}
LUA_BINDING_END( "SurfaceData", "The surface data." )

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, GetSurfaceIndex, "class", "Gets the surface index" )
{
    lua_pushinteger( L, physprops->GetSurfaceIndex( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "surfacePropName" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The surface index." )

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, ParseSurfaceData, "class", "Parses surface data" )
{
    lua_pushinteger( L, physprops->ParseSurfaceData( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "stringData" ), LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "textFileName" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The surface data." )

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, SurfacePropCount, "class", "Gets the surface property count" )
{
    lua_pushinteger( L, physprops->SurfacePropCount() );
    return 1;
}
LUA_BINDING_END( "integer", "The surface property count." )

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, __tostring, "class", "Provides a string representation of the object" )
{
    lua_pushfstring( L, "PhysicsSurfacePropertiesHandle: %p", physprops );
    return 1;
}
LUA_BINDING_END( "string", "The string representation." )

LUA_BINDING_BEGIN( PhysicsSurfacePropertiesHandle, __eq, "class", "Checks if two objects are equal" )
{
    IPhysicsSurfaceProps *physicsProps1 = LUA_BINDING_ARGUMENT( luaL_checkphysicssurfaceprops, 1, "physicsProps1" );
    IPhysicsSurfaceProps *physicsProps2 = LUA_BINDING_ARGUMENT( luaL_checkphysicssurfaceprops, 1, "physicsProps2" );
    lua_pushboolean( L, physicsProps1 == physicsProps2 );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the objects are equal or not." )

LUA_REGISTRATION_INIT( PhysicsSurfaceProperties );

LUA_BINDING_BEGIN( PhysicsSurfaceProperties, GetGlobal, "library", "Gets the global surface properties" )
{
    lua_pushphysicssurfaceprops( L, physprops );
    return 1;
}
LUA_BINDING_END( "PhysicsSurfacePropertiesHandle", "The global surface properties." )

/*
** Open IPhysicsSurfaceProps object
*/
LUALIB_API int luaopen_IPhysicsSurfaceProps( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_PHYSICSSURFACEPROPSMETANAME );

    LUA_REGISTRATION_COMMIT( PhysicsSurfacePropertiesHandle );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_PHYSICSSURFACEPROPSMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "PhysicsSurfacePropertiesHandle" */

    LUA_REGISTRATION_COMMIT_LIBRARY( PhysicsSurfaceProperties );

    return 1;
}
