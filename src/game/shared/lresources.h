//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Definitions that are shared by the game DLL and the client DLL.
//
// $NoKeywords: $
//=============================================================================//

#ifndef LRESOURCES_H
#define LRESOURCES_H
#ifdef _WIN32
#pragma once
#endif

int ResourcesGetFilesCount();
const char *ResourcesGetFile( int i );
void ResourcesFreeFiles();

#endif  // LRESOURCES_H
