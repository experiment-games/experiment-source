//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Provides access to cvars that are bounded in the client DLL.
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_PNG_H
#define C_PNG_H
#ifdef _WIN32
#pragma once
#endif

#include <bitmap/bitmap.h>
#include "utlmap.h"
#include "utlbuffer.h"
#include "materialsystem/itexture.h"
#include <materialsystem/imaterial.h>
#include <materialsystem/imaterialsystem.h>
#include "materialsystem/imaterialvar.h"

class CPngTextureRegen : public ITextureRegenerator
{
   public:
    CPngTextureRegen( const char *pFileName );

    virtual void RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect );
    virtual void Release()
    {
        delete this;
    }

   private:
    char m_pFileName[MAX_PATH];

    public:
        static CUtlVector< IMaterial * > m_vecProceduralMaterials;
        static IMaterial *GetOrCreateProceduralMaterial( const char *materialName, const char *filePath );
        static void ReleaseAllTextureData();
        
};

unsigned char *PNG_ReadFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, int &colorType, int &bitDepth, int &sizeInBytes );
bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, int &colorType, int &bitDepth );

#endif  // C_PNG_H
