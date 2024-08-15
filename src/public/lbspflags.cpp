#include "cbase.h"
#include "bspflags.h"
#include "luamanager.h"
#include "luasrclib.h"

/*
** Open CONTENTS library
*/
LUALIB_API int luaopen_CONTENTS( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_CONTENTSLIBNAME );
    lua_pushenum( L, CONTENTS_EMPTY, "EMPTY" );

    lua_pushenum( L, CONTENTS_SOLID, "SOLID" );
    lua_pushenum( L, CONTENTS_WINDOW, "WINDOW" );
    lua_pushenum( L, CONTENTS_AUX, "AUX" );
    lua_pushenum( L, CONTENTS_GRATE, "GRATE" );
    lua_pushenum( L, CONTENTS_SLIME, "SLIME" );
    lua_pushenum( L, CONTENTS_WATER, "WATER" );
    lua_pushenum( L, CONTENTS_BLOCKLOS, "BLOCK_LINE_OF_SIGHT" );
    lua_pushenum( L, CONTENTS_OPAQUE, "OPAQUE" );

    lua_pushenum( L, CONTENTS_TESTFOGVOLUME, "TEST_FOG_VOLUME" );
    lua_pushenum( L, CONTENTS_UNUSED, "UNUSED" );

    lua_pushenum( L, CONTENTS_TEAM1, "TEAM1" );
    lua_pushenum( L, CONTENTS_TEAM2, "TEAM2" );

    lua_pushenum( L, CONTENTS_IGNORE_NODRAW_OPAQUE, "IGNORE_NODRAW_OPAQUE" );

    lua_pushenum( L, CONTENTS_MOVEABLE, "MOVEABLE" );

    lua_pushenum( L, CONTENTS_AREAPORTAL, "AREA_PORTAL" );

    lua_pushenum( L, CONTENTS_PLAYERCLIP, "PLAYER_CLIP" );
    lua_pushenum( L, CONTENTS_MONSTERCLIP, "MONSTER_CLIP" );

    lua_pushenum( L, CONTENTS_CURRENT_0, "CURRENT_0" );
    lua_pushenum( L, CONTENTS_CURRENT_90, "CURRENT_90" );
    lua_pushenum( L, CONTENTS_CURRENT_180, "CURRENT_180" );
    lua_pushenum( L, CONTENTS_CURRENT_270, "CURRENT_270" );
    lua_pushenum( L, CONTENTS_CURRENT_UP, "CURRENT_UP" );
    lua_pushenum( L, CONTENTS_CURRENT_DOWN, "CURRENT_DOWN" );

    lua_pushenum( L, CONTENTS_ORIGIN, "ORIGIN" );

    lua_pushenum( L, CONTENTS_MONSTER, "MONSTER" );
    lua_pushenum( L, CONTENTS_DEBRIS, "DEBRIS" );
    lua_pushenum( L, CONTENTS_DETAIL, "DETAIL" );
    lua_pushenum( L, CONTENTS_TRANSLUCENT, "TRANSLUCENT" );
    lua_pushenum( L, CONTENTS_LADDER, "LADDER" );
    lua_pushenum( L, CONTENTS_HITBOX, "HITBOX" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open SURF library
*/
LUALIB_API int luaopen_SURF( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_SURFENUMNAME );
    lua_pushenum( L, SURF_LIGHT, "LIGHT" );
    lua_pushenum( L, SURF_SKY2D, "SKY_2D" );
    lua_pushenum( L, SURF_SKY, "SKY" );
    lua_pushenum( L, SURF_WARP, "WARP" );
    lua_pushenum( L, SURF_TRANS, "TRANSPARENT" );
    lua_pushenum( L, SURF_NOPORTAL, "NO_PORTAL" );
    lua_pushenum( L, SURF_TRIGGER, "TRIGGER" );
    lua_pushenum( L, SURF_NODRAW, "NO_DRAW" );

    lua_pushenum( L, SURF_HINT, "HINT" );

    lua_pushenum( L, SURF_SKIP, "SKIP" );
    lua_pushenum( L, SURF_NOLIGHT, "NO_LIGHT" );
    lua_pushenum( L, SURF_BUMPLIGHT, "BUMP_LIGHT" );
    lua_pushenum( L, SURF_NOSHADOWS, "NO_SHADOWS" );
    lua_pushenum( L, SURF_NODECALS, "NO_DECALS" );
    lua_pushenum( L, SURF_NOCHOP, "NO_CHOP" );
    lua_pushenum( L, SURF_HITBOX, "HITBOX" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}

/*
** Open MASK library
*/
LUALIB_API int luaopen_MASK( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_MASKLIBNAME );
    lua_pushenum( L, MASK_ALL, "ALL" );
    lua_pushenum( L, MASK_SOLID, "SOLID" );
    lua_pushenum( L, MASK_PLAYERSOLID, "PLAYER_SOLID" );
    lua_pushenum( L, MASK_NPCSOLID, "NPC_SOLID" );
    lua_pushenum( L, MASK_WATER, "WATER" );
    lua_pushenum( L, MASK_OPAQUE, "OPAQUE" );
    lua_pushenum( L, MASK_OPAQUE_AND_NPCS, "OPAQUE_AND_NPCS" );
    lua_pushenum( L, MASK_BLOCKLOS, "BLOCK_LINE_OF_SIGHT" );
    lua_pushenum( L, MASK_BLOCKLOS_AND_NPCS, "BLOCK_LINE_OF_SIGHT_AND_NPCS" );
    lua_pushenum( L, MASK_VISIBLE, "VISIBLE" );
    lua_pushenum( L, MASK_VISIBLE_AND_NPCS, "VISIBLE_AND_NPCS" );
    lua_pushenum( L, MASK_SHOT, "SHOT" );
    lua_pushenum( L, MASK_SHOT_HULL, "SHOT_HULL" );
    lua_pushenum( L, MASK_SHOT_PORTAL, "SHOT_PORTAL" );
    lua_pushenum( L, MASK_SOLID_BRUSHONLY, "SOLID_BRUSH_ONLY" );
    lua_pushenum( L, MASK_PLAYERSOLID_BRUSHONLY, "PLAYER_SOLID_BRUSH_ONLY" );
    lua_pushenum( L, MASK_NPCSOLID_BRUSHONLY, "NPC_SOLID_BRUSH_ONLY" );
    lua_pushenum( L, MASK_NPCWORLDSTATIC, "NPC_WORLD_STATIC" );
    lua_pushenum( L, MASK_SPLITAREAPORTAL, "SPLIT_AREA_PORTAL" );
    lua_pushenum( L, MASK_CURRENT, "CURRENT" );
    lua_pushenum( L, MASK_DEADSOLID, "DEAD_SOLID" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}
