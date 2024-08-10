#include "cbase.h"
#include "const.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "multiplayer_animstate.h"

/*
** Open engine flags FL_* enum library (see CBaseEntity::m_fFlags)
*/
LUALIB_API int luaopen_FL( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_ENGINEFLAGSLIBNAME );
    lua_pushenum( L, FL_ONGROUND, "ONGROUND" );
    lua_pushenum( L, FL_DUCKING, "DUCKING" );
    lua_pushenum( L, FL_ANIMDUCKING, "ANIMDUCKING" );
    lua_pushenum( L, FL_WATERJUMP, "WATERJUMP" );
    lua_pushenum( L, FL_ONTRAIN, "ONTRAIN" );
    lua_pushenum( L, FL_INRAIN, "INRAIN" );
    lua_pushenum( L, FL_FROZEN, "FROZEN" );
    lua_pushenum( L, FL_ATCONTROLS, "ATCONTROLS" );
    lua_pushenum( L, FL_CLIENT, "CLIENT" );
    lua_pushenum( L, FL_FAKECLIENT, "FAKECLIENT" );
    lua_pushenum( L, FL_INWATER, "INWATER" );
    lua_pushenum( L, FL_FLY, "FLY" );
    lua_pushenum( L, FL_SWIM, "SWIM" );
    lua_pushenum( L, FL_CONVEYOR, "CONVEYOR" );
    lua_pushenum( L, FL_NPC, "NPC" );
    lua_pushenum( L, FL_GODMODE, "GODMODE" );
    lua_pushenum( L, FL_NOTARGET, "NOTARGET" );
    lua_pushenum( L, FL_AIMTARGET, "AIMTARGET" );
    lua_pushenum( L, FL_PARTIALGROUND, "PARTIALGROUND" );
    lua_pushenum( L, FL_STATICPROP, "STATICPROP" );
    lua_pushenum( L, FL_GRAPHED, "GRAPHED" );
    lua_pushenum( L, FL_GRENADE, "GRENADE" );
    lua_pushenum( L, FL_STEPMOVEMENT, "STEPMOVEMENT" );
    lua_pushenum( L, FL_DONTTOUCH, "DONTTOUCH" );
    lua_pushenum( L, FL_BASEVELOCITY, "BASEVELOCITY" );
    lua_pushenum( L, FL_WORLDBRUSH, "WORLDBRUSH" );
    lua_pushenum( L, FL_OBJECT, "OBJECT" );
    lua_pushenum( L, FL_KILLME, "KILLME" );
    lua_pushenum( L, FL_ONFIRE, "ONFIRE" );
    lua_pushenum( L, FL_DISSOLVING, "DISSOLVING" );
    lua_pushenum( L, FL_TRANSRAGDOLL, "TRANSRAGDOLL" );
    lua_pushenum( L, FL_UNBLOCKABLE_BY_PLAYER, "UNBLOCKABLE_BY_PLAYER" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open OBS_MODE_* enum library
*/
LUALIB_API int luaopen_OBS_MODE( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_OBSMODELIBNAME );
    lua_pushenum( L, OBS_MODE_NONE, "NONE" );
    lua_pushenum( L, OBS_MODE_DEATHCAM, "DEATHCAM" );
    lua_pushenum( L, OBS_MODE_FREEZECAM, "FREEZECAM" );
    lua_pushenum( L, OBS_MODE_FIXED, "FIXED" );
    lua_pushenum( L, OBS_MODE_IN_EYE, "IN_EYE" );
    lua_pushenum( L, OBS_MODE_CHASE, "CHASE" );
    lua_pushenum( L, OBS_MODE_POI, "POI" );
    lua_pushenum( L, OBS_MODE_ROAMING, "ROAMING" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open GESTURE_SLOT enum library
*/
LUALIB_API int luaopen_GESTURE_SLOT( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_GESTURESLOTLIBNAME );
    lua_pushenum( L, GESTURE_SLOT_ATTACK_AND_RELOAD, "ATTACK_AND_RELOAD" );
    lua_pushenum( L, GESTURE_SLOT_GRENADE, "GRENADE" );
    lua_pushenum( L, GESTURE_SLOT_JUMP, "JUMP" );
    lua_pushenum( L, GESTURE_SLOT_SWIM, "SWIM" );
    lua_pushenum( L, GESTURE_SLOT_FLINCH, "FLINCH" );
    lua_pushenum( L, GESTURE_SLOT_VCD, "VCD" );
    lua_pushenum( L, GESTURE_SLOT_CUSTOM, "CUSTOM" );
    lua_pushenum( L, GESTURE_SLOT_COUNT, "COUNT" );
    lua_pushenum( L, GESTURE_SLOT_INVALID, "INVALID" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open MOVETYPE_* enum library
*/
LUALIB_API int luaopen_MOVETYPE( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_MOVETYPELIBNAME );
    lua_pushenum( L, MOVETYPE_NONE, "NONE" );
    lua_pushenum( L, MOVETYPE_ISOMETRIC, "ISOMETRIC" );
    lua_pushenum( L, MOVETYPE_WALK, "WALK" );
    lua_pushenum( L, MOVETYPE_STEP, "STEP" );
    lua_pushenum( L, MOVETYPE_FLY, "FLY" );
    lua_pushenum( L, MOVETYPE_FLYGRAVITY, "FLYGRAVITY" );
    lua_pushenum( L, MOVETYPE_VPHYSICS, "VPHYSICS" );
    lua_pushenum( L, MOVETYPE_PUSH, "PUSH" );
    lua_pushenum( L, MOVETYPE_NOCLIP, "NOCLIP" );
    lua_pushenum( L, MOVETYPE_LADDER, "LADDER" );
    lua_pushenum( L, MOVETYPE_OBSERVER, "OBSERVER" );
    lua_pushenum( L, MOVETYPE_CUSTOM, "CUSTOM" );
    lua_pushenum( L, MOVETYPE_LAST, "LAST" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open MOVECOLLIDE_* enum library
*/
LUALIB_API int luaopen_MOVECOLLIDE( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_MOVECOLLIDELIBNAME );
    lua_pushenum( L, MOVECOLLIDE_DEFAULT, "DEFAULT" );
    lua_pushenum( L, MOVECOLLIDE_FLY_BOUNCE, "FLY_BOUNCE" );
    lua_pushenum( L, MOVECOLLIDE_FLY_CUSTOM, "FLY_CUSTOM" );
    lua_pushenum( L, MOVECOLLIDE_FLY_SLIDE, "FLY_SLIDE" );
    lua_pushenum( L, MOVECOLLIDE_COUNT, "COUNT" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open SOLID_* enum library
*/
LUALIB_API int luaopen_SOLID( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_SOLIDLIBNAME );
    lua_pushenum( L, SOLID_NONE, "NONE" );
    lua_pushenum( L, SOLID_BSP, "BSP" );
    lua_pushenum( L, SOLID_BBOX, "BBOX" );
    lua_pushenum( L, SOLID_OBB, "OBB" );
    lua_pushenum( L, SOLID_OBB_YAW, "OBB_YAW" );
    lua_pushenum( L, SOLID_CUSTOM, "CUSTOM" );
    lua_pushenum( L, SOLID_VPHYSICS, "VPHYSICS" );
    lua_pushenum( L, SOLID_LAST, "LAST" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open SOLIDFLAG_* enum library
*/
LUALIB_API int luaopen_SOLIDFLAG( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_SOLIDFLAGLIBNAME );
    lua_pushenum( L, FSOLID_CUSTOMRAYTEST, "CUSTOMRAYTEST" );
    lua_pushenum( L, FSOLID_CUSTOMBOXTEST, "CUSTOMBOXTEST" );
    lua_pushenum( L, FSOLID_NOT_SOLID, "NOT_SOLID" );
    lua_pushenum( L, FSOLID_TRIGGER, "TRIGGER" );
    lua_pushenum( L, FSOLID_NOT_STANDABLE, "NOT_STANDABLE" );
    lua_pushenum( L, FSOLID_VOLUME_CONTENTS, "VOLUME_CONTENTS" );
    lua_pushenum( L, FSOLID_FORCE_WORLD_ALIGNED, "FORCE_WORLD_ALIGNED" );
    lua_pushenum( L, FSOLID_USE_TRIGGER_BOUNDS, "USE_TRIGGER_BOUNDS" );
    lua_pushenum( L, FSOLID_ROOT_PARENT_ALIGNED, "ROOT_PARENT_ALIGNED" );
    lua_pushenum( L, FSOLID_TRIGGER_TOUCH_DEBRIS, "TRIGGER_TOUCH_DEBRIS" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open LIFE_* enum library
*/
LUALIB_API int luaopen_LIFE( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_LIFELIBNAME );
    lua_pushenum( L, LIFE_ALIVE, "ALIVE" );
    lua_pushenum( L, LIFE_DYING, "DYING" );
    lua_pushenum( L, LIFE_DEAD, "DEAD" );
    lua_pushenum( L, LIFE_RESPAWNABLE, "RESPAWNABLE" );
    lua_pushenum( L, LIFE_DISCARDBODY, "DISCARDBODY" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open EF_* enum library
*/
LUALIB_API int luaopen_EF( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_EFLIBNAME );
    lua_pushenum( L, EF_BONEMERGE, "BONEMERGE" );
    lua_pushenum( L, EF_BRIGHTLIGHT, "BRIGHTLIGHT" );
    lua_pushenum( L, EF_DIMLIGHT, "DIMLIGHT" );
    lua_pushenum( L, EF_NOINTERP, "NOINTERP" );
    lua_pushenum( L, EF_NOSHADOW, "NOSHADOW" );
    lua_pushenum( L, EF_NODRAW, "NODRAW" );
    lua_pushenum( L, EF_NORECEIVESHADOW, "NORECEIVESHADOW" );
    lua_pushenum( L, EF_BONEMERGE_FASTCULL, "BONEMERGE_FASTCULL" );
    lua_pushenum( L, EF_ITEM_BLINK, "ITEM_BLINK" );
    lua_pushenum( L, EF_PARENT_ANIMATES, "PARENT_ANIMATES" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open FL_EDICT_* enum library
*/
LUALIB_API int luaopen_FL_EDICT( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_FLEDICTLIBNAME );
    lua_pushenum( L, FL_EDICT_CHANGED, "CHANGED" );
    lua_pushenum( L, FL_EDICT_FREE, "FREE" );
    lua_pushenum( L, FL_EDICT_FULL, "FULL" );
    lua_pushenum( L, FL_EDICT_FULLCHECK, "FULLCHECK" );
    lua_pushenum( L, FL_EDICT_ALWAYS, "ALWAYS" );
    lua_pushenum( L, FL_EDICT_DONTSEND, "DONTSEND" );
    lua_pushenum( L, FL_EDICT_PVSCHECK, "PVSCHECK" );
    lua_pushenum( L, FL_EDICT_PENDING_DORMANT_CHECK, "PENDING_DORMANT_CHECK" );
    lua_pushenum( L, FL_EDICT_DIRTY_PVS_INFORMATION, "DIRTY_PVS_INFORMATION" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}
