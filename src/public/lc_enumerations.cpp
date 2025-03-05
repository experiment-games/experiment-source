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

    LUA_SET_ENUM_LIB_BEGIN( L, "RENDER_GROUP" );
    lua_pushenum( L, RENDER_GROUP_OPAQUE_STATIC_HUGE, "OPAQUE_STATIC_HUGE" );
    lua_pushenum( L, RENDER_GROUP_OPAQUE_ENTITY_HUGE, "OPAQUE_ENTITY_HUGE" );
    lua_pushenum( L, RENDER_GROUP_OPAQUE_STATIC, "OPAQUE_STATIC" );
    lua_pushenum( L, RENDER_GROUP_OPAQUE_ENTITY, "OPAQUE_ENTITY" );
    lua_pushenum( L, RENDER_GROUP_TRANSLUCENT_ENTITY, "TRANSLUCENT_ENTITY" );
    lua_pushenum( L, RENDER_GROUP_TWOPASS, "TWOPASS" );
    lua_pushenum( L, RENDER_GROUP_VIEW_MODEL_OPAQUE, "VIEW_MODEL_OPAQUE" );
    lua_pushenum( L, RENDER_GROUP_VIEW_MODEL_TRANSLUCENT, "VIEW_MODEL_TRANSLUCENT" );
    lua_pushenum( L, RENDER_GROUP_OPAQUE_BRUSH, "OPAQUE_BRUSH" );
    lua_pushenum( L, RENDER_GROUP_OTHER, "OTHER" );
    lua_pushenum( L, RENDER_GROUP_COUNT, "COUNT" );
    LUA_SET_ENUM_LIB_END( L );

    return 0;
}
