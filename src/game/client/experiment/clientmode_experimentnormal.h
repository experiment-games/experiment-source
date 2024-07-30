//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( CLIENTMODE_HLNORMAL_H )
#define CLIENTMODE_HLNORMAL_H
#ifdef _WIN32
#pragma once
#endif

#include "clientmode_shared.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui/Cursor.h>

#ifdef LUA_SDK
class CScriptedHudViewport;
#endif
class CHudViewport;
#ifdef LUA_SDK
class CScriptedClientLuaPanel;
#endif

namespace vgui
{
typedef unsigned long HScheme;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class ClientModeExperimentNormal : public ClientModeShared
{
   public:
    DECLARE_CLASS( ClientModeExperimentNormal, ClientModeShared );

    ClientModeExperimentNormal();
    ~ClientModeExperimentNormal();

    virtual void Init();
    virtual int GetDeathMessageStartHeight( void );
};

extern IClientMode* GetClientModeNormal();
extern vgui::HScheme g_hVGuiCombineScheme;

extern ClientModeExperimentNormal* GetClientModeExperimentNormal();

#endif  // CLIENTMODE_HLNORMAL_H
