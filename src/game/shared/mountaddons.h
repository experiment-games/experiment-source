//========== Copyleft © 2011, Team Sandbox, Some rights reserved. ===========//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#ifndef MOUNTADDONS_H
#define MOUNTADDONS_H
#ifdef _WIN32
#pragma once
#endif

void MountAddon( const char *addonDirectoryName, const char *addonPath );
void MountAddons();
int GetMountedAddons( CUtlVector< CUtlString > &addons );
void SetupMountedAddons( lua_State *L );

#endif  // MOUNTADDONS_H
