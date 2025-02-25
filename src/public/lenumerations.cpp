#include "cbase.h"
#include "const.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "multiplayer_animstate.h"
#include <activitylist.h>

// Quick and easy place to register some enums
LUALIB_API int luaopen_ServerEnumerations( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, "AI_CLASS" );
    lua_pushenum( L, CLASS_NONE, "NONE" );
    lua_pushenum( L, CLASS_PLAYER, "PLAYER" );
    lua_pushenum( L, CLASS_PLAYER_ALLY, "PLAYER_ALLY" );
    lua_pushenum( L, CLASS_PLAYER_ALLY_VITAL, "PLAYER_ALLY_VITAL" );
    lua_pushenum( L, CLASS_ANTLION, "ANTLION" );
    lua_pushenum( L, CLASS_BARNACLE, "BARNACLE" );
    lua_pushenum( L, CLASS_BULLSEYE, "BULLSEYE" );
    lua_pushenum( L, CLASS_CITIZEN_PASSIVE, "CITIZEN_PASSIVE" );
    lua_pushenum( L, CLASS_CITIZEN_REBEL, "CITIZEN_REBEL" );
    lua_pushenum( L, CLASS_COMBINE, "COMBINE" );
    lua_pushenum( L, CLASS_COMBINE_GUNSHIP, "COMBINE_GUNSHIP" );
    lua_pushenum( L, CLASS_CONSCRIPT, "CONSCRIPT" );
    lua_pushenum( L, CLASS_HEADCRAB, "HEADCRAB" );
    lua_pushenum( L, CLASS_MANHACK, "MANHACK" );
    lua_pushenum( L, CLASS_METROPOLICE, "METROPOLICE" );
    lua_pushenum( L, CLASS_MILITARY, "MILITARY" );
    lua_pushenum( L, CLASS_SCANNER, "SCANNER" );
    lua_pushenum( L, CLASS_STALKER, "STALKER" );
    lua_pushenum( L, CLASS_VORTIGAUNT, "VORTIGAUNT" );
    lua_pushenum( L, CLASS_ZOMBIE, "ZOMBIE" );
    lua_pushenum( L, CLASS_PROTOSNIPER, "PROTOSNIPER" );
    lua_pushenum( L, CLASS_MISSILE, "MISSILE" );
    lua_pushenum( L, CLASS_FLARE, "FLARE" );
    lua_pushenum( L, CLASS_EARTH_FAUNA, "EARTH_FAUNA" );
    lua_pushenum( L, CLASS_HACKED_ROLLERMINE, "HACKED_ROLLERMINE" );
    lua_pushenum( L, CLASS_COMBINE_HUNTER, "COMBINE_HUNTER" );
    lua_pushenum( L, NUM_AI_CLASSES, "COUNT" );
    LUA_SET_ENUM_LIB_END( L );

    return 0;
}
