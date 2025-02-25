//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef Experiment_WEAPON_PARSE_H
#define Experiment_WEAPON_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_parse.h"
#include "networkvar.h"

//--------------------------------------------------------------------------------------------------------
class CExperimentSWeaponInfo : public FileWeaponInfo_t
{
   public:
    DECLARE_CLASS_GAMEROOT( CExperimentSWeaponInfo, FileWeaponInfo_t );

    CExperimentSWeaponInfo();

    virtual void Parse( ::KeyValues *pKeyValuesData, const char *szWeaponName );

   public:
    int m_iPlayerDamage;
};

#endif  // Experiment_WEAPON_PARSE_H
