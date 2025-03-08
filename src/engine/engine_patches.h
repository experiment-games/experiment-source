#ifndef ENGINE_PATCHES_H
#define ENGINE_PATCHES_H

#ifdef _WIN32
#pragma once
#endif

#include "MinHook.h"
#undef CreateEvent  // WinBase.h defines this, but it messes with out gameeventmanager->CreateEvent

DWORD_PTR GetModuleBaseAddress( HMODULE module );
DWORD_PTR FindPattern( HMODULE module, const char* pattern, const char* mask );

void ApplyDetoursOnDllStartup();
void NotifyDetoursOnLevelShutdown();

/*
** Specific detours
*/

#ifdef GAME_DLL  // Server detours

#include <player.h>

// void HandleCheckPasswordDetour( void *serverObject, const char *steamIdAsString );
// void UpdateCheckPasswordDetourOnLevelShutdown();

void ApplyClientConnectDetour();
void Engine_PlayerAllowNewUserInfo( CBasePlayer* pPlayer, const char* userInfoName );

#endif

#ifdef CLIENT_DLL  // Client detours

void ApplyClientStateDetours();

#endif

/*
** Definitions
*/

#define SIGN_ON_STATE_NONE 0         // Not connected yet
#define SIGN_ON_STATE_CHALLENGING 1  // Challenging server (retryChallenge?)
#define SIGN_ON_STATE_CONNECTED 2    // NetChannels can be used now
#define SIGN_ON_STATE_SPAWNING 3     // Got server info and string tables
#define SIGN_ON_STATE_PRESPAWN 4     // On getting this, the server spawns the player
#define SIGN_ON_STATE_SPAWN 5        // Server spawned player. Start sending entity packets
#define SIGN_ON_STATE_FULL 6         // Fully connected, initial handshake completed
#define SIGN_ON_STATE_CHANGELEVEL 7

/*
** Macro's for accessing structured memory
*/

// Macro to begin defining a memory layout structure
#define BEGIN_MEMORY_LAYOUT( structName, basePtr )             \
    uintptr_t _##structName##_base = ( uintptr_t )( basePtr ); \
    int _##structName##_offset = 0;

// Macro to skip unknown definitions
#define SKIP_UNKNOWN( structName, size ) \
    _##structName##_offset += size;
#define SKIP_UNKNOWN_TO( structName, absolutePosition ) \
    _##structName##_offset = absolutePosition;

// Macro to end the structure definition
#define END_MEMORY_LAYOUT( structName )

// Macro for defining an int field
#define DEFINE_INT( structName, fieldName )                                     \
    ALIGN_OFFSET( structName, 4 );                                              \
    int fieldName = *( int* )( _##structName##_base + _##structName##_offset ); \
    _##structName##_offset += sizeof( int );

// Macro for defining a double field
#define DEFINE_DOUBLE( structName, fieldName )                                        \
    ALIGN_OFFSET( structName, 8 );                                                    \
    double fieldName = *( double* )( _##structName##_base + _##structName##_offset ); \
    _##structName##_offset += sizeof( double );

// Macro for defining a string field with a fixed length
#define DEFINE_STRING( structName, fieldName, length )                                        \
    const char* fieldName = ( const char* )( _##structName##_base + _##structName##_offset ); \
    _##structName##_offset += ( length ) * sizeof( char );

// Macro for defining a custom type field
#define DEFINE_TYPE( structName, type, fieldName )                                \
    type* fieldName = ( type* )( _##structName##_base + _##structName##_offset ); \
    _##structName##_offset += sizeof( type );

// Macro for a pointer of a certain type
#define DEFINE_PTR( structName, type, fieldName )                                 \
    ALIGN_OFFSET( structName, 8 );                                                \
    type fieldName = *( type* )( _##structName##_base + _##structName##_offset ); \
    _##structName##_offset += sizeof( type );

// Macro for defining a boolean field
#define DEFINE_BOOL( structName, fieldName )                                      \
    ALIGN_OFFSET( structName, 4 );                                                \
    bool fieldName = *( bool* )( _##structName##_base + _##structName##_offset ); \
    _##structName##_offset += sizeof( bool );

// Macro to align the current offset to a specific boundary
#define ALIGN_OFFSET( structName, alignment ) \
    _##structName##_offset = ( _##structName##_offset + ( ( alignment )-1 ) ) & ~( ( alignment )-1 );

// Macro to get the current offset
#define CURRENT_OFFSET( structName ) _##structName##_offset

// Generic field access macro for getting typed fields at specific offsets
#define GET_MEMORY_FIELD( basePtr, offset, type ) ( *( type* )( ( uintptr_t )( basePtr ) + ( offset ) ) )

// Specialized macros for common field types
#define GET_MEMORY_INT( basePtr, offset ) GET_MEMORY_FIELD( basePtr, offset, int )
#define GET_MEMORY_BOOL( basePtr, offset ) GET_MEMORY_FIELD( basePtr, offset, bool )
#define GET_MEMORY_PTR( basePtr, offset, type ) GET_MEMORY_FIELD( basePtr, offset, type* )
#define GET_MEMORY_STRING( basePtr, offset ) ( ( const char* )( ( uintptr_t )( basePtr ) + ( offset ) ) )
// Get a pointer to a field's memory location for modification
#define GET_MEMORY_FIELD_PTR( basePtr, offset, type ) ( ( type* )( ( uintptr_t )( basePtr ) + ( offset ) ) )

// Specialized macro for bool field modification
#define GET_MEMORY_BOOL_FOR_CHANGE( basePtr, offset ) ( *GET_MEMORY_FIELD_PTR( basePtr, offset, bool ) )

#endif  // ENGINE_PATCHES_H
