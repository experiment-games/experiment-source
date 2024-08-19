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
#include "libpng/png.h"
#include <pixelwriter.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CUtlMap< const char *, CPngMaterialProxy * > CPngTextureRegen::m_mapProceduralMaterials;

void CPngTextureRegen::RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect )
{
    m_pProxy->LoadTexture( pTexture, pVTFTexture, pSubRect );
}

static void PNG_ReadData( png_structp readPointer, png_bytep outBytes, png_size_t byteCountToRead )
{
    CUtlBuffer *buffer = ( CUtlBuffer * )png_get_io_ptr( readPointer );
    buffer->Get( outBytes, byteCountToRead );
}

unsigned char *PNG_ReadFromBuffer( CUtlBuffer &buffer, const char *pFilePath, int &width, int &height, int &colorType, int &bitDepth, int &sizeInBytes )
{
    png_const_bytep fileHeader = ( png_const_bytep )buffer.Base();

    if ( png_sig_cmp( fileHeader, 0, 8 ) )
    {
        DevWarning( "Failed loading PNG as Material! File is missing PNG header \"%s\"\n", pFilePath );
        return NULL;
    }

    png_structp readPointer = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    png_infop infoPointer = png_create_info_struct( readPointer );

    if ( setjmp( png_jmpbuf( readPointer ) ) )
    {
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        DevWarning( "Failed loading PNG as Material! Error during PNG creation \"%s\"\n", pFilePath );
        return NULL;
    }

    png_set_read_fn( readPointer, &buffer, PNG_ReadData );
    png_read_info( readPointer, infoPointer );

    // Reference: https://gist.github.com/jeroen/10eb17a9fb0e5799b772
    width = png_get_image_width( readPointer, infoPointer );
    height = png_get_image_height( readPointer, infoPointer );
    bitDepth = png_get_bit_depth( readPointer, infoPointer );
    colorType = png_get_color_type( readPointer, infoPointer );

    if ( bitDepth == 16 )
        png_set_strip_16( readPointer );

    if ( colorType == PNG_COLOR_TYPE_PALETTE )
        png_set_palette_to_rgb( readPointer );

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if ( colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8 )
        png_set_expand_gray_1_2_4_to_8( readPointer );

    if ( png_get_valid( readPointer, infoPointer, PNG_INFO_tRNS ) )
        png_set_tRNS_to_alpha( readPointer );

    // These color types don't have an alpha channel then fill it with 0xff.
    if ( colorType == PNG_COLOR_TYPE_RGB ||
         colorType == PNG_COLOR_TYPE_GRAY ||
         colorType == PNG_COLOR_TYPE_PALETTE )
        png_set_filler( readPointer, 0xFF, PNG_FILLER_AFTER );

    if ( colorType == PNG_COLOR_TYPE_GRAY ||
         colorType == PNG_COLOR_TYPE_GRAY_ALPHA )
        png_set_gray_to_rgb( readPointer );

    png_read_update_info( readPointer, infoPointer );

    int rowBytes = png_get_rowbytes( readPointer, infoPointer );
    sizeInBytes = rowBytes * height;

    unsigned char *imageData = ( unsigned char * )malloc( sizeInBytes );

    if ( !imageData )
    {
        DevWarning( "Failed loading PNG as Material! Memory allocation failed \"%s\"\n", pFilePath );
        return NULL;
    }

    png_bytepp rowPointers = ( png_bytepp )malloc( sizeof( png_bytep ) * height );

    if ( !rowPointers )
    {
        free( imageData );
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        DevWarning( "Failed loading PNG as Material! Memory allocation for row pointers failed \"%s\"\n", pFilePath );
        return NULL;
    }

    for ( int y = 0; y < height; y++ )
    {
        rowPointers[y] = imageData + ( y * rowBytes );
    }

    png_read_image( readPointer, rowPointers );
    free( rowPointers );

    png_destroy_read_struct( &readPointer, &infoPointer, NULL );

//#ifdef CLIENT_DLL
//    ImageFormat targetImageFormat = IMAGE_FORMAT_RGBA8888;
//    int memRequired = ImageLoader::GetMemRequired( width, height, bitDepth, targetImageFormat, false );
//    unsigned char *convertedImageData = ( unsigned char * )malloc( memRequired );
//    ImageLoader::ConvertImageFormat( imageData, IMAGE_FORMAT_UNKNOWN, convertedImageData, targetImageFormat, width, height, 0, 0 );
//    free( imageData );
//    return convertedImageData;
//#else
    return imageData;
//#endif
}

bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *pFilePath, int &width, int &height, int &colorType, int &bitDepth )
{
    png_const_bytep fileHeader = ( png_const_bytep )buffer.Base();

    if ( png_sig_cmp( fileHeader, 0, 8 ) )
    {
        DevWarning( "Failed loading PNG as Material! File is missing PNG header \"%s\"\n", pFilePath );
        return false;
    }

    png_structp readPointer = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    png_infop infoPointer = png_create_info_struct( readPointer );

    if ( setjmp( png_jmpbuf( readPointer ) ) )
    {
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        DevWarning( "Failed loading PNG as Material! Error during PNG creation \"%s\"\n", pFilePath );
        return false;
    }

    png_set_read_fn( readPointer, &buffer, PNG_ReadData );
    png_read_info( readPointer, infoPointer );

    width = png_get_image_width( readPointer, infoPointer );
    height = png_get_image_height( readPointer, infoPointer );
    bitDepth = png_get_bit_depth( readPointer, infoPointer );
    colorType = png_get_color_type( readPointer, infoPointer );

    png_read_update_info( readPointer, infoPointer );

    png_destroy_read_struct( &readPointer, &infoPointer, NULL );

    return true;
}

bool PNG_ReadInfoFromBuffer( CUtlBuffer &buffer, const char *pFilePath, int &width, int &height )
{
    int colorType, bitDepth;
    return PNG_ReadInfoFromBuffer( buffer, pFilePath, width, height, colorType, bitDepth );
}

static void PNG_WriteData( png_structp writePointer, png_bytep data, png_size_t length )
{
    CUtlBuffer *buffer = ( CUtlBuffer * )png_get_io_ptr( writePointer );
    buffer->Put( data, length );
}

static void PNG_FlushData( png_structp writePointer )
{
    // TODO: Do we need to do anything here?
}

bool PNG_WriteToFile( const char *filePath, unsigned char *pImageData, int width, int height )
{
    CUtlBuffer buffer;

    png_structp writePointer = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    png_infop infoPointer = png_create_info_struct( writePointer );

    if ( setjmp( png_jmpbuf( writePointer ) ) )
    {
        png_destroy_write_struct( &writePointer, &infoPointer );

        DevWarning( "Failed writing PNG as Material! Error during PNG creation \"%s\"\n", filePath );
        return false;
    }

    png_set_write_fn( writePointer, ( void * )&buffer, PNG_WriteData, PNG_FlushData );

    png_set_IHDR(
        writePointer,
        infoPointer,
        width,
        height,
        8,
        PNG_COLOR_TYPE_RGB_ALPHA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT );
    png_write_info( writePointer, infoPointer );

    png_bytepp rowPointers = ( png_bytepp )malloc( sizeof( png_bytep ) * height );

    if ( !rowPointers )
    {
        png_destroy_write_struct( &writePointer, &infoPointer );
        DevWarning( "Failed writing PNG as Material! Memory allocation for row pointers failed \"%s\"\n", filePath );
        return false;
    }

    png_size_t rowBytes = png_get_rowbytes( writePointer, infoPointer );

    for ( int i = 0; i < height; ++i )
    {
        rowPointers[i] = pImageData + i * rowBytes;
    }

    png_write_image( writePointer, rowPointers );
    free( rowPointers );

    png_write_end( writePointer, infoPointer );
    png_destroy_write_struct( &writePointer, &infoPointer );

    // force create this directory incase it doesn't exist
    char fileDirectoryPath[MAX_PATH];
    Q_snprintf( fileDirectoryPath, sizeof( fileDirectoryPath ), "%s", filePath );
    Q_StripFilename( fileDirectoryPath );
    filesystem->CreateDirHierarchy( fileDirectoryPath, "GAME" );

    if ( !filesystem->WriteFile( filePath, "GAME", buffer ) )
    {
        DevWarning( "Failed writing PNG as Material! Couldn't write PNG to file \"%s\"\n", filePath );
        return false;
    }

    return true;
}

void CleanMaterialName( const char *pMaterialName, char *pCleanMaterialName, int iOutNameSize )
{
    Q_StripExtension( pMaterialName, pCleanMaterialName, iOutNameSize );
    Q_strlower( pCleanMaterialName );
    Q_FixSlashes( pCleanMaterialName, '/' );
}

IMaterial *GetErrorMaterial()
{
    return g_pMaterialSystem->FindMaterial( "___error.vmt", TEXTURE_GROUP_OTHER, true );
}

IMaterial *CPngTextureRegen::GetOrCreateProceduralMaterial(
    const char *pMaterialName,
    const char *pFilePath,
    KeyValues *pVMTKeyValues /* = nullptr */,
    bool bSmooth /* = false */
)
{
    char fullFilePath[MAX_PATH];
    Q_snprintf( fullFilePath, sizeof( fullFilePath ), "materials/%s", pFilePath );
    Q_FixSlashes( fullFilePath );

    CUtlBuffer buffer;
    if ( !filesystem->ReadFile( fullFilePath, "GAME", buffer ) )
    {
        DevWarning( "Failed loading PNG as Material! Couldn't read PNG from file \"%s\"\n", pFilePath );
        return GetErrorMaterial();
    }

    int width = 512;
    int height = 512;
    ImageFormat imageFormat = IMAGE_FORMAT_RGBA8888;
    PNG_ReadInfoFromBuffer( buffer, pFilePath, width, height );

    char cleanMaterialName[MAX_PATH];
    CleanMaterialName( pMaterialName, cleanMaterialName, sizeof( cleanMaterialName ) );

    ITexture *pTexture = NULL;
    if ( g_pMaterialSystem->IsTextureLoaded( cleanMaterialName ) )
    {
        pTexture = g_pMaterialSystem->FindTexture( cleanMaterialName, TEXTURE_GROUP_VGUI, false );

        if ( pTexture->IsError() )
        {
            pTexture = NULL;
        }
    }

    if ( !pTexture )
    {
        int nFlags = TEXTUREFLAGS_PROCEDURAL | TEXTUREFLAGS_NOMIP;

        if ( !bSmooth )
        {
            nFlags |= TEXTUREFLAGS_POINTSAMPLE | TEXTUREFLAGS_NOLOD;
        }

        pTexture = g_pMaterialSystem->CreateProceduralTexture(
            cleanMaterialName,
            TEXTURE_GROUP_VGUI,
            width,
            height,
            imageFormat,
            nFlags );
        //pTexture->IncrementReferenceCount();
    }

    if ( pVMTKeyValues == nullptr )
    {
        pVMTKeyValues = new KeyValues( "UnlitGeneric" );
        pVMTKeyValues->SetInt( "$vertexalpha", 1 );
        pVMTKeyValues->SetInt( "$vertexcolor", 1 );
    }

    Assert( !pVMTKeyValues->FindKey( "$basetexture" ) );
    Assert( !pVMTKeyValues->FindKey( "$proceduraltexturepath" ) );

    pVMTKeyValues->SetString( "$basetexture", cleanMaterialName );
    pVMTKeyValues->SetString( "$proceduraltexturepath", fullFilePath );

    KeyValues *pVMTProxies = new KeyValues( "Proxies" );
    pVMTKeyValues->AddSubKey( pVMTProxies );

    KeyValues *pVMTPngValues = new KeyValues( "ProceduralPng" );
    pVMTProxies->AddSubKey( pVMTPngValues );

    IMaterial *pMaterial = g_pMaterialSystem->FindProceduralMaterial( cleanMaterialName, TEXTURE_GROUP_VGUI, pVMTKeyValues );

    bool bIsPrecached = pMaterial->IsPrecached();

    if ( !bIsPrecached )
    {
        // DevWarning( "CPngTextureRegen::GetOrCreateProceduralMaterial: %s not precached\n", cleanMaterialName );
        pMaterial->Refresh();
        //pMaterial->IncrementReferenceCount();
    }

    return pMaterial;
}

lua_Color CPngTextureRegen::GetProceduralMaterialProxyColorAtPosition( const char *pMaterialName, int x, int y )
{
    char cleanMaterialName[MAX_PATH];
    CleanMaterialName( pMaterialName, cleanMaterialName, sizeof( cleanMaterialName ) );

    unsigned short index = m_mapProceduralMaterials.Find( cleanMaterialName );

    if ( index == m_mapProceduralMaterials.InvalidIndex() )
    {
        DevWarning( "Failed getting PNG as Material! Couldn't find material \"%s\"!\n", cleanMaterialName );
        return lua_Color();
    }

    CPngMaterialProxy *pMaterialProxy = m_mapProceduralMaterials.Element( index );

    if ( !pMaterialProxy )
    {
        DevWarning( "Failed getting PNG as Material! Material \"%s\" no longer valid!\n", cleanMaterialName );
        return lua_Color();
    }

    return pMaterialProxy->GetColorAtPosition( x, y );
}

void CPngTextureRegen::ReleaseAllTextureData()
{
    m_mapProceduralMaterials.RemoveAll();

    g_pMaterialSystem->UncacheAllMaterials();  // doesnt work to have IsTextureLoaded return false :/
}

/*
** CPngMaterialProxy
*/

#pragma warning( disable : 4355 )  //  warning: 'this' : used in base member initializer list
CPngMaterialProxy::CPngMaterialProxy()
    : m_TextureRegen( this )
{
    m_pTexturePointer = NULL;
    m_iSizeInBytes = 0;
    m_pTextureVar = NULL;
    m_pFullPathVar = NULL;
    m_pTexture = NULL;
}
#pragma warning( default : 4355 )

CPngMaterialProxy::~CPngMaterialProxy()
{
    // DevWarning( "CPngMaterialProxy::~CPngMaterialProxy for texture %s\n", m_pTexture->GetName() );
    //  Disconnect the texture regenerator, or else we will get a crash on exit in MaterialSystem.dll
    //  in certain situations (like having the ESC menu opened when finding a material)
    if ( m_pTexture != NULL )
    {
        m_pTexture->SetTextureRegenerator( NULL );
    }

    free( m_pTexturePointer );  // We don't need this anymore
    m_pTexturePointer = NULL;
}

bool CPngMaterialProxy::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
    bool bFound;
    m_pFullPathVar = pMaterial->FindVar( "$proceduraltexturepath", &bFound );

    if ( !bFound )
    {
        Assert( 0 );
        m_pFullPathVar = NULL;
        return false;
    }

    m_pTextureVar = pMaterial->FindVar( "$basetexture", &bFound );

    if ( !bFound )
    {
        Assert( 0 );
        m_pTextureVar = NULL;
        return false;
    }

    m_pTexture = m_pTextureVar->GetTextureValue();
    //DevWarning( "CPngMaterialProxy::Init: %s\n", m_pTexture->GetName() );

    m_pTexture->SetTextureRegenerator( &m_TextureRegen );
    // m_pTexture->Download(); // This will load it into memory, which will have us run out of memory quickly

    CPngTextureRegen::m_mapProceduralMaterials.Insert( m_pTexture->GetName(), this );

    LoadTextureFromDisk();

    return true;
}

/// <summary>
/// This is called everytime a DrawTexturedRect is called.
/// Also pBind will be NULL since it is not bounnd to an entity?
/// </summary>
/// <param name="pBind"></param>
void CPngMaterialProxy::OnBind( void *pBind )
{
    // Keep track of our dirty state, so we don't call download every frame (which is expensive)
    if ( !m_bIsDirty )
        return;

    m_bIsDirty = false;

    Assert( m_pTexture );
    Assert( &m_TextureRegen );

    // Although the texture regenerator should be set (see Init), for some reason
    // we need to re-set it, or else calling 'Download' will not call the RegenerateTextureBits :/
    m_pTexture->SetTextureRegenerator( &m_TextureRegen );

    m_bAllowRegenerating = true;
    m_pTexture->Download();
    m_bAllowRegenerating = false;
}

void CPngMaterialProxy::LoadTextureFromDisk()
{
#ifdef CLIENT_DLL
    const char *fileName = m_pFullPathVar->GetStringValue();

    CUtlBuffer buffer;
    if ( !filesystem->ReadFile( fileName, CONTENT_SEARCH_PATH, buffer ) )
    {
        DevWarning( "Failed regenerating PNG as Texture! Couldn't read PNG from file \"%s\"\n", fileName );
        return;
    }

    int width, height, colorType, bitDepth, sizeInBytes;

    m_pTexturePointer = PNG_ReadFromBuffer( buffer, fileName, width, height, colorType, bitDepth, sizeInBytes );
    m_nWidth = width;
    m_nHeight = height;

    if ( !m_pTexturePointer )
    {
        // Warning already printed by PNG_ReadFromBuffer
        return;
    }

    m_iSizeInBytes = sizeInBytes;
    m_bIsDirty = true;
#endif
}

void CPngMaterialProxy::LoadTexture( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect )
{
    if ( !m_bAllowRegenerating )
        return;

#ifdef CLIENT_DLL
    Assert( m_pTexturePointer );

    // Not supported at the moment:
    Assert( !pTexture->IsMipmapped() );
    Assert( pVTFTexture->FaceCount() == 1 );
    Assert( pVTFTexture->FrameCount() == 1 );

    int width, height, depth;
    pVTFTexture->ComputeMipLevelDimensions( 0, &width, &height, &depth );

    Assert( width == m_nWidth && height == m_nHeight );
    Assert( depth == 1 );

    // Point the pixel writer to the vtf texture memory
    CPixelWriter pixelWriter;
    pixelWriter.SetPixelMemory(
        pVTFTexture->Format(),
        pVTFTexture->ImageData( 0, 0, 0 ),
        pVTFTexture->RowSizeInBytes( 0 ) );

    int bytesForFormat = 4; // IMAGE_FORMAT_RGBA8888
    int xMax = pSubRect->x + pSubRect->width;
    int yMax = pSubRect->y + pSubRect->height;
    int x, y;

    // Update the subrect that needs to be updated
    for ( y = pSubRect->y; y < yMax; ++y )
    {
        pixelWriter.Seek( pSubRect->x, y );
        unsigned char *rgba = &m_pTexturePointer[( y * m_nWidth + pSubRect->x ) * bytesForFormat];

        for ( x = pSubRect->x; x < xMax; ++x )
        {
            pixelWriter.WritePixel( rgba[0], rgba[1], rgba[2], rgba[3] );
            rgba += bytesForFormat;
        }
    }
#endif
}

lua_Color ColorAtPosition( unsigned char *imageData, int width, int height, int x, int y )
{
    if ( x < 0 || x >= width || y < 0 || y >= height )
    {
        return lua_Color();
    }

    int i = ( y * width + x ) * 4;

    return lua_Color( imageData[i], imageData[i + 1], imageData[i + 2], imageData[i + 3] );
}

lua_Color CPngMaterialProxy::GetColorAtPosition( int x, int y )
{
    if ( m_pTexturePointer == NULL )
    {
        Assert( 0 );  // Call LoadTextureFromDisk first
        return lua_Color();
    }

    Assert( m_pTexture );

    int width = m_pTexture->GetActualWidth();
    int height = m_pTexture->GetActualHeight();

    return ColorAtPosition( m_pTexturePointer, width, height, x, y );
}

EXPOSE_INTERFACE( CPngMaterialProxy, IMaterialProxy, "ProceduralPng" IMATERIAL_PROXY_INTERFACE_VERSION );
