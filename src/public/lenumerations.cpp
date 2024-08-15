#include "cbase.h"
#include "const.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "multiplayer_animstate.h"
#include <activitylist.h>

// Example
///*
//** Open SOLID_* enum library
//*/
//LUALIB_API int luaopen_SOLID( lua_State *L )
//{
//    LUA_SET_ENUM_LIB_BEGIN( L, LUA_SOLIDLIBNAME );
//    lua_pushenum( L, SOLID_NONE, "NONE" );
//    lua_pushenum( L, SOLID_BSP, "BSP" );
//    lua_pushenum( L, SOLID_BBOX, "BBOX" );
//    lua_pushenum( L, SOLID_OBB, "OBB" );
//    lua_pushenum( L, SOLID_OBB_YAW, "OBB_YAW" );
//    lua_pushenum( L, SOLID_CUSTOM, "CUSTOM" );
//    lua_pushenum( L, SOLID_VPHYSICS, "VPHYSICS" );
//    lua_pushenum( L, SOLID_LAST, "LAST" );
//    LUA_SET_ENUM_LIB_END( L );
//    return 0;
//}
