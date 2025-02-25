#ifndef MAPLOAD_BACKGROUND_H
#define MAPLOAD_BACKGROUND_H

#ifdef _WIN32
#pragma once
#endif

#include "vgui\ISurface.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls\ImagePanel.h>
#include "ienginevgui.h"

// Source: https://developer.valvesoftware.com/wiki/Custom_loading_screen
class CMapLoadBG : public vgui::EditablePanel
{
    DECLARE_CLASS_SIMPLE( CMapLoadBG, vgui::EditablePanel );

   public:
    CMapLoadBG( char const *panelName );
    ~CMapLoadBG();

    void SetNewBackgroundImage( char const *imageName );
    void OnMessage( const KeyValues *params, vgui::VPANEL fromPanel );
    void OnFileReceived( const char *fileName, unsigned int transferID );

   protected:
    void ApplySchemeSettings( vgui::IScheme *pScheme );

   private:
    vgui::ImagePanel *m_pBackground;
};

static CMapLoadBG *g_pMapLoadingPanel;

#endif  // !MAPLOAD_BACKGROUND_H
