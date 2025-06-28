#ifndef LAVATAR_IMAGE_H
#define LAVATAR_IMAGE_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_avatarimage.h>
#include "scripted_controls/lPanel.h"

namespace vgui
{

class LAvatarImage : public CAvatarImagePanel
{
    DECLARE_CLASS_SIMPLE( LAvatarImage, CAvatarImagePanel );

    LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( LAvatarImage, "AvatarImage" );

   public:
    LAvatarImage( Panel *parent, const char *panelName, lua_State *L = NULL );
};
}  // namespace vgui

/* type for AvatarImage functions */
typedef LAvatarImage lua_AvatarImage;

/*
** access functions (stack -> C)
*/

LUA_API lua_AvatarImage *( lua_toavatarImage )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_AvatarImage *( luaL_checkavatarImage )( lua_State *L, int narg );

#endif  // LAVATAR_IMAGE_H
