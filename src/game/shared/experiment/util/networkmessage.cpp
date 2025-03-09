#include "cbase.h"
#include "networkmessage.h"
#include <lnetwork.h>

CBaseNetMessage::CBaseNetMessage()
{
    m_pNetChannel = NULL;
    m_bReliable = true;

    // Subclasses must change this to the size of the message, or it
    // will get rejected
    m_ExpectedSize = 0;
}

void CBaseNetMessage::SetNetChannel( INetChannel* netchan )
{
    m_pNetChannel = netchan;
}

INetChannel* CBaseNetMessage::GetNetChannel( void ) const
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

const char* CBaseNetMessage::ToString( void ) const
{
    char buffer[512];
    Q_snprintf( buffer, sizeof( buffer ), "Type: %d, Group: %d, Name: %s", GetType(), GetGroup(), GetName() );
    return buffer;
}

size_t CBaseNetMessage::GetSize() const
{
    return m_ExpectedSize;
}

/// <summary>
/// If messageTypeId is -1 then this is simply used to register a listener
/// </summary>
/// <param name="messageTypeId"></param>
CDynamicWriteNetworkMessage::CDynamicWriteNetworkMessage( int messageTypeId /* = -1 */ )
{
    m_pBuffer = NULL;
    m_iTypeId = messageTypeId;
    m_iSender = -1;
}

CDynamicWriteNetworkMessage::~CDynamicWriteNetworkMessage()
{
    if ( m_pBuffer )
    {
        free( ( void* )m_pBuffer );
    }
};
const char* CDynamicWriteNetworkMessage::GetData() const
{
    return m_pBuffer;
}

void CDynamicWriteNetworkMessage::SetBuffer( const char* buffer, int bufferLength )
{
    m_nBufferLength = bufferLength;
    m_pBuffer = ( const char* )malloc( bufferLength );
    Q_memcpy( ( void* )m_pBuffer, buffer, bufferLength );
}

void CDynamicWriteNetworkMessage::SetSender( int userId )
{
    m_iSender = userId;
}

bool CDynamicWriteNetworkMessage::Process( void )
{
    CBasePlayer* player = nullptr;

#ifdef GAME_DLL
    if ( m_iSender > -1 )
    {
        player = UTIL_PlayerByUserId( m_iSender );

        if ( !player )
        {
            // Experiment; TODO: This happens when messages are sent before the player has fully
            //                   spawned and gotten their entity. :/ Unsure how to handle, other
            //                   than just not processing the message for now.
            return true;
        }
    }
#endif

    if ( L )
    {
        lua_getglobal( L, LUA_NETWORKSLIBNAME );
        lua_getfield( L, -1, "HandleIncomingMessage" );

        lua_remove( L, -2 );  // Remove the Networks table
        Assert( lua_isfunction( L, -1 ) );

        bf_read buffer( m_pBuffer, m_nBufferLength );

        lua_pushnumber( L, m_iTypeId );
        lua_pushbf_read( L, &buffer );

        if ( player != nullptr )
        {
            CBasePlayer::PushLuaInstanceSafe( L, player );
        }
        else
        {
            lua_pushnil( L );
        }

        luasrc_pcall( L, 3, 0 );  // Call HandleIncomingMessage and pop it with 3 arguments
    }

    return true;
}

bool CDynamicWriteNetworkMessage::ReadFromBuffer( bf_read& buffer )
{
    // We could call a Lua function here (e.g: net.Incoming) to handle
    // reading the buffer

    m_iTypeId = buffer.ReadUBitLong( 32 );

    m_nBufferLength = buffer.ReadUBitLong( 32 );

    char* temp = ( char* )malloc( m_nBufferLength );
    buffer.ReadBytes( temp, m_nBufferLength );
    SetBuffer( temp, m_nBufferLength );
    free( temp );

    m_ExpectedSize = buffer.GetNumBytesRead();

    return !buffer.IsOverflowed();
}

bool CDynamicWriteNetworkMessage::WriteToBuffer( bf_write& buffer )
{
    int type = GetType();
    buffer.WriteUBitLong( type, NETWORK_MESSAGE_TYPE_BITS );  // handled in the engine

    buffer.WriteUBitLong( m_iTypeId, 32 );

    buffer.WriteUBitLong( m_nBufferLength, 32 );
    buffer.WriteBytes( m_pBuffer, m_nBufferLength );

    return !buffer.IsOverflowed();
}
