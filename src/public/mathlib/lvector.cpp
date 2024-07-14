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

LUA_API lua_QAngle &lua_toangle( lua_State *L, int idx )
{
    lua_QAngle *v = ( lua_QAngle * )luaL_checkudata( L, idx, LUA_QANGLELIBNAME );
    return *v;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushvector( lua_State *L, lua_Vector &v )
{
    lua_Vector *pVec = ( lua_Vector * )lua_newuserdata( L, sizeof( lua_Vector ) );
    *pVec = v;
    luaL_getmetatable( L, LUA_VECTORLIBNAME );
    lua_setmetatable( L, -2 );
}

LUA_API void lua_pushangle( lua_State *L, lua_QAngle &v )
{
    lua_QAngle *pVec = ( lua_QAngle * )lua_newuserdata( L, sizeof( lua_QAngle ) );
    *pVec = v;
    luaL_getmetatable( L, LUA_QANGLELIBNAME );
    lua_setmetatable( L, -2 );
}

LUALIB_API lua_Vector &luaL_checkvector( lua_State *L, int narg )
{
    lua_Vector *d = ( lua_Vector * )luaL_checkudata( L, narg, LUA_VECTORLIBNAME );
    return *d;
}

LUALIB_API lua_QAngle &luaL_checkangle( lua_State *L, int narg )
{
    lua_QAngle *d = ( lua_QAngle * )luaL_checkudata( L, narg, LUA_QANGLELIBNAME );
    return *d;
}

LUALIB_API lua_Vector &luaL_optvector( lua_State *L, int narg, Vector *def )
{
    return luaL_opt( L, luaL_checkvector, narg, *def );
}

LUALIB_API lua_QAngle &luaL_optangle( lua_State *L, int narg, QAngle *def )
{
    return luaL_opt( L, luaL_checkangle, narg, *def );
}

static int Vector_Angle( lua_State *L )
{
    Vector vec = luaL_checkvector( L, 1 );
    QAngle angle;

    Vector up( 0.0f, 0.0f, 1.0f );
    VectorAngles( vec, up, angle );
    lua_pushangle( L, angle );

    return 1;
}

static int Vector_AngleWithUp( lua_State *L )
{
    Vector vec = luaL_checkvector( L, 1 );
    QAngle angle;
    Vector up = luaL_checkvector( L, 2 );

    VectorAngles( vec, up, angle );
    lua_pushangle( L, angle );

    return 1;
}

static int Vector_Cross( lua_State *L )
{
    lua_pushvector( L, luaL_checkvector( L, 1 ).Cross( luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int Vector_DistTo( lua_State *L )
{
    lua_pushnumber( L, luaL_checkvector( L, 1 ).DistTo( luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int Vector_DistToSqr( lua_State *L )
{
    lua_pushnumber( L, luaL_checkvector( L, 1 ).DistToSqr( luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int Vector_DistanceTo2D( lua_State *L )
{
    Vector delta;
    Vector a = luaL_checkvector( L, 1 );
    Vector b = luaL_checkvector( L, 2 );

    CHECK_VALID( a );
    CHECK_VALID( b );
    delta.x = a.x - b.x;
    delta.y = a.y - b.y;

    lua_pushnumber( L, delta.Length() );

    return 1;
}

static int Vector_DistanceTo2DSqr( lua_State *L )
{
    Vector delta;
    Vector a = luaL_checkvector( L, 1 );
    Vector b = luaL_checkvector( L, 2 );

    CHECK_VALID( a );
    CHECK_VALID( b );
    delta.x = a.x - b.x;
    delta.y = a.y - b.y;

    lua_pushnumber( L, delta.LengthSqr() );

    return 1;
}

static int Vector_Dot( lua_State *L )
{
    lua_pushnumber( L, luaL_checkvector( L, 1 ).Dot( luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int Vector_Init( lua_State *L )
{
    luaL_checkvector( L, 1 ).Init( luaL_optnumber( L, 2, 0.0f ), luaL_optnumber( L, 3, 0.0f ), luaL_optnumber( L, 4, 0.0f ) );
    return 0;
}

static int Vector_Invalidate( lua_State *L )
{
    luaL_checkvector( L, 1 ).Invalidate();
    return 0;
}

static int Vector_IsLengthGreaterThan( lua_State *L )
{
    lua_pushboolean( L, luaL_checkvector( L, 1 ).IsLengthGreaterThan( luaL_checknumber( L, 2 ) ) );
    return 1;
}

static int Vector_IsLengthLessThan( lua_State *L )
{
    lua_pushboolean( L, luaL_checkvector( L, 1 ).IsLengthLessThan( luaL_checknumber( L, 2 ) ) );
    return 1;
}

static int Vector_IsValid( lua_State *L )
{
    lua_pushboolean( L, luaL_checkvector( L, 1 ).IsValid() );
    return 1;
}

static int Vector_Length( lua_State *L )
{
    lua_pushnumber( L, luaL_checkvector( L, 1 ).Length() );
    return 1;
}

static int Vector_Length2D( lua_State *L )
{
    lua_pushnumber( L, luaL_checkvector( L, 1 ).Length2D() );
    return 1;
}

static int Vector_Length2DSqr( lua_State *L )
{
    lua_pushnumber( L, luaL_checkvector( L, 1 ).Length2DSqr() );
    return 1;
}

static int Vector_LengthSqr( lua_State *L )
{
    lua_pushnumber( L, luaL_checkvector( L, 1 ).LengthSqr() );
    return 1;
}

static int Vector_Max( lua_State *L )
{
    lua_pushvector( L, luaL_checkvector( L, 1 ).Max( luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int Vector_Min( lua_State *L )
{
    lua_pushvector( L, luaL_checkvector( L, 1 ).Min( luaL_checkvector( L, 2 ) ) );
    return 1;
}

static int Vector_MulAdd( lua_State *L )
{
    luaL_checkvector( L, 1 ).MulAdd( luaL_checkvector( L, 2 ), luaL_checkvector( L, 3 ), luaL_checknumber( L, 4 ) );
    return 0;
}

static int Vector_Negate( lua_State *L )
{
    luaL_checkvector( L, 1 ).Negate();
    return 0;
}

static int Vector_NormalizeInPlace( lua_State *L )
{
    lua_pushnumber( L, luaL_checkvector( L, 1 ).NormalizeInPlace() );
    return 1;
}

static int Vector_Random( lua_State *L )
{
    luaL_checkvector( L, 1 ).Random( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
    return 0;
}

static int Vector_WithinAABox( lua_State *L )
{
    lua_pushboolean( L, luaL_checkvector( L, 1 ).WithinAABox( luaL_checkvector( L, 2 ), luaL_checkvector( L, 3 ) ) );
    return 1;
}

static int Vector_Zero( lua_State *L )
{
    luaL_checkvector( L, 1 ).Zero();
    return 0;
}

static int Vector___index( lua_State *L )
{
    Vector vector = luaL_checkvector( L, 1 );
    const char *field = luaL_checkstring( L, 2 );

    if ( strcmp( field, "x" ) == 0 )
        lua_pushnumber( L, vector.x );
    else if ( strcmp( field, "y" ) == 0 )
        lua_pushnumber( L, vector.y );
    else if ( strcmp( field, "z" ) == 0 )
        lua_pushnumber( L, vector.z );
    else
    {
        if ( lua_getmetatable( L, 1 ) )
        {
            LUA_METATABLE_INDEX_CHECK_TABLE( L );
        }
        else
        {
            lua_pushnil( L );
        }
    }

    return 1;
}

static int Vector___newindex( lua_State *L )
{
    const char *field = luaL_checkstring( L, 2 );
    if ( strcmp( field, "x" ) == 0 )
        luaL_checkvector( L, 1 ).x = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "y" ) == 0 )
        luaL_checkvector( L, 1 ).y = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "z" ) == 0 )
        luaL_checkvector( L, 1 ).z = ( vec_t )luaL_checknumber( L, 3 );
    return 0;
}

static int Vector___tostring( lua_State *L )
{
    lua_pushfstring( L, "Vector: %s", VecToString( luaL_checkvector( L, 1 ) ) );
    return 1;
}

static int Vector___eq( lua_State *L )
{
    lua_pushboolean( L, luaL_checkvector( L, 1 ) == luaL_checkvector( L, 2 ) );
    return 1;
}

static int Vector___add( lua_State *L )
{
    lua_pushvector( L, luaL_checkvector( L, 1 ) + luaL_checkvector( L, 2 ) );
    return 1;
}

static int Vector___sub( lua_State *L )
{
    lua_pushvector( L, luaL_checkvector( L, 1 ) - luaL_checkvector( L, 2 ) );
    return 1;
}

static int Vector___mul( lua_State *L )
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

static int Vector___div( lua_State *L )
{
    lua_pushvector( L, luaL_checkvector( L, 1 ) / luaL_checknumber( L, 2 ) );
    return 1;
}

static int Vector___unm( lua_State *L )
{
    lua_pushvector( L, -luaL_checkvector( L, 1 ) );
    return 1;
}

static const luaL_Reg Vectormeta[] = {
    { "Angle", Vector_Angle },
    { "AngleWithUp", Vector_AngleWithUp },
    { "Cross", Vector_Cross },
    { "DistanceTo", Vector_DistTo },
    { "DistanceToAsSqr", Vector_DistToSqr },
    { "DistanceTo2D", Vector_DistanceTo2D },
    { "DistanceTo2DSqr", Vector_DistanceTo2DSqr },
    { "Dot", Vector_Dot },
    { "Init", Vector_Init },
    { "Invalidate", Vector_Invalidate },
    { "IsLengthGreaterThan", Vector_IsLengthGreaterThan },
    { "IsLengthLessThan", Vector_IsLengthLessThan },
    { "IsValid", Vector_IsValid },
    { "Length", Vector_Length },
    { "Length2D", Vector_Length2D },
    { "Length2DSqr", Vector_Length2DSqr },
    { "LengthSqr", Vector_LengthSqr },
    { "Max", Vector_Max },
    { "Min", Vector_Min },
    { "MulAdd", Vector_MulAdd },
    { "Negate", Vector_Negate },
    { "NormalizeInPlace", Vector_NormalizeInPlace },
    { "Random", Vector_Random },
    { "WithinAABox", Vector_WithinAABox },
    { "Zero", Vector_Zero },
    { "__index", Vector___index },
    { "__newindex", Vector___newindex },
    { "__tostring", Vector___tostring },
    { "__eq", Vector___eq },
    { "__add", Vector___add },
    { "__sub", Vector___sub },
    { "__mul", Vector___mul },
    { "__div", Vector___div },
    { "__unm", Vector___unm },
    { NULL, NULL } };

static int luasrc_Vector( lua_State *L )
{
    lua_pushvector( L, Vector( ( vec_t )luaL_optnumber( L, 1, 0.0f ), ( vec_t )luaL_optnumber( L, 2, 0.0f ), ( vec_t )luaL_optnumber( L, 3, 0.0f ) ) );
    return 1;
}

static const luaL_Reg Vector_funcs[] = {
    { LUA_VECTORLIBNAME, luasrc_Vector },
    { NULL, NULL } };

/*
** Open Vector object
*/
LUALIB_API int luaopen_Vector( lua_State *L )
{
    luaL_newmetatable( L, LUA_VECTORLIBNAME );
    luaL_register( L, NULL, Vectormeta );
    lua_pushstring( L, "Vector" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Vector" */
    luaL_register( L, LUA_GNAME, Vector_funcs );
    lua_pop( L, 1 );
    Vector origin = vec3_origin;
    lua_pushvector( L, origin );
    lua_setglobal( L, "vec3_origin" ); /* set global vec3_origin */
    Vector invalid = vec3_invalid;
    lua_pushvector( L, invalid );
    lua_setglobal( L, "vec3_invalid" ); /* set global vec3_invalid */
    return 1;
}

static int QAngle_Forward( lua_State *L )
{
    QAngle angle = luaL_checkangle( L, 1 );
    Vector forward;

    AngleVectors( angle, &forward );
    lua_pushvector( L, forward );

    return 1;
}

static int QAngle_Right( lua_State *L )
{
    QAngle angle = luaL_checkangle( L, 1 );
    Vector right;

    AngleVectors( angle, NULL, &right, NULL );
    lua_pushvector( L, right );

    return 1;
}

static int QAngle_Up( lua_State *L )
{
    QAngle angle = luaL_checkangle( L, 1 );
    Vector up;

    AngleVectors( angle, NULL, NULL, &up );
    lua_pushvector( L, up );

    return 1;
}

static int QAngle_IsEqualTol( lua_State *L )
{
    QAngle angle = luaL_checkangle( L, 1 );
    QAngle other = luaL_checkangle( L, 2 );
    float tolerance = luaL_checknumber( L, 3 );

    lua_pushboolean( L, QAnglesAreEqual( angle, other, tolerance ) );

    return 1;
}

static int QAngle_Init( lua_State *L )
{
    luaL_checkangle( L, 1 ).Init( luaL_optnumber( L, 1, 0.0f ), luaL_optnumber( L, 1, 0.0f ), luaL_optnumber( L, 1, 0.0f ) );
    return 0;
}

static int QAngle_Invalidate( lua_State *L )
{
    luaL_checkangle( L, 1 ).Invalidate();
    return 0;
}

static int QAngle_IsValid( lua_State *L )
{
    lua_pushboolean( L, luaL_checkangle( L, 1 ).IsValid() );
    return 1;
}

static int QAngle_Length( lua_State *L )
{
    lua_pushnumber( L, luaL_checkangle( L, 1 ).Length() );
    return 1;
}

static int QAngle_LengthSqr( lua_State *L )
{
    lua_pushnumber( L, luaL_checkangle( L, 1 ).LengthSqr() );
    return 1;
}

static int QAngle___index( lua_State *L )
{
    QAngle angle = luaL_checkangle( L, 1 );
    const char *field = luaL_checkstring( L, 2 );

    if ( strcmp( field, "x" ) == 0 || strcmp( field, "p" ) == 0 )
        lua_pushnumber( L, angle.x );
    else if ( strcmp( field, "y" ) == 0 || strcmp( field, "y" ) == 0 )
        lua_pushnumber( L, angle.y );
    else if ( strcmp( field, "z" ) == 0 || strcmp( field, "r" ) == 0 )
        lua_pushnumber( L, angle.z );
    else
    {
        if ( lua_getmetatable( L, 1 ) )
        {
            LUA_METATABLE_INDEX_CHECK_TABLE( L );
        }
        else
        {
            lua_pushnil( L );
        }
    }

    return 1;
}

static int QAngle___newindex( lua_State *L )
{
    const char *field = luaL_checkstring( L, 2 );
    if ( strcmp( field, "x" ) == 0 )
        luaL_checkangle( L, 1 ).x = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "y" ) == 0 )
        luaL_checkangle( L, 1 ).y = ( vec_t )luaL_checknumber( L, 3 );
    else if ( strcmp( field, "z" ) == 0 )
        luaL_checkangle( L, 1 ).z = ( vec_t )luaL_checknumber( L, 3 );
    return 0;
}

static int QAngle___tostring( lua_State *L )
{
    lua_pushfstring( L, VecToString( luaL_checkangle( L, 1 ) ) );
    return 1;
}

static int QAngle___eq( lua_State *L )
{
    lua_pushboolean( L, luaL_checkangle( L, 1 ) == luaL_checkangle( L, 2 ) );
    return 1;
}

static int QAngle___add( lua_State *L )
{
    lua_pushangle( L, luaL_checkangle( L, 1 ) + luaL_checkangle( L, 2 ) );
    return 1;
}

static int QAngle___sub( lua_State *L )
{
    lua_pushangle( L, luaL_checkangle( L, 1 ) - luaL_checkangle( L, 2 ) );
    return 1;
}

static int QAngle___mul( lua_State *L )
{
    lua_pushangle( L, luaL_checkangle( L, 1 ) * luaL_checknumber( L, 2 ) );
    return 1;
}

static int QAngle___div( lua_State *L )
{
    lua_pushangle( L, luaL_checkangle( L, 1 ) / luaL_checknumber( L, 2 ) );
    return 1;
}

static int QAngle___unm( lua_State *L )
{
    lua_pushangle( L, -luaL_checkangle( L, 1 ) );
    return 1;
}

static const luaL_Reg QAnglemeta[] = {
    { "Forward", QAngle_Forward },
    { "Right", QAngle_Right },
    { "Up", QAngle_Up },
    { "IsEqualTol", QAngle_IsEqualTol },
    { "Init", QAngle_Init },
    { "Invalidate", QAngle_Invalidate },
    { "IsValid", QAngle_IsValid },
    { "Length", QAngle_Length },
    { "LengthSqr", QAngle_LengthSqr },
    { "__index", QAngle___index },
    { "__newindex", QAngle___newindex },
    { "__tostring", QAngle___tostring },
    { "__eq", QAngle___eq },
    { "__add", QAngle___add },
    { "__sub", QAngle___sub },
    { "__mul", QAngle___mul },
    { "__div", QAngle___div },
    { "__unm", QAngle___unm },
    { NULL, NULL } };

static int luasrc_QAngle( lua_State *L )
{
    lua_pushangle( L, QAngle( ( vec_t )luaL_optnumber( L, 1, 0.0f ), ( vec_t )luaL_optnumber( L, 2, 0.0f ), ( vec_t )luaL_optnumber( L, 3, 0.0f ) ) );
    return 1;
}

static const luaL_Reg QAngle_funcs[] = {
    { LUA_QANGLELIBNAME, luasrc_QAngle },
    { NULL, NULL } };

/*
** Open QAngle object
*/
LUALIB_API int luaopen_QAngle( lua_State *L )
{
    luaL_newmetatable( L, LUA_QANGLELIBNAME );
    luaL_register( L, NULL, QAnglemeta );
    lua_pushstring( L, "Angle" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Angle" */
    luaL_register( L, LUA_GNAME, QAngle_funcs );
    lua_pop( L, 1 );
    QAngle v = vec3_angle;
    lua_pushangle( L, v );
    lua_setglobal( L, "vec3_angle" ); /* set global vec3_angle */
    return 1;
}
