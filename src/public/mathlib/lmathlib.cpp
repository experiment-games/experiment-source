#include "cbase.h"
#include "mathlib.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "lmathlib.h"
#include "lvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_matrix3x4_t &lua_tomatrix( lua_State *L, int idx )
{
    lua_matrix3x4_t *matrix = ( lua_matrix3x4_t * )luaL_checkudata( L, idx, LUA_MATRIXLIBNAME );
    return *matrix;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushmatrix( lua_State *L, lua_matrix3x4_t &matrix )
{
    lua_matrix3x4_t *pMat = ( lua_matrix3x4_t * )lua_newuserdata( L, sizeof( lua_matrix3x4_t ) );
    *pMat = matrix;
    LUA_SAFE_SET_METATABLE( L, LUA_MATRIXLIBNAME );
}

LUALIB_API lua_matrix3x4_t &luaL_checkmatrix( lua_State *L, int narg )
{
    lua_matrix3x4_t *d = ( lua_matrix3x4_t * )luaL_checkudata( L, narg, LUA_MATRIXLIBNAME );
    return *d;
}

LUA_REGISTRATION_INIT( Matrix3x4 );

// Experiment; disabled since we don't want to deal with pointers in Lua.
// LUA_BINDING_BEGIN( Matrix3x4, Base, "class", "Returns the base address of the matrix." )
//{
//    lua_pushnumber( L, *LUA_BINDING_ARGUMENT( luaL_checkmatrix, 1, "Matrix3x4" ).Base() );
//    return 1;
//}
// LUA_BINDING_END( "number", "The base address of the matrix." )

LUA_BINDING_BEGIN( Matrix3x4, Init, "class", "Initializes the matrix." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmatrix, 1, "Matrix3x4" )
        .Init(
            LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "Vector" ),
            LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "Vector" ),
            LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "Vector" ),
            LUA_BINDING_ARGUMENT( luaL_checkvector, 5, "Vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix3x4, Invalidate, "class", "Invalidates the matrix." )
{
    LUA_BINDING_ARGUMENT( luaL_checkmatrix, 1, "Matrix3x4" ).Invalidate();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix3x4, __index, "class", "Metatable called when a non-existant field is index" )
{
    lua_matrix3x4_t matrix = LUA_BINDING_ARGUMENT( luaL_checkmatrix, 1, "Matrix3x4" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "string" );

    if ( strcmp( field, "0" ) == 0 )
    {
        lua_newtable( L );

        for ( int j = 0; j < 4; j++ )
        {
            lua_pushinteger( L, j );
            lua_pushnumber( L, matrix[0][j] );
            lua_settable( L, -3 );
        }
    }
    else if ( strcmp( field, "1" ) == 0 )
    {
        lua_newtable( L );

        for ( int j = 0; j < 4; j++ )
        {
            lua_pushinteger( L, j );
            lua_pushnumber( L, matrix[1][j] );
            lua_settable( L, -3 );
        }
    }
    else if ( strcmp( field, "2" ) == 0 )
    {
        lua_newtable( L );
        for ( int j = 0; j < 4; j++ )
        {
            lua_pushinteger( L, j );
            lua_pushnumber( L, matrix[2][j] );
            lua_settable( L, -3 );
        }
    }
    else
    {
        if ( lua_getmetatable( L, 1 ) )
        {
            LUA_METATABLE_INDEX_CHECK_TABLE( L );
        }

        lua_pushnil( L );
    }

    return 1;
}
LUA_BINDING_END( "any", "The value of the field." )

LUA_BINDING_BEGIN( Matrix3x4, __newindex, "class", "Metatable called when a non-existant field is set" )
{
    // TODO
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix3x4, __tostring, "class", "Metatable called when the object is converted to a string" )
{
    lua_matrix3x4_t matrix = LUA_BINDING_ARGUMENT( luaL_checkmatrix, 1, "Matrix3x4" );
    lua_pushfstring( L,
                     "Matrix3x4:\n\t%f, %f, %f, %f\n\t%f, %f, %f, %f\n\t%f, %f, %f, %f",
                     matrix[0][0],
                     matrix[0][1],
                     matrix[0][2],
                     matrix[0][3],
                     matrix[1][0],
                     matrix[1][1],
                     matrix[1][2],
                     matrix[1][3],
                     matrix[2][0],
                     matrix[2][1],
                     matrix[2][2],
                     matrix[2][3] );
    return 1;
}
LUA_BINDING_END()

/*
** Open matrix3x4_t object
*/
LUALIB_API int luaopen_Matrix3x4( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_MATRIXLIBNAME );

    LUA_REGISTRATION_COMMIT( Matrix3x4 );

    lua_pushstring( L, LUA_MATRIXLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Matrix3x4" */
    lua_pop( L, 1 );
    return 1;
}

LUA_REGISTRATION_INIT( math );

LUA_BINDING_BEGIN( math, Clamp, "library", "Clamps a number between two values." )
{
    lua_pushnumber( L, clamp( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "number" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "number" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "number" ) ) );

    return 1;
}
LUA_BINDING_END( "number", "The number clamped between the two values." )

LUA_BINDING_BEGIN( math, AngleVectors, "library", "Converts an angle to a direction vector." )
{
    if ( lua_gettop( L ) > 2 )
        AngleVectors(
            LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "Angle" ),
            &LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "Vector" ),
            &LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "Vector" ),
            &LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "Vector" ) );
    else
        AngleVectors(
            LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "Angle" ),
            &LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "Vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( math, VectorAngles, "library", "Converts a direction vector to an angle." )
{
    if ( lua_gettop( L ) > 2 )
        VectorAngles(
            LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "Vector" ),
            LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "Vector" ),
            LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "Angle" ) );
    else
        VectorAngles(
            LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "Vector" ),
            LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "Angle" ) );

    return 0;
}
LUA_BINDING_END()

/*
** Open mathlib library
*/
LUALIB_API int luaopen_mathlib( lua_State *L )
{
    // Extend the default math library
    LUA_REGISTRATION_COMMIT_LIBRARY( math );

    return 1;
}
