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
        DevWarning( "Failed loading PNG as Material! Memory allocation failed \"%s\"\n", pFilePath );
        return NULL;
    }

    png_bytepp rowPointers = ( png_bytepp )malloc( sizeof( png_bytep ) * pngHeight );

    if ( !rowPointers )
    {
        free( imageData );
        png_destroy_read_struct( &readPointer, &infoPointer, NULL );
        DevWarning( "Failed loading PNG as Material! Memory allocation for row pointers failed \"%s\"\n", pFilePath );
        return NULL;
    }

    for ( png_uint_32 i = 0; i < pngHeight; ++i )
    {
        rowPointers[i] = imageData + i * rowBytes;
    }

    png_read_image( readPointer, rowPointers );
    free( rowPointers );

    png_read_end( readPointer, NULL );

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
        PNG_FILTER_TYPE_DEFAULT
    );
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
#endif

void CleanMaterialName( const char *pMaterialName, char *pCleanMaterialName, int iOutNameSize )
{
    Q_StripExtension( pMaterialName, pCleanMaterialName, iOutNameSize );
    Q_strlower( pCleanMaterialName );
    Q_FixSlashes( pCleanMaterialName, '/' );
}

IMaterial* GetErrorMaterial()
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
    // TODO: The server will have incorrect info on the material (acceptable for now)
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
        DevWarning( "Failed loading PNG as Material! Couldn't read PNG from file \"%s\"\n", pFilePath );
        return GetErrorMaterial();
    }

    PNG_ReadInfoFromBuffer( buffer, pFilePath, width, height );
#endif

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
    }

    return pMaterial;
}

CPngMaterialProxy *CPngTextureRegen::GetProceduralMaterialProxy( const char *pMaterialName )
{
    char cleanMaterialName[MAX_PATH];
    CleanMaterialName( pMaterialName, cleanMaterialName, sizeof( cleanMaterialName ) );

    unsigned short index = m_mapProceduralMaterials.Find( cleanMaterialName );

    if ( index == m_mapProceduralMaterials.InvalidIndex() )
    {
        return nullptr;
    }

    CPngMaterialProxy *pMaterialProxy = m_mapProceduralMaterials.Element( index );

    return pMaterialProxy;
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
    // DevWarning( "CPngMaterialProxy::Init: %s\n", m_pTexture->GetName() );

    m_pTexture->SetTextureRegenerator( &m_TextureRegen );
    m_pTexture->Download();

    CPngTextureRegen::m_mapProceduralMaterials.Insert( m_pTexture->GetName(), this );

    return true;
}

void CPngMaterialProxy::OnBind( void *pBind )
{
    // This is called everytime a DrawTexturedRect is called, so we shouldnt Download here.
    // Also pBind will be NULL since it is not bounnd to an entity?
    if ( pBind == NULL )
        return;

    Assert( m_pTextureVar );
    Assert( m_pTextureVar->IsTexture() );

    DevWarning( "CPngMaterialProxy::OnBind: %s\n", m_pFullPathVar->GetStringValue() );

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
        DevWarning( "Failed regenerating PNG as Texture! Couldn't read PNG from file \"%s\"\n", fileName );
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
    PreLoadTexture();

    unsigned char *vtfImageData = pVTFTexture->ImageData( 0, 0, 0 );

    // Ensure the texture we are copying to is the same format as the texture we are copying from
    pVTFTexture->ConvertImageFormat( pTexture->GetImageFormat(), false );

    if ( pTexture->IsMipmapped() )
    {
        DevWarning( "Failed regenerating PNG as Texture! Mipmapped textures are not supported \"%s\"\n", pTexture->GetName() );
        return;
    }

    Q_memcpy( vtfImageData, m_pTexturePointer, m_iSizeInBytes );
#endif
}

EXPOSE_INTERFACE( CPngMaterialProxy, IMaterialProxy, "ProceduralPng" IMATERIAL_PROXY_INTERFACE_VERSION );
