#include "cbase.h"
#include "bassmanager.h"

static CBassManager s_BassManager;
extern CBassManager *g_pBassManager = &s_BassManager;

bool CBassManager::Init()
{
    return BASS_Init( -1, 44100, 0, 0, NULL );
}

void CBassManager::Shutdown()
{
    for ( int i = m_Streams.Count() - 1; i >= 0; --i )
    {
        BASS_StreamFree( m_Streams[i] );
    }

    BASS_Free();
}

int CBassManager::ToBassFlags( int flags )
{
    int bassFlags = 0;

    if ( flags & BassManagerFlags::SAMPLE_3D )
    {
        bassFlags |= BASS_SAMPLE_3D;
    }

    if ( flags & BassManagerFlags::SAMPLE_MONO )
    {
        bassFlags |= BASS_SAMPLE_MONO;
    }

    if ( flags & BassManagerFlags::SAMPLE_LOOP )
    {
        bassFlags |= BASS_SAMPLE_LOOP;
    }

    if ( flags & BassManagerFlags::STREAM_BLOCK )
    {
        bassFlags |= BASS_STREAM_BLOCK;
    }

    return bassFlags;
}

void CBassManager::PlayUrl( const char *url, int flags /* = 0 */ )
{
    if ( flags & BassManagerFlags::DONT_PLAY )
    {
        DevWarning( "Cannot play stream with DONT_PLAY flag, unless callback is provided\n" );
        return;
    }

    int bassFlags = ToBassFlags( flags );

    HSTREAM stream = BASS_StreamCreateURL( url, 0, bassFlags, NULL, NULL );

    if ( !stream )
    {
        DevWarning( "Failed to create stream from URL: %s\n", url );
        return;
    }

    if ( !BASS_ChannelPlay( stream, TRUE ) )
    {
        DevWarning( "Failed to play stream from URL: %s\n", url );
        BASS_StreamFree( stream );
        return;
    }

    m_Streams.AddToTail( stream );
}

struct PlayUrlCallbackDataWrapper
{
    CBassManagerCallback *callback;
    IBassManagerCallbackData *callbackData;
    CBassManager *manager;
};

void CALLBACK EnqueueCallback( const void *buffer, DWORD length, void *user )
{
    PlayUrlCallbackDataWrapper *wrapper = ( PlayUrlCallbackDataWrapper * )user;

    wrapper->manager->EnqueueCallbackTask(
        [wrapper, buffer, length]()
        {
            if ( wrapper->callback )
            {
                wrapper->callback( buffer, length, wrapper->callbackData );
            }

            // Done downloading the sound, clean up the callback data
            if ( !buffer )
            {
                wrapper->callbackData->Release();
                delete wrapper;
            }
        } );
}

/// <summary>
/// Plays a sound from a URL, calling the provided callback (on the main thread) for each buffer of data received.
/// </summary>
/// <param name="url"></param>
/// <param name="flags"></param>
/// <param name="callback"></param>
/// <param name="callbackData"></param>
void CBassManager::PlayUrlEx( const char *url, int flags, CBassManagerCallback callback, IBassManagerCallbackData *callbackData )
{
    int bassFlags = ToBassFlags( flags );

    PlayUrlCallbackDataWrapper *wrapper = new PlayUrlCallbackDataWrapper;
    wrapper->callback = callback;
    wrapper->callbackData = callbackData;
    wrapper->manager = this;

    HSTREAM stream = BASS_StreamCreateURL( url, 0, bassFlags, EnqueueCallback, wrapper );

    if ( !stream )
    {
        DevWarning( "Failed to create stream from URL: %s\n", url );
        return;
    }

    if ( ( flags & BassManagerFlags::DONT_PLAY ) == 0 && !BASS_ChannelPlay( stream, TRUE ) )
    {
        DevWarning( "Failed to play stream from URL: %s\n", url );
        BASS_StreamFree( stream );
        return;
    }

    m_Streams.AddToTail( stream );
}

void CBassManager::Update( float frametime )
{
    // Goes through all streams to clean up the ones that are no longer playing
    for ( int i = m_Streams.Count() - 1; i >= 0; --i )
    {
        HSTREAM stream = m_Streams[i];

        if ( !BASS_ChannelIsActive( stream ) )
        {
            BASS_StreamFree( stream );
            m_Streams.Remove( i );
        }
    }

    AUTO_LOCK( m_TaskQueueMutex );
    while ( !m_TaskQueue.empty() )
    {
        // Execute the callback task on the main thread
        m_TaskQueue.front()();
        m_TaskQueue.pop();
    }
}

void CBassManager::EnqueueCallbackTask( std::function< void() > task )
{
    AUTO_LOCK( m_TaskQueueMutex );
    m_TaskQueue.push( task );
}
