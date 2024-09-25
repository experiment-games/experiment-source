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

// struct PlayUrlCallbackDataWrapper
//{
//     CBassManagerBlockDownloadedCallback *callback;
//     IBassManagerCallbackData *callbackData;
//     CBassManager *manager;
// };
//  generic struct so we can change CBassManagerBlockDownloadedCallback
template < class T >
struct PlayUrlCallbackDataWrapper
{
    T *callback;
    IBassManagerCallbackData *callbackData;
    CBassManager *manager;
};

void CALLBACK EnqueueBlockDownloadedCallback( const void *buffer, DWORD length, void *user )
{
    auto *wrapper = ( PlayUrlCallbackDataWrapper< CBassManagerBlockDownloadedCallback > * )user;

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

void CALLBACK EnqueueReadyCallback( HSYNC handle, DWORD channel, DWORD data, void *user )
{
    auto *wrapper = ( PlayUrlCallbackDataWrapper< CBassManagerReadyCallback > * )user;

    wrapper->manager->EnqueueCallbackTask(
        [wrapper, handle, channel, data]()
        {
            if ( wrapper->callback )
            {
                wrapper->callback( handle, channel, data, wrapper->callbackData );
            }

            // Done downloading the sound, clean up the callback data
            if ( !handle )
            {
                wrapper->callbackData->Release();
                delete wrapper;
            }
        } );
}

/// <summary>
/// Plays a sound from a URL, calling the provided callback (on the main thread) when the sound is ready to play.
/// </summary>
/// <param name="url"></param>
/// <param name="flags"></param>
IAudioChannel *CBassManager::PlayUrlGetAudioChannel(
    const char *url,
    int flags )
{
    int bassFlags = ToBassFlags( flags );

    HSTREAM stream = BASS_StreamCreateURL( url, 0, bassFlags, NULL, NULL );
    // TODO: Takes too long to download: BASS_ChannelSetSync( stream, BASS_SYNC_DOWNLOAD, 0, EnqueueReadyCallback, wrapper );

    if ( !stream )
    {
        DevWarning( "Failed to create stream from URL: %s\n", url );
        return nullptr;
    }

    if ( ( flags & BassManagerFlags::DONT_PLAY ) == 0 && !BASS_ChannelPlay( stream, TRUE ) )
    {
        DevWarning( "Failed to play stream from URL: %s\n", url );
        BASS_StreamFree( stream );
        return nullptr;
    }

    return new CAudioChannel( stream );

    // We don't have the stream cleaned up, since the callback might use it to replay the sound:
    // m_Streams.AddToTail( stream ); // hence this is commented out
}

/// <summary>
/// Plays a sound from a URL, calling the provided callback (on the main thread) for each buffer of data received.
/// </summary>
/// <param name="url"></param>
/// <param name="flags"></param>
/// <param name="blockDownloadedCallback"></param>
/// <param name="callbackData"></param>
void CBassManager::PlayUrlWithBlockCallback(
    const char *url,
    int flags,
    CBassManagerBlockDownloadedCallback blockDownloadedCallback,
    IBassManagerCallbackData *callbackData )
{
    int bassFlags = ToBassFlags( flags );

    auto *wrapper = new PlayUrlCallbackDataWrapper< CBassManagerBlockDownloadedCallback >;
    wrapper->callback = blockDownloadedCallback;
    wrapper->callbackData = callbackData;
    wrapper->manager = this;

    HSTREAM stream = BASS_StreamCreateURL( url, 0, bassFlags, EnqueueBlockDownloadedCallback, wrapper );

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

    // We don't have the stream cleaned up, since the callback might use it to replay the sound:
    // m_Streams.AddToTail( stream ); // hence this is commented out
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

/*
 * Wrapper around a BASS handle
 */
CAudioChannel::CAudioChannel( HSTREAM handle )
    : m_Handle( handle )
{
}

CAudioChannel::~CAudioChannel()
{
    BASS_StreamFree( m_Handle );
}

void CAudioChannel::Release()
{
    delete this;
}

void CAudioChannel::Play()
{
    if ( !BASS_ChannelPlay( m_Handle, TRUE ) )
    {
        int error = BASS_ErrorGetCode();
        DevWarning( "Failed to play stream (error code: %d)\n", error );
    }
}

void CAudioChannel::Pause()
{
    BASS_ChannelPause( m_Handle );
}

void CAudioChannel::Stop()
{
    BASS_ChannelStop( m_Handle );
}

void CAudioChannel::SetVolume( float volume )
{
    BASS_ChannelSetAttribute( m_Handle, BASS_ATTRIB_VOL, volume );
}

float CAudioChannel::GetVolume()
{
    float volume;
    BASS_ChannelGetAttribute( m_Handle, BASS_ATTRIB_VOL, &volume );
    return volume;
}

void CAudioChannel::SetPan( float pan )
{
    BASS_ChannelSetAttribute( m_Handle, BASS_ATTRIB_PAN, pan );
}

float CAudioChannel::GetPan()
{
    float pan;
    BASS_ChannelGetAttribute( m_Handle, BASS_ATTRIB_PAN, &pan );
    return pan;
}

void CAudioChannel::SetPlaybackRate( float rate )
{
    BASS_ChannelSetAttribute( m_Handle, BASS_ATTRIB_FREQ, rate * 44100.0f );
}

float CAudioChannel::GetPlaybackRate()
{
    float rate;
    BASS_ChannelGetAttribute( m_Handle, BASS_ATTRIB_FREQ, &rate );
    return rate / 44100.0f;
}

void CAudioChannel::SetTime( float time, bool dontDecode /*= false*/ )
{
    int mode = BASS_POS_BYTE;

    if ( !dontDecode )
    {
        mode |= BASS_POS_DECODETO;
    }

    int bytes = BASS_ChannelSeconds2Bytes( m_Handle, time );
    BASS_ChannelSetPosition( m_Handle, bytes, mode );
}

float CAudioChannel::GetTime()
{
    int bytes = BASS_ChannelGetPosition( m_Handle, BASS_POS_BYTE );
    return BASS_ChannelBytes2Seconds( m_Handle, bytes );
}

float CAudioChannel::GetLength()
{
    return BASS_ChannelBytes2Seconds( m_Handle, BASS_ChannelGetLength( m_Handle, BASS_POS_BYTE ) );
}

float CAudioChannel::GetBufferedTime()
{
    return BASS_ChannelBytes2Seconds( m_Handle, BASS_StreamGetFilePosition( m_Handle, BASS_FILEPOS_DOWNLOAD ) );
}

void CAudioChannel::SetPos( const Vector &pos, const Vector &dir /*= Vector( 0, 0, 0 )*/ )
{
    BASS_3DVECTOR position = { pos.x, pos.y, pos.z };
    BASS_3DVECTOR direction = { dir.x, dir.y, dir.z };

    BASS_ChannelSet3DPosition( m_Handle, &position, &direction, NULL );
}

Vector CAudioChannel::GetPos()
{
    Vector pos;
    BASS_3DVECTOR position;

    BASS_ChannelGet3DPosition( m_Handle, &position, NULL, NULL );
    pos = Vector( position.x, position.y, position.z );

    return pos;
}

void CAudioChannel::Set3DEnabled( bool enable )
{
    BASS_ChannelSet3DAttributes( m_Handle, enable ? BASS_3DMODE_NORMAL : BASS_3DMODE_OFF, -1, -1, -1, -1, -1 );
}

bool CAudioChannel::Get3DEnabled()
{
    DWORD mode;
    BASS_ChannelGet3DAttributes( m_Handle, &mode, NULL, NULL, NULL, NULL, NULL );
    return mode != BASS_3DMODE_OFF;
}

void CAudioChannel::Set3DCone( float innerAngle, float outerAngle, float outerVolume )
{
    BASS_ChannelSet3DAttributes( m_Handle, -1, -1, -1, innerAngle, outerAngle, outerVolume );
}

void CAudioChannel::Get3DCone( DWORD *innerAngle, DWORD *outerAngle, float *outerVolume )
{
    BASS_ChannelGet3DAttributes( m_Handle, NULL, NULL, NULL, innerAngle, outerAngle, outerVolume );
}

void CAudioChannel::Set3DFadeDistance( float min, float max )
{
    BASS_ChannelSet3DAttributes( m_Handle, -1, min, max, -1, -1, -1 );
}

void CAudioChannel::Get3DFadeDistance( float &min, float &max )
{
    BASS_ChannelGet3DAttributes( m_Handle, NULL, &min, &max, NULL, NULL, NULL );
}

void CAudioChannel::SetLooping( bool enable )
{
    BASS_ChannelFlags( m_Handle, enable ? BASS_SAMPLE_LOOP : 0, BASS_SAMPLE_LOOP );
}

bool CAudioChannel::IsLooping()
{
    return BASS_ChannelFlags( m_Handle, 0, 0 ) & BASS_SAMPLE_LOOP ? true : false;
}

bool CAudioChannel::Is3D()
{
    return BASS_ChannelFlags( m_Handle, 0, 0 ) & BASS_SAMPLE_3D ? true : false;
}

bool CAudioChannel::IsOnline()
{
    return BASS_ChannelFlags( m_Handle, 0, 0 ) & BASS_STREAM_STATUS ? true : false;
}

bool CAudioChannel::IsBlockStreamed()
{
    return BASS_ChannelFlags( m_Handle, 0, 0 ) & BASS_STREAM_BLOCK ? true : false;
}

bool CAudioChannel::IsValid()
{
    // TODO: When is a channel invalid?
    return BASS_ChannelIsActive( m_Handle ) != BASS_ACTIVE_STOPPED;
}

int CAudioChannel::GetState()
{
    return BASS_ChannelIsActive( m_Handle );
}

int CAudioChannel::GetSamplingRate()
{
    float rate;
    BASS_ChannelGetAttribute( m_Handle, BASS_ATTRIB_FREQ, &rate );
    return rate;
}

int CAudioChannel::GetBitsPerSample()
{
    // TODO: https://wiki.facepunch.com/gmod/IGModAudioChannel:GetBitsPerSample
    return 0;
}

int CAudioChannel::GetAverageBitRate()
{
    float rate;
    BASS_ChannelGetAttribute( m_Handle, BASS_ATTRIB_BITRATE, &rate );
    return rate;
}

void CAudioChannel::GetLevel( float &left, float &right )
{
    DWORD level = BASS_ChannelGetLevel( m_Handle );
    left = LOWORD( level ) / 32768.0f;
    right = HIWORD( level ) / 32768.0f;
}

void CAudioChannel::GetTagsID3( CUtlMap< CUtlString, CUtlString > &tags )
{
    const char *comments = BASS_ChannelGetTags( m_Handle, BASS_TAG_ID3 );
    if ( !comments )
    {
        return;
    }

    while ( *comments )
    {
        CUtlString key = comments;
        comments += key.Length() + 1;

        CUtlString value = comments;
        comments += value.Length() + 1;

        tags.Insert( key, value );
    }
}

void CAudioChannel::GetTagsOGG( CUtlMap< CUtlString, CUtlString > &tags )
{
    const char *comments = BASS_ChannelGetTags( m_Handle, BASS_TAG_OGG );
    if ( !comments )
    {
        return;
    }

    while ( *comments )
    {
        CUtlString key = comments;
        comments += key.Length() + 1;

        CUtlString value = comments;
        comments += value.Length() + 1;

        tags.Insert( key, value );
    }
}

void CAudioChannel::GetTagsHTTP( CUtlMap< CUtlString, CUtlString > &tags )
{
    const char *comments = BASS_ChannelGetTags( m_Handle, BASS_TAG_HTTP );
    if ( !comments )
    {
        return;
    }

    while ( *comments )
    {
        CUtlString key = comments;
        comments += key.Length() + 1;

        CUtlString value = comments;
        comments += value.Length() + 1;

        tags.Insert( key, value );
    }
}

void CAudioChannel::GetTagsMeta( CUtlString &tags )
{
    const char *comments = BASS_ChannelGetTags( m_Handle, BASS_TAG_META );
    if ( !comments )
    {
        return;
    }

    tags = comments;
}

void CAudioChannel::GetTagsVendor( CUtlString &tags )
{
    const char *comments = BASS_ChannelGetTags( m_Handle, BASS_TAG_VENDOR );
    if ( !comments )
    {
        return;
    }

    tags = comments;
}

void CAudioChannel::FFT( CUtlVector< float > &fft, int size )
{
    fft.SetSize( size );
    BASS_ChannelGetData( m_Handle, fft.Base(), BASS_DATA_FFT2048 );
}
