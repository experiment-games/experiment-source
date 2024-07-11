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
#include "materialsystem/imaterialproxy.h"

struct PngTexturePointer
{
};

class CPngMaterialProxy;

class CPngTextureRegen : public ITextureRegenerator
{
   public:
    CPngTextureRegen( CPngMaterialProxy *pProxy )
        : m_pProxy( pProxy ) {}

    virtual void RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect );
    virtual void Release() {}

   private:
    CPngMaterialProxy *m_pProxy;

   public:
    static CUtlMap< const char *, CPngMaterialProxy * > m_mapProceduralMaterials;

    static IMaterial *GetOrCreateProceduralMaterial( const char *pMaterialName, const char *filePath );
    static CPngMaterialProxy *GetProceduralMaterialProxy( const char *pMaterialName );
    static void ReleaseAllTextureData();
};

class CPngMaterialProxy : public IMaterialProxy
{
   public:
    CPngMaterialProxy();
    virtual ~CPngMaterialProxy();
    virtual bool Init( IMaterial *pMaterial, KeyValues *pKeyValues );
    virtual void OnBind( void *pBind );
    virtual void Release( void )
    {
        delete this;
    }

    virtual IMaterial *GetMaterial()
    {
        return m_pMaterial;
    }
    virtual unsigned char *GetTexturePointer() const
    {
        return m_pTexturePointer;
    }
    virtual int GetSizeInBytes() const
    {
        return m_iSizeInBytes;
    }

    void PreLoadTexture();
    void LoadTexture( ITexture *pTexture, IVTFTexture *pVTFTexture );

   private:
    IMaterial *m_pMaterial;
    CPngTextureRegen m_TextureRegen;
    IMaterialVar *m_pTextureVar;
    IMaterialVar *m_pFullPathVar;

    unsigned char *m_pTexturePointer;
    int m_iSizeInBytes;
};

unsigned char *PNG_ReadFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, int &colorType, int &bitDepth, int &sizeInBytes );
bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, int &colorType, int &bitDepth );

#endif  // C_PNG_H
