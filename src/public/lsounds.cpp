#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include <lsounds.h>
#include <engine/IEngineSound.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ISoundEmitterSystemBase *soundemitterbase;

#ifdef CLIENT_DLL

/*
** access functions (stack -> C)
*/

LUA_API lua_IAudioChannel *lua_toaudiochannel( lua_State *L, int idx )
{
    lua_IAudioChannel **ppHelper = ( lua_IAudioChannel ** )lua_touserdata( L, idx );
    return *ppHelper;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushaudiochannel( lua_State *L, lua_IAudioChannel *pData )
{
    lua_IAudioChannel **ppData = ( lua_IAudioChannel ** )lua_newuserdata( L, sizeof( lua_IAudioChannel ) );
    *ppData = pData;
    LUA_SAFE_SET_METATABLE( L, LUA_AUDIOCHANNELMETANAME );
}

LUALIB_API lua_IAudioChannel *luaL_checkaudiochannel( lua_State *L, int narg )
{
    lua_IAudioChannel **ppData = ( lua_IAudioChannel ** )luaL_checkudata( L, narg, LUA_AUDIOCHANNELMETANAME );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "AudioChannel expected, got NULL" );

    return *ppData;
}

/*
/*
 * Wrapper around a BASS handle
 */
LUA_REGISTRATION_INIT( AudioChannel );

LUA_BINDING_BEGIN( AudioChannel, Play, "class", "Plays the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    audioChannel->Play();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, Pause, "class", "Pauses the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    audioChannel->Pause();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, Stop, "class", "Stops the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    audioChannel->Stop();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, SetVolume, "class", "Sets the volume of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    float volume = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "volume" );
    audioChannel->SetVolume( volume );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, GetVolume, "class", "Gets the volume of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetVolume() );
    return 1;
}
LUA_BINDING_END( "number", "The volume of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, SetPan, "class", "Sets the pan of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    float pan = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "pan" );
    audioChannel->SetPan( pan );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, GetPan, "class", "Gets the pan of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetPan() );
    return 1;
}
LUA_BINDING_END( "number", "The pan of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, SetPlaybackRate, "class", "Sets the playback rate of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    float rate = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "rate" );
    audioChannel->SetPlaybackRate( rate );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, GetPlaybackRate, "class", "Gets the playback rate of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetPlaybackRate() );
    return 1;
}
LUA_BINDING_END( "number", "The playback rate of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, SetTime, "class", "Sets the time of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    float time = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "time" );
    bool dontDecode = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "dontDecode" );
    audioChannel->SetTime( time, dontDecode );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, GetTime, "class", "Gets the time of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetTime() );
    return 1;
}
LUA_BINDING_END( "number", "The time of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetLength, "class", "Gets the length of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetLength() );
    return 1;
}
LUA_BINDING_END( "number", "The length of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetBufferedTime, "class", "Gets the buffered time of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetBufferedTime() );
    return 1;
}
LUA_BINDING_END( "number", "The buffered time of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, SetPosition, "class", "Sets the position of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    Vector pos = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "pos" );
    Vector dir = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optvector, 3, NULL, "dir" );

    audioChannel->SetPos( pos, dir );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, GetPosition, "class", "Gets the position of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    Vector pos = audioChannel->GetPos();
    lua_pushvector( L, pos );
    return 1;
}
LUA_BINDING_END( "vector", "The position of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, Set3dEnabled, "class", "Sets whether 3D sound is enabled for the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    bool enable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" );
    audioChannel->Set3DEnabled( enable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, Get3dEnabled, "class", "Gets whether 3D sound is enabled for the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushboolean( L, audioChannel->Get3DEnabled() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether 3D sound is enabled for the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, Set3dCone, "class", "Sets the 3D cone of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    float innerAngle = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "innerAngle" );
    float outerAngle = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "outerAngle" );
    float outerVolume = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "outerVolume" );
    audioChannel->Set3DCone( innerAngle, outerAngle, outerVolume );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, Get3dCone, "class", "Gets the 3D cone of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    DWORD innerAngle, outerAngle;
    float outerVolume;
    audioChannel->Get3DCone( &innerAngle, &outerAngle, &outerVolume );
    lua_pushnumber( L, innerAngle );
    lua_pushnumber( L, outerAngle );
    lua_pushnumber( L, outerVolume );
    return 3;
}
LUA_BINDING_END( "number", "The inner angle of the 3D cone", "number", "The outer angle of the 3D cone", "number", "The outer volume of the 3D cone" )

LUA_BINDING_BEGIN( AudioChannel, Set3dFadeDistance, "class", "Sets the 3D fade distance of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    float min = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "min" );
    float max = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "max" );
    audioChannel->Set3DFadeDistance( min, max );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, Get3dFadeDistance, "class", "Gets the 3D fade distance of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    float min, max;
    audioChannel->Get3DFadeDistance( min, max );
    lua_pushnumber( L, min );
    lua_pushnumber( L, max );
    return 2;
}
LUA_BINDING_END( "number", "The minimum fade distance of the audio channel", "number", "The maximum fade distance of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, SetLooping, "class", "Sets whether the audio channel is looping" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    bool enable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" );
    audioChannel->SetLooping( enable );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( AudioChannel, IsLooping, "class", "Gets whether the audio channel is looping" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushboolean( L, audioChannel->IsLooping() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the audio channel is looping" )

LUA_BINDING_BEGIN( AudioChannel, Is3d, "class", "Gets whether the audio channel is 3D" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushboolean( L, audioChannel->Is3D() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the audio channel is 3D" )

LUA_BINDING_BEGIN( AudioChannel, IsOnline, "class", "Gets whether the audio channel is online" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushboolean( L, audioChannel->IsOnline() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the audio channel is online" )

LUA_BINDING_BEGIN( AudioChannel, IsBlockStreamed, "class", "Gets whether the audio channel is block streamed" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushboolean( L, audioChannel->IsBlockStreamed() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the audio channel is block streamed" )

LUA_BINDING_BEGIN( AudioChannel, IsValid, "class", "Gets whether the audio channel is valid (Not properly implemented, returns true if not stopped)" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushboolean( L, audioChannel->IsValid() );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the audio channel is valid" )

LUA_BINDING_BEGIN( AudioChannel, GetState, "class", "Gets the state of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetState() );
    return 1;
}
LUA_BINDING_END( "number", "The state of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetSamplingRate, "class", "Gets the sampling rate of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetSamplingRate() );
    return 1;
}
LUA_BINDING_END( "number", "The sampling rate of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetBitsPerSample, "class", "Gets the bits per sample of the audio channel (not properly implemented, always returns 0)" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetBitsPerSample() );
    return 1;
}
LUA_BINDING_END( "number", "The bits per sample of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetAverageBitRate, "class", "Gets the average bit rate of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    lua_pushnumber( L, audioChannel->GetAverageBitRate() );
    return 1;
}
LUA_BINDING_END( "number", "The average bit rate of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetLevel, "class", "Gets the level of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    float left, right;
    audioChannel->GetLevel( left, right );
    lua_pushnumber( L, left );
    lua_pushnumber( L, right );
    return 2;
}
LUA_BINDING_END( "number", "The left level of the audio channel", "number", "The right level of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetTagsOfId3, "class", "Gets the ID3 tags of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    CUtlMap< CUtlString, CUtlString > tags;
    audioChannel->GetTagsID3( tags );
    lua_newtable( L );

    for ( unsigned int i = 0; i < tags.Count(); i++ )
    {
        lua_pushstring( L, tags.Key( i ).String() );
        lua_pushstring( L, tags.Element( i ).String() );
        lua_settable( L, -3 );
    }

    return 1;
}
LUA_BINDING_END( "table", "The ID3 tags of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetTagsOfOgg, "class", "Gets the OGG tags of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    CUtlMap< CUtlString, CUtlString > tags;
    audioChannel->GetTagsOGG( tags );
    lua_newtable( L );

    for ( unsigned int i = 0; i < tags.Count(); i++ )
    {
        lua_pushstring( L, tags.Key( i ).String() );
        lua_pushstring( L, tags.Element( i ).String() );
        lua_settable( L, -3 );
    }

    return 1;
}
LUA_BINDING_END( "table", "The OGG tags of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetTagsOfHttp, "class", "Gets the HTTP tags of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    CUtlMap< CUtlString, CUtlString > tags;
    audioChannel->GetTagsHTTP( tags );
    lua_newtable( L );

    for ( unsigned int i = 0; i < tags.Count(); i++ )
    {
        lua_pushstring( L, tags.Key( i ).String() );
        lua_pushstring( L, tags.Element( i ).String() );
        lua_settable( L, -3 );
    }

    return 1;
}
LUA_BINDING_END( "table", "The HTTP tags of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetTagsOfMeta, "class", "Gets the meta tags of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    CUtlString tags;
    audioChannel->GetTagsMeta( tags );
    lua_pushstring( L, tags.String() );
    return 1;
}
LUA_BINDING_END( "string", "The meta tags of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetTagsOfVendor, "class", "Gets the vendor tags of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    CUtlString tags;
    audioChannel->GetTagsVendor( tags );
    lua_pushstring( L, tags.String() );
    return 1;
}
LUA_BINDING_END( "string", "The vendor tags of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, GetFft, "class", "Gets the Discrete Fourier Transform of the audio channel" )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    // TODO: Make an enum for the size
    int size = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 2, 2048, "size" );
    CUtlVector< float > fft;
    audioChannel->FFT( fft, size );
    lua_newtable( L );

    for ( int i = 0; i < fft.Count(); i++ )
    {
        lua_pushnumber( L, fft[ i ] );
        lua_rawseti( L, -2, i + 1 );
    }

    return 1;
}
LUA_BINDING_END( "table", "The Discrete Fourier Transform of the audio channel" )

LUA_BINDING_BEGIN( AudioChannel, __gc, "class", "Called when the audio channel is garbage collected. Cleans up the audio channel so it can't be reused and memory is freed." )
{
    lua_IAudioChannel *audioChannel = LUA_BINDING_ARGUMENT( luaL_checkaudiochannel, 1, "audioChannel" );
    audioChannel->Release();
    return 0;
}
LUA_BINDING_END()

#endif

LUA_REGISTRATION_INIT( Sounds )

LUA_BINDING_BEGIN( Sounds, Add, "library", "Creates a sound script." )
{
    if ( !LUA_BINDING_ARGUMENT( lua_istable, 1, "soundData" ) )
    {
        luaL_argerror( L, 1, "expected table" );
        return 0;
    }

    CSoundParametersInternal parameters;

    // For gmod compat we check both lowecase and UpperCamelCase
    GET_FIELD_WITH_COMPATIBILITY_OR_ERROR( L, 1, "Name", "name", lua_isstring );
    const char *name = luaL_checkstring( L, -1 );
    lua_pop( L, 1 );  // pop the name value

    GET_FIELD_WITH_COMPATIBILITY_OR_ERROR( L, 1, "Channel", "channel", lua_isnumber );
    parameters.SetChannel( luaL_checknumber( L, -1 ) );
    lua_pop( L, 1 );  // pop the channel value

    GET_FIELD_WITH_COMPATIBILITY( L, 1, "Level", "level" );
    if ( lua_isnumber( L, -1 ) )
        parameters.SetSoundLevel( luaL_checknumber( L, -1 ) );
    else
        parameters.SetSoundLevel( SNDLVL_NORM );
    lua_pop( L, 1 );  // pop the level value

    GET_FIELD_WITH_COMPATIBILITY( L, 1, "Volume", "volume" );
    if ( lua_isnumber( L, -1 ) )
        parameters.SetVolume( luaL_checknumber( L, -1 ) );
    else
        parameters.SetVolume( 1.0f );
    lua_pop( L, 1 );  // pop the volume value

    GET_FIELD_WITH_COMPATIBILITY( L, 1, "Volume", "volume" );
    if ( lua_istable( L, -1 ) )
    {
        lua_rawgeti( L, -1, 1 );
        lua_rawgeti( L, -2, 2 );
        parameters.SetVolume( luaL_checknumber( L, -2 ), luaL_checknumber( L, -1 ) );
        lua_pop( L, 2 );  // pop the volume values
    }
    else if ( lua_isnumber( L, -1 ) )
    {
        parameters.SetVolume( luaL_checknumber( L, -1 ), 0 );
    }
    else
    {
        parameters.SetVolume( 1.0f, 0 );
    }
    lua_pop( L, 1 );  // pop the volume value

    GET_FIELD_WITH_COMPATIBILITY( L, 1, "Pitch", "pitch" );
    if ( lua_istable( L, -1 ) )
    {
        lua_rawgeti( L, -1, 1 );
        lua_rawgeti( L, -2, 2 );
        parameters.SetPitch( luaL_checknumber( L, -2 ), luaL_checknumber( L, -1 ) );
        lua_pop( L, 2 );  // pop the pitch values
    }
    else if ( lua_isnumber( L, -1 ) )
    {
        parameters.SetPitch( luaL_checknumber( L, -1 ), 0 );
    }
    else
    {
        parameters.SetPitch( 100, 0 );
    }
    lua_pop( L, 1 );  // pop the pitch value

    GET_FIELD_WITH_COMPATIBILITY( L, 1, "Sound", "sound" );
    if ( lua_istable( L, -1 ) )
    {
        // Loop through the table and add each sound file
        lua_pushnil( L );

        while ( lua_next( L, -2 ) != 0 )
        {
            if ( lua_isstring( L, -1 ) )
            {
                CUtlSymbol soundSymbol = soundemitterbase->AddWaveName( luaL_checkstring( L, -1 ) );
                SoundFile soundFile;
                soundFile.symbol = soundSymbol;
                soundFile.gender = GENDER_NONE;
                parameters.AddSoundName( soundFile );
            }
            lua_pop( L, 1 );
        }
    }
    else if ( lua_isstring( L, -1 ) )
    {
        CUtlSymbol soundSymbol = soundemitterbase->AddWaveName( luaL_checkstring( L, -1 ) );
        SoundFile soundFile;
        soundFile.symbol = soundSymbol;
        soundFile.gender = GENDER_NONE;
        parameters.AddSoundName( soundFile );
    }
    else
    {
        luaL_argerror( L, 1, "expected field 'sound' to be a string or table of strings" );
        return 0;
    }
    lua_pop( L, 1 );  // pop the sound value

    // TODO: Check if the file needs to exist, or if we can just create a sound script without a file
    soundemitterbase->AddSound( name, "scripts/sounds/lua_procedural.txt", parameters );

    return 0;
}
LUA_BINDING_END()

// lua_run_cl Sounds.Play("ambient/levels/labs/teleport_alarm_loop1.wav", Vectors.Create(0, 0, 0))
LUA_BINDING_BEGIN( Sounds, Play, "library", "Plays a sound emitting from a place in the world. Not properly tested for sound script names (didn't work when I tried it)" )
{
    const char *pszSoundName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "soundName" ); // doc: sound script name or sound file name relative to sound/ folder
    const Vector vecOrigin = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "origin" );      // doc: position of the sound
    soundlevel_t soundLevel = LUA_BINDING_ARGUMENT_ENUM_WITH_DEFAULT( soundlevel_t, 3, SNDLVL_NORM, "soundLevel" );
    float flPitchPercent = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 100, "pitchPercent" );
    float flVolume = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 1, "volume" );
    int nDSP = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 6, 0, "dsp" );

    float duration = 0;

    EmitSound_t params;
    params.m_pSoundName = pszSoundName;
    params.m_pOrigin = &vecOrigin;
    params.m_flVolume = flVolume;
    params.m_SoundLevel = soundLevel;
    params.m_nPitch = flPitchPercent;
    params.m_nSpecialDSP = nDSP;
    params.m_flSoundTime = 0;
    params.m_pflSoundDuration = &duration;
    params.m_bWarnOnDirectWaveReference = false;

    CPVSFilter filter( vecOrigin );

    CBaseEntity::EmitSound( filter, SOUND_FROM_WORLD, params );

    lua_pushnumber( L, duration );

    return 0;
}
LUA_BINDING_END()

#ifdef CLIENT_DLL

#define PLAY_SOUND_FLAG BassManagerFlags

void CPlayUrlCallbackData::Release()
{
    lua_unref( L, callbackRef );
}

void CALLBACK CallLuaBlockDownloadedCallback( const void *buffer, DWORD length, void *user )
{
    CPlayUrlCallbackData *callbackData = ( CPlayUrlCallbackData * )user;
    lua_State *L = callbackData->L;
    int callbackRef = callbackData->callbackRef;

    lua_rawgeti( L, LUA_REGISTRYINDEX, callbackRef );
    lua_pushlstring( L, ( const char * )buffer, length );
    lua_call( L, 1, 0 );
}

// E.g: lua_run_menu Sounds.PlayUrl("https://www2.cs.uic.edu/~i101/SoundFiles/StarWars3.wav")
LUA_BINDING_BEGIN( Sounds, PlayUrl, "library", "Plays a sound from a URL.", "client" )
{
    const char *url = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "url" );
    int flags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 2, PLAY_SOUND_FLAG::STREAM_BLOCK, "flags" );

    // If no callback is provided, we just play the sound
    if ( lua_isnoneornil( L, 3 ) )
    {
        g_pBassManager->PlayUrl( url, flags );
        return 0;
    }

    // If a callback is provided, we call the callback when the sound is ready to be played
    // lua_run_menu Sounds.PlayUrl( "https://www2.cs.uic.edu/~i101/SoundFiles/StarWars3.wav", _E.PLAY_SOUND_FLAG.SAMPLE_3D, function( channel, errorId, errorName ) print( channel, errorId, errorName ) end )
    // long file:
    // lua_run_menu Sounds.PlayUrl( "https://www2.cs.uic.edu/~i101/SoundFiles/StarWars60.wav", _E.PLAY_SOUND_FLAG.SAMPLE_3D, function( channel, errorId, errorName ) print( channel, errorId, errorName ) end )
    
    // not autoplaying and playing and stopping from callback: (timers dont work in the menu because there is no tick/think there)
    // lua_run_cl Sounds.PlayUrl("https://www2.cs.uic.edu/~i101/SoundFiles/StarWars60.wav", _E.PLAY_SOUND_FLAG.SAMPLE_3D|_E.PLAY_SOUND_FLAG.DONT_PLAY, function( channel, errorId, errorName ) channel:Play() Timers.Simple(5,function() channel:Stop() end)end)
    luaL_argcheck( L, lua_isfunction( L, 3 ), 3, "expected function" );

    IAudioChannel *audioChannel = g_pBassManager->PlayUrlGetAudioChannel( url, flags );

    if ( !audioChannel )
    {
        lua_pushaudiochannel( L, audioChannel );
        lua_pushnil( L );  // TODO: implement errorId
        lua_pushnil( L );  // TODO: implement errorString
        lua_call( L, 3, 0 );  // Call the callback that is on the stack

        return 0;
    }

    lua_pushaudiochannel( L, audioChannel );
    lua_pushnil( L );  // TODO: implement errorId
    lua_pushnil( L );  // TODO: implement errorString
    lua_call( L, 3, 0 );  // Call the callback that is on the stack

    return 0;
}
LUA_BINDING_END()

// Experiment; TODO: This and related code is commented to reduce complexity and focus on the core features for now.
//LUA_BINDING_BEGIN( Sounds, PlayUrlWithBlockCallback, "library", "Plays a sound from a URL with a callback for each chunk downloaded", "client" )
//{
//    const char *url = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "url" );
//    int flags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 2, PLAY_SOUND_FLAG::STREAM_BLOCK, "flags" );
//
//    // If a callback is provided, we call the callback when the sound is ready to be played
//    // lua_run_menu Sounds.PlayUrlWithBlockCallback( "https://www2.cs.uic.edu/~i101/SoundFiles/StarWars3.wav", _E.PLAY_SOUND_FLAG.SAMPLE_3D, function( buffer ) print( buffer ) end )
//    // long file:
//    // lua_run_menu Sounds.PlayUrlWithBlockCallback( "https://www2.cs.uic.edu/~i101/SoundFiles/StarWars60.wav", _E.PLAY_SOUND_FLAG.SAMPLE_3D, function( buffer ) print( buffer ) end )
//    luaL_argcheck( L, lua_isfunction( L, 3 ), 3, "expected function" );
//
//    // Create a reference to the callback function
//    lua_pushvalue( L, 3 );
//    int callbackRef = luaL_ref( L, LUA_REGISTRYINDEX );
//
//    CPlayUrlCallbackData *callbackData = new CPlayUrlCallbackData;
//    callbackData->L = L;
//    callbackData->callbackRef = callbackRef;
//    g_pBassManager->PlayUrlWithBlockCallback( url, flags, CallLuaBlockDownloadedCallback, callbackData );
//
//    return 0;
//}
//LUA_BINDING_END()

LUA_BINDING_BEGIN( Sounds, PlayFile, "library", "Plays a sound from a file.", "client" )
{
    const char *file = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "file" );
    int flags = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optinteger, 2, PLAY_SOUND_FLAG::STREAM_BLOCK, "flags" );

    // If no callback is provided, we just play the sound
    if ( lua_isnoneornil( L, 3 ) )
    {
        g_pBassManager->PlayFile( file, flags );
        return 0;
    }

    // If a callback is provided, we call the callback when the sound is ready to be played
    // lua_run_menu Sounds.PlayFile( "sound/ambient/levels/labs/teleport_alarm_loop1.wav", _E.PLAY_SOUND_FLAG.SAMPLE_3D, function( channel, errorId, errorName ) print( channel, errorId, errorName ) end )
    luaL_argcheck( L, lua_isfunction( L, 3 ), 3, "expected function" );

    IAudioChannel *audioChannel = g_pBassManager->PlayFileGetAudioChannel( file, flags );

    if ( !audioChannel )
    {
        lua_pushaudiochannel( L, audioChannel );
        lua_pushnil( L );  // TODO: implement errorId
        lua_pushnil( L );  // TODO: implement errorString
        lua_call( L, 3, 0 );

        return 0;
    }

    lua_pushaudiochannel( L, audioChannel );
    lua_pushnil( L );  // TODO: implement errorId
    lua_pushnil( L );  // TODO: implement errorString
    lua_call( L, 3, 0 ); // Call the callback that is on the stack

    return 0;
}
LUA_BINDING_END()

#endif

/*
** Open Sounds library
*/
LUALIB_API int luaopen_Sounds( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Sounds );

    return 1;
}

#ifdef CLIENT_DLL
LUALIB_API int luaopen_AudioChannel( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_AUDIOCHANNELMETANAME );

    LUA_REGISTRATION_COMMIT( AudioChannel );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_AUDIOCHANNELMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "AudioChannel" */

    LUA_SET_ENUM_LIB_BEGIN( L, "PLAY_SOUND_FLAG" );
    lua_pushenum( L, BassManagerFlags::SAMPLE_3D, "SAMPLE_3D" );
    lua_pushenum( L, BassManagerFlags::SAMPLE_MONO, "SAMPLE_MONO" );
    lua_pushenum( L, BassManagerFlags::SAMPLE_LOOP, "SAMPLE_LOOP" );
    lua_pushenum( L, BassManagerFlags::DONT_PLAY, "DONT_PLAY" );
    lua_pushenum( L, BassManagerFlags::STREAM_BLOCK, "STREAM_BLOCK" );
    LUA_SET_ENUM_LIB_END( L );

    return 1;
}
#endif

