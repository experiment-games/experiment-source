#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "llocalization.h"
#include "vgui/ILocalize.h"
#include "tier3/tier3.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Localizations );

LUA_BINDING_BEGIN( Localizations, Find, "library", "Finds a localized string." )
{
    const char *token = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "string" );
    const char *translation = g_pVGuiLocalize->FindAsUTF8( token );

    lua_pushstring( L, translation );
    return 1;
}
LUA_BINDING_END( "string", "Returns the localized string." )

LUA_BINDING_BEGIN( Localizations, AddString, "library", "Adds a localized string." )
{
    const char *token = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "string" );
    const char *translation = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "string" );
    wchar_t translationBuffer[4096];

    translationBuffer[0] = 0;
    V_UTF8ToUnicode( translation, translationBuffer, sizeof( translationBuffer ) );

    g_pVGuiLocalize->AddString( token, translationBuffer, "" );
    return 0;
}
LUA_BINDING_END()

/*
** Open localization library
*/
LUALIB_API int luaopen_localization( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Localizations );

    return 1;
}
