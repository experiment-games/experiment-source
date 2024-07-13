#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "llocalization.h"
#include "vgui/ILocalize.h"
#include "tier3/tier3.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static int localization_Find( lua_State *L )
{
    const char *token = luaL_checkstring( L, 1 );
    const char *translation = g_pVGuiLocalize->FindAsUTF8( token );

    lua_pushstring( L, translation );
    return 1;
}

static int localization_AddString( lua_State *L )
{
    const char *token = luaL_checkstring( L, 1 );
    const char *translation = luaL_checkstring( L, 2 );
    wchar_t translationBuffer[4096];

    translationBuffer[0] = 0;
    V_UTF8ToUnicode( translation, translationBuffer, sizeof( translationBuffer ) );

    g_pVGuiLocalize->AddString( token, translationBuffer, "" );
    return 0;
}

static const luaL_Reg localizationLib[] = {
    { "Find", localization_Find },
    { "AddString", localization_AddString },
    { NULL, NULL } };

/*
** Open localization library
*/
LUALIB_API int luaopen_localization( lua_State *L )
{
    luaL_register( L, LUA_LOCALIZATIONLIBNAME, localizationLib );
    return 1;
}
