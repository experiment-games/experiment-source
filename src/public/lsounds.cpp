#include "cbase.h"
#include "luamanager.h"
#include "luasrclib.h"
#include <lsounds.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ISoundEmitterSystemBase *soundemitterbase;

LUA_REGISTRATION_INIT( Sounds )

// LUA_BINDING_BEGIN( GameEvents, Listen, "library", "Call a hook for this game event name, when the event occurs." )
//{
//     luaGameEventListener->AddListener( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "eventName" ) );
//     return 0;
// }
// LUA_BINDING_END()

/*abstract_class ISoundEmitterSystemBase : public IAppSystem
{
public:
  // Init, shutdown called after we know what mod is running
  virtual bool			ModInit() = 0;
  virtual void			ModShutdown() = 0;

  virtual int				GetSoundIndex( const char *pName ) const = 0;
  virtual bool			IsValidIndex( int index ) = 0;
  virtual int				GetSoundCount( void ) = 0;

  virtual const char		*GetSoundName( int index ) = 0;
  virtual bool			GetParametersForSound( const char *soundname, CSoundParameters& params, gender_t gender, bool isbeingemitted = false ) = 0;

  virtual const char		*GetWaveName( CUtlSymbol& sym ) = 0;
  virtual CUtlSymbol		AddWaveName( const char *name ) = 0;

  virtual soundlevel_t	LookupSoundLevel( const char *soundname ) = 0;
  virtual const char		*GetWavFileForSound( const char *soundname, char const *actormodel ) = 0;
  virtual const char		*GetWavFileForSound( const char *soundname, gender_t gender ) = 0;
  virtual int				CheckForMissingWavFiles( bool verbose ) = 0;
  virtual const char		*GetSourceFileForSound( int index ) const = 0;

  // Iteration methods
  virtual int				First() const = 0;
  virtual int				Next( int i ) const = 0;
  virtual int				InvalidIndex() const = 0;

  virtual CSoundParametersInternal *InternalGetParametersForSound( int index ) = 0;

  // The host application is responsible for dealing with dirty sound scripts, etc.
  virtual bool			AddSound( const char *soundname, const char *scriptfile, const CSoundParametersInternal& params ) = 0;
  virtual void			RemoveSound( const char *soundname ) = 0;
  virtual void			MoveSound( const char *soundname, const char *newscript ) = 0;
  virtual void			RenameSound( const char *soundname, const char *newname ) = 0;

  virtual void			UpdateSoundParameters( const char *soundname, const CSoundParametersInternal& params ) = 0;

  virtual int				GetNumSoundScripts() const = 0;
  virtual char const		*GetSoundScriptName( int index ) const = 0;
  virtual bool			IsSoundScriptDirty( int index ) const = 0;
  virtual int				FindSoundScript( const char *name ) const = 0;
  virtual void			SaveChangesToSoundScript( int scriptindex ) = 0;

  virtual void			ExpandSoundNameMacros( CSoundParametersInternal& params, char const *wavename ) = 0;
  virtual gender_t		GetActorGender( char const *actormodel ) = 0;
  virtual void			GenderExpandString( char const *actormodel, char const *in, char *out, int maxlen ) = 0;
  virtual void			GenderExpandString( gender_t gender, char const *in, char *out, int maxlen ) = 0;
  virtual bool			IsUsingGenderToken( char const *soundname ) = 0;

  // For blowing away caches based on filetimstamps of the manifest, or of any of the
  //  .txt files that are read into the sound emitter system
  virtual unsigned int	GetManifestFileTimeChecksum() = 0;

  // Called from both client and server (single player) or just one (server only in dedicated server and client only if connected to a remote server)
  // Called by LevelInitPreEntity to override sound scripts for the mod with level specific overrides based on custom mapnames, etc.
  virtual void			AddSoundOverrides( char const *scriptfile, bool bPreload = false ) = 0;

  // Called by either client or server in LevelShutdown to clear out custom overrides
  virtual void			ClearSoundOverrides() = 0;

  virtual bool			GetParametersForSoundEx( const char *soundname, HSOUNDSCRIPTHANDLE& handle, CSoundParameters& params, gender_t gender, bool isbeingemitted = false ) = 0;
  virtual soundlevel_t	LookupSoundLevelByHandle( char const *soundname, HSOUNDSCRIPTHANDLE& handle ) = 0;

  virtual void			ReloadSoundEntriesInList( IFileList *pFilesToReload ) = 0;

  // Called by either client or server to force ModShutdown and ModInit
  virtual void			Flush() = 0;
};
*/

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

/*
** Open Sounds library
*/
LUALIB_API int luaopen_Sounds( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Sounds );

    return 1;
}
