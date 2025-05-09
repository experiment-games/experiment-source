#include "cbase.h"
#include "lua.hpp"
#include "lControls.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** Open vgui library
*/
LUALIB_API int luaopen_vgui( lua_State *L )
{
    luaopen_vgui_Label( L );
    luaopen_vgui_Button( L );
    luaopen_vgui_EditablePanel( L );
    luaopen_vgui_Panel( L );
    luaopen_vgui_CheckButton( L );
    luaopen_vgui_Frame( L );
    luaopen_vgui_HTML( L );
    luaopen_vgui_TextEntry( L );
    luaopen_vgui_ModelImagePanel( L );
    return 0;
}
