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

#ifdef CLIENT_DLL
#include "libpng/png.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CUtlVector< IMaterial * > CPngTextureRegen::m_vecProceduralMaterials;

CPngTextureRegen::CPngTextureRegen( const char *pFileName )
{
    Q_strncpy( m_pFileName, pFileName, sizeof( m_pFileName ) );
}

void CPngTextureRegen::RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect )
{
#ifdef CLIENT_DLL
    unsigned char *vtfImageData = pVTFTexture->ImageData( 0, 0, 0 );

    // Ensure the texture we are copying to is the same format as the texture we are copying from
    pVTFTexture->ConvertImageFormat( pTexture->GetImageFormat(), false );

    CUtlBuffer buffer;
    if ( !filesystem->ReadFile( m_pFileName, "GAME", buffer ) )
    {
        Warning( "Failed regenerating PNG as Texture! Couldn't read PNG from file \"%s\"\n", m_pFileName );
        return;
    }

    int width, height, colorType, bitDepth, sizeInBytes;

    unsigned char *imageData = PNG_ReadFromBuffer( buffer, m_pFileName, width, height, colorType, bitDepth, sizeInBytes );

    if ( !imageData )
    {
        // Warning already printed by PNG_ReadFromBuffer
        return;
    }

    Q_memcpy( vtfImageData, imageData, sizeInBytes );

    // Now that we have copied the image data, we can free the original image data
    free( imageData );
#endif
}

#ifdef CLIENT_DLL
static void PNG_ReadData( png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead )
{
    CUtlBuffer *buffer = ( CUtlBuffer * )png_get_io_ptr( png_ptr );
    buffer->Get( outBytes, byteCountToRead );
}

unsigned char *PNG_ReadFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, int &colorType, int &bitDepth, int &sizeInBytes )
{
    png_const_bytep fileHeader = ( png_const_bytep )buffer.Base();

    if ( png_sig_cmp( fileHeader, 0, 8 ) )
    {
        Warning( "Failed loading PNG as Material! File is missing PNG header \"%s\"\n", filePath );
        return NULL;
    }

    png_structp readPointer = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    png_infop infoPointer = png_create_info_struct( readPointer );

    if ( setjmp( png_jmpbuf( readPointer ) ) )
    {
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        Warning( "Failed loading PNG as Material! Error during PNG creation \"%s\"\n", filePath );
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
        Warning( "Failed loading PNG as Material! Memory allocation failed \"%s\"\n", filePath );
        return NULL;
    }

    png_bytep *rowPointers = ( png_bytep * )malloc( sizeof( png_bytep ) * pngHeight );

    if ( !rowPointers )
    {
        free( imageData );
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        Warning( "Failed loading PNG as Material! Memory allocation for row pointers failed \"%s\"\n", filePath );
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

bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, int &colorType, int &bitDepth )
{
    png_const_bytep fileHeader = ( png_const_bytep )buffer.Base();

    if ( png_sig_cmp( fileHeader, 0, 8 ) )
    {
        Warning( "Failed loading PNG as Material! File is missing PNG header \"%s\"\n", filePath );
        return false;
    }

    png_structp readPointer = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    png_infop infoPointer = png_create_info_struct( readPointer );

    if ( setjmp( png_jmpbuf( readPointer ) ) )
    {
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        Warning( "Failed loading PNG as Material! Error during PNG creation \"%s\"\n", filePath );
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
bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height )
{
    int colorType, bitDepth;
    return PNG_ReadInfoFromBuffer( buffer, filePath, width, height, colorType, bitDepth );
}
#endif

IMaterial *CPngTextureRegen::GetOrCreateProceduralMaterial( const char *materialName, const char *filePath )
{
    KeyValues *keys = new KeyValues( "UnlitGeneric" );
    keys->SetString( "$basetexture", materialName );
    keys->SetString( "$translucent", "1" );
    IMaterial *pMaterial = g_pMaterialSystem->FindProceduralMaterial( materialName, TEXTURE_GROUP_VGUI, keys );

    // The server will have incorrect info on the material
    int width = 512;
    int height = 512;
    ImageFormat imageFormat = IMAGE_FORMAT_RGBA8888;

    char fullFilePath[MAX_PATH];
    Q_snprintf( fullFilePath, sizeof( fullFilePath ), "materials/%s", filePath );
    Q_FixSlashes( fullFilePath );

#ifdef CLIENT_DLL
    CUtlBuffer buffer;
    if ( !filesystem->ReadFile( fullFilePath, "GAME", buffer ) )
    {
        Warning( "Failed loading PNG as Material! Couldn't read PNG from file \"%s\"\n", filePath );
        return NULL;
    }

    PNG_ReadInfoFromBuffer( buffer, filePath, width, height );
#endif

    bool bFound = false;
    IMaterialVar *pVar = pMaterial->FindVar( "$basetexture", &bFound );

    if ( bFound && pVar )
    {
        ITexture *pTexture = nullptr;
        bool bLoadInitial = false;

        if ( !g_pMaterialSystem->IsTextureLoaded( materialName ) )
        {
            pTexture = g_pMaterialSystem->CreateProceduralTexture( materialName, TEXTURE_GROUP_VGUI, width, height, imageFormat, TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_NOLOD | TEXTUREFLAGS_PROCEDURAL );
            bLoadInitial = true;
        }
        else
        {
            pTexture = g_pMaterialSystem->FindTexture( materialName, TEXTURE_GROUP_VGUI, false );
            bLoadInitial = pTexture->IsError();
        }

        if ( bLoadInitial )
        {
            pTexture->SetTextureRegenerator( new CPngTextureRegen( fullFilePath ) );
            pTexture->Download();

            pMaterial->IncrementReferenceCount();
            m_vecProceduralMaterials.AddToTail( pMaterial );
            pVar->SetTextureValue( pTexture );
        }
    }
    else
    {
        Warning( "Failed loading PNG as Material! Couldn't find basetexture var in material \"%s\"\n", materialName );
    }

    return pMaterial;
}

void CPngTextureRegen::ReleaseAllTextureData()
{
    for ( int i = 0; i < m_vecProceduralMaterials.Count(); ++i )
    {
        IMaterial *pMaterial = m_vecProceduralMaterials[i];

        // Disconnect the texture regenerator, or else we will get a crash on exit in MaterialSystem.dll
        // in certain situations (like having the ESC menu opened when finding a material)
        bool bFound = false;
        IMaterialVar *pVar = pMaterial->FindVar( "$basetexture", &bFound );

        if ( bFound && pVar )
        {
            ITexture *pTexture = pVar->GetTextureValue();
            if ( pTexture )
            {
                pTexture->SetTextureRegenerator( NULL );
                pTexture->SetErrorTexture( true );
            }
        }

        pMaterial->DecrementReferenceCount();
    }

    m_vecProceduralMaterials.RemoveAll();
}
