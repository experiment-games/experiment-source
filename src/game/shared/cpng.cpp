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

#ifdef CLIENT_DLL
#include "libpng/png.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CUtlMap< ITexture *, unsigned char * > CPngTextureRegen::m_PngTextures( DefLessFunc( ITexture * ) );

CPngTextureRegen::CPngTextureRegen( const char *pFileName )
{
    Q_strncpy( m_pFileName, pFileName, sizeof( m_pFileName ) );
}

void CPngTextureRegen::RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect )
{
#ifdef CLIENT_DLL
    unsigned char *vtfImageData = pVTFTexture->ImageData( 0, 0, 0 );

    CUtlBuffer buffer;
    if ( !filesystem->ReadFile( m_pFileName, "GAME", buffer ) )
    {
        Warning( "Failed loading PNG as Material! Couldn't read PNG from file \"%s\"\n", m_pFileName );
        return;
    }

    int width, height;
    ImageFormat _;

    unsigned char *imageData = PNG_ReadFromBuffer( buffer, m_pFileName, width, height, _ );

    if ( !imageData )
    {
        // Warning already printed by PNG_ReadFromBuffer
        return;
    }

    //pVTFTexture->ConvertImageFormat( IMAGE_FORMAT_RGBA8888, false );  // not sure if this is needed

    //m_PngTextures.InsertOrReplace( pTexture, imageData );

    // copy the image data to the vtf texture
    for ( int y = 0; y < height; ++y )
    {
        for ( int x = 0; x < width; ++x )
        {
            int vtfIndex = ( y * width + x ) * 4;
            int imageIndex = ( y * width + x ) * 4;

            vtfImageData[vtfIndex + 0] = imageData[imageIndex + 0];
            vtfImageData[vtfIndex + 1] = imageData[imageIndex + 1];
            vtfImageData[vtfIndex + 2] = imageData[imageIndex + 2];
            vtfImageData[vtfIndex + 3] = imageData[imageIndex + 3];
        }
    }
#endif
}

unsigned char *CPngTextureRegen::GetPngTextureData( ITexture *pTexture )
{
    unsigned short index = m_PngTextures.Find( pTexture );

    if ( !m_PngTextures.IsValidIndex( index ) )
    {
        Warning( "CPngTextureRegen tried to get data for a texture that doesn't exist in the map!\n" );
        return nullptr;
    }

    return m_PngTextures[index];
}

#ifdef CLIENT_DLL
static void PNG_ReadData( png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead )
{
    CUtlBuffer *buffer = ( CUtlBuffer * )png_get_io_ptr( png_ptr );
    buffer->Get( outBytes, byteCountToRead );
}

unsigned char *PNG_ReadFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, ImageFormat &imageFormat )
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
    int bitDepth = 0;
    int colorType = -1;

    png_get_IHDR( readPointer, infoPointer, &pngWidth, &pngHeight, &bitDepth, &colorType, NULL, NULL, NULL );

    png_read_update_info( readPointer, infoPointer );

    png_size_t rowBytes = png_get_rowbytes( readPointer, infoPointer );
    unsigned char *imageData = ( unsigned char * )malloc( rowBytes * pngHeight );

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
    imageFormat = IMAGE_FORMAT_RGBA8888;  // TODO: Is this always the case?

    return imageData;
}

void PNG_ReadToBitmap( CUtlBuffer &buffer, const char *filePath, Bitmap_t &bitmap )
{
    bitmap.Clear();

    int width, height;
    ImageFormat imageFormat;
    unsigned char *imageData = PNG_ReadFromBuffer( buffer, filePath, width, height, imageFormat );

    bitmap.SetBuffer( width, height, imageFormat, imageData, true, width * 4 );
}

bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *filePath, int &width, int &height, ImageFormat &imageFormat )
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
    int bitDepth = 0;
    int colorType = -1;

    png_get_IHDR( readPointer, infoPointer, &pngWidth, &pngHeight, &bitDepth, &colorType, NULL, NULL, NULL );

    png_read_update_info( readPointer, infoPointer );

    png_destroy_read_struct( &readPointer, &infoPointer, NULL );

    width = pngWidth;
    height = pngHeight;
    imageFormat = IMAGE_FORMAT_RGBA8888;  // TODO: Is this always the case?

    return true;
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

    PNG_ReadInfoFromBuffer( buffer, filePath, width, height, imageFormat );
#endif

    ITexture *pTexture = g_pMaterialSystem->CreateProceduralTexture( materialName, TEXTURE_GROUP_VGUI, width, height, imageFormat, TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_NOLOD | TEXTUREFLAGS_PROCEDURAL );
    pTexture->SetTextureRegenerator( new CPngTextureRegen( fullFilePath ) );
    pTexture->Download();

    bool bFound = false;
    IMaterialVar *pVar = pMaterial->FindVar( "$basetexture", &bFound );

    if ( bFound && pVar )
    {
        pMaterial->IncrementReferenceCount();
        pVar->SetTextureValue( pTexture );
    }
    else
    {
        Warning( "Failed loading PNG as Material! Couldn't find basetexture var in material \"%s\"\n", materialName );
    }

    return pMaterial;
}
