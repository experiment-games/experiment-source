#include "cbase.h"
#include <inetchannel.h>
#include <netadr.h>
#include "../engine_patches.h"

// Found 0E8h as CheckPassword (vtable offset) in IDA deducing the offset of the function by looking for the parameters being pushed before the call to the function, e.g:
//  .text : 0015BC5D loc_15BC5D:       ; CODE XREF : sub_15BB80 + C3↑j
//  .text : 0015BC5D push[ebp + arg_name]
//  .text : 0015BC60 mov eax, [eax + 0E8h]
//  .text : 0015BC66 push[ebp + arg_password]
const DWORD_PTR OFFSET_OF_BASE_SERVER_CHECK_PASSWORD_VTABLE = 0x0E8;
typedef bool( __thiscall *Function_BaseServer_CheckPassword_t )( void *thisPointer, netadr_t &adr, const char *password, const char *name );
static Function_BaseServer_CheckPassword_t OriginalServerCheckPassword = nullptr;

static bool HasAlreadyDetouredCheckPassword = false;
static const char *CurrentlyConnectingSteamId = nullptr;

bool __fastcall DetourCheckPassword( void *thisPointer, DWORD edx, netadr_t &adr, const char *password, const char *name )
{
#ifdef LUA_SDK
    if ( !adr.IsLocalhost() && !adr.IsLoopback() )
    {
        ConVarRef sv_password( "sv_password" );
        const char *steamId64 = CurrentlyConnectingSteamId;
        const char *ipAddress = adr.ToString();
        const char *actualPassword = sv_password.GetString();
        LUA_CALL_HOOK_BEGIN( "CheckPassword", "Called to check if the password the user provides is correct." );
        lua_pushstring( L, steamId64 );
        lua_pushstring( L, ipAddress );
        lua_pushstring( L, actualPassword );
        lua_pushstring( L, password );
        lua_pushstring( L, name );
        LUA_CALL_HOOK_END( 5, 2 );

        if ( !lua_isnil( L, -2 ) )
        {
            bool isAllowedToJoin = lua_isboolean( L, -2 ) && lua_toboolean( L, -2 );

            if ( !isAllowedToJoin )
            {
                if ( lua_isstring( L, -1 ) )
                {
                    // E.g: #GameUI_ConnectionFailed, #GameUI_ServerRejectLANRestrict or #GameUI_ServerRejectBadPassword (default)
                    const char *reason = lua_tostring( L, -1 );
                    // TODO: Implement setting reason as kick message (not possible with current detour)
                    // We could do this by specifying a disconnect reason somewhere and then in
                    // CServerGameClients::ClientConnect we could deny the player access with that reason.
                    DevWarning( "Player %s (%s) was denied access to the server: %s\n", name, steamId64, reason );
                }
                else
                {
                    DevWarning( "Player %s (%s) was denied access to the server.\n", name, steamId64 );
                }

                lua_pop( L, 2 );  // Pop the boolean and the string
                return false;
            }
        }

        lua_pop( L, 2 );  // Pop the boolean and the string
    }
#endif

    bool result = OriginalServerCheckPassword( thisPointer, adr, password, name );

    return result;
}

void HandleCheckPasswordDetour( void *serverObject, const char *steamIdAsString )
{
    CurrentlyConnectingSteamId = steamIdAsString;

    if ( HasAlreadyDetouredCheckPassword )
    {
        DevWarning( "CheckPassword already detoured.\n" );
        return;
    }

    HasAlreadyDetouredCheckPassword = true;

    // The first 4 bytes of the 'this' pointer is the vtable pointer
    DWORD_PTR *vtable = *reinterpret_cast< DWORD_PTR ** >( serverObject );
    DWORD_PTR functionAddress = vtable[OFFSET_OF_BASE_SERVER_CHECK_PASSWORD_VTABLE / sizeof( DWORD_PTR )];

    if ( MH_CreateHook( ( LPVOID )functionAddress, &DetourCheckPassword, reinterpret_cast< LPVOID * >( &OriginalServerCheckPassword ) ) != MH_OK )
    {
        DevWarning( "Failed to hook CheckPassword.\n" );
    }
    else
    {
        MH_EnableHook( ( LPVOID )functionAddress );
    }
}

void UpdateCheckPasswordDetourOnLevelShutdown()
{
    HasAlreadyDetouredCheckPassword = false;
}
