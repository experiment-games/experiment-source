#include "cbase.h"
#include "const.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "multiplayer_animstate.h"
#include <activitylist.h>

// Quick and easy place to register some enums
LUALIB_API int luaopen_ClientEnumerations( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, "COLLIDE_TYPE" );
    lua_pushenum( L, CollideType_t::ENTITY_SHOULD_NOT_COLLIDE, "SHOULD_NOT_COLLIDE" );
    lua_pushenum( L, CollideType_t::ENTITY_SHOULD_COLLIDE, "SHOULD_COLLIDE" );
    lua_pushenum( L, CollideType_t::ENTITY_SHOULD_RESPOND, "SHOULD_RESPOND" );
    LUA_SET_ENUM_LIB_END( L );

    LUA_SET_ENUM_LIB_BEGIN( L, "SHADOW_TYPE" );
    lua_pushenum( L, SHADOWS_NONE, "NONE" );
    lua_pushenum( L, SHADOWS_SIMPLE, "SIMPLE" );
    lua_pushenum( L, SHADOWS_RENDER_TO_TEXTURE, "RENDER_TO_TEXTURE" );
    lua_pushenum( L, SHADOWS_RENDER_TO_TEXTURE_DYNAMIC, "RENDER_TO_TEXTURE_DYNAMIC" );
    lua_pushenum( L, SHADOWS_RENDER_TO_DEPTH_TEXTURE, "RENDER_TO_DEPTH_TEXTURE" );
    LUA_SET_ENUM_LIB_END( L );

    LUA_SET_ENUM_LIB_BEGIN( L, "SKYBOX_VISIBILITY" );
    lua_pushenum( L, SKYBOX_NOT_VISIBLE, "NOT_VISIBLE" );
    lua_pushenum( L, SKYBOX_3DSKYBOX_VISIBLE, "3D_VISIBLE" );
    lua_pushenum( L, SKYBOX_2DSKYBOX_VISIBLE, "2D_VISIBLE" );
    LUA_SET_ENUM_LIB_END( L );

    return 0;
}
