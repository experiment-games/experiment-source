#include "cbase.h"
#include <inetchannel.h>
#include "../engine_patches.h"
#include <util.h>
#include <netadr.h>

#include <psapi.h>

typedef void( __fastcall *Function_BaseClient_Connect_t )(
    void *thisClient,
    const char *playerName,
    int userID,
    INetChannel *netChannel,
    bool isFakePlayer,
    int challenge );

#ifdef _WIN64

/*
    To find the CBaseClient::Connect pattern, we can use the fact that the first
    instruction in it is: `COM_TimestampedLog("CBaseClient::Connect")` to find it
    easily in IDA (using `Search > Text`).

    Setup IDA to show 10 opcode bytes: `Options > General > Number of opcode bytes
    (non-graph): 10`

    For bin/x64/engine.dll, you will find a disassembly like this:
    ```
    .text:0000000180062270                               ; =============== S U B R O U T I N E =======================================
    .text:0000000180062270
    .text:0000000180062270
    .text:0000000180062270                               sub_180062270   proc near               ; CODE XREF: sub_18012F7E0+2E↓p
    .text:0000000180062270                                                                       ; DATA XREF: .rdata:0000000180362F40↓o ...
    .text:0000000180062270
    .text:0000000180062270                               arg_0           = qword ptr  8
    .text:0000000180062270                               arg_8           = qword ptr  10h
    .text:0000000180062270                               arg_10          = qword ptr  18h
    .text:0000000180062270                               arg_20          = byte ptr  28h
    .text:0000000180062270                               arg_28          = dword ptr  30h
    .text:0000000180062270
    .text:0000000180062270 48 89 5C 24 08                                mov     [rsp+arg_0], rbx
    .text:0000000180062275 48 89 6C 24 10                                mov     [rsp+arg_8], rbp
    .text:000000018006227A 48 89 74 24 18                                mov     [rsp+arg_10], rsi
    .text:000000018006227F 57                                            push    rdi
    .text:0000000180062280 48 83 EC 20                                   sub     rsp, 20h
    .text:0000000180062284 48 8B D9                                      mov     rbx, rcx
    .text:0000000180062287 49 8B E9                                      mov     rbp, r9
    .text:000000018006228A 48 8D 0D 5F 0F 30 00                          lea     rcx, aCbaseclientCon ; "CBaseClient::Connect"
    .text:0000000180062291 41 8B F8                                      mov     edi, r8d
    ... etc
    ```

    Subtract `0x180000000` from the function's address to get the true offset
    (`0x62270` in this example). This can be used later to verify dynamically
    found addresses.

    Extract the opcode bytes and create a signature with a corresponding mask.
    Replace memory references with `\x00` and mark their positions with `?` in
    the mask.

    A memory reference is any operand that points to a specific memory address,
    such as the four-byte offset in the `LEA` instruction above.
*/

// First 35 bytes of the function, wildcarding the four-byte offset in the LEA instruction.
const char *CLIENT_CONNECT_SIG =
    "\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18"
    "\x57\x48\x83\xEC\x20\x48\x8B\xD9\x49\x8B\xE9\x48\x8D\x0D\x00"
    "\x00\x00\x00\x41\x8B";

const char *CLIENT_CONNECT_MASK =
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xx";

#else

// TOOD: Not yet tested x86
const char *CLIENT_CONNECT_SIG =
    "\x55\x8B\xEC\x53\x56\x57\x68\x00\x00\x00\x00\x8B\xF1\xFF\x15"
    "\x00\x00\x00\x00\x83\xC4\x04\xE8\x00\x00\x00\x00\x6A\x0E\x8B"
    "\xC8\x8B\x10\xFF\x52\x6C\x8B\x06\x8B\xCE\xFF\x50\x68\x6A\x20"
    "\xE8\x00\x00\x00\x00\x83\xC4\x04\x85\xC0\x74\x0E\x68\x00\x00"
    "\x00\x00\x8B\xC8\xE8\x00\x00\x00\x00\xEB\x02";

const char *CLIENT_CONNECT_MASK =
    "xxxxxxx????xxx????xxx????xxxxxxx"
    "xxxxxxxxxx????xxxxxxx????xx";

#endif

static Function_BaseClient_Connect_t OriginalClientConnect = nullptr;

// Static map of connected clients to their user id so we can fetch em later
static CUtlMap< int, void * > connectedClients;

static int s_OffsetConVars;

#pragma warning( disable : 4189 )  // Disable warning about unused variable

// __fastcall workaround because you cant declare a __thiscall function without a class
// This behaves similar to __thiscall, but safely getting the ECX and EDX registry as its first parameters.
// ECX holds the address of 'this' for the detoured method.
void __fastcall DetourClientConnect(
    void *thisClient,
    const char *playerName,
    int userID,
    INetChannel *pNetChannel,
    bool bFakePlayer,
    int challenge )
{
    if ( !bFakePlayer )
    {
        int version = pNetChannel->GetProtocolVersion();
        const char *address = pNetChannel->GetAddress();
        netadr_t remote = pNetChannel->GetRemoteAddress();
        DevWarning( "Detour: Client connecting with name %s (#%i) (protocol version: %i, address: %s, port: %i)\n", playerName, userID, version, address, remote.GetPort() );
    }
    else
    {
        DevWarning( "Detour: Fake player connecting with name %s (#%i)\n", playerName, userID );
    }

    OriginalClientConnect( thisClient, playerName, userID, pNetChannel, bFakePlayer, challenge );

    // Offset of m_nUserID from CBaseClient
#ifdef _WIN64

    // int offset = 16;
    // int clientSlot = *( int * )( ( uintptr_t )thisClient + offset );
    // offset += sizeof( int );
    // int entityIndex = *( int * )( ( uintptr_t )thisClient + offset );
    // offset += sizeof( int );
    // int validateUserID = *( int * )( ( uintptr_t )thisClient + offset );
    // offset += sizeof( int );
    // const char *validateName = ( const char * )( ( uintptr_t )thisClient + offset );
    // offset += MAX_PLAYER_NAME_LENGTH * sizeof( char );
    // const char *_guid = ( const char * )( ( uintptr_t )thisClient + offset );
    // offset += ( SIGNED_GUID_LEN * sizeof( char ) ) + 1;
    // CSteamID *validateSteamId = ( CSteamID * )( ( uintptr_t )thisClient + offset );  // wont be set yet
    // offset += sizeof( CSteamID );

    //// Align int to 4 bytes
    // offset = ( offset + 3 ) & ~3;
    // int _friendsID = *( int * )( ( uintptr_t )thisClient + offset );
    // offset += sizeof( int );
    // const char *_friendsName = ( const char * )( ( uintptr_t )thisClient + offset );
    // offset += MAX_PLAYER_NAME_LENGTH * sizeof( char );

    //// Align KeyValues* to 8 bytes
    // offset = ( offset + 7 ) & ~7;
    // KeyValues *conVars = ( KeyValues * )( ( uintptr_t )thisClient + offset ); // offset: 144
    // offset += sizeof( KeyValues * );

    //// Align bools to 4 bytes
    // offset = ( offset + 3 ) & ~3;
    // bool conVarsChanged = *( bool * )( ( uintptr_t )thisClient + offset ); // offset: 152
    // offset += sizeof( bool );
    // bool conVarsInitialSet = *( bool * )( ( uintptr_t )thisClient + offset );
    // offset += sizeof( bool );
    // bool sentServerInfo = *( bool * )( ( uintptr_t )thisClient + offset );
    // offset += sizeof( bool );

    //// Align void* to 8 bytes
    // offset = ( offset + 7 ) & ~7;
    // void *serverObject = *( void ** )( ( uintptr_t )thisClient + offset );  // offset: 160
    // offset += sizeof( void * );
    // bool isHLTV = *( bool * )( ( uintptr_t )thisClient + offset );
    // offset += sizeof( bool );

    // Cleaned up variant of the above:
    BEGIN_MEMORY_LAYOUT( Client, thisClient );
    SKIP_UNKNOWN_TO( Client, 16 );

    DEFINE_INT( Client, clientSlot );
    DEFINE_INT( Client, entityIndex );
    DEFINE_INT( Client, validateUserID );
    DEFINE_STRING( Client, validateName, MAX_PLAYER_NAME_LENGTH );
    DEFINE_STRING( Client, guid, SIGNED_GUID_LEN + 1 );  // +1 for null terminator

    DEFINE_TYPE( Client, CSteamID, validateSteamId );

    DEFINE_INT( Client, friendsID );
    DEFINE_STRING( Client, friendsName, MAX_PLAYER_NAME_LENGTH );

    s_OffsetConVars = DEFINE_PTR( Client, KeyValues *, conVars );  // offset: 144
    DEFINE_BOOL( Client, conVarsChanged );
    DEFINE_BOOL( Client, conVarsInitiallySet );
    DEFINE_BOOL( Client, sentServerInfo );

    DEFINE_PTR( Client, void *, serverObject );  // offset: 160
    DEFINE_BOOL( Client, isHLTV );

    END_MEMORY_LAYOUT( Client );
#else
    // int validateUserID = *( int * )( ( uintptr_t )thisClient + 0x10 );
    // int entityIndex = *( int * )( ( uintptr_t )thisClient + 0x0C );
    // const char *validateName = ( const char * )( ( uintptr_t )thisClient + 0x14 );
    //// Offset of m_SteamID from CBaseClient
    //// CSteamID *steamId = ( CSteamID * )( ( uintptr_t )thisClient + 0x5C ); // wont be set yet
    //// Offset of m_ConVars from CBaseClient
    //// KeyValues *conVars = ( KeyValues * )( ( uintptr_t )thisClient + 0x84 );
    //// Offset of m_pServer from CBaseClient
    // void *serverObject = *( void ** )( ( uintptr_t )thisClient + 0x8C );
    //  TODO: Test this cleaned up version of the above on x86:
    BEGIN_MEMORY_LAYOUT( Client, thisClient );
    SKIP_UNKNOWN_TO( Client, 0x0C );  // Skip to entityIndex at 0x0C
    DEFINE_INT( Client, entityIndex );
    DEFINE_INT( Client, validateUserID );                           // At 0x10
    DEFINE_STRING( Client, validateName, MAX_PLAYER_NAME_LENGTH );  // At 0x14

    SKIP_UNKNOWN_TO( Client, 0x5C );  // Skip to CSteamID at 0x5C
    DEFINE_TYPE( Client, CSteamID, validateSteamId );

    SKIP_UNKNOWN_TO( Client, 0x84 );  // Skip to conVars at 0x84
    s_OffsetConVars = DEFINE_PTR( Client, KeyValues *, conVars );
    DEFINE_BOOL( Client, conVarsChanged );
    DEFINE_BOOL( Client, conVarsInitiallySet );

    // Skip to serverObject at 0x8C (skipping friendsID, friendsName, etc.)
    SKIP_UNKNOWN_TO( Client, 0x8C );

    DEFINE_PTR( Client, void *, serverObject );  // At 0x8C
    END_MEMORY_LAYOUT( Client );
#endif
    const CSteamID *steamId = engine->GetClientSteamID(
        engine->PEntityOfEntIndex( entityIndex ) );  // will be valid

    // Check that our alignment is correct
    Assert( steamId );
    Assert( userID == validateUserID );
    Assert( Q_strcmp( validateName, playerName ) == 0 );

    // Since we need a pointer to the server object in order to apply the CheckPassword detour, we do it here
    // where the CBaseClient has a pointer to the CBaseServer object as a member.
    const char *steamIdAsString = steamId->Render();
    // HandleCheckPasswordDetour( serverObject, steamIdAsString );

    connectedClients.Insert( userID, thisClient );
}

#pragma warning( default : 4189 )  // Enable warning about unused variable

/// <summary>
/// This is a proof-of-concept that shows how we can force new userinfo to be allowed.
/// Normally the Source Engine omits any new userinfo that is registered as a cvar after the
/// first client update. This is no good for us, since we let Lua register client userinfo
/// at any time, even in the middle of gameplay. So with this hack into the engine we can
/// add userinfo to the player's ConVars.
/// I imagine we could call this in GetInfo, to add the name if it's not already added. That
/// way we never get an error like "Client ... userinfo ignored: gmod_toolmode"
/// </summary>
CON_COMMAND_F( hack_allow_new_userinfo, "Allow the given CVAR name to become new userinfo", FCVAR_GAMEDLL )
{
    CBasePlayer *pPlayer = UTIL_GetCommandClient();

    if ( !pPlayer )
    {
        DevWarning( "No player found.\n" );
        return;
    }

    if ( args.ArgC() < 1 )
    {
        DevWarning( "No userinfo name provided!\n" );
        return;
    }

    const char *userInfoName = args[1];
    unsigned short index = connectedClients.Find( pPlayer->GetUserID() );

    if ( index == connectedClients.InvalidIndex() )
    {
        DevWarning( "No client connect object found for player.\n" );
        return;
    }

    void *connectedClient = connectedClients.Element( index );

    KeyValues *conVars = GET_MEMORY_PTR( connectedClient, s_OffsetConVars, KeyValues );
    conVars->SetString( userInfoName, "" );

    DevWarning( "Initial ConVars reset.\n" );
}

DWORD_PTR FindPattern( HMODULE module, const char *pattern, const char *mask )
{
    if ( !module || !pattern || !mask )
        return 0;

    PIMAGE_DOS_HEADER dos = ( PIMAGE_DOS_HEADER )module;
    PIMAGE_NT_HEADERS nt = ( PIMAGE_NT_HEADERS )( ( BYTE * )module + dos->e_lfanew );
    DWORD size = nt->OptionalHeader.SizeOfImage;
    BYTE *data = ( BYTE * )module;
    DWORD length = ( DWORD )strlen( mask );

    for ( DWORD i = 0; i <= size - length; i++ )
    {
        bool found = true;
        for ( DWORD j = 0; j < length; j++ )
        {
            if ( mask[j] != '?' && pattern[j] != ( char )data[i + j] )
            {
                found = false;
                break;
            }
        }
        if ( found )
            return ( DWORD_PTR )&data[i];
    }
    return 0;
}

void ApplyClientConnectDetour()
{
    HMODULE hModule = GetModuleHandle( "engine.dll" );
    DWORD_PTR functionAddress = FindPattern( hModule, CLIENT_CONNECT_SIG, CLIENT_CONNECT_MASK );
    DWORD_PTR baseAddress = GetModuleBaseAddress( hModule );

    // DWORD_PTR expectedAsInIda = baseAddress + 0x62270;
    // bool same = ( functionAddress ) == ( expectedAsInIda );

    if ( !functionAddress )
    {
        DevWarning( "Failed to find BaseClient::Connect function address!\n" );
        return;
    }

    SetDefLessFunc( connectedClients );

    if ( MH_CreateHook( ( LPVOID )functionAddress, &DetourClientConnect, reinterpret_cast< LPVOID * >( &OriginalClientConnect ) ) != MH_OK )
    {
        DevWarning( "Failed to hook ClientConnect.\n" );
        Assert( 0 );
    }
    else
    {
        MH_EnableHook( ( LPVOID )functionAddress );
    }
}
