#include "cbase.h"
#include "vector.h"
#include "fmtstr.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "lvector.h"
#include "langle.h"

#ifdef CLIENT_DLL
#include <vgui/ISurface.h>
#include <view_scene.h>
#include <sourcevr/isourcevirtualreality.h>
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_Vector &lua_tovector( lua_State *L, int idx )
{
    lua_Vector *v = ( lua_Vector * )luaL_checkudata( L, idx, LUA_VECTORMETANAME );
    return *v;
}

/*
** checker functions (without type error)
*/

LUA_API bool lua_isvector( lua_State *L, int idx )
{
    void *p = luaL_testudata( L, idx, LUA_VECTORMETANAME );
    return p != NULL;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushvector( lua_State *L, lua_Vector &v )
{
    lua_Vector *pVec = ( lua_Vector * )lua_newuserdata( L, sizeof( lua_Vector ) );
    *pVec = v;
    LUA_SAFE_SET_METATABLE( L, LUA_VECTORMETANAME );
}

LUA_API void lua_pushvector( lua_State *L, const lua_Vector &v )
{
    lua_Vector *pVec = ( lua_Vector * )lua_newuserdata( L, sizeof( lua_Vector ) );
    *pVec = v;
    LUA_SAFE_SET_METATABLE( L, LUA_VECTORMETANAME );
}

LUALIB_API lua_Vector &luaL_checkvector( lua_State *L, int narg )
{
    lua_Vector *d = ( lua_Vector * )luaL_checkudata( L, narg, LUA_VECTORMETANAME );
    return *d;
}

LUALIB_API lua_Vector &luaL_optvector( lua_State *L, int narg, Vector *def )
{
    return luaL_opt( L, luaL_checkvector, narg, *def );
}

LUA_REGISTRATION_INIT( Vector )

LUA_BINDING_BEGIN( Vector, Angle, "class", "Converts a vector to an angle." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_Vector up( 0.0f, 0.0f, 1.0f );
    QAngle angle;
    VectorAngles( vec, up, angle );
    lua_pushangle( L, angle );

    return 1;
}
LUA_BINDING_END( "Angle", "The converted angle." )

LUA_BINDING_BEGIN( Vector, AngleWithUp, "class", "Converts a vector to an angle with a specified up vector." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_Vector up = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "up" );
    QAngle angle;
    VectorAngles( vec, up, angle );
    lua_pushangle( L, angle );

    return 1;
}
LUA_BINDING_END( "Angle", "The converted angle." )

LUA_BINDING_BEGIN( Vector, Cross, "class", "Calculates the cross product of two vectors." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_pushvector( L, vec1.Cross( vec2 ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The cross product of the two vectors." )

LUA_BINDING_BEGIN( Vector, DistanceTo, "class", "Calculates the distance between two vectors." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_pushnumber( L, vec1.DistTo( vec2 ) );

    return 1;
}
LUA_BINDING_END( "number", "The distance between the two vectors." )

LUA_BINDING_BEGIN( Vector, DistanceToAsSqr, "class", "Calculates the squared distance between two vectors." )
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

LUA_BINDING_BEGIN( Vector, Initialize, "class", "Initializes the vector with the specified values." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec_t x = ( vec_t )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    vec_t y = ( vec_t )LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );
    vec_t z = ( vec_t )LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "z" );
    vec.Init( x, y, z );

    lua_pushvector( L, vec );

    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, Invalidate, "class", "Invalidates the vector." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec.Invalidate();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, IsLengthGreaterThan, "class", "Checks if the length of the vector is greater than the specified value." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushboolean( L, vec.IsLengthGreaterThan( luaL_checknumber( L, 2 ) ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the length of the vector is greater than the specified value, false otherwise." )

LUA_BINDING_BEGIN( Vector, IsLengthLessThan, "class", "Checks if the length of the vector is less than the specified value." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushboolean( L, vec.IsLengthLessThan( luaL_checknumber( L, 2 ) ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the length of the vector is less than the specified value, false otherwise." )

LUA_BINDING_BEGIN( Vector, IsValid, "class", "Checks if the vector is valid." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushboolean( L, vec.IsValid() );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the vector is valid, false otherwise." )

LUA_BINDING_BEGIN( Vector, Length, "class", "Calculates the length of the vector." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, vec.Length() );

    return 1;
}
LUA_BINDING_END( "number", "The length of the vector." )

LUA_BINDING_BEGIN( Vector, Length2D, "class", "Calculates the 2D length of the vector." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, vec.Length2D() );

    return 1;
}
LUA_BINDING_END( "number", "The 2D length of the vector." )

LUA_BINDING_BEGIN( Vector, Length2DSqr, "class", "Calculates the squared 2D length of the vector." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, vec.Length2DSqr() );

    return 1;
}
LUA_BINDING_END( "number", "The squared 2D length of the vector." )

LUA_BINDING_BEGIN( Vector, LengthSqr, "class", "Calculates the squared length of the vector." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
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
LUA_BINDING_END( "Vector", "The maximum values of the two vectors." )

LUA_BINDING_BEGIN( Vector, GetSmallest, "class", "Calculates the minimum values of two vectors." )
{
    lua_Vector vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector1" );
    lua_Vector vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector2" );
    lua_pushvector( L, vec1.Min( vec2 ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The minimum values of the two vectors." )

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
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec.Negate();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, NormalizeInPlace, "class", "Normalizes the vector in place." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, vec.NormalizeInPlace() );

    return 1;
}
LUA_BINDING_END( "number", "The length of the vector before normalization." )

LUA_BINDING_BEGIN( Vector, Random, "class", "Generates a random vector." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec.Random( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, WithinAABox, "class", "Checks if the vector is within an axis-aligned box." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_Vector boxMin = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "boxMin" );
    lua_Vector boxMax = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "boxMax" );
    lua_pushboolean( L, vec.WithinAABox( boxMin, boxMax ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the vector is within the axis-aligned box, false otherwise." )

LUA_BINDING_BEGIN( Vector, Zero, "class", "Zeroes the vector." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    vec.Zero();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, Add, "class", "Modify the existing vector, adding another vector to it." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_Vector vecToAdd = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vectorToAdd" );
    vec += vecToAdd;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, Divide, "class", "Modify the existing vector, dividing it by a number." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    float divideBy = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "divideBy" );
    vec /= divideBy;

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, Scale, "class", "Modify the existing vector, scaling it by a number." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    float scaleBy = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "scaleBy" );
    vec *= scaleBy;

    return 0;
}
LUA_BINDING_END()

#ifdef CLIENT_DLL
LUA_BINDING_BEGIN( Vector, ToScreen, "class", "Converts the vector to screen coordinates.", "client" )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    bool isVisible;
    float x, y;

    Vector screen;
    screen.Init();

    if ( UseVR() )
    {
        isVisible = ScreenTransform( vec, screen ) != 0;

        int vx, vy, vw, vh;
        vgui::surface()->GetFullscreenViewport( vx, vy, vw, vh );

        float screenWidth = vw;
        float screenHeight = vh;

        x = 0.5f * ( 1.0f + screen[0] ) * screenWidth + 0.5f;
        y = 0.5f * ( 1.0f - screen[1] ) * screenHeight + 0.5f;
    }
    else
    {
        x = ScreenWidth() / 2;
        y = ScreenHeight() / 2;

        isVisible = ScreenTransform( vec, screen );
        x += 0.5 * screen[0] * ScreenWidth() + 0.5;
        y -= 0.5 * screen[1] * ScreenHeight() + 0.5;
    }

    lua_newtable( L );

    lua_pushnumber( L, x );
    lua_setfield( L, -2, "x" ); // GMod compatibility

    lua_pushnumber( L, x );
    lua_setfield( L, -2, "X" );

    lua_pushnumber( L, y );
    lua_setfield( L, -2, "y" ); // GMod compatibility

    lua_pushnumber( L, y );
    lua_setfield( L, -2, "Y" );

    lua_pushboolean( L, isVisible );
    lua_setfield( L, -2, "visible" ); // GMod compatibility

    lua_pushboolean( L, isVisible );
    lua_setfield( L, -2, "IsVisible" );

    return 1;
}
LUA_BINDING_END( "table", "A table containing the screen coordinates and whether it is visible." )
#endif

LUA_BINDING_BEGIN( Vector, __index, "class", "Gets the value of a vector field." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
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
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( strcmp( field, "x" ) == 0 || strcmp( field, "r" ) == 0 || strcmp( field, "1" ) == 0 )
        vec.x = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "y" ) == 0 || strcmp( field, "g" ) == 0 || strcmp( field, "2" ) == 0 )
        vec.y = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "z" ) == 0 || strcmp( field, "b" ) == 0 || strcmp( field, "3" ) == 0 )
        vec.z = ( vec_t )luaL_checknumber( L, 3 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Vector, __tostring, "class", "Converts the vector to a string." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushfstring( L, "Vector: %s", VecToString( vec ) );

    return 1;
}
LUA_BINDING_END( "string", "The string representation of the vector." )

LUA_BINDING_BEGIN( Vector, __eq, "class", "Checks if two vectors are equal." )
{
    lua_Vector vectorA = LUA_BINDING_ARGUMENT( lua_tovector, 1, "vectorA" );
    lua_Vector vectorB = LUA_BINDING_ARGUMENT( lua_tovector, 2, "vectorB" );
    lua_pushboolean( L, vectorA == vectorB );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the vectors are equal, false otherwise." )

LUA_BINDING_BEGIN( Vector, __add, "class", "Adds two vectors." )
{
    lua_Vector vectorA = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vectorA" );
    lua_Vector vectorB = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vectorB" );
    lua_pushvector( L, vectorA + vectorB );

    return 1;
}
LUA_BINDING_END( "Vector", "The sum of the two vectors." )

LUA_BINDING_BEGIN( Vector, __sub, "class", "Subtracts two vectors." )
{
    lua_Vector vectorA = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vectorA" );
    lua_Vector vectorB = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vectorB" );
    lua_pushvector( L, vectorA - vectorB );

    return 1;
}
LUA_BINDING_END( "Vector", "The difference of the two vectors." )

LUA_BINDING_BEGIN( Vector, __mul, "class", "Multiplies two vectors or a vector by a number." )
{
    if ( lua_isnumber( L, 1 ) )
    {
        lua_Number number = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "number" );
        lua_Vector vector = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" );
        lua_pushvector( L, vector * number );
    }
    else if ( lua_isnumber( L, 2 ) )
    {
        lua_Vector vector = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
        lua_Number number = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "number" );
        lua_pushvector( L, vector * number );
    }
    else
    {
        lua_Vector vectorA = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vectorA" );
        lua_Vector vectorB = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vectorB" );
        lua_pushvector( L, vectorA * vectorB );
    }

    return 1;
}
LUA_BINDING_END( "Vector", "The product of the two vectors or the vector and the number." )

LUA_BINDING_BEGIN( Vector, __div, "class", "Divides a vector by a number." )
{
    lua_Vector vector = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_Number number = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "number" );
    lua_pushvector( L, vector / number );

    return 1;
}
LUA_BINDING_END( "Vector", "The vector divided by the number." )

LUA_BINDING_BEGIN( Vector, __unm, "class", "Negates the vector." )
{
    lua_Vector vector = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushvector( L, -vector );

    return 1;
}
LUA_BINDING_END( "Vector", "The negated vector." )

/*
** Vectors Library
*/

LUA_REGISTRATION_INIT( Vectors );

LUA_BINDING_BEGIN( Vectors, Create, "library", "Creates a new vector." )
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
LUA_BINDING_END( "Vector", "The created vector." )

LUA_BINDING_BEGIN( Vectors, VectorToYaw, "library", "Converts a vector to a yaw angle." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, UTIL_VecToYaw( vec ) );

    return 1;
}
LUA_BINDING_END( "number", "The converted yaw angle." )

LUA_BINDING_BEGIN( Vectors, VectorToPitch, "library", "Converts a vector to a pitch angle." )
{
    lua_Vector &vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" );
    lua_pushnumber( L, UTIL_VecToPitch( vec ) );

    return 1;
}
LUA_BINDING_END( "number", "The converted pitch angle." )

LUA_BINDING_BEGIN( Vectors, YawToVector, "library", "Converts a yaw angle to a vector." )
{
    float yaw = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "yaw" );
    lua_pushvector( L, UTIL_YawToVector( yaw ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The converted vector." )

LUA_BINDING_BEGIN( Vectors, Lerp, "library", "Linearly interpolates between two vectors." )
{
    float fraction = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "fraction" );
    lua_Vector &vec1 = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "from" );
    lua_Vector &vec2 = LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "to" );
    lua_pushvector( L, VectorLerp( vec1, vec2, fraction ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The interpolated vector." )

#ifdef GAME_DLL

LUA_BINDING_BEGIN( Vectors, AxisStringToPointDirection, "library", "Converts an axis string to a point direction.", "server" )
{
    Vector start, direction;
    UTIL_AxisStringToPointDir( start, direction, LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "axisString" ) );
    lua_pushvector( L, start );
    lua_pushvector( L, direction );

    return 2;
}
LUA_BINDING_END( "Vector", "The converted start point.", "Vector", "The converted direction." )

LUA_BINDING_BEGIN( Vectors, AxisStringToPointPoint, "library", "Converts an axis string to a point point.", "server" )
{
    Vector start, end;
    UTIL_AxisStringToPointPoint( start, end, LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "axisString" ) );
    lua_pushvector( L, start );
    lua_pushvector( L, end );

    return 2;
}
LUA_BINDING_END( "Vector", "The converted start point.", "Vector", "The converted end point." )

LUA_BINDING_BEGIN( Vectors, AxisStringToUnitDirection, "library", "Converts an axis string to a unit direction.", "server" )
{
    Vector direction;
    UTIL_AxisStringToUnitDir( direction, LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "axisString" ) );
    lua_pushvector( L, direction );

    return 1;
}
LUA_BINDING_END( "Vector", "The converted unit direction." )

LUA_BINDING_BEGIN( Vectors, SnapDirectionToAxis, "library", "Modifies a vector so it snaps to the nearest axis vector (if within epsilon).", "server" )
{
    UTIL_SnapDirectionToAxis(
        LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "direction" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.002f, "epsilon" ) );

    return 0;
}
LUA_BINDING_END()

#endif  // GAME_DLL

/*
** Open Vector object
*/
LUALIB_API int luaopen_Vector( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_VECTORMETANAME );

    LUA_REGISTRATION_COMMIT( Vector );

    lua_pushstring( L, LUA_VECTORMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Vector" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Vectors );
    lua_pop( L, 1 );  // pop metatable

    Vector origin = vec3_origin;
    lua_pushvector( L, origin );
    lua_setglobal( L, "vec3_origin" ); /* set global vec3_origin */

    Vector invalid = vec3_invalid;
    lua_pushvector( L, invalid );
    lua_setglobal( L, "vec3_invalid" ); /* set global vec3_invalid */

    return 1;
}
