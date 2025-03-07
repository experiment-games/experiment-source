#include "cbase.h"
#include <inetchannel.h>
#include "../engine_patches.h"
#include <netadr.h>

#include <psapi.h>

using Function_ClientState_SetSignonState_t = bool( __fastcall* )( void* pThis, int state, int count );

#ifdef _WIN64
/*
    ```
    48 89 5C 24 18
    55
    56
    57
    48 8D 6C 24 90
    48 81 EC 70 01 00 00
    41 8B F0
    8B FA
    48 8B D9
    E8 ? ? ? ?
    84 C0
    75 0C
    E8 ? ? ? ?
    32 C0
    E9 ? ? ? ?
    8B D7
    ```
*/
const char* CLIENTSTATE_SETSIGNONSTATE_SIG =
    "\x48\x89\x5C\x24\x18"
    "\x55"
    "\x56"
    "\x57"
    "\x48\x8D\x6C\x24\x90"
    "\x48\x81\xEC\x70\x01\x00\x00"
    "\x41\x8B\xF0"
    "\x8B\xFA"
    "\x48\x8B\xD9"
    "\xE8\x00\x00\x00\x00"
    "\x84\xC0"
    "\x75\x0C"
    "\xE8\x00\x00\x00\x00"
    "\x32\xC0"
    "\xE9\x00\x00\x00\x00"
    "\x8B\xD7";

const char* CLIENTSTATE_SETSIGNONSTATE_MASK =
    "xxxxx"
    "x"
    "x"
    "x"
    "xxxxx"
    "xxxxxxx"
    "xxx"
    "xx"
    "xxx"
    "x????"
    "xx"
    "xx"
    "x????"
    "xx"
    "x????"
    "xx";

#else
#endif

static Function_ClientState_SetSignonState_t OriginalClientStateSetSignOnState = nullptr;

#pragma warning( disable : 4189 )  // Disable warning about unused variable

bool __fastcall DetourClientStateSetSignOnState(
    void* thisClientState,
    int state,
    int count )
{
    bool result = OriginalClientStateSetSignOnState( thisClientState, state, count );

#ifdef _WIN64
    BEGIN_MEMORY_LAYOUT( ClientState, thisClientState );

    SKIP_UNKNOWN_TO( ClientState, 21 );

    DEFINE_INT( ClientState, socket );
    DEFINE_PTR( ClientState, INetChannel*, netChannel );
    DEFINE_INT( ClientState, challengeNumber );
    DEFINE_DOUBLE( ClientState, connectTime );
    DEFINE_INT( ClientState, retryNumber );
    DEFINE_STRING( ClientState, retryAddress, MAX_PATH );
    DEFINE_TYPE( ClientState, CUtlString, retrySourceTag );
    DEFINE_INT( ClientState, retryChallenge );  // Matches retryChallenge in DetourClientConnect
    DEFINE_INT( ClientState, signOnState );

    END_MEMORY_LAYOUT( ClientState );
#else
    //  TODO: x86 version
#endif

    // TODO: This is only for validating we have correctly deduced the memory layout. Remove this later.
    if ( netChannel )
    {
        // Ensure we've got a pointer to the real net channel
        Assert( netChannel->GetProtocolVersion() == 24 );

        // Make sure we really have the correct setup
        Assert( strcmp( retrySourceTag->Get(), "listenserver" ) == 0 );

        if ( signOnState == SIGN_ON_STATE_CONNECTED )
        {
            // TODO: Testing if we have successfully hooked into networking
        }
    }

    return result;
}

#pragma warning( default : 4189 )  // Enable warning about unused variable

void ApplyClientStateDetours()
{
    HMODULE hModule = GetModuleHandle( "engine.dll" );
    DWORD_PTR functionAddress = FindPattern(
        hModule,
        CLIENTSTATE_SETSIGNONSTATE_SIG,
        CLIENTSTATE_SETSIGNONSTATE_MASK );
    DWORD_PTR baseAddress = GetModuleBaseAddress( hModule );

    // DWORD_PTR expectedAsInIda = baseAddress + 0xA3DF0;
    // bool same = ( functionAddress ) == ( expectedAsInIda );

    if ( !functionAddress )
    {
        DevWarning( "Failed to find ClientState::SetSignonState function address!\n" );
        return;
    }

    if ( MH_CreateHook(
             ( LPVOID )functionAddress,
             &DetourClientStateSetSignOnState,
             reinterpret_cast< LPVOID* >( &OriginalClientStateSetSignOnState ) ) != MH_OK )
    {
        DevWarning( "Failed to hook ClientConnect.\n" );
        Assert( 0 );
    }
    else
    {
        MH_EnableHook( ( LPVOID )functionAddress );
    }
}
