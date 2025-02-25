#include "cbase.h"
#include "KeyValues.h"
#include "filesystem.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "LKeyValues.h"
#include "lColor.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_KeyValues *lua_tokeyvalues( lua_State *L, int idx )
{
    lua_KeyValues *kv = *( lua_KeyValues ** )lua_touserdata( L, idx );
    return kv;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushkeyvalues( lua_State *L, lua_KeyValues *pKV )
{
    lua_KeyValues **ppKV = ( lua_KeyValues ** )lua_newuserdata( L, sizeof( lua_KeyValues * ) );
    *ppKV = pKV;
    LUA_SAFE_SET_METATABLE( L, LUA_KEYVALUESMETANAME );
}

LUALIB_API lua_KeyValues *luaL_checkkeyvalues( lua_State *L, int narg )
{
    lua_KeyValues **d = ( lua_KeyValues ** )luaL_checkudata( L, narg, LUA_KEYVALUESMETANAME );

    if ( *d == 0 ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "KeyValues expected, got NULL" );

    return *d;
}

LUALIB_API lua_KeyValues *luaL_optkeyvalues( lua_State *L, int narg, KeyValues *def )
{
    return luaL_opt( L, luaL_checkkeyvalues, narg, def );
}

/// <summary>
/// Pushes a KeyValues object as a table onto the stack.
/// </summary>
/// <param name="L">The Lua state.</param>
/// <param name="pKV">The KeyValues object.</param>
LUA_API void lua_pushkeyvalues_as_table( lua_State *L, lua_KeyValues *pKV )
{
    lua_newtable( L );

    for ( KeyValues *subKey = pKV->GetFirstSubKey(); subKey != NULL; subKey = subKey->GetNextKey() )
    {
        const char *keyName = subKey->GetName();

        lua_pushstring( L, keyName );

        switch ( subKey->GetDataType() )
        {
            case KeyValues::TYPE_STRING:
                lua_pushstring( L, subKey->GetString() );
                break;
            case KeyValues::TYPE_INT:
                lua_pushinteger( L, subKey->GetInt() );
                break;
            case KeyValues::TYPE_FLOAT:
                lua_pushnumber( L, subKey->GetFloat() );
                break;
            case KeyValues::TYPE_COLOR:
                lua_pushcolor( L, subKey->GetColor() );
                break;
            default:
                lua_pushnil( L );
                // Experiment; TODO: Shouldn't we handle sub-keyvalues?
                break;
        }

        lua_settable( L, -3 );
    }
}

LUA_REGISTRATION_INIT( KeyValuesHandle )

LUA_BINDING_BEGIN( KeyValuesHandle, AddSubKey, "class", "Add a subkey to the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    KeyValues *subKey = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 2, "subKey" );
    keyValues->AddSubKey( subKey );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, ChainKeyValue, "class", "Chain a keyvalue to the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    KeyValues *keyValue = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 2, "keyValue" );
    keyValues->ChainKeyValue( keyValue );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, Clear, "class", "Clear the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    keyValues->Clear();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, CopySubkeys, "class", "Copy the subkeys from another keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    KeyValues *subKey = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 2, "subKey" );
    keyValues->CopySubkeys( subKey );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, CreateNewKey, "class", "Create a new keyvalues key." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushkeyvalues( L, keyValues->CreateNewKey() );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The new keyvalues key." )

LUA_BINDING_BEGIN( KeyValuesHandle, DeleteThis, "class", "Delete the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    keyValues->deleteThis();
    // Andrew; this isn't standard behavior or usage, but we do this for the sake
    // of things being safe in Lua
    *( void ** )lua_touserdata( L, 1 ) = ( void ** )NULL;
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, FindKey, "class", "Find a key in the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            lua_pushkeyvalues( L,
                               keyValues->FindKey(
                                   LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" ) ) );
            break;
        case LUA_TSTRING:
        default:
            lua_pushkeyvalues( L,
                               keyValues->FindKey(
                                   LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" ),
                                   LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, false, "recursive" ) ) );
            break;
    }
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The keyvalues key." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetColor, "class", "Get a color from the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            lua_pushcolor( L,
                           keyValues->GetColor(
                               LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" ) ) );
            break;
        case LUA_TNONE:
        case LUA_TSTRING:
        default:
            lua_pushcolor( L,
                           keyValues->GetColor(
                               LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "key" ) ) );
            break;
    }
    return 1;
}
LUA_BINDING_END( "Color", "The value as a color." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetDataType, "class", "Get the data type of a key in the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushinteger( L,
                     keyValues->GetDataType(
                         LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "key" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The data type." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetFirstSubKey, "class", "Get the first subkey of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushkeyvalues( L, keyValues->GetFirstSubKey() );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The first subkey." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetFirstTrueSubKey, "class", "Get the first true subkey of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushkeyvalues( L, keyValues->GetFirstTrueSubKey() );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The first true subkey." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetFirstValue, "class", "Get the first value of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushkeyvalues( L, keyValues->GetFirstValue() );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The first value." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetFloat, "class", "Get a float from the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            lua_pushnumber( L,
                            keyValues->GetFloat(
                                LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" ),
                                LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0.0f, "default" ) ) );
            break;
        case LUA_TNONE:
        case LUA_TSTRING:
        default:
            lua_pushnumber( L,
                            keyValues->GetFloat(
                                LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "key" ),
                                LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0.0f, "default" ) ) );
            break;
    }
    return 1;
}
LUA_BINDING_END( "number", "The value as a float." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetInt, "class", "Get an integer from the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            lua_pushinteger( L,
                             keyValues->GetInt(
                                 LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" ),
                                 ( int )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "default" ) ) );
            break;
        case LUA_TNONE:
        case LUA_TSTRING:
        default:
            lua_pushinteger( L,
                             keyValues->GetInt(
                                 LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "key" ),
                                 ( int )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "default" ) ) );
            break;
    }
    return 1;
}
LUA_BINDING_END( "integer", "The value as an integer." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetName, "class", "Get the name of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushstring( L, keyValues->GetName() );
    return 1;
}
LUA_BINDING_END( "string", "The name." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetNameSymbol, "class", "Get the name symbol of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushinteger( L, keyValues->GetNameSymbol() );
    return 1;
}
LUA_BINDING_END( "integer", "The name symbol." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetNextKey, "class", "Get the next key of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushkeyvalues( L, keyValues->GetNextKey() );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The next key." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetNextTrueSubKey, "class", "Get the next true subkey of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushkeyvalues( L, keyValues->GetNextTrueSubKey() );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The next true subkey." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetNextValue, "class", "Get the next value of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushkeyvalues( L, keyValues->GetNextValue() );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The next value." )

LUA_BINDING_BEGIN( KeyValuesHandle, GetString, "class", "Get a string from the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            lua_pushstring( L,
                            keyValues->GetString(
                                LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" ),
                                LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, "", "default" ) ) );
            break;
        case LUA_TNONE:
        case LUA_TSTRING:
        default:
            lua_pushstring( L,
                            keyValues->GetString(
                                LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "key" ),
                                LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, "", "default" ) ) );
            break;
    }
    return 1;
}
LUA_BINDING_END( "string", "The value as a string." )

LUA_BINDING_BEGIN( KeyValuesHandle, IsEmpty, "class", "Check if the keyvalues is empty." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
            lua_pushboolean( L,
                             keyValues->IsEmpty(
                                 LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "key" ) ) );
            break;
        case LUA_TNONE:
        case LUA_TSTRING:
        default:
            lua_pushboolean( L,
                             keyValues->IsEmpty(
                                 LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 2, 0, "key" ) ) );
            break;
    }
    return 1;
}
LUA_BINDING_END( "boolean", "true if the keyvalues is empty, false otherwise." )

LUA_BINDING_BEGIN( KeyValuesHandle, LoadFromFile, "class", "Load keyvalues from a file." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushboolean( L,
                     keyValues->LoadFromFile(
                         filesystem,
                         LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "filename" ),
                         LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, 0, "pathID" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the keyvalues were loaded, false otherwise." )

LUA_BINDING_BEGIN( KeyValuesHandle, MakeCopy, "class", "Make a copy of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushkeyvalues( L, keyValues->MakeCopy() );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The copy." )

LUA_BINDING_BEGIN( KeyValuesHandle, ProcessResolutionKeys, "class", "Process resolution keys in the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushboolean( L,
                     keyValues->ProcessResolutionKeys(
                         LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the resolution keys were processed, false otherwise." )

LUA_BINDING_BEGIN( KeyValuesHandle, RemoveSubKey, "class", "Remove a subkey from the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    KeyValues *subKey = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 2, "subKey" );
    keyValues->RemoveSubKey( subKey );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, SaveToFile, "class", "Save keyvalues to a file." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    lua_pushboolean( L,
                     keyValues->SaveToFile(
                         filesystem,
                         LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "filename" ),
                         LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optstring, 3, 0, "pathID" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the keyvalues were saved, false otherwise." )

LUA_BINDING_BEGIN( KeyValuesHandle, SetColor, "class", "Set a color in the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    keyValues->SetColor(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" ),
        LUA_BINDING_ARGUMENT( luaL_checkcolor, 3, "color" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, SetFloat, "class", "Set a float in the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    keyValues->SetFloat(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, SetInt, "class", "Set an integer in the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    keyValues->SetInt(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, SetName, "class", "Set the name of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    keyValues->SetName(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "name" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, SetNextKey, "class", "Set the next key of the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    KeyValues *nextKey = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 2, "nextKey" );
    keyValues->SetNextKey( nextKey );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, SetString, "class", "Set a string in the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    keyValues->SetString(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "key" ),
        LUA_BINDING_ARGUMENT( luaL_checkstring, 3, "value" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, SetStringValue, "class", "Set a string value in the keyvalues." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    keyValues->SetStringValue(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "value" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, UsesEscapeSequences, "class", "Set if the keyvalues uses escape sequences." )
{
    lua_KeyValues *keyValues = LUA_BINDING_ARGUMENT( luaL_checkkeyvalues, 1, "keyValues" );
    keyValues->UsesEscapeSequences(
        LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "usesEscapeSequences" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( KeyValuesHandle, __eq, "class", "Check if two keyvalues are equal." )
{
    lua_pushboolean( L,
                     LUA_BINDING_ARGUMENT( lua_tokeyvalues, 1, "keyValues" ) ==
                         LUA_BINDING_ARGUMENT( lua_tokeyvalues, 2, "other" ) );
    return 1;
}
LUA_BINDING_END( "boolean", "true if the keyvalues are equal, false otherwise." )

LUA_BINDING_BEGIN( KeyValuesHandle, __tostring, "class", "Get the string representation of the keyvalues." )
{
    KeyValues *keyValues = LUA_BINDING_ARGUMENT( lua_tokeyvalues, 1, "keyValues" );
    if ( keyValues == NULL )
        lua_pushstring( L, "NULL_KEYVALUES" );
    else
        lua_pushfstring( L, "KeyValues: %p", keyValues );
    return 1;
}
LUA_BINDING_END( "string", "The string representation." )

LUA_REGISTRATION_INIT( KeyValues )

LUA_BINDING_BEGIN( KeyValues, Create, "library", "Create a new keyvalues object." )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );

    KeyValues *pKV = NULL;

    if ( lua_gettop( L ) <= 1 )
        pKV = new KeyValues( name );
    else
    {
        const char *firstSubKey = LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkstring, 2, "firstKey" );

        if ( lua_gettop( L ) <= 3 )
        {
            switch ( lua_type( L, 3 ) )
            {
                case LUA_TNUMBER:
                    pKV = new KeyValues(
                        name,
                        firstSubKey,
                        LUA_BINDING_ARGUMENT_NILLABLE( luaL_checknumber, 3, "firstValue" ) );
                    break;
                case LUA_TSTRING:
                    pKV = new KeyValues(
                        name,
                        firstSubKey,
                        LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkstring, 3, "firstValue" ) );
                    break;
                default:
                    luaL_typerror( L, 3, "string or number" );
                    break;
            }
        }
        else if ( lua_gettop( L ) <= 5 )
        {
            switch ( lua_type( L, 3 ) )
            {
                case LUA_TNUMBER:
                    pKV = new KeyValues(
                        name,
                        firstSubKey,
                        LUA_BINDING_ARGUMENT_NILLABLE( luaL_checknumber, 3, "firstValue" ),
                        LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkstring, 4, "secondKey" ),
                        LUA_BINDING_ARGUMENT_NILLABLE( luaL_checknumber, 5, "secondValue" ) );
                    break;
                case LUA_TSTRING:
                    pKV = new KeyValues(
                        name,
                        firstSubKey,
                        LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkstring, 3, "firstValue" ),
                        LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkstring, 4, "secondKey" ),
                        LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkstring, 5, "secondValue" ) );
                    break;
                default:
                    luaL_typerror( L, 3, "string or number" );
                    break;
            }
        }
    }

    lua_pushkeyvalues( L, pKV );

    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The new keyvalues object." )

LUA_BINDING_BEGIN( KeyValues, CreateFromTable, "library", "Create a new keyvalues object from a table." )
{
    const char *name = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "name" );
    KeyValues *pKV = new KeyValues( name );

    if ( !LUA_BINDING_ARGUMENT( lua_istable, 2, "table" ) )
        luaL_typerror( L, 2, "table" );

    lua_pushnil( L );  // first key

    while ( lua_next( L, 2 ) != 0 )
    {
        switch ( lua_type( L, -2 ) )
        {
            case LUA_TNUMBER:
                pKV->SetFloat( lua_tostring( L, -2 ), lua_tonumber( L, -1 ) );
                break;
            case LUA_TSTRING:
                pKV->SetString( lua_tostring( L, -2 ), lua_tostring( L, -1 ) );
                break;
            case LUA_TBOOLEAN:
                pKV->SetBool( lua_tostring( L, -2 ), lua_toboolean( L, -1 ) );
                break;
            default:
                luaL_typerror( L, -2, "number, string or boolean" );
                break;
        }
        lua_pop( L, 1 );
    }

    lua_pushkeyvalues( L, pKV );
    return 1;
}
LUA_BINDING_END( "KeyValuesHandle", "The new keyvalues object." )

/*
** Open KeyValues object
*/
LUALIB_API int luaopen_KeyValues( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_KEYVALUESMETANAME );

    LUA_REGISTRATION_COMMIT( KeyValuesHandle );

    lua_pushvalue( L, -1 );           /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    lua_pushstring( L, LUA_KEYVALUESMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "KeyValuesHandle" */

    LUA_REGISTRATION_COMMIT_LIBRARY( KeyValues );

    lua_pop( L, 1 );  // pop metatable

    // Andrew; This is nasty, but we can't really repurpose the NULL global
    // entity.
    lua_pushkeyvalues( L, NULL );
    lua_setglobal( L, "NULL_KEYVALUES" ); /* set global NULL_KEYVALUES */

    return 1;
}
