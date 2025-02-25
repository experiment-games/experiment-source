#ifndef BASSMANAGER_H
#define BASSMANAGER_H
#pragma once

#include "GameEventListener.h"
#include "bass.h"
#include <queue>
#include <functional>

// since bass.h includes winlite we need to undef this
#ifdef CreateEvent
#undef CreateEvent
#endif

typedef void( CALLBACK CBassManagerBlockDownloadedCallback )( const void *buffer, DWORD length, void *user );

typedef void( CALLBACK CBassManagerReadyCallback )( HSYNC handle, DWORD channel, DWORD data, void *user );

class IAudioChannel;  // forward declaration

namespace BassManagerFlags
{
enum Type
{
    NONE = 0,
    SAMPLE_3D = 1 << 0,
    SAMPLE_MONO = 1 << 1,
    SAMPLE_LOOP = 1 << 2,
    DONT_PLAY = 1 << 3,
    STREAM_BLOCK = 1 << 4,
};
}

abstract_class IBassManagerCallbackData
{
   public:
    virtual void Release() = 0;
};

class CBassManager : public CBaseGameSystemPerFrame
{
   public:
    virtual bool Init();
    virtual void Shutdown();

    virtual void Update( float frametime );

   public:
    bool IsInitialized() const
    {
        return m_bInitialized;
    }

    void PlayUrl( const char *url, int flags = 0 );
    IAudioChannel *PlayUrlGetAudioChannel(
        const char *url,
        int flags );

    void PlayFile( const char *filename, int flags = 0 );
    IAudioChannel *PlayFileGetAudioChannel(
        const char *filename,
        int flags );

    // void PlayUrlWithBlockCallback(
    //     const char *url,
    //     int flags,
    //     CBassManagerBlockDownloadedCallback blockDownloadedCallback,
    //     IBassManagerCallbackData *callbackData );

    // void EnqueueCallbackTask( std::function< void() > task );

   protected:
    int ToBassFlags( int flags );

    bool m_bInitialized;
    CUtlVector< HSTREAM > m_Streams;

    //// Task queue for processing callbacks on the main thread
    // std::queue< std::function< void() > > m_TaskQueue;
    // CThreadFastMutex m_TaskQueueMutex;
};

extern CBassManager *g_pBassManager;

abstract_class IAudioChannel
{
   public:
    virtual ~IAudioChannel() {}

    virtual void Release() = 0;

    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;

    virtual void SetVolume( float volume ) = 0;
    virtual float GetVolume() = 0;

    virtual void SetPan( float pan ) = 0;
    virtual float GetPan() = 0;

    virtual void SetPlaybackRate( float rate ) = 0;
    virtual float GetPlaybackRate() = 0;

    virtual void SetTime( float time, bool dontDecode = false ) = 0;
    virtual float GetTime() = 0;

    virtual float GetLength() = 0;
    virtual float GetBufferedTime() = 0;

    virtual void SetPos( const Vector &pos, const Vector &dir = Vector( 0, 0, 0 ) ) = 0;
    virtual Vector GetPos() = 0;

    virtual void Set3DEnabled( bool enable ) = 0;
    virtual bool Get3DEnabled() = 0;

    virtual void Set3DCone( float innerAngle, float outerAngle, float outerVolume ) = 0;
    virtual void Get3DCone( DWORD * innerAngle, DWORD * outerAngle, float *outerVolume ) = 0;

    virtual void Set3DFadeDistance( float min, float max ) = 0;
    virtual void Get3DFadeDistance( float &min, float &max ) = 0;

    virtual void SetLooping( bool enable ) = 0;
    virtual bool IsLooping() = 0;

    virtual bool Is3D() = 0;
    virtual bool IsOnline() = 0;
    virtual bool IsBlockStreamed() = 0;
    virtual bool IsValid() = 0;

    virtual int GetState() = 0;

    virtual int GetSamplingRate() = 0;
    virtual int GetBitsPerSample() = 0;
    virtual int GetAverageBitRate() = 0;

    virtual void GetLevel( float &left, float &right ) = 0;

    virtual void GetTagsID3( CUtlMap< CUtlString, CUtlString > & tags ) = 0;
    virtual void GetTagsOGG( CUtlMap< CUtlString, CUtlString > & tags ) = 0;
    virtual void GetTagsHTTP( CUtlMap< CUtlString, CUtlString > & tags ) = 0;
    virtual void GetTagsMeta( CUtlString & tags ) = 0;
    virtual void GetTagsVendor( CUtlString & tags ) = 0;

    virtual void FFT( CUtlVector< float > & fft, int size ) = 0;
};

/* Wrapper around a BASS handle */
class CAudioChannel : public IAudioChannel
{
   public:
    CAudioChannel( HSTREAM handle );
    virtual ~CAudioChannel();

    virtual void Release() OVERRIDE;

    virtual void Play() OVERRIDE;
    virtual void Pause() OVERRIDE;
    virtual void Stop() OVERRIDE;

    virtual void SetVolume( float volume ) OVERRIDE;
    virtual float GetVolume() OVERRIDE;

    virtual void SetPan( float pan ) OVERRIDE;
    virtual float GetPan() OVERRIDE;

    virtual void SetPlaybackRate( float rate ) OVERRIDE;
    virtual float GetPlaybackRate() OVERRIDE;

    virtual void SetTime( float time, bool dontDecode = false ) OVERRIDE;
    virtual float GetTime() OVERRIDE;

    virtual float GetLength() OVERRIDE;
    virtual float GetBufferedTime() OVERRIDE;

    virtual void SetPos( const Vector &pos, const Vector &dir = Vector( 0, 0, 0 ) ) OVERRIDE;
    virtual Vector GetPos() OVERRIDE;

    virtual void Set3DEnabled( bool enable ) OVERRIDE;
    virtual bool Get3DEnabled() OVERRIDE;

    virtual void Set3DCone( float innerAngle, float outerAngle, float outerVolume ) OVERRIDE;
    virtual void Get3DCone( DWORD *innerAngle, DWORD *outerAngle, float *outerVolume ) OVERRIDE;

    virtual void Set3DFadeDistance( float min, float max ) OVERRIDE;
    virtual void Get3DFadeDistance( float &min, float &max ) OVERRIDE;

    virtual void SetLooping( bool enable ) OVERRIDE;
    virtual bool IsLooping() OVERRIDE;

    virtual bool Is3D() OVERRIDE;
    virtual bool IsOnline() OVERRIDE;
    virtual bool IsBlockStreamed() OVERRIDE;
    virtual bool IsValid() OVERRIDE;

    virtual int GetState() OVERRIDE;

    virtual int GetSamplingRate() OVERRIDE;
    virtual int GetBitsPerSample() OVERRIDE;
    virtual int GetAverageBitRate() OVERRIDE;

    virtual void GetLevel( float &left, float &right ) OVERRIDE;

    virtual void GetTagsID3( CUtlMap< CUtlString, CUtlString > &tags ) OVERRIDE;
    virtual void GetTagsOGG( CUtlMap< CUtlString, CUtlString > &tags ) OVERRIDE;
    virtual void GetTagsHTTP( CUtlMap< CUtlString, CUtlString > &tags ) OVERRIDE;
    virtual void GetTagsMeta( CUtlString &tags ) OVERRIDE;
    virtual void GetTagsVendor( CUtlString &tags ) OVERRIDE;

    virtual void FFT( CUtlVector< float > &fft, int size ) OVERRIDE;

   private:
    HSTREAM m_Handle;
};

/*
    Help to read Source Engine files into a BASS stream

    For example:
        FileHandle_t fileHandle = filesystem->Open( filename, "rb" );

        if ( !fileHandle )
        {
            DevWarning( "Failed to open file: %s\n", filename );
            return nullptr;
        }

        HSTREAM stream = BASS_StreamCreateFileUser( STREAMFILE_NOBUFFER, bassFlags, &BASSMANAGER_SDK_FILES, fileHandle );
*/
void CALLBACK SourceFileCloseProc( void *user );
QWORD CALLBACK SourceFileLenProc( void *user );
DWORD CALLBACK SourceFileReadProc( void *buffer, DWORD length, void *user );
BOOL CALLBACK SourceFileSeekProc( QWORD offset, void *user );
extern BASS_FILEPROCS BASSMANAGER_SDK_FILES;

#endif  // BASSMANAGER_H
