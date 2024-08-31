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

LUA_API lua_QAngle &lua_toangle( lua_State *L, int idx )
{
    lua_QAngle *v = ( lua_QAngle * )luaL_checkudata( L, idx, LUA_QANGLEMETANAME );
    return *v;
}

/*
** checker functions (without type error)
*/

LUA_API bool lua_isangle( lua_State *L, int idx )
{
    void *p = luaL_testudata( L, idx, LUA_QANGLEMETANAME );
    return p != NULL;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushangle( lua_State *L, lua_QAngle &v )
{
    lua_QAngle *pVec = ( lua_QAngle * )lua_newuserdata( L, sizeof( lua_QAngle ) );
    *pVec = v;
    LUA_SAFE_SET_METATABLE( L, LUA_QANGLEMETANAME );
}

LUA_API void lua_pushangle( lua_State *L, const lua_QAngle &v )
{
    lua_QAngle *pVec = ( lua_QAngle * )lua_newuserdata( L, sizeof( lua_QAngle ) );
    *pVec = v;
    LUA_SAFE_SET_METATABLE( L, LUA_QANGLEMETANAME );
}

LUALIB_API lua_QAngle &luaL_checkangle( lua_State *L, int narg )
{
    lua_QAngle *d = ( lua_QAngle * )luaL_checkudata( L, narg, LUA_QANGLEMETANAME );
    return *d;
}

LUALIB_API lua_QAngle &luaL_optangle( lua_State *L, int narg, lua_QAngle *def )
{
    return luaL_opt( L, luaL_checkangle, narg, *def );
}

LUA_REGISTRATION_INIT( Angle );

LUA_BINDING_BEGIN( Angle, Forward, "class", "Returns the forward vector of the angle." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_Vector forward;
    AngleVectors( angle, &forward, NULL, NULL );
    lua_pushvector( L, forward );

    return 1;
}
LUA_BINDING_END( "Vector", "The forward vector of the angle." )

LUA_BINDING_BEGIN( Angle, Right, "class", "Returns the right vector of the angle." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_Vector right;
    AngleVectors( angle, NULL, &right, NULL );
    lua_pushvector( L, right );

    return 1;
}
LUA_BINDING_END( "Vector", "The right vector of the angle." )

LUA_BINDING_BEGIN( Angle, Up, "class", "Returns the up vector of the angle." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_Vector up;
    AngleVectors( angle, NULL, NULL, &up );
    lua_pushvector( L, up );

    return 1;
}
LUA_BINDING_END( "Vector", "The up vector of the angle." )

LUA_BINDING_BEGIN( Angle, IsEqualWithTolerance, "class", "Checks if the angle is equal to another angle within a tolerance." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_QAngle other = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "other" );
    float tolerance = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "tolerance" );
    lua_pushboolean( L, QAnglesAreEqual( angle, other, tolerance ) );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the angles are equal within the tolerance, false otherwise." )

LUA_BINDING_BEGIN( Angle, Initialize, "class", "Initializes the angle with the specified values." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    vec_t x = ( vec_t )LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" );
    vec_t y = ( vec_t )LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" );
    vec_t z = ( vec_t )LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "z" );
    angle.Init( x, y, z );
    lua_pushangle( L, angle );

    return 1;
}
LUA_BINDING_END( "Angle", "The initialized angle." )

LUA_BINDING_BEGIN( Angle, Invalidate, "class", "Makes the angle invalid." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    angle.Invalidate();
    lua_pushangle( L, angle );

    return 1;
}
LUA_BINDING_END( "Angle", "The invalidated angle." )

LUA_BINDING_BEGIN( Angle, IsValid, "class", "Checks if the angle is valid." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_pushboolean( L, angle.IsValid() );

    return 1;
}
LUA_BINDING_END( "boolean", "True if the angle is valid, false otherwise." )

LUA_BINDING_BEGIN( Angle, Length, "class", "Returns the length of the angle." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_pushnumber( L, angle.Length() );

    return 1;
}
LUA_BINDING_END( "number", "The length of the angle." )

LUA_BINDING_BEGIN( Angle, LengthSqr, "class", "Returns the squared length of the angle." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_pushnumber( L, angle.LengthSqr() );

    return 1;
}
LUA_BINDING_END( "number", "The squared length of the angle." )

//  Rotates the angle around the specified axis by the specified degrees. (doesnt return a new angle)
LUA_BINDING_BEGIN( Angle, RotateAroundAxis, "class", "Rotates the angle around the specified axis by the specified degrees." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_Vector axis = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "axis" );
    float degrees = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "degrees" );

    VMatrix rotationMatrix, angleAsMatrix, resultMatrix;
    MatrixFromAngles( angle, angleAsMatrix );
    MatrixBuildRotationAboutAxis( rotationMatrix, axis, degrees );
    MatrixMultiply( angleAsMatrix, rotationMatrix, resultMatrix );
    // Sets the result into the existing angle
    MatrixToAngles( resultMatrix, angle );

    lua_pushangle( L, angle );

    return 1;
}
LUA_BINDING_END( "Angle", "The rotated angle." )

//  Modifies the existing angle, adding another angle to it
LUA_BINDING_BEGIN( Angle, Add, "class", "Modifies the existing angle, adding another angle to it." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_QAngle angleToAdd = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle2" );

    lua_pushangle( L, angle + angleToAdd );

    return 1;
}
LUA_BINDING_END( "Angle", "The added angle." )

LUA_BINDING_BEGIN( Angle, Divide, "class", "Modifies the existing angle, dividing it by a number." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    float divideBy = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "divideBy" );

    lua_pushangle( L, angle / divideBy );

    return 1;
}
LUA_BINDING_END( "Angle", "The divided angle." )

LUA_BINDING_BEGIN( Angle, Scale, "class", "Modifies the existing angle, scaling it by a number." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    float scaleBy = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "scaleBy" );

    lua_pushangle( L, angle * scaleBy );

    return 1;
}
LUA_BINDING_END( "Angle", "The same angle, now scaled." )

LUA_BINDING_BEGIN( Angle, Normalize, "class", "Modifies the existing angle, normalizing it between -180 to 180 degrees pitch, yaw and roll." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    angle.x = AngleNormalize( angle.x );
    angle.y = AngleNormalize( angle.y );
    angle.z = AngleNormalize( angle.z );
    lua_pushangle( L, angle );

    return 1;
}
LUA_BINDING_END( "Angle", "The same angle, now normalized." )

LUA_BINDING_BEGIN( Angle, __index, "class", "Metatable that is called when a key is not found in the table." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( strcmp( field, "x" ) == 0 || strcmp( field, "p" ) == 0 || strcmp( field, "pitch" ) == 0 || strcmp( field, "1" ) == 0 )
        lua_pushnumber( L, angle.x );
    else if ( strcmp( field, "y" ) == 0 || strcmp( field, "y" ) == 0 || strcmp( field, "yaw" ) == 0 || strcmp( field, "2" ) == 0 )
        lua_pushnumber( L, angle.y );
    else if ( strcmp( field, "z" ) == 0 || strcmp( field, "r" ) == 0 || strcmp( field, "roll" ) == 0 || strcmp( field, "3" ) == 0 )
        lua_pushnumber( L, angle.z );
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
LUA_BINDING_END( "any", "The value of the key." )

LUA_BINDING_BEGIN( Angle, __newindex, "class", "Metatable that is called to set a value to a key that is not found in the table." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( strcmp( field, "x" ) == 0 || strcmp( field, "p" ) == 0 || strcmp( field, "pitch" ) == 0 || strcmp( field, "1" ) == 0 )
        angle.x = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "y" ) == 0 || strcmp( field, "y" ) == 0 || strcmp( field, "yaw" ) == 0 || strcmp( field, "2" ) == 0 )
        angle.y = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "z" ) == 0 || strcmp( field, "r" ) == 0 || strcmp( field, "roll" ) == 0 || strcmp( field, "3" ) == 0 )
        angle.z = ( vec_t )luaL_checknumber( L, 3 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Angle, __tostring, "class", "Metatable that is called when the angle is to be converted to a string." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( lua_toangle, 1, "angle" );
    lua_pushfstring( L, "Angle: %s", VecToString( angle ) );
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the angle." )

LUA_BINDING_BEGIN( Angle, __eq, "class", "Metatable that is called when the angle is to be compared with another angle." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( lua_toangle, 1, "angle" );
    lua_QAngle angle2 = LUA_BINDING_ARGUMENT( lua_toangle, 2, "other" );
    lua_pushboolean( L, angle == angle2 );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the angles are equal, false otherwise." )

LUA_BINDING_BEGIN( Angle, __add, "class", "Metatable that is called when the angle is to be added to another angle." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_QAngle angle2 = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle2" );
    lua_pushangle( L, angle + angle2 );
    return 1;
}
LUA_BINDING_END( "Angle", "The added angle." )

LUA_BINDING_BEGIN( Angle, __sub, "class", "Metatable that is called when the angle is to be subtracted from another angle." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_QAngle angle2 = LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle2" );
    lua_pushangle( L, angle - angle2 );
    return 1;
}
LUA_BINDING_END( "Angle", "The subtracted angle." )

LUA_BINDING_BEGIN( Angle, __mul, "class", "Metatable that is called when the angle is to be multiplied by a number." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    float number = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "number" );
    lua_pushangle( L, angle * number );
    return 1;
}
LUA_BINDING_END( "Angle", "The multiplied angle." )

LUA_BINDING_BEGIN( Angle, __div, "class", "Metatable that is called when the angle is to be divided by a number." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    float number = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "number" );
    lua_pushangle( L, angle / number );
    return 1;
}
LUA_BINDING_END( "Angle", "The divided angle." )

LUA_BINDING_BEGIN( Angle, __unm, "class", "Metatable that is called when the angle is to be negated." )
{
    lua_QAngle &angle = LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" );
    lua_pushangle( L, -angle );
    return 1;
}
LUA_BINDING_END( "Angle", "The negated angle." )

LUA_REGISTRATION_INIT( Angles );

LUA_BINDING_BEGIN( Angles, Create, "library", "Creates a new angle." )
{
    // If the argument is an angle, copy it
    if ( lua_isangle( L, 1 ) )
    {
        lua_pushangle( L, LUA_BINDING_ARGUMENT( lua_toangle, 1, "angle" ) );
        return 1;
    }

    // If its a string, parse the space separated values
    if ( lua_type( L, 1 ) == LUA_TSTRING )
    {
        lua_QAngle angle;
        UTIL_StringToVector( angle.Base(), LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "angleString" ) );
        lua_pushangle( L, angle );
        return 1;
    }

    // Otherwise, create a new angle where all values are 0 if no x, y, z are provided
    vec_t x = ( vec_t )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 1, 0.0f, "p" );
    vec_t y = ( vec_t )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 2, 0.0f, "y" );
    vec_t z = ( vec_t )LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0.0f, "r" );
    lua_pushangle( L, lua_QAngle( x, y, z ) );
    return 1;
}
LUA_BINDING_END( "Angle", "The created angle." )

/*
** Open QAngle object
*/
LUALIB_API int luaopen_QAngle( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_QANGLEMETANAME );

    LUA_REGISTRATION_COMMIT( Angle );

    lua_pushstring( L, "Angle" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Angle" */

    LUA_REGISTRATION_COMMIT_LIBRARY( Angles );
    lua_pop( L, 1 );  // pop metatable

    lua_QAngle v = vec3_angle;
    lua_pushangle( L, v );
    lua_setglobal( L, "vec3_angle" ); /* set global vec3_angle */

    return 1;
}
