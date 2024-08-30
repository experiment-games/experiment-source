#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include <engine/IEngineSound.h>
#include <hud_chat.h>
#include <hud_basechat.h>
#include <iclientmode.h>
#include <lColor.h>
#include <lbaseentity_shared.h>
#include <lbaseplayer_shared.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LUA_REGISTRATION_INIT( Chats )

LUA_BINDING_BEGIN( Chats, PlaySound, "library", "Play the 'tick' chat sound." )
{
    CLocalPlayerFilter filter;
    C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, "HudChat.Message" );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Chats, StartMessageMode, "library", "Open the chat with the specified message mode." )
{
    CHAT_MESSAGE_MODE mode = LUA_BINDING_ARGUMENT_ENUM( CHAT_MESSAGE_MODE, 1, "messageMode" );
    CBaseHudChat *hudChat = ( CBaseHudChat * )GET_HUDELEMENT( CHudChat );

    if ( hudChat )
    {
        hudChat->StartMessageMode( mode );
    }
    else
    {
        Assert( 0 ); // does this happen?
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Chats, StopMessageMode, "library", "Close the chat." )
{
    CBaseHudChat *hudChat = ( CBaseHudChat * )GET_HUDELEMENT( CHudChat );

    if ( hudChat )
    {
        hudChat->StopMessageMode();
    }
    else
    {
        Assert( 0 );  // does this happen?
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Chats, GetChatBoxPosition, "library", "Get the position of the chat box." )
{
    CBaseHudChat *hudChat = ( CBaseHudChat * )GET_HUDELEMENT( CHudChat );

    if ( hudChat )
    {
        int x, y;
        hudChat->GetPos( x, y );
        lua_pushnumber( L, x );
        lua_pushnumber( L, y );
        return 2;
    }

    Assert( 0 );  // does this happen?
    lua_pushnil( L );
    lua_pushnil( L );
    return 2;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Chats, GetChatBoxSize, "library", "Get the size of the chat box." )
{
    CBaseHudChat *hudChat = ( CBaseHudChat * )GET_HUDELEMENT( CHudChat );

    if ( hudChat )
    {
        int w, h;
        hudChat->GetSize( w, h );
        lua_pushnumber( L, w );
        lua_pushnumber( L, h );
        return 2;
    }

    Assert( 0 );  // does this happen?
    lua_pushnil( L );
    lua_pushnil( L );
    return 2;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Chats, AddText, "library", "Adds (optionally colored) text to the local chatbox" )
{
    CBaseHudChat *hudChat = ( CBaseHudChat * )GET_HUDELEMENT( CHudChat );

    if ( !hudChat )
    {
        Assert( 0 );  // does this happen?
    }

    int nArgs = lua_gettop( L );

    for ( int i = 1; i <= nArgs; i++ )
    {
        if ( lua_iscolor( L, i ) )
        {
            Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, i, "color" );
            hudChat->SetCustomColor( color );
        }
        if ( lua_isstring( L, i ) )
        {
            hudChat->ChatPrintf( 0, CHAT_FILTER_NONE, "%s", lua_tostring( L, i ) );
        }
        else if ( lua_toentity( L, i ) && lua_toentity( L, i )->IsPlayer() )
        {
            CBasePlayer *player = LUA_BINDING_ARGUMENT( luaL_checkplayer, i, "player" );
            hudChat->ChatPrintf( player->entindex(), CHAT_FILTER_NONE, "%s", player->GetPlayerName() );
        }
        else
        {
            hudChat->ChatPrintf( 0, CHAT_FILTER_NONE, "%s", lua_tostring( L, i ) );
        }
    }

    return 0;
}
LUA_BINDING_END()

/*
** Open Chats library
*/
LUALIB_API int luaopen_Chats( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Chats );

    LUA_SET_ENUM_LIB_BEGIN( L, "CHAT_MESSAGE_MODE" );
    lua_pushenum( L, CHAT_MESSAGE_MODE::MM_NONE, "NONE" );
    lua_pushenum( L, CHAT_MESSAGE_MODE::MM_SAY, "GLOBAL" );
    lua_pushenum( L, CHAT_MESSAGE_MODE::MM_SAY_TEAM, "TEAM" );
    LUA_SET_ENUM_LIB_END( L );

    return 1;
}
