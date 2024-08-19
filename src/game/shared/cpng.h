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
#include <lColor.h>

class CPngMaterialProxy;

class CPngTextureRegen : public ITextureRegenerator
{
   public:
    CPngTextureRegen( CPngMaterialProxy *pProxy )
        : m_pProxy( pProxy ) {}

    virtual void RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect );
    virtual void Release()
    { }

   private:
    CPngMaterialProxy *m_pProxy;

   public:
    static CUtlMap< const char *, CPngMaterialProxy * > m_mapProceduralMaterials;

    static IMaterial *GetOrCreateProceduralMaterial(
        const char *pMaterialName,
        const char *pFilePath,
        KeyValues *pVMTKeyValues = nullptr,
        bool bSmooth = false );
    static lua_Color GetProceduralMaterialProxyColorAtPosition( const char *pMaterialName, int x, int y );
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
        return m_pTextureVar->GetOwningMaterial();
    }

    virtual int GetSizeInBytes() const
    {
        return m_iSizeInBytes;
    }

    void LoadTexture( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect );

    lua_Color GetColorAtPosition( int x, int y );

   private:
    void LoadTextureFromDisk();
    CPngTextureRegen m_TextureRegen;
    ITexture *m_pTexture;
    IMaterialVar *m_pTextureVar;
    IMaterialVar *m_pFullPathVar;
    bool m_bAllowRegenerating;

    unsigned char *m_pTexturePointer;
    bool m_bIsDirty;
    int m_iSizeInBytes;
    int m_nWidth;
    int m_nHeight;
};

unsigned char *PNG_ReadFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, int &colorType, int &bitDepth, int &sizeInBytes );
bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, int &colorType, int &bitDepth );
bool PNG_WriteToFile( const char *filePath, unsigned char *pImageData, int width, int height );

#endif  // C_PNG_H
