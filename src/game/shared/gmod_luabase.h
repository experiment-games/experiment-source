//========= Copyright © 1996-2005, Valve Corporation, All rights reserved.
//============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef GMOD_LUABASE_H
#define GMOD_LUABASE_H
#ifdef _WIN32
#pragma once
#endif

#include <mathlib/langle.h>
#include <mathlib/lvector.h>

struct lua_StateWithCompat;  // Forward declaration

// Source: https://github.com/Facepunch/gmod-module-base/blob/development/include/GarrysMod/Lua/LuaBase.h
namespace GarrysMod
{
namespace Lua
{
typedef int ( *CFunc )( lua_StateWithCompat *L );

struct UserData
{
    void *data;
    unsigned char type;
};

//
// Use this to communicate between C and Lua
//
class ILuaBase
{
    public:
    // You shouldn't need to use this struct
    // Instead, use the UserType functions
    struct UserData
    {
        void *data;
        unsigned char type;  // Change me to a uint32 one day
    };

    protected:
    template < class T >
    struct UserData_Value : UserData
    {
        T value;
    };

    public:
    // Returns the amount of values on the stack
    virtual int Top( void ) = 0;

    // Pushes a copy of the value at iStackPos to the top of the stack
    virtual void Push( int iStackPos ) = 0;

    // Pops iAmt values from the top of the stack
    virtual void Pop( int iAmt = 1 ) = 0;

    // Pushes table[key] on to the stack
    // table = value at iStackPos
    // key   = value at top of the stack
    // Pops the key from the stack
    virtual void GetTable( int iStackPos ) = 0;

    // Pushes table[key] on to the stack
    // table = value at iStackPos
    // key   = strName
    virtual void GetField( int iStackPos, const char *strName ) = 0;

    // Sets table[key] to the value at the top of the stack
    // table = value at iStackPos
    // key   = strName
    // Pops the value from the stack
    virtual void SetField( int iStackPos, const char *strName ) = 0;

    // Creates a new table and pushes it to the top of the stack
    virtual void CreateTable() = 0;

    // Sets table[key] to the value at the top of the stack
    // table = value at iStackPos
    // key   = value 2nd to the top of the stack
    // Pops the key and the value from the stack
    virtual void SetTable( int iStackPos ) = 0;

    // Sets the metatable for the value at iStackPos to the value at the top of the stack
    // Pops the value off of the top of the stack
    virtual void SetMetaTable( int iStackPos ) = 0;

    // Pushes the metatable of the value at iStackPos on to the top of the stack
    // Upon failure, returns false and does not push anything
    virtual bool GetMetaTable( int i ) = 0;

    // Calls a function
    // To use it: Push the function on to the stack followed by each argument
    // Pops the function and arguments from the stack, leaves iResults values on the stack
    // If this function errors, any local C values will not have their destructors called!
    virtual void Call( int iArgs, int iResults ) = 0;

    // Similar to Call
    // See: lua_pcall( lua_State*, int, int, int )
    virtual int PCall( int iArgs, int iResults, int iErrorFunc ) = 0;

    // Returns true if the values at iA and iB are equal
    virtual int Equal( int iA, int iB ) = 0;

    // Returns true if the value at iA and iB are equal
    // Does not invoke metamethods
    virtual int RawEqual( int iA, int iB ) = 0;

    // Moves the value at the top of the stack in to iStackPos
    // Any elements above iStackPos are shifted upwards
    virtual void Insert( int iStackPos ) = 0;

    // Removes the value at iStackPos from the stack
    // Any elements above iStackPos are shifted downwards
    virtual void Remove( int iStackPos ) = 0;

    // Allows you to iterate tables similar to pairs(...)
    // See: lua_next( lua_State*, int );
    virtual int Next( int iStackPos ) = 0;

#ifndef GMOD_ALLOW_DEPRECATED
    protected:
#endif
    // Deprecated: Use the UserType functions instead of this
    virtual void *NewUserdata( unsigned int iSize ) = 0;

    public:
    // Throws an error and ceases execution of the function
    // If this function is called, any local C values will not have their destructors called!
    virtual void ThrowError( const char *strError ) = 0;

    // Checks that the type of the value at iStackPos is iType
    // Throws and error and ceases execution of the function otherwise
    // If this function errors, any local C values will not have their destructors called!
    virtual void CheckType( int iStackPos, int iType ) = 0;

    // Throws a pretty error message about the given argument
    // If this function is called, any local C values will not have their destructors called!
    virtual void ArgError( int iArgNum, const char *strMessage ) = 0;

    // Pushes table[key] on to the stack
    // table = value at iStackPos
    // key   = value at top of the stack
    // Does not invoke metamethods
    virtual void RawGet( int iStackPos ) = 0;

    // Sets table[key] to the value at the top of the stack
    // table = value at iStackPos
    // key   = value 2nd to the top of the stack
    // Pops the key and the value from the stack
    // Does not invoke metamethods
    virtual void RawSet( int iStackPos ) = 0;

    // Returns the string at iStackPos. iOutLen is set to the length of the string if it is not NULL
    // If the value at iStackPos is a number, it will be converted in to a string
    // Returns NULL upon failure
    virtual const char *GetString( int iStackPos = -1, unsigned int *iOutLen = NULL ) = 0;

    // Returns the number at iStackPos
    // Returns 0 upon failure
    virtual double GetNumber( int iStackPos = -1 ) = 0;

    // Returns the boolean at iStackPos
    // Returns false upon failure
    virtual bool GetBool( int iStackPos = -1 ) = 0;

    // Returns the C-Function at iStackPos
    // returns NULL upon failure
    virtual CFunc GetCFunction( int iStackPos = -1 ) = 0;

#ifndef GMOD_ALLOW_DEPRECATED
    protected:
#endif
    // Deprecated: You should probably be using the UserType functions instead of this
    virtual void *GetUserdata( int iStackPos = -1 ) = 0;

    public:
    // Pushes a nil value on to the stack
    virtual void PushNil() = 0;

    // Pushes the given string on to the stack
    // If iLen is 0, strlen will be used to determine the string's length
    virtual void PushString( const char *val, unsigned int iLen = 0 ) = 0;

    // Pushes the given double on to the stack
    virtual void PushNumber( double val ) = 0;

    // Pushes the given bobolean on to the stack
    virtual void PushBool( bool val ) = 0;

    // Pushes the given C-Function on to the stack
    virtual void PushCFunction( CFunc val ) = 0;

    // Pushes the given C-Function on to the stack with upvalues
    // See: GetUpvalueIndex()
    virtual void PushCClosure( CFunc val, int iVars ) = 0;

#ifndef GMOD_ALLOW_DEPRECATED
    protected:
#endif
    // Deprecated: Don't use light userdata in GMod
    virtual void PushUserdata( void * ) = 0;

    public:
    // Allows for values to be stored by reference for later use
    // Make sure you call ReferenceFree when you are done with a reference
    virtual int ReferenceCreate() = 0;
    virtual void ReferenceFree( int i ) = 0;
    virtual void ReferencePush( int i ) = 0;

    // Push a special value onto the top of the stack (see SPECIAL_* enums)
    virtual void PushSpecial( int iType ) = 0;

    // Returns true if the value at iStackPos is of type iType
    // See: Types.h
    virtual bool IsType( int iStackPos, int iType ) = 0;

    // Returns the type of the value at iStackPos
    // See: Types.h
    virtual int GetType( int iStackPos ) = 0;

    // Returns the name associated with the given type ID
    // See: Types.h
    // Note: GetTypeName does not work with user-created types
    virtual const char *GetTypeName( int iType ) = 0;

#ifndef GMOD_ALLOW_DEPRECATED
    protected:
#endif
    // Deprecated: Use CreateMetaTable
    virtual void CreateMetaTableType( const char *strName, int iType ) = 0;

    public:
    // Like Get* but throws errors and returns if they're not of the expected type
    // If these functions error, any local C values will not have their destructors called!
    virtual const char *CheckString( int iStackPos = -1 ) = 0;
    virtual double CheckNumber( int iStackPos = -1 ) = 0;

    // Returns the length of the object at iStackPos
    // Works for: strings, tables, userdata
    virtual int ObjLen( int iStackPos = -1 ) = 0;

    // Returns the angle at iStackPos
    virtual const QAngle &GetAngle( int iStackPos = -1 ) = 0;

    // Returns the vector at iStackPos
    virtual const Vector &GetVector( int iStackPos = -1 ) = 0;

    // Pushes the given angle to the top of the stack
    virtual void PushAngle( const QAngle &val ) = 0;

    // Pushes the given vector to the top of the stack
    virtual void PushVector( const Vector &val ) = 0;

    // Sets the lua_State to be used by the ILuaBase implementation
    // You don't need to use this if you use the LUA_FUNCTION macro
    virtual void SetState( lua_StateWithCompat *L ) = 0;

    // Pushes the metatable associated with the given type name
    // Returns the type ID to use for this type
    // If the type doesn't currently exist, it will be created
    virtual int CreateMetaTable( const char *strName ) = 0;

    // Pushes the metatable associated with the given type
    virtual bool PushMetaTable( int iType ) = 0;

    // Creates a new UserData of type iType that references the given data
    virtual void PushUserType( void *data, int iType ) = 0;

    // Sets the data pointer of the UserType at iStackPos
    // You can use this to invalidate a UserType by passing NULL
    virtual void SetUserType( int iStackPos, void *data ) = 0;

    // Returns the data of the UserType at iStackPos if it is of the given type
    template < class T >
    T *GetUserType( int iStackPos, int iType )
    {
        auto *ud = static_cast< UserData * >( GetUserdata( iStackPos ) );

        if ( ud == nullptr || ud->data == nullptr || ud->type != iType )
            return nullptr;

        return static_cast< T * >( ud->data );
    }

    // Creates a new UserData with your own data embedded within it
    template < class T >
    void PushUserType_Value( const T &val, int iType )
    {
        using UserData_T = UserData_Value< T >;

        // The UserData allocated by CLuaInterface is only guaranteed to have a data alignment of 8
        static_assert( std::alignment_of< UserData_T >::value <= 8,
                        "PushUserType_Value given type with unsupported alignment requirement" );

        // Don't give this function objects that can't be trivially destructed
        // You could ignore this limitation if you implement object destruction in `__gc`
        static_assert( std::is_trivially_destructible< UserData_T >::value,
                        "PushUserType_Value given type that is not trivially destructible" );

        auto *ud = static_cast< UserData_T * >( NewUserdata( sizeof( UserData_T ) ) );
        ud->data = new ( &ud->value ) T( val );
        ud->type = iType;

        // Set the metatable
        if ( PushMetaTable( iType ) )
            SetMetaTable( -2 );
    }
};

// For use with ILuaBase::PushSpecial
enum
{
    SPECIAL_GLOB,  // Global table
    SPECIAL_ENV,   // Environment table
    SPECIAL_REG,   // Registry table
};
}  // namespace Lua
}  // namespace GarrysMod

class CLuaBase : public GarrysMod::Lua::ILuaBase
{
    private:
    CLuaBase() {}
    CUtlVector< const char * > m_mapMetaTableNameToID;
    lua_State *L;

    public:
    // Singleton based on: https://stackoverflow.com/a/1008289
    CLuaBase( CLuaBase const & ) = delete;
    void operator=( CLuaBase const & ) = delete;

    static CLuaBase &GetInstance( lua_State *L );

    virtual int Top( void )
    {
        return lua_gettop( L );
    }

    virtual void Push( int iStackPos )
    {
        lua_pushvalue( L, iStackPos );
    }

    virtual void Pop( int iAmt = 1 )
    {
        lua_pop( L, iAmt );
    }

    virtual void GetTable( int iStackPos )
    {
        lua_gettable( L, iStackPos );
    }

    virtual void GetField( int iStackPos, const char *strName )
    {
        lua_getfield( L, iStackPos, strName );
    }

    virtual void SetField( int iStackPos, const char *strName )
    {
        lua_setfield( L, iStackPos, strName );
    }

    virtual void CreateTable()
    {
        lua_createtable( L, 0, 0 );
    }

    virtual void SetTable( int iStackPos )
    {
        lua_settable( L, iStackPos );
    }

    virtual void SetMetaTable( int iStackPos )
    {
        lua_setmetatable( L, iStackPos );
    }

    virtual bool GetMetaTable( int i )
    {
        return lua_getmetatable( L, i ) == 1;
    }

    virtual void Call( int iArgs, int iResults )
    {
        lua_call( L, iArgs, iResults );
    }

    virtual int PCall( int iArgs, int iResults, int iErrorFunc )
    {
        return lua_pcall( L, iArgs, iResults, iErrorFunc );
    }

    virtual int Equal( int iA, int iB )
    {
        return lua_compare( L, iA, iB, LUA_OPEQ );
    }

    virtual int RawEqual( int iA, int iB )
    {
        return lua_rawequal( L, iA, iB );
    }

    virtual void Insert( int iStackPos )
    {
        lua_insert( L, iStackPos );
    }

    virtual void Remove( int iStackPos )
    {
        lua_remove( L, iStackPos );
    }

    virtual void CheckType( int iStackPos, int iType )
    {
        luaL_checktype( L, iStackPos, iType );
    }

    virtual const char *GetString( int iStackPos = -1, unsigned int *iOutLen = NULL )
    {
        return lua_tolstring( L, iStackPos, iOutLen );
    }

    virtual double GetNumber( int iStackPos = -1 )
    {
        return lua_tonumber( L, iStackPos );
    }
    virtual bool GetBool( int iStackPos = -1 )
    {
        return lua_toboolean( L, iStackPos ) == 1;
    }

    virtual void PushNil()
    {
        lua_pushnil( L );
    }

    virtual void PushString( const char *val, unsigned int iLen = 0 )
    {
        if ( iLen == 0 )
            iLen = strlen( val );

        lua_pushlstring( L, val, iLen );
    }
    virtual void PushNumber( double val )
    {
        lua_pushnumber( L, val );
    }

    virtual void PushBool( bool val )
    {
        lua_pushboolean( L, val );
    }

    virtual void PushUserdata( void *data )
    {
        lua_pushlightuserdata( L, data );
    }

    virtual int ReferenceCreate();

    virtual void ReferenceFree( int i );

    virtual void ReferencePush( int i );

    virtual bool IsType( int iStackPos, int iType )
    {
        return lua_type( L, iStackPos ) == iType;
    }

    virtual int GetType( int iStackPos )
    {
        return lua_type( L, iStackPos );
    }

    virtual const char *GetTypeName( int iType )
    {
        return lua_typename( L, iType );
    }

    virtual const char *CheckString( int iStackPos = -1 )
    {
        return luaL_checkstring( L, iStackPos );
    }

    virtual double CheckNumber( int iStackPos = -1 )
    {
        return luaL_checknumber( L, iStackPos );
    }

    virtual int ObjLen( int iStackPos = -1 )
    {
        return lua_rawlen( L, iStackPos );
    }

    virtual const QAngle &GetAngle( int iStackPos = -1 )
    {
        return luaL_checkangle( L, iStackPos );
    }

    virtual const Vector &GetVector( int iStackPos = -1 )
    {
        return luaL_checkvector( L, iStackPos );
    }

    virtual void PushAngle( const QAngle &val )
    {
        lua_pushangle( L, val );
    }

    virtual void PushVector( const Vector &val )
    {
        lua_pushvector( L, val );
    }

    virtual void SetState( lua_StateWithCompat *LCompat );

    virtual void ThrowError( const char *strError )
    {
        luaL_error( L, strError );
    }

    virtual void ArgError( int iArgNum, const char *strMessage )
    {
        luaL_argerror( L, iArgNum, strMessage );
    }

    virtual void RawGet( int iStackPos )
    {
        lua_rawget( L, iStackPos );
    }

    virtual void RawSet( int iStackPos )
    {
        lua_rawset( L, iStackPos );
    }

    virtual void PushSpecial( int iType )
    {
        switch ( iType )
        {
            case GarrysMod::Lua::SPECIAL_GLOB:
                lua_pushglobaltable( L );
                break;
            case GarrysMod::Lua::SPECIAL_ENV:
                // lua_pushvalue( L, LUA_ );
                DevWarning( "GarrysMod::Lua::SPECIAL_ENV is not supported yet\n" );
                Assert( false );  // TODO: Implement this (if it's still relevant in Lua 5.4)
                break;
            case GarrysMod::Lua::SPECIAL_REG:
                lua_pushvalue( L, LUA_REGISTRYINDEX );
                break;
        }
    }

    virtual void CreateMetaTableType( const char *strName, int iType )
    {
        luaL_newmetatable( L, strName );
        lua_pushvalue( L, -1 );
        lua_setfield( L, -2, "__index" );
        lua_pushstring( L, strName );
        lua_setfield( L, -2, "__type" );
        lua_setfield( L, LUA_REGISTRYINDEX, strName );
    }

    virtual int Next( int iStackPos )
    {
        return lua_next( L, iStackPos );
    }

    virtual void *NewUserdata( unsigned int iSize )
    {
        return lua_newuserdata( L, iSize );
    }

    virtual void *GetUserdata( int iStackPos )
    {
        return lua_touserdata( L, iStackPos );
    }

    virtual GarrysMod::Lua::CFunc GetCFunction( int iStackPos )
    {
        Assert( 0 );  // This isn't implemented yet, this is generated by copilot and wont work
        return reinterpret_cast< GarrysMod::Lua::CFunc >( lua_tocfunction( L, iStackPos ) );
    }

    virtual void PushCFunction( GarrysMod::Lua::CFunc val );

    virtual void PushCClosure( GarrysMod::Lua::CFunc val, int iVars );

    virtual int CreateMetaTable( const char *strName );

    virtual bool PushMetaTable( int iType );

    virtual void PushUserType( void *data, int iType );

    virtual void SetUserType( int iStackPos, void *data );
};

// Not really a lua_State, but it's compatible with gmod
// modules. The real lua_State is stored after the luabase
// pointer.
typedef struct lua_StateWithCompat
{
#if defined( _WIN32 ) && !defined( _M_X64 )
    // Win32
    unsigned char _ignore_this_common_lua_header_[48 + 22];
#elif defined( _WIN32 ) && defined( _M_X64 )
    // Win64
    unsigned char _ignore_this_common_lua_header_[92 + 22];
#elif defined( __linux__ ) && !defined( __x86_64__ )
    // Linux32
    unsigned char _ignore_this_common_lua_header_[48 + 22];
#elif defined( __linux__ ) && defined( __x86_64__ )
    // Linux64
    unsigned char _ignore_this_common_lua_header_[92 + 22];
#elif defined( __APPLE__ ) && !defined( __x86_64__ )
    // macOS32
    unsigned char _ignore_this_common_lua_header_[48 + 22];
#elif defined( __APPLE__ ) && defined( __x86_64__ )
    // macOS64
    unsigned char _ignore_this_common_lua_header_[92 + 22];
#else
#error "Platform not supported"
#endif

    GarrysMod::Lua::ILuaBase *luabase;
    lua_State *L;
} lua_StateWithCompat;

lua_StateWithCompat *ToLuaStateWithCompat( lua_State *L );

lua_StateWithCompat *CreateLuaStateWithCompat( lua_State *L );

#endif  // GMOD_LUABASE_H
