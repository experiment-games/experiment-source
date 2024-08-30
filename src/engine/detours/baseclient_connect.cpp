#include "cbase.h"
#include <inetchannel.h>
#include "../engine_patches.h"
#include <util.h>

const DWORD_PTR OFFSET_OF_BASE_CLIENT_CONNECT = 0x9A380;
typedef void( __thiscall *Function_BaseClient_Connect_t )( void *thisClient, const char *, int, INetChannel *, bool, int );
static Function_BaseClient_Connect_t OriginalClientConnect = nullptr;

// Static map of connected clients to their user id so we can fetch em later
static CUtlMap< int, void * > connectedClients;

#pragma warning( disable : 4189 )  // Disable warning about unused variable

/*
    Notes on offsets

public:
                                                                                    // The first 8 bytes are the vtable
  int			m_nClientSlot;                                                      // 0x08 - 0x0B (4 bytes)
  int			m_nEntityIndex;                                                     // 0x0C - 0x0F (4 bytes)

  int			m_UserID;                                                           // 0x10 - 0x13 (4 bytes)
  char			m_Name[MAX_PLAYER_NAME_LENGTH]; // MAX_PLAYER_NAME_LENGTH = 32      // 0x14 - 0x37 (32 bytes)
  char			m_GUID[SIGNED_GUID_LEN + 1]; // SIGNED_GUID_LEN = 32                // 0x38 - 0x5B (33 bytes)

  CSteamID		m_SteamID; // 64 bits                                               // 0x5C - 0x63 (8 bytes)

  uint32		m_nFriendsID;
  char			m_FriendsName[MAX_PLAYER_NAME_LENGTH];                              // 0x57 - 0x7A (32 bytes)
  KeyValues		*m_ConVars;                                                         // 0x84 - 0x87 (4 bytes)
  bool			m_bConVarsChanged;                                                  // 0x88 (1 byte)
  bool			m_bInitialConVarsSet;                                               // 0x89 (1 byte)
  bool			m_bSendServerInfo;                                                  // 0x8A (1 byte)
                                                                                    // 1 byte padding to ensure 4 byte alignment
  CBaseServer	*m_Server;                                                          // 0x8C - 0x8F (4 bytes)
*/

// __fastcall workaround because you cant declare a __thiscall function without a class
// This behaves similar to __thiscall, but safely getting the ECX and EDX registriy as its first parameters.
// ECX holds the address of 'this' for the detoured method.
void __fastcall DetourClientConnect( void *thisClient, DWORD edx, const char *szName, int nUserID, INetChannel *pNetChannel, bool bFakePlayer, int clientChallenge )
{
    if ( !bFakePlayer )
    {
        int version = pNetChannel->GetProtocolVersion();
        DevWarning( "Detour: Client connecting with name %s (#%i) (protocol version: %i)\n", szName, nUserID, version );
    }
    else
    {
        DevWarning( "Detour: Fake player connecting with name %s (#%i)\n", szName, nUserID );
    }

    OriginalClientConnect( thisClient, szName, nUserID, pNetChannel, bFakePlayer, clientChallenge );

    // Offset of m_nUserID from CBaseClient
    int userID = *( int * )( ( uintptr_t )thisClient + 0x10 );

    // Offset of m_SteamID from CBaseClient
    // CSteamID *steamId = ( CSteamID * )( ( uintptr_t )thisClient + 0x5C ); // wont be set yet
    int entityIndex = *( int * )( ( uintptr_t )thisClient + 0x0C );
    const CSteamID *steamId = engine->GetClientSteamID( engine->PEntityOfEntIndex( entityIndex ) ); // will be valid

    // Offset of m_ConVars from CBaseClient
    // KeyValues *conVars = ( KeyValues * )( ( uintptr_t )thisClient + 0x84 );

    const char *name = ( const char * )( ( uintptr_t )thisClient + 0x14 );

    // Check that our alignment is correct
    Assert( steamId );
    Assert( userID == nUserID );
    Assert( Q_strcmp( name, szName ) == 0 );

    // Offset of m_pServer from CBaseClient
    void *serverObject = *( void ** )( ( uintptr_t )thisClient + 0x8C );

    // Since we need a pointer to the server object in order to apply the CheckPassword detour, we do it here
    // where the CBaseClient has a pointer to the CBaseServer object as a member.
    const char *steamIdAsString = steamId->Render();
    HandleCheckPasswordDetour( serverObject, steamIdAsString );

    connectedClients.Insert( nUserID, thisClient );
}

#pragma warning( default : 4189 )  // Enable warning about unused variable

CON_COMMAND_F( hack_reset_cvars, "Reset the initial convars to allow setting em again", FCVAR_GAMEDLL )
{
    CBasePlayer *pPlayer = UTIL_GetCommandClient();

    if ( !pPlayer )
    {
        DevWarning( "No player found.\n" );
        return;
    }

    unsigned short index = connectedClients.Find( pPlayer->GetUserID() );

    if ( index == connectedClients.InvalidIndex() )
    {
        DevWarning( "No client connect object found for player.\n" );
        return;
    }

    void *connectedClient = connectedClients.Element( index );

    // The bool that allows re-setting the ConVars is at offset 0x89 from the 'this' pointer
    // Offset of m_bInitialConVarsSet from CBaseClient
    bool *bInitialConVarsSet = ( bool * )( ( uintptr_t )connectedClient + 0x89 );
    *bInitialConVarsSet = false;
    DevWarning( "Initial ConVars reset.\n" );
}

void ApplyClientConnectDetour()
{
    DWORD_PTR baseAddress = GetModuleBaseAddress( "engine.dll" );
    DWORD_PTR functionAddress = baseAddress + OFFSET_OF_BASE_CLIENT_CONNECT;

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
