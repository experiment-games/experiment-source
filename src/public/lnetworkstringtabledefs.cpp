#include "cbase.h"
#include "networkstringtabledefs.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "lnetworkstringtabledefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_INetworkStringTable *lua_tostringtable( lua_State *L, int idx )
{
    lua_INetworkStringTable **ppNetworkStringTable = ( lua_INetworkStringTable ** )lua_touserdata( L, idx );
    return *ppNetworkStringTable;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushstringtable( lua_State *L, lua_INetworkStringTable *pNetworkStringTable )
{
    lua_INetworkStringTable **ppNetworkStringTable = ( lua_INetworkStringTable ** )lua_newuserdata( L, sizeof( pNetworkStringTable ) );
    *ppNetworkStringTable = pNetworkStringTable;
    LUA_SAFE_SET_METATABLE( L, LUA_INETWORKSTRINGTABLELIBNAME );
}

LUALIB_API lua_INetworkStringTable *luaL_checkstringtable( lua_State *L, int narg )
{
    lua_INetworkStringTable **ppData = ( lua_INetworkStringTable ** )luaL_checkudata( L, narg, LUA_INETWORKSTRINGTABLELIBNAME );

    if ( *ppData == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "NetworkStringTable expected, got NULL" );

    return *ppData;
}

LUA_REGISTRATION_INIT( NetworkStringTable )

//static int INetworkStringTable_AddString( lua_State *L )
//{
//    lua_pushinteger( L, luaL_checkstringtable( L, 1 )->AddString( luaL_checkboolean( L, 2 ), luaL_checkstring( L, 3 ) ) );
//    return 1;
//}
//
//static int INetworkStringTable_ChangedSinceTick( lua_State *L )
//{
//    lua_pushboolean( L, luaL_checkstringtable( L, 1 )->ChangedSinceTick( luaL_checknumber( L, 2 ) ) );
//    return 1;
//}
//
//static int INetworkStringTable_FindStringIndex( lua_State *L )
//{
//    lua_pushinteger( L, luaL_checkstringtable( L, 1 )->FindStringIndex( luaL_checkstring( L, 2 ) ) );
//    return 1;
//}
//
//static int INetworkStringTable_GetEntryBits( lua_State *L )
//{
//    lua_pushinteger( L, luaL_checkstringtable( L, 1 )->GetEntryBits() );
//    return 1;
//}
//
//static int INetworkStringTable_GetMaxStrings( lua_State *L )
//{
//    lua_pushinteger( L, luaL_checkstringtable( L, 1 )->GetMaxStrings() );
//    return 1;
//}
//
//static int INetworkStringTable_GetNumStrings( lua_State *L )
//{
//    lua_pushinteger( L, luaL_checkstringtable( L, 1 )->GetNumStrings() );
//    return 1;
//}
//
//static int INetworkStringTable_GetString( lua_State *L )
//{
//    lua_pushstring( L, luaL_checkstringtable( L, 1 )->GetString( luaL_checknumber( L, 2 ) ) );
//    return 1;
//}
//
//static int INetworkStringTable_GetTableId( lua_State *L )
//{
//    lua_pushinteger( L, luaL_checkstringtable( L, 1 )->GetTableId() );
//    return 1;
//}
//
//static int INetworkStringTable_GetTableName( lua_State *L )
//{
//    lua_pushstring( L, luaL_checkstringtable( L, 1 )->GetTableName() );
//    return 1;
//}
//
//static int INetworkStringTable_SetStringUserData( lua_State *L )
//{
//    luaL_checkstringtable( L, 1 )->SetStringUserData( luaL_checknumber( L, 2 ), strlen( luaL_checkstring( L, 3 ) ) + 1, luaL_checkstring( L, 3 ) );
//    return 0;
//}
//
//static int INetworkStringTable_SetTick( lua_State *L )
//{
//    luaL_checkstringtable( L, 1 )->SetTick( luaL_checknumber( L, 2 ) );
//    return 0;
//}
//
//static int INetworkStringTable___tostring( lua_State *L )
//{
//    INetworkStringTable *pNetworkStringTable = luaL_checkstringtable( L, 1 );
//    lua_pushfstring( L, "INetworkStringTable: %s", pNetworkStringTable->GetTableName() );
//    return 1;
//}
LUA_BINDING_BEGIN( NetworkStringTable, AddString, "class", "Add a string to the table." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );
    bool isServer = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "isServer" );
    const char *value = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "value" );

    lua_pushinteger( L, networkStringTable->AddString( isServer, value ) );

    return 1;
}
LUA_BINDING_END( "integer", "The index of the string." )

LUA_BINDING_BEGIN( NetworkStringTable, ChangedSinceTick, "class", "Check if the table has changed since the given tick." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );
    int tick = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "tick" );

    lua_pushboolean( L, networkStringTable->ChangedSinceTick( tick ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the table has changed, false otherwise." )

LUA_BINDING_BEGIN( NetworkStringTable, FindStringIndex, "class", "Find the index of a string in the table." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );
    const char *value = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "value" );

    lua_pushinteger( L, networkStringTable->FindStringIndex( value ) );

    return 1;
}
LUA_BINDING_END( "integer", "The index of the string." )

LUA_BINDING_BEGIN( NetworkStringTable, GetEntryBits, "class", "Get the number of bits used to store each string." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );

    lua_pushinteger( L, networkStringTable->GetEntryBits() );

    return 1;
}
LUA_BINDING_END( "integer", "The number of bits." )

LUA_BINDING_BEGIN( NetworkStringTable, GetMaxStrings, "class", "Get the maximum number of strings that can be stored in the table." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );

    lua_pushinteger( L, networkStringTable->GetMaxStrings() );

    return 1;
}
LUA_BINDING_END( "integer", "The maximum number of strings." )

LUA_BINDING_BEGIN( NetworkStringTable, GetStringsAmount, "class", "Get the number of strings stored in the table." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );

    lua_pushinteger( L, networkStringTable->GetNumStrings() );

    return 1;
}
LUA_BINDING_END( "integer", "The number of strings." )

LUA_BINDING_BEGIN( NetworkStringTable, GetString, "class", "Get a string from the table." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );
    int index = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" );

    lua_pushstring( L, networkStringTable->GetString( index ) );

    return 1;
}
LUA_BINDING_END( "string", "The string." )

LUA_BINDING_BEGIN( NetworkStringTable, GetTableId, "class", "Get the table's ID." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );

    lua_pushinteger( L, networkStringTable->GetTableId() );

    return 1;
}
LUA_BINDING_END( "integer", "The table's ID." )

LUA_BINDING_BEGIN( NetworkStringTable, GetTableName, "class", "Get the table's name." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );

    lua_pushstring( L, networkStringTable->GetTableName() );

    return 1;
}
LUA_BINDING_END( "string", "The table's name." )

LUA_BINDING_BEGIN( NetworkStringTable, SetStringUserData, "class", "Set the user data of a string." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );
    int index = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "index" );
    const char *value = LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "value" );

    networkStringTable->SetStringUserData( index, strlen( value ) + 1, value );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( NetworkStringTable, SetTick, "class", "Set the tick of the table." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );
    int tick = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "tick" );

    networkStringTable->SetTick( tick );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( NetworkStringTable, __tostring, "class", "Get a string representation of the table." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );

    lua_pushfstring( L, "INetworkStringTable: %s", networkStringTable->GetTableName() );

    return 1;
}
LUA_BINDING_END()

/*
** Open INetworkStringTable object
*/
LUALIB_API int luaopen_INetworkStringTable( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_INETWORKSTRINGTABLELIBNAME );

    LUA_REGISTRATION_COMMIT( NetworkStringTable );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_NETWORKSTRINGTABLELIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "NetworkStringTable" */

    return 1;
}

// Engine interfaces.
extern INetworkStringTableContainer *networkstringtable;

LUA_REGISTRATION_INIT( NetworkStringTables )

LUA_BINDING_BEGIN( NetworkStringTables, Create, "library", "Create a new string table." )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    int maxEntries = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "maxEntries" );
    int userDataFixedSize = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "userDataFixedSize" );
    int userDataNetworkBits = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 0, "userDataNetworkBits" );

    lua_pushstringtable( L, networkstringtable->CreateStringTable( name, maxEntries, userDataFixedSize, userDataNetworkBits ) );

    return 1;
}
LUA_BINDING_END( "NetworkStringTable", "The created string table." )

LUA_BINDING_BEGIN( NetworkStringTables, CreateExtended, "library", "Create a new string table." )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    int maxEntries = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "maxEntries" );
    int userDataFixedSize = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "userDataFixedSize" );
    int userDataNetworkBits = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 0, "userDataNetworkBits" );
    bool isFilenames = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 5, false, "isFilenames" );

    lua_pushstringtable( L, networkstringtable->CreateStringTableEx( name, maxEntries, userDataFixedSize, userDataNetworkBits, isFilenames ) );

    return 1;
}
LUA_BINDING_END( "NetworkStringTable", "The created string table." )

LUA_BINDING_BEGIN( NetworkStringTables, FindTable, "library", "Find a string table by name." )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );

    lua_pushstringtable( L, networkstringtable->FindTable( name ) );

    return 1;
}
LUA_BINDING_END( "NetworkStringTable", "The found string table." )

LUA_BINDING_BEGIN( NetworkStringTables, GetTablesAmount, "library", "Get the number of string tables." )
{
    lua_pushinteger( L, networkstringtable->GetNumTables() );

    return 1;
}
LUA_BINDING_END( "integer", "The amount of string tables." )

LUA_BINDING_BEGIN( NetworkStringTables, GetTable, "library", "Get a string table by index." )
{
    int index = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "index" );

    lua_pushstringtable( L, networkstringtable->GetTable( index ) );

    return 1;
}
LUA_BINDING_END( "NetworkStringTable", "The gotten string table." )

LUA_BINDING_BEGIN( NetworkStringTables, RemoveAllTables, "library", "Remove all string tables." )
{
    networkstringtable->RemoveAllTables();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( NetworkStringTables, SetAllowClientSideAddString, "library", "Set whether clients can add strings to a table." )
{
    lua_INetworkStringTable *networkStringTable = LUA_BINDING_ARGUMENT( luaL_checkstringtable, 1, "networkStringTable" );
    bool allow = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "allow" );

    networkstringtable->SetAllowClientSideAddString( networkStringTable, allow );

    return 0;
}
LUA_BINDING_END()

/*
** Open networkstringtable library
*/
LUALIB_API int luaopen_NetworkStringTables( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( NetworkStringTables );
    return 1;
}
