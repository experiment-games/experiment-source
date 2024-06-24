#ifndef LBUTTON_H
#define LBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Button.h>
#include "scripted_controls/lPanel.h"

namespace vgui
{

class LButton : public LPanel, public Button
{
	DECLARE_CLASS_SIMPLE( LButton, Button );

public:
	// You can optionally pass in the panel to send the click message to and the name of the command to send to that panel.
	LButton(Panel *parent, const char *panelName, const char *text, Panel *pActionSignalTarget=NULL, const char *pCmd=NULL, lua_State *L=NULL);

public:
    void DoClick( void );
};

} // namespace vgui

/* type for Button functions */
typedef vgui::Button lua_Button;



/*
** access functions (stack -> C)
*/

LUA_API lua_Button     *(lua_tobutton) (lua_State *L, int idx);


/*
** push functions (C -> stack)
*/
LUA_API void  (lua_pushbutton) (lua_State *L, lua_Button *pButton);



LUALIB_API lua_Button *(luaL_checkbutton) (lua_State *L, int narg);

#endif // LBUTTON_H
