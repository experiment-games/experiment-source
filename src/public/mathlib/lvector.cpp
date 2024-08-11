//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//

#include "cbase.h"
#include "vector.h"
#include "fmtstr.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "lvector.h"
#include "langle.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_Vector &lua_tovector( lua_State *L, int idx )
{
    lua_Vector *v = ( lua_Vector * )luaL_checkudata( L, idx, LUA_VECTORLIBNAME );
    return *v;
}

/*
** checker functions (without type error)
*/

LUA_API bool lua_isvector(lua_State* L, int idx)
{
    void *p = luaL_testudata( L, idx, LUA_VECTORLIBNAME );
    return p != NULL;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushvector( lua_State *L, lua_Vector &v )
{
    lua_Vector *pVec = ( lua_Vector * )lua_newuserdata( L, sizeof( lua_Vector ) );
    *pVec = v;
    LUA_SAFE_SET_METATABLE( L, LUA_VECTORLIBNAME );
}

LUA_API void lua_pushvector( lua_State *L, const lua_Vector &v )
{
    lua_Vector *pVec = ( lua_Vector * )lua_newuserdata( L, sizeof( lua_Vector ) );
    *pVec = v;
    LUA_SAFE_SET_METATABLE( L, LUA_VECTORLIBNAME );
}

LUALIB_API lua_Vector &luaL_checkvector( lua_State *L, int narg )
{
    lua_Vector *d = ( lua_Vector * )luaL_checkudata( L, narg, LUA_VECTORLIBNAME );
    return *d;
}

LUALIB_API lua_Vector &luaL_optvector( lua_State *L, int narg, Vector *def )
{
    return luaL_opt( L, luaL_checkvector, narg, *def );
}

LUA_REGISTRATION_INIT( Vector )

LUA_BINDING_BEGIN( Vector, Angle, "class", "Converts a vector to an angle." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_Vector up( 0.0f, 0.0f, 1.0f );
    QAngle angle;
    VectorAngles( vec, up, angle );
    lua_pushangle( L, angle );

    return 1;
}
LUA_BINDING_END( "angle", "The converted angle." )

LUA_BINDING_BEGIN( Vector, AngleWithUp, "class", "Converts a vector to an angle with a specified up vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_Vector up = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "up" );
    QAngle angle;
    VectorAngles( vec, up, angle );
    lua_pushangle( L, angle );

    return 1;
}
LUA_BINDING_END( "angle", "The converted angle." )

LUA_BINDING_BEGIN( Vector, Cross, "class", "Calculates the cross product of two vectors." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_pushvector( L, vec1.Cross( vec2 ) );

    return 1;
}
LUA_BINDING_END( "vector", "The cross product of the two vectors." )

LUA_BINDING_BEGIN( Vector, DistanceTo, "class", "Calculates the distance between two vectors." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_pushnumber( L, vec1.DistTo( vec2 ) );

    return 1;
}
LUA_BINDING_END( "number", "The distance between the two vectors." )

LUA_BINDING_BEGIN( Vector, DistanceToSqr, "class", "Calculates the squared distance between two vectors." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_pushnumber( L, vec1.DistToSqr( vec2 ) );

    return 1;
}
LUA_BINDING_END( "number", "The squared distance between the two vectors." )

LUA_BINDING_BEGIN( Vector, DistanceTo2D, "class", "Calculates the distance between two vectors in 2D space." )
{
    lua_Vector a = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector b = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    Vector delta;
    CHECK_VALID( a );
    CHECK_VALID( b );
    delta.x = a.x - b.x;
    delta.y = a.y - b.y;
    lua_pushnumber( L, delta.Length() );

    return 1;
}
LUA_BINDING_END( "number", "The distance between the two vectors in 2D space." )

LUA_BINDING_BEGIN( Vector, DistanceTo2DSqr, "class", "Calculates the squared distance between two vectors in 2D space." )
{
    lua_Vector a = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector b = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    Vector delta;
    CHECK_VALID( a );
    CHECK_VALID( b );
    delta.x = a.x - b.x;
    delta.y = a.y - b.y;
    lua_pushnumber( L, delta.LengthSqr() );

    return 1;
}
LUA_BINDING_END( "number", "The squared distance between the two vectors in 2D space." )

LUA_BINDING_BEGIN( Vector, Dot, "class", "Calculates the dot product of two vectors." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_pushnumber( L, vec1.Dot( vec2 ) );

    return 1;
}
LUA_BINDING_END( "number", "The dot product of the two vectors." )

LUA_BINDING_BEGIN( Vector, Init, "class", "Initializes the vector with the specified values." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec.Init( luaL_optnumber( L, 2, 0.0f ), luaL_optnumber( L, 3, 0.0f ), luaL_optnumber( L, 4, 0.0f ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, Invalidate, "class", "Invalidates the vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec.Invalidate();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, IsLengthGreaterThan, "class", "Checks if the length of the vector is greater than the specified value." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushboolean( L, vec.IsLengthGreaterThan( luaL_checknumber( L, 2 ) ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the length of the vector is greater than the specified value, false otherwise." )

LUA_BINDING_BEGIN( Vector, IsLengthLessThan, "class", "Checks if the length of the vector is less than the specified value." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushboolean( L, vec.IsLengthLessThan( luaL_checknumber( L, 2 ) ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the length of the vector is less than the specified value, false otherwise." )

LUA_BINDING_BEGIN( Vector, IsValid, "class", "Checks if the vector is valid." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushboolean( L, vec.IsValid() );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the vector is valid, false otherwise." )

LUA_BINDING_BEGIN( Vector, Length, "class", "Calculates the length of the vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, vec.Length() );

    return 1;
}
LUA_BINDING_END( "number", "The length of the vector." )

LUA_BINDING_BEGIN( Vector, Length2D, "class", "Calculates the 2D length of the vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, vec.Length2D() );

    return 1;
}
LUA_BINDING_END( "number", "The 2D length of the vector." )

LUA_BINDING_BEGIN( Vector, Length2DSqr, "class", "Calculates the squared 2D length of the vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, vec.Length2DSqr() );

    return 1;
}
LUA_BINDING_END( "number", "The squared 2D length of the vector." )

LUA_BINDING_BEGIN( Vector, LengthSqr, "class", "Calculates the squared length of the vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, vec.LengthSqr() );

    return 1;
}
LUA_BINDING_END( "number", "The squared length of the vector." )

LUA_BINDING_BEGIN( Vector, GetGreatest, "class", "Calculates the maximum values of two vectors." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_pushvector( L, vec1.Max( vec2 ) );

    return 1;
}
LUA_BINDING_END( "vector", "The maximum values of the two vectors." )

LUA_BINDING_BEGIN( Vector, GetSmallest, "class", "Calculates the minimum values of two vectors." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_pushvector( L, vec1.Min( vec2 ) );

    return 1;
}
LUA_BINDING_END( "vector", "The minimum values of the two vectors." )

LUA_BINDING_BEGIN( Vector, MultiplyThenAdd, "class", "Multiplies the first vector by the second vector and adds the third vector." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_Vector vec3 = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "vector3" );
    vec1.MulAdd( vec2, vec3, luaL_checknumber( L, 4 ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, Negate, "class", "Negates the vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec.Negate();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, NormalizeInPlace, "class", "Normalizes the vector in place." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, vec.NormalizeInPlace() );

    return 1;
}
LUA_BINDING_END( "number", "The length of the vector before normalization." )

LUA_BINDING_BEGIN( Vector, Random, "class", "Generates a random vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec.Random( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, WithinAABox, "class", "Checks if the vector is within an axis-aligned box." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_Vector boxMin = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "boxMin" );
    lua_Vector boxMax = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "boxMax" );
    lua_pushboolean( L, vec.WithinAABox( boxMin, boxMax ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the vector is within the axis-aligned box, false otherwise." )

LUA_BINDING_BEGIN( Vector, Zero, "class", "Zeroes the vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec.Zero();

    return 0;
}
LUA_BINDING_END()

// Modify the existing vector, adding another vector to it.
LUA_BINDING_BEGIN( Vector, Add, "class", "Adds another vector to the vector." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_Vector vecToAdd = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vectorToAdd" );
    vec += vecToAdd;

    return 0;
}
LUA_BINDING_END()

// Modify the existing vector, dividing it by a number.
LUA_BINDING_BEGIN( Vector, Divide, "class", "Divides the vector by a number." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    float divideBy = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "divideBy" );
    vec /= divideBy;

    return 0;
}
LUA_BINDING_END()

// Modify the existing vector, scaling it by a number.
LUA_BINDING_BEGIN( Vector, Scale, "class", "Scales the vector by a number." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    float scaleBy = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "scaleBy" );
    vec *= scaleBy;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, __index, "class", "Gets the value of a vector field." )
{
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( strcmp( field, "x" ) == 0 || strcmp( field, "r" ) == 0 || strcmp( field, "1" ) == 0 )
        lua_pushnumber( L, vec.x );
    else if ( strcmp( field, "y" ) == 0 || strcmp( field, "g" ) == 0 || strcmp( field, "2" ) == 0 )
        lua_pushnumber( L, vec.y );
    else if ( strcmp( field, "z" ) == 0 || strcmp( field, "b" ) == 0 || strcmp( field, "3" ) == 0 )
        lua_pushnumber( L, vec.z );
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
LUA_BINDING_END( "number", "The value of the vector field." )


LUA_BINDING_BEGIN( Vector, __newindex, "class", "Sets the value of a vector field." )
{
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( strcmp( field, "x" ) == 0 || strcmp( field, "r" ) == 0 || strcmp( field, "1" ) == 0 )
        luaL_checkvector( L, 1 ).x = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "y" ) == 0 || strcmp( field, "g" ) == 0 || strcmp( field, "2" ) == 0 )
        luaL_checkvector( L, 1 ).y = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "z" ) == 0 || strcmp( field, "b" ) == 0 || strcmp( field, "3" ) == 0 )
        luaL_checkvector( L, 1 ).z = ( vec_t )luaL_checknumber( L, 3 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, __tostring, "class", "Converts the vector to a string." )
{
    lua_pushfstring( L, "Vector: %s", VecToString( luaL_checkvector( L, 1 ) ) );

    return 1;
}
LUA_BINDING_END( "string", "The string representation of the vector." )

LUA_BINDING_BEGIN( Vector, __eq, "class", "Checks if two vectors are equal." )
{
    lua_pushboolean( L, luaL_checkvector( L, 1 ) == luaL_checkvector( L, 2 ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the vectors are equal, false otherwise." )

LUA_BINDING_BEGIN( Vector, __add, "class", "Adds two vectors." )
{
    lua_pushvector( L, luaL_checkvector( L, 1 ) + luaL_checkvector( L, 2 ) );

    return 1;
}
LUA_BINDING_END( "vector", "The sum of the two vectors." )

LUA_BINDING_BEGIN( Vector, __sub, "class", "Subtracts two vectors." )
{
    lua_pushvector( L, luaL_checkvector( L, 1 ) - luaL_checkvector( L, 2 ) );

    return 1;
}
LUA_BINDING_END( "vector", "The difference of the two vectors." )

LUA_BINDING_BEGIN( Vector, __mul, "class", "Multiplies two vectors or a vector by a number." )
{
    switch ( lua_type( L, 1 ) )
    {
        case LUA_TNUMBER:
            lua_pushvector( L, luaL_checknumber( L, 1 ) * luaL_checkvector( L, 2 ) );
            break;
        case LUA_TUSERDATA:
        default:
            lua_pushvector( L, luaL_checkvector( L, 1 ) * luaL_checknumber( L, 2 ) );
            break;
    }

    return 1;
}
LUA_BINDING_END( "vector", "The product of the two vectors or the vector and the number." )

LUA_BINDING_BEGIN( Vector, __div, "class", "Divides a vector by a number." )
{
    lua_pushvector( L, luaL_checkvector( L, 1 ) / luaL_checknumber( L, 2 ) );

    return 1;
}
LUA_BINDING_END( "vector", "The vector divided by the number." )

LUA_BINDING_BEGIN( Vector, __unm, "class", "Negates the vector." )
{
    lua_pushvector( L, -luaL_checkvector( L, 1 ) );

    return 1;
}
LUA_BINDING_END( "vector", "The negated vector." )

LUA_REGISTRATION_INIT( _G );

LUA_BINDING_BEGIN( _G, Vector, "library", "Creates a new vector." )
{
    // If the argument is a vector, copy it
    if ( lua_isvector( L, 1 ) )
    {
        lua_pushvector( L, LUA_BINDING_ARGUMENT( lua_tovector, 1, "vector" ) );
        return 1;
    }

    // If its a string, parse the space separated values
    if ( lua_type( L, 1 ) == LUA_TSTRING )
    {
        Vector vec;
        UTIL_StringToVector( vec.Base(), LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "vectorString" ) );
        lua_pushvector( L, vec );
        return 1;
    }

    // Otherwise, create a new vector where all values are 0 if no x, y, z are provided
    vec_t x = ( vec_t )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 1, 0.0f, "x" );
    vec_t y = ( vec_t )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.0f, "y" );
    vec_t z = ( vec_t )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0.0f, "z" );
    lua_pushvector( L, Vector( x, y, z ) );
    return 1;
}
LUA_BINDING_END( "vector", "The created vector." )

/*
** Open Vector object
*/
LUALIB_API int luaopen_Vector( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_VECTORLIBNAME );

    LUA_REGISTRATION_COMMIT( Vector );

    lua_pushstring( L, "Vector" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Vector" */

    LUA_REGISTRATION_COMMIT_GLOBAL( _G );
    lua_pop( L, 1 ); // pop metatable

    Vector origin = vec3_origin;
    lua_pushvector( L, origin );
    lua_setglobal( L, "vec3_origin" ); /* set global vec3_origin */

    Vector invalid = vec3_invalid;
    lua_pushvector( L, invalid );
    lua_setglobal( L, "vec3_invalid" ); /* set global vec3_invalid */

    return 1;
}
