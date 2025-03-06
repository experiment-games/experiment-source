#ifndef ENGINE_PATCHES_H
#define ENGINE_PATCHES_H

#ifdef _WIN32
#pragma once
#endif

#include "MinHook.h"
#undef CreateEvent  // WinBase.h defines this, but it messes with out gameeventmanager->CreateEvent

DWORD_PTR GetModuleBaseAddress( HMODULE module );

void ApplyDetoursOnDllStartup();
void NotifyDetoursOnLevelShutdown();

/*
** Specific detours
*/

#ifdef GAME_DLL  // Server detours

// void HandleCheckPasswordDetour( void *serverObject, const char *steamIdAsString );
// void UpdateCheckPasswordDetourOnLevelShutdown();

void ApplyClientConnectDetour();

#endif

#ifdef CLIENT_DLL  // Client detours

// ...

#endif

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
    _##structName##_offset = ( _##structName##_offset + ( ( alignment ) - 1 ) ) & ~( ( alignment ) - 1 );

// Macro to get the current offset
#define CURRENT_OFFSET( structName ) _##structName##_offset

// Generic field access macro for getting typed fields at specific offsets
#define GET_MEMORY_FIELD( basePtr, offset, type ) ( *( type* )( ( uintptr_t )( basePtr ) + ( offset ) ) )

// Specialized macros for common field types
#define GET_MEMORY_INT( basePtr, offset ) GET_MEMORY_FIELD( basePtr, offset, int )
#define GET_MEMORY_BOOL( basePtr, offset ) GET_MEMORY_FIELD( basePtr, offset, bool )
#define GET_MEMORY_PTR( basePtr, offset, type ) GET_MEMORY_FIELD( basePtr, offset, type* )
#define GET_MEMORY_STRING( basePtr, offset ) ( ( const char* )( ( uintptr_t )( basePtr ) + ( offset ) ) )

#endif  // ENGINE_PATCHES_H
