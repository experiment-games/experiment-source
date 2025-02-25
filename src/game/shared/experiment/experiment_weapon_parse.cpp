//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include <KeyValues.h>
#include "experiment_weapon_parse.h"
#include "ammodef.h"

FileWeaponInfo_t *CreateWeaponInfo()
{
    return new CExperimentSWeaponInfo;
}

CExperimentSWeaponInfo::CExperimentSWeaponInfo()
{
    m_iPlayerDamage = 0;
}

void CExperimentSWeaponInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
    BaseClass::Parse( pKeyValuesData, szWeaponName );

    m_iPlayerDamage = pKeyValuesData->GetInt( "damage", 0 );
}
