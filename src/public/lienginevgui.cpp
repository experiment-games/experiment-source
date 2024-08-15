#include "cbase.h"
#include "ienginevgui.h"
#include "lua.hpp"
#include "luasrclib.h"

#include "scripted_controls/lPanel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Forward declarations.
namespace vgui
{
class Panel;
};

LUA_REGISTRATION_INIT( EngineVgui )

LUA_BINDING_BEGIN( EngineVgui, GetPanel, "library", "Get a VGUI panel instance by panel type." )
{
    VGuiPanel_t panelType = LUA_BINDING_ARGUMENT_ENUM( VGuiPanel_t, 1, "panelType" );
    Panel::PushVPanelLuaInstance( L, enginevgui->GetPanel( panelType ) );
    return 1;
}
LUA_BINDING_END( "VPanel", "The VGUI panel instance." )

LUA_BINDING_BEGIN( EngineVgui, IsGameUIVisible, "library", "Check if the game UI is currently visible." )
{
    lua_pushboolean( L, enginevgui->IsGameUIVisible() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the game UI is visible, otherwise false." )

/*
** Open enginevgui library
*/
LUALIB_API int luaopen_enginevgui( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( EngineVgui );
    return 1;
}
