//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#include "cbase.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "filesystem.h"
#include "cpng.h"
#include "strtools.h"
#include "materialsystem/imaterialsystem.h"
#include "datacache/idatacache.h"

#ifdef CLIENT_DLL
#include "libpng/png.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CUtlMap< const char *, CPngMaterialProxy * > CPngTextureRegen::m_mapProceduralMaterials;

void CPngTextureRegen::RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect )
{
    m_pProxy->LoadTexture( pTexture, pVTFTexture );
}

#ifdef CLIENT_DLL
static void PNG_ReadData( png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead )
{
    CUtlBuffer *buffer = ( CUtlBuffer * )png_get_io_ptr( png_ptr );
    buffer->Get( outBytes, byteCountToRead );
}

unsigned char *PNG_ReadFromBuffer( CUtlBuffer &buffer, const char *pFilePath, int &width, int &height, int &colorType, int &bitDepth, int &sizeInBytes )
{
    png_const_bytep fileHeader = ( png_const_bytep )buffer.Base();

    if ( png_sig_cmp( fileHeader, 0, 8 ) )
    {
        Warning( "Failed loading PNG as Material! File is missing PNG header \"%s\"\n", pFilePath );
        return NULL;
    }

    png_structp readPointer = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    png_infop infoPointer = png_create_info_struct( readPointer );

    if ( setjmp( png_jmpbuf( readPointer ) ) )
    {
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        Warning( "Failed loading PNG as Material! Error during PNG creation \"%s\"\n", pFilePath );
        return NULL;
    }

    png_set_read_fn( readPointer, &buffer, PNG_ReadData );
    png_read_info( readPointer, infoPointer );

    png_uint_32 pngWidth = 0;
    png_uint_32 pngHeight = 0;

    png_get_IHDR( readPointer, infoPointer, &pngWidth, &pngHeight, &bitDepth, &colorType, NULL, NULL, NULL );

    if ( colorType == PNG_COLOR_TYPE_PALETTE )
    {
        png_set_expand( readPointer );
    }

    if ( colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8 )
    {
        png_set_expand( readPointer );
    }

    if ( png_get_valid( readPointer, infoPointer, PNG_INFO_tRNS ) )
    {
        png_set_expand( readPointer );
    }

    if ( bitDepth == 16 )
    {
        png_set_strip_16( readPointer );
    }

    if ( colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA )
    {
        png_set_gray_to_rgb( readPointer );
    }

    if ( !( colorType & PNG_COLOR_MASK_ALPHA ) )
    {
        png_set_add_alpha( readPointer, 255, PNG_FILLER_AFTER );
    }

    png_read_update_info( readPointer, infoPointer );

    png_size_t rowBytes = png_get_rowbytes( readPointer, infoPointer );
    sizeInBytes = rowBytes * pngHeight;
    unsigned char *imageData = ( unsigned char * )malloc( sizeInBytes );

    if ( !imageData )
    {
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        Warning( "Failed loading PNG as Material! Memory allocation failed \"%s\"\n", pFilePath );
        return NULL;
    }

    png_bytep *rowPointers = ( png_bytep * )malloc( sizeof( png_bytep ) * pngHeight );

    if ( !rowPointers )
    {
        free( imageData );
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        Warning( "Failed loading PNG as Material! Memory allocation for row pointers failed \"%s\"\n", pFilePath );
        return NULL;
    }

    for ( png_uint_32 i = 0; i < pngHeight; ++i )
    {
        rowPointers[i] = imageData + i * rowBytes;
    }

    png_read_image( readPointer, rowPointers );
    png_read_end( readPointer, NULL );

    free( rowPointers );
    png_destroy_read_struct( &readPointer, &infoPointer, NULL );

    width = pngWidth;
    height = pngHeight;

    return imageData;
}

bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *pFilePath, int &width, int &height, int &colorType, int &bitDepth )
{
    png_const_bytep fileHeader = ( png_const_bytep )buffer.Base();

    if ( png_sig_cmp( fileHeader, 0, 8 ) )
    {
        Warning( "Failed loading PNG as Material! File is missing PNG header \"%s\"\n", pFilePath );
        return false;
    }

    png_structp readPointer = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    png_infop infoPointer = png_create_info_struct( readPointer );

    if ( setjmp( png_jmpbuf( readPointer ) ) )
    {
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        Warning( "Failed loading PNG as Material! Error during PNG creation \"%s\"\n", pFilePath );
        return false;
    }

    png_set_read_fn( readPointer, &buffer, PNG_ReadData );
    png_read_info( readPointer, infoPointer );

    png_uint_32 pngWidth = 0;
    png_uint_32 pngHeight = 0;

    png_get_IHDR( readPointer, infoPointer, &pngWidth, &pngHeight, &bitDepth, &colorType, NULL, NULL, NULL );

    png_read_update_info( readPointer, infoPointer );

    png_destroy_read_struct( &readPointer, &infoPointer, NULL );

    width = pngWidth;
    height = pngHeight;

    return true;
}

bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *pFilePath, int &width, int &height )
{
    int colorType, bitDepth;
    return PNG_ReadInfoFromBuffer( buffer, pFilePath, width, height, colorType, bitDepth );
}
#endif

IMaterial *CPngTextureRegen::GetOrCreateProceduralMaterial( const char *pMaterialName, const char *pFilePath )
{
    KeyValues *keys = new KeyValues( "UnlitGeneric" );
    keys->SetString( "$basetexture", pMaterialName );
    keys->SetString( "$fullfilepath", pFilePath );
    keys->SetString( "$translucent", "1" );
    IMaterial *pMaterial = g_pMaterialSystem->FindProceduralMaterial( pMaterialName, TEXTURE_GROUP_VGUI, keys );

    // The server will have incorrect info on the material
    int width = 512;
    int height = 512;
    ImageFormat imageFormat = IMAGE_FORMAT_RGBA8888;

    char fullFilePath[MAX_PATH];
    Q_snprintf( fullFilePath, sizeof( fullFilePath ), "materials/%s", pFilePath );
    Q_FixSlashes( fullFilePath );

#ifdef CLIENT_DLL
    CUtlBuffer buffer;
    if ( !filesystem->ReadFile( fullFilePath, "GAME", buffer ) )
    {
        Warning( "Failed loading PNG as Material! Couldn't read PNG from file \"%s\"\n", pFilePath );
        return NULL;
    }

    PNG_ReadInfoFromBuffer( buffer, pFilePath, width, height );
#endif

    IMaterialVar *pVar = pMaterial->FindVar( "$basetexture", NULL );

    if ( pVar )
    {
        ITexture *pTexture = nullptr;
        bool bLoadInitial = false;

        if ( !g_pMaterialSystem->IsTextureLoaded( pMaterialName ) )
        {
            pTexture = g_pMaterialSystem->CreateProceduralTexture( pMaterialName, TEXTURE_GROUP_VGUI, width, height, imageFormat, TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_NOLOD | TEXTUREFLAGS_PROCEDURAL );
            bLoadInitial = true;
        }
        else
        {
            pTexture = g_pMaterialSystem->FindTexture( pMaterialName, TEXTURE_GROUP_VGUI, false );
            bLoadInitial = pTexture->IsError();
        }

        if ( bLoadInitial )
        {
            // pTexture->SetErrorTexture( false );
            IMaterialVar *pFullPathVar = pMaterial->FindVar( "$fullfilepath", NULL );
            pFullPathVar->SetStringValue( fullFilePath );

            CPngMaterialProxy *pMaterialProxy = new CPngMaterialProxy();
            pMaterialProxy->Init( pMaterial, keys );

            //CMaterialReference hMaterial;
            //hMaterial.Init( pMaterial );
            //hMaterial->Refresh();

            //CMatRenderContextPtr pRenderContext( g_pMaterialSystem );
            //pRenderContext->Bind( pMaterial );

            m_mapProceduralMaterials.Insert( pTexture->GetName(), pMaterialProxy );
        }

        pMaterial->IncrementReferenceCount();
    }
    else
    {
        Warning( "Failed loading PNG as Material! Couldn't find basetexture var in material \"%s\"\n", pMaterialName );
    }

    return pMaterial;
}

CPngMaterialProxy *CPngTextureRegen::GetProceduralMaterialProxy( const char *pMaterialName )
{
    unsigned short index = m_mapProceduralMaterials.Find( pMaterialName );

    if ( index == m_mapProceduralMaterials.InvalidIndex() )
    {
        return nullptr;
    }

    CPngMaterialProxy *pMaterialProxy = m_mapProceduralMaterials.Element( index );

    return pMaterialProxy;
}

void CPngTextureRegen::ReleaseAllTextureData()
{
    FOR_EACH_MAP_FAST( m_mapProceduralMaterials, i )
    {
        CPngMaterialProxy *pMaterialProxy = m_mapProceduralMaterials[i];
        IMaterial *pMaterial = pMaterialProxy->GetMaterial();

        pMaterial->DecrementReferenceCount();
    }

    m_mapProceduralMaterials.RemoveAll();
}

/*
** CPngMaterialProxy
*/

#pragma warning( disable : 4355 )  //  warning: 'this' : used in base member initializer list
CPngMaterialProxy::CPngMaterialProxy()
    : m_TextureRegen( this )
{
    m_pMaterial = NULL;
    m_pTexturePointer = NULL;
    m_iSizeInBytes = 0;
    m_pTextureVar = NULL;
    m_pFullPathVar = NULL;
}
#pragma warning( default : 4355 )

CPngMaterialProxy::~CPngMaterialProxy()
{
    // Disconnect the texture regenerator, or else we will get a crash on exit in MaterialSystem.dll
    // in certain situations (like having the ESC menu opened when finding a material)
    if ( m_pTextureVar )
    {
        ITexture *pTexture = m_pTextureVar->GetTextureValue();
        if ( pTexture )
            pTexture->SetTextureRegenerator( NULL );
    }

    delete m_pTexturePointer;
}

bool CPngMaterialProxy::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
    m_pMaterial = pMaterial;

    bool bFound;
    m_pTextureVar = m_pMaterial->FindVar( "$basetexture", &bFound );

    if ( !bFound )
    {
        Assert( 0 );
        m_pTextureVar = NULL;
        return false;
    }

    m_pFullPathVar = m_pMaterial->FindVar( "$fullfilepath", &bFound );

    if ( !bFound )
    {
        Assert( 0 );
        m_pFullPathVar = NULL;
        return false;
    }

    PreLoadTexture();

    ITexture *pTexture = m_pTextureVar->GetTextureValue();

    if ( pTexture )
    {
        pTexture->SetTextureRegenerator( &m_TextureRegen );
        pTexture->Download();
    }

    return true;
}

void CPngMaterialProxy::OnBind( void *pBind )
{
    Assert( m_pTextureVar );

    ITexture *pTexture = m_pTextureVar->GetTextureValue();
    pTexture->Download();
}

void CPngMaterialProxy::PreLoadTexture()
{
#ifdef CLIENT_DLL
    const char *fileName = m_pFullPathVar->GetStringValue();

    CUtlBuffer buffer;
    if ( !filesystem->ReadFile( fileName, "GAME", buffer ) )
    {
        Warning( "Failed regenerating PNG as Texture! Couldn't read PNG from file \"%s\"\n", fileName );
        return;
    }

    int width, height, colorType, bitDepth, sizeInBytes;

    m_pTexturePointer = PNG_ReadFromBuffer( buffer, fileName, width, height, colorType, bitDepth, sizeInBytes );

    if ( !m_pTexturePointer )
    {
        // Warning already printed by PNG_ReadFromBuffer
        return;
    }

    m_iSizeInBytes = sizeInBytes;
#endif
}

void CPngMaterialProxy::LoadTexture( ITexture *pTexture, IVTFTexture *pVTFTexture )
{
#ifdef CLIENT_DLL
    unsigned char *vtfImageData = pVTFTexture->ImageData( 0, 0, 0 );

    // Ensure the texture we are copying to is the same format as the texture we are copying from
    pVTFTexture->ConvertImageFormat( pTexture->GetImageFormat(), false );

    Q_memcpy( vtfImageData, m_pTexturePointer, m_iSizeInBytes );
#endif
}

EXPOSE_INTERFACE( CPngMaterialProxy, IMaterialProxy, "Png" IMATERIAL_PROXY_INTERFACE_VERSION );
