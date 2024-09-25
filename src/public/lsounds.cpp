#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include <lsounds.h>

#ifdef CLIENT_DLL
#include <util/bassmanager.h>
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ISoundEmitterSystemBase *soundemitterbase;

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

#ifdef CLIENT_DLL

#define PLAY_SOUND_FLAG BassManagerFlags

struct PlayUrlCallbackData
{
    lua_State *L;
    int callbackRef;
};

void CALLBACK CallLuaCallback( const void *buffer, DWORD length, void *user )
{
    PlayUrlCallbackData *callbackData = ( PlayUrlCallbackData * )user;
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
    // lua_run_menu Sounds.PlayUrl( "https://www2.cs.uic.edu/~i101/SoundFiles/StarWars3.wav", _E.PLAY_SOUND_FLAG.SAMPLE_3D, function( a, b, c ) print( a, b, c ) end )

    luaL_argcheck( L, lua_isfunction( L, 3 ), 3, "expected function" );

    // Create a reference to the callback function
    lua_pushvalue( L, 3 );
    int callbackRef = luaL_ref( L, LUA_REGISTRYINDEX );

    PlayUrlCallbackData *callbackData = new PlayUrlCallbackData;
    callbackData->L = L;
    callbackData->callbackRef = callbackRef;
    g_pBassManager->PlayUrlEx( url, flags, CallLuaCallback, callbackData );
    delete callbackData;

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

#ifdef CLIENT_DLL
    LUA_SET_ENUM_LIB_BEGIN( L, "PLAY_SOUND_FLAG" );
    lua_pushenum( L, BassManagerFlags::SAMPLE_3D, "SAMPLE_3D" );
    lua_pushenum( L, BassManagerFlags::SAMPLE_MONO, "SAMPLE_MONO" );
    lua_pushenum( L, BassManagerFlags::SAMPLE_LOOP, "SAMPLE_LOOP" );
    lua_pushenum( L, BassManagerFlags::DONT_PLAY, "DONT_PLAY" );
    lua_pushenum( L, BassManagerFlags::STREAM_BLOCK, "STREAM_BLOCK" );
    LUA_SET_ENUM_LIB_END( L );
#endif

    return 1;
}
