#include "engine_patches.h"
#include <strtools.h>

// Experiment;
// These detours are experimental and give us access to engine features we
// wouldn't have access to otherwise. It's a shame that we have to be this
// hacky, but it's the only way to get access to things like CheckPassword and
// ClientConnect.

DWORD_PTR GetModuleBaseAddress( HMODULE module )
{
    return reinterpret_cast< DWORD_PTR >( module );
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

void ApplyDetoursOnDllStartup()
{
    if ( MH_Initialize() != MH_OK )
    {
        // DevWarning( "Failed to initialize MinHook.\n" );
        return;
    }

#if defined( CLIENT_DLL )

#elif defined( GAME_DLL )
    // HandleCheckPasswordDetour();
    ApplyClientConnectDetour();
#endif
}

void NotifyDetoursOnLevelShutdown()
{
#if defined( CLIENT_DLL )

#elif defined( GAME_DLL )
    // UpdateCheckPasswordDetourOnLevelShutdown();
#endif
}

/*
** Testing Net Channel detours
*/
// TODO: Move this to its own file(s)

CBaseNetMessage::CBaseNetMessage()
{
    m_pNetChannel = NULL;
    m_bReliable = true;

    // Subclasses must change this to the size of the message, or it
    // will get rejected
    m_ExpectedSize = 0;
}

void CBaseNetMessage::SetNetChannel( INetChannel *netchan )
{
    m_pNetChannel = netchan;
}

INetChannel *CBaseNetMessage::GetNetChannel( void ) const
{
    return m_pNetChannel;
}

void CBaseNetMessage::SetReliable( bool state )
{
    m_bReliable = state;
}

bool CBaseNetMessage::IsReliable( void ) const
{
    return m_bReliable;
}

const char *CBaseNetMessage::ToString( void ) const
{
    char buffer[512];
    Q_snprintf( buffer, sizeof( buffer ), "Type: %d, Group: %d, Name: %s", GetType(), GetGroup(), GetName() );
    return buffer;
}

size_t CBaseNetMessage::GetSize() const
{
    return m_ExpectedSize;
}

//
// Example of a NetMessage implementation
//
const char *CNetMessage_LuaString::GetLuaString() const
{
    return m_szLuaString;
}

void CNetMessage_LuaString::SetLuaString( const char *szLuaString )
{
    Q_strncpy( m_szLuaString, szLuaString, sizeof( m_szLuaString ) );
}

bool CNetMessage_LuaString::Process( void )
{
    //DevWarning( "Processing LuaString message: %s\n", m_szLuaString );
    return true;
}

bool CNetMessage_LuaString::ReadFromBuffer( bf_read &buffer )
{
    // We could call a Lua function here (e.g: net.Incoming) to handle
    // reading the buffer

    buffer.ReadString( m_szLuaString, sizeof( m_szLuaString ) );

    m_ExpectedSize = buffer.GetNumBytesRead();

    return !buffer.IsOverflowed();
}

bool CNetMessage_LuaString::WriteToBuffer( bf_write &buffer )
{
    buffer.WriteUBitLong( GetType(), NET_MESSAGE_TYPE_BITS );
    buffer.WriteString( m_szLuaString );

    return !buffer.IsOverflowed();
}
