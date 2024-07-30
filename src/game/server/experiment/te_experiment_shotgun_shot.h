//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef TE_Experiment_SHOTGUN_SHOT_H
#define TE_Experiment_SHOTGUN_SHOT_H
#ifdef _WIN32
#pragma once
#endif

void TE_ExperimentFireBullets(
    int iPlayerIndex,
    const Vector &vOrigin,
    const Vector &vDir,
    int iAmmoID,
    int iSeed,
    int iShots,
    float flSpread,
    bool bDoTracers,
    bool bDoImpacts );

#endif  // TE_Experiment_SHOTGUN_SHOT_H
