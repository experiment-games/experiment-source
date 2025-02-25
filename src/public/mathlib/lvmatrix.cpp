#include "cbase.h"
#include <string.h>
#include "mathlib/vmatrix.h"
#include "mathlib/vector.h"
#include "mathlib/mathlib.h"
#include "fmtstr.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "lvmatrix.h"
#include "lvector.h"
#include "lmathlib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_VMatrix &lua_tovmatrix( lua_State *L, int idx )
{
    lua_VMatrix *matrix = ( lua_VMatrix * )luaL_checkudata( L, idx, LUA_VMATRIXLIBNAME );
    return *matrix;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushvmatrix( lua_State *L, lua_VMatrix &matrix )
{
    lua_VMatrix *pMat = ( lua_VMatrix * )lua_newuserdata( L, sizeof( lua_VMatrix ) );
    *pMat = matrix;
    LUA_SAFE_SET_METATABLE( L, LUA_VMATRIXLIBNAME );
}

LUALIB_API lua_VMatrix &luaL_checkvmatrix( lua_State *L, int narg )
{
    lua_VMatrix *d = ( lua_VMatrix * )luaL_checkudata( L, narg, LUA_VMATRIXLIBNAME );
    return *d;
}

LUA_REGISTRATION_INIT( Matrix )

LUA_BINDING_BEGIN( Matrix, ApplyRotation, "class", "Applies a rotation to a vector." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_Vector vec = LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" );
    lua_pushvector( L, matrix.ApplyRotation( vec ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The rotated vector." )

LUA_BINDING_BEGIN( Matrix, As3x4, "class", "Converts a matrix to a 3x4 matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushmatrix( L, const_cast< matrix3x4_t & >( matrix.As3x4() ) );

    return 1;
}
LUA_BINDING_END( "Matrix", "The converted 3x4 matrix." )

// Experiment; disabled since we don't want to deal with pointers in Lua.
// LUA_BINDING_BEGIN( Matrix, Base, "class", "Gets a pointer to the matrix." )
//{
//    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
//    lua_pushinteger( L, *matrix.Base() );
//
//    return 1;
//}
// LUA_BINDING_END( "integer", "A pointer to the matrix." )

LUA_BINDING_BEGIN( Matrix, CopyFrom3x4, "class", "Copies a 3x4 matrix to a matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.CopyFrom3x4( LUA_BINDING_ARGUMENT( luaL_checkmatrix, 2, "Matrix3x4" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, GetBasisVectors, "class", "Gets the basis vectors of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_Vector forward, left, up;
    matrix.GetBasisVectors( forward, left, up );
    lua_pushvector( L, forward );
    lua_pushvector( L, left );
    lua_pushvector( L, up );

    return 3;
}
LUA_BINDING_END( "Vector", "The forward vector.", "Vector", "The left vector.", "Vector", "The up vector." )

LUA_BINDING_BEGIN( Matrix, GetForward, "class", "Gets the forward vector of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvector( L, matrix.GetForward() );

    return 1;
}
LUA_BINDING_END( "Vector", "The forward vector." )

LUA_BINDING_BEGIN( Matrix, GetLeft, "class", "Gets the left vector of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvector( L, matrix.GetLeft() );

    return 1;
}
LUA_BINDING_END( "Vector", "The left vector." )

LUA_BINDING_BEGIN( Matrix, GetScale, "class", "Gets the scale of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvector( L, matrix.GetScale() );

    return 1;
}
LUA_BINDING_END( "Vector", "The scale." )

LUA_BINDING_BEGIN( Matrix, GetTranslation, "class", "Gets the translation of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvector( L, matrix.GetTranslation() );

    return 1;
}
LUA_BINDING_END( "Vector", "The translation." )

LUA_BINDING_BEGIN( Matrix, GetUp, "class", "Gets the up vector of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvector( L, matrix.GetUp() );

    return 1;
}
LUA_BINDING_END( "Vector", "The up vector." )

LUA_BINDING_BEGIN( Matrix, Identity, "class", "Sets the matrix to the identity matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.Identity();

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, Init, "class", "Initializes the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    if ( lua_gettop( L ) <= 2 )
        matrix.Init( LUA_BINDING_ARGUMENT( luaL_checkmatrix, 2, "Matrix3x4" ) );
    else
        matrix.Init( LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "m11" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "m12" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "m13" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "m14" ),

                    LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "m21" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "m22" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 8, "m23" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 9, "m24" ),

                    LUA_BINDING_ARGUMENT( luaL_checknumber, 10, "m31" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 11, "m32" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 12, "m33" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 13, "m34" ),

                    LUA_BINDING_ARGUMENT( luaL_checknumber, 14, "m41" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 15, "m42" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 16, "m43" ),
                    LUA_BINDING_ARGUMENT( luaL_checknumber, 17, "m44" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, InverseGeneral, "class", "Inverts the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushboolean( L, matrix.InverseGeneral( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ) ) );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the matrix was successfully inverted." )

LUA_BINDING_BEGIN( Matrix, InverseTranslation, "class", "Does a fast inverse, assuming the matrix only contains translation and rotation." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvmatrix( L, matrix.InverseTR() );

    return 1;
}
LUA_BINDING_END( "Matrix", "The inverted matrix." )

LUA_BINDING_BEGIN( Matrix, IsIdentity, "class", "Checks if the matrix is the identity matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushboolean( L, matrix.IsIdentity() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the matrix is the identity matrix." )

LUA_BINDING_BEGIN( Matrix, IsRotationMatrix, "class", "Checks if the matrix is a rotation matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushboolean( L, matrix.IsRotationMatrix() );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the matrix is a rotation matrix." )

LUA_BINDING_BEGIN( Matrix, MatrixMul, "class", "Multiplies two matrices." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.MatrixMul( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ),
                    LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 3, "Matrix" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, NormalizeBasisVectors, "class", "Normalizes the basis vectors of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvmatrix( L, matrix.NormalizeBasisVectors() );

    return 1;
}
LUA_BINDING_END( "Matrix", "The normalized matrix." )

LUA_BINDING_BEGIN( Matrix, PostTranslate, "class", "Translates the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.PostTranslate( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, PreTranslate, "class", "Translates the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.PreTranslate( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, Scale, "class", "Scales the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvmatrix( L, matrix.Scale( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) ) );

    return 1;
}
LUA_BINDING_END( "Matrix", "The scaled matrix." )

LUA_BINDING_BEGIN( Matrix, Set3x4, "class", "Sets the matrix to a 3x4 matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.Set3x4( LUA_BINDING_ARGUMENT( luaL_checkmatrix, 2, "Matrix3x4" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, SetBasisVectors, "class", "Sets the basis vectors of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.SetBasisVectors( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ),
                            LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "vector" ),
                            LUA_BINDING_ARGUMENT( luaL_checkvector, 4, "vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, SetForward, "class", "Sets the forward vector of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.SetForward( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, SetLeft, "class", "Sets the left vector of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.SetLeft( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, SetTranslation, "class", "Sets the translation of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.SetTranslation( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, SetUp, "class", "Sets the up vector of the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.SetUp( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, SetupMatrixForOriginAndAngles, "class", "Sets the matrix to a matrix with the given origin and angles." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.SetupMatrixOrgAngles( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ),
                                LUA_BINDING_ARGUMENT( luaL_checkangle, 3, "angle" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, Transpose, "class", "Transposes the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvmatrix( L, matrix.Transpose() );

    return 1;
}
LUA_BINDING_END( "Matrix", "The transposed matrix." )

LUA_BINDING_BEGIN( Matrix, Transpose3x3, "class", "Transpose upper-left 3x3." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvmatrix( L, matrix.Transpose3x3() );

    return 1;
}
LUA_BINDING_END( "Matrix", "The transposed matrix." )

LUA_BINDING_BEGIN( Matrix, MultiplyByVector, "class", "Multiplies a vector by the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    matrix.V3Mul( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ),
                LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "vector" ) );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, MultiplyByVector3x3, "class", "Multiplies a vector by the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvector( L, matrix.VMul3x3( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The multiplied vector." )

LUA_BINDING_BEGIN( Matrix, MultiplyByVectorTranspose3x3, "class", "Apply the inverse (transposed) rotation (only works on pure rotation matrix)." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvector( L, matrix.VMul3x3Transpose( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The multiplied vector." )

LUA_BINDING_BEGIN( Matrix, MultiplyByVector4x3, "class", "Multiplies a vector by the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvector( L, matrix.VMul4x3( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The multiplied vector." )

LUA_BINDING_BEGIN( Matrix, MultiplyByVectorTranspose4x3, "class", "Multiplies a vector by the matrix." )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    lua_pushvector( L, matrix.VMul4x3Transpose( LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) ) );

    return 1;
}
LUA_BINDING_END( "Vector", "The multiplied vector." )

LUA_BINDING_BEGIN( Matrix, __index, "class", "Metatable called when a non-existant field is index" )
{
    lua_VMatrix matrix = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
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

LUA_BINDING_BEGIN( Matrix, __newindex, "class", "Metatable called when a non-existant field is set" )
{
    // TODO
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrix, __tostring, "class", "Metatable called when the object is to be converted to a string" )
{
    lua_pushfstring( L, "VMatrix: %s", VMatToString( LUA_BINDING_ARGUMENT( lua_tovmatrix, 1, "Matrix" ) ) );

    return 1;
}
LUA_BINDING_END( "string", "The string representation of the object." )

LUA_BINDING_BEGIN( Matrix, __eq, "class", "Metatable called when the object is to be compared" )
{
    lua_pushboolean( L, LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "matrix" ) == LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "other" ) );

    return 1;
}
LUA_BINDING_END( "boolean", "Whether the objects are equal." )

LUA_BINDING_BEGIN( Matrix, __add, "class", "Metatable called when the object is to be added" )
{
    lua_pushvmatrix( L, LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ) + LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ) );

    return 1;
}
LUA_BINDING_END( "Matrix", "The sum of the objects." )

LUA_BINDING_BEGIN( Matrix, __sub, "class", "Metatable called when the object is to be subtracted" )
{
    lua_pushvmatrix( L, LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ) - LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ) );

    return 1;
}
LUA_BINDING_END( "Matrix", "The difference of the objects." )

LUA_BINDING_BEGIN( Matrix, __mul, "class", "Metatable called when the object is to be multiplied" )
{
    lua_pushvmatrix( L, LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ) * LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ) );

    return 1;
}
LUA_BINDING_END( "Matrix", "The product of the objects." )

LUA_BINDING_BEGIN( Matrix, __unm, "class", "Metatable called when the object is to be negated" )
{
    lua_pushvmatrix( L, -LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ) );

    return 1;
}
LUA_BINDING_END( "Matrix", "The negated object." )

/*
** Library functions
*/
LUA_REGISTRATION_INIT( Matrixes );

LUA_BINDING_BEGIN( Matrixes, Create, "library", "Creates a new matrix." )
{
    if ( lua_gettop( L ) < 1 )
    {
        VMatrix matrix = VMatrix();
        memset( &matrix, 0, sizeof( VMatrix ) );
        lua_pushvmatrix( L, matrix );
    }
    else if ( lua_gettop( L ) < 3 )
    {
        VMatrix matrix = VMatrix( LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkmatrix, 1, "Matrix3x4" ) );
        lua_pushvmatrix( L, matrix );
    }
    else if ( lua_gettop( L ) < 16 )
    {
        VMatrix matrix = VMatrix(
            LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkvector, 1, "xAxis" ),
            LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkvector, 2, "yAxis" ),
            LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkvector, 3, "zAxis" ) );
        lua_pushvmatrix( L, matrix );
    }
    else
    {
        VMatrix matrix =
            VMatrix(
                LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "m11" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "m12" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "m13" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "m14" ),

                LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "m21" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "m22" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "m23" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 8, "m24" ),

                LUA_BINDING_ARGUMENT( luaL_checknumber, 9, "m31" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 10, "m32" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 11, "m33" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 12, "m34" ),

                LUA_BINDING_ARGUMENT( luaL_checknumber, 13, "m41" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 14, "m42" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 15, "m43" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 16, "m44" ) );

        lua_pushvmatrix( L, matrix );
    }

    return 1;
}
LUA_BINDING_END( "Matrix", "The created matrix." )

LUA_BINDING_BEGIN( Matrixes, Create3x4, "library", "Creates a new 3x4 matrix." )
{
    if ( lua_gettop( L ) < 4 )
    {
        matrix3x4_t matrix;
        memset( &matrix, 0, sizeof( matrix3x4_t ) );
        lua_pushmatrix( L, matrix );
    }
    else if ( lua_gettop( L ) < 12 )
    {
        matrix3x4_t matrix = matrix3x4_t(
            LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkvector, 1, "xAxis" ),
            LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkvector, 2, "yAxis" ),
            LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkvector, 3, "zAxis" ),
            LUA_BINDING_ARGUMENT_NILLABLE( luaL_checkvector, 4, "origin" ) );
        lua_pushmatrix( L, matrix );
    }
    else
    {
        matrix3x4_t matrix = matrix3x4_t(
            LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "m11" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "m12" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "m13" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "m14" ),

            LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "m21" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "m22" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "m23" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 8, "m24" ),

            LUA_BINDING_ARGUMENT( luaL_checknumber, 9, "m31" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 10, "m32" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 11, "m33" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 12, "m34" ) );

        lua_pushmatrix( L, matrix );
    }

    return 1;
}
LUA_BINDING_END( "Matrix3x4", "The created 3x4 matrix." )

LUA_BINDING_BEGIN( Matrixes, CreateIdentityMatrix, "library", "Sets the matrix to the identity matrix." )
{
    lua_pushvmatrix( L, SetupMatrixIdentity() );
    return 1;
}
LUA_BINDING_END( "Matrix", "The created identity matrix." )

LUA_BINDING_BEGIN( Matrixes, CreateScaleMatrix, "library", "Sets the matrix to a scale matrix." )
{
    lua_pushvmatrix( L, SetupMatrixScale( LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" ) ) );
    return 1;
}
LUA_BINDING_END( "Matrix", "The created scaled matrix." )

LUA_BINDING_BEGIN( Matrixes, CreateTranslationMatrix, "library", "Sets the matrix to a translation matrix." )
{
    lua_pushvmatrix( L, SetupMatrixTranslation( LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" ) ) );
    return 1;
}
LUA_BINDING_END( "Matrix", "The translation matrix." )

LUA_BINDING_BEGIN( Matrixes, CreateMatrixWithAxisAndRotation, "library", "Sets the matrix to a rotation matrix." )
{
    lua_pushvmatrix(
        L, SetupMatrixAxisRot( LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "number" ) ) );
    return 1;
}
LUA_BINDING_END( "Matrix", "The rotation matrix." )

LUA_BINDING_BEGIN( Matrixes, CreateMatrixWithAngles, "library", "Sets the matrix to a rotation matrix." )
{
    lua_pushvmatrix( L, SetupMatrixAngles( LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" ) ) );
    return 1;
}
LUA_BINDING_END( "Matrix", "The rotation matrix." )

LUA_BINDING_BEGIN( Matrixes, CreateMatrixWithOriginAndAngles, "library", "Sets the matrix to a matrix with the given origin and angles." )
{
    lua_pushvmatrix(
        L, SetupMatrixOrgAngles( LUA_BINDING_ARGUMENT( luaL_checkvector, 1, "vector" ), LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" ) ) );
    return 1;
}
LUA_BINDING_END( "Matrix", "The matrix." )

// Experiment; Disabled in favor of __tostring
// LUA_BINDING_BEGIN( Matrixes, VMatrixToString, "library", "Converts the matrix to a string." )
//{
//    lua_pushstring( L, VMatToString( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ) ) );
//    return 1;
//}
// LUA_BINDING_END( "string", "The string representation of the matrix." )

LUA_BINDING_BEGIN( Matrixes, MatrixSetIdentity, "library", "Sets the matrix to the identity matrix." )
{
    MatrixSetIdentity( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ) );
    return 0;
}
LUA_BINDING_END()

// Experiment; Disabled in favor of :Transpose
// LUA_BINDING_BEGIN( Matrixes, MatrixTranspose, "library", "Transposes the matrix." )
//{
//    MatrixTranspose( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ) );
//    return 0;
//}
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixCopy, "library", "Copies the matrix." )
{
    VMatrix destination;
    MatrixCopy( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), destination );
    return 1;
}
LUA_BINDING_END( "Matrix", "The copied matrix." )

// Experiment; Disabled due to dest reference being possibly confusing in Lua
// LUA_BINDING_BEGIN( Matrixes, MatrixMultiply, "library", "Multiplies two matrices." )
//{
//    MatrixMultiply( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 3, "Matrix" ) );
//    return 0;
//}
// LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixGetColumn, "library", "Gets a column of the matrix." )
{
    Vector vec;
    MatrixGetColumn( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "column" ), &vec );
    lua_pushvector( L, vec );
    return 1;
}
LUA_BINDING_END( "Vector", "The column vector." )

LUA_BINDING_BEGIN( Matrixes, MatrixSetColumn, "library", "Sets a column of the matrix." )
{
    MatrixSetColumn( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "column" ), LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "vector" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixGetRow, "library", "Gets a row of the matrix." )
{
    Vector vec;
    MatrixGetRow( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "row" ), &vec );
    lua_pushvector( L, vec );
    return 1;
}
LUA_BINDING_END( "Vector", "The row vector." )

LUA_BINDING_BEGIN( Matrixes, MatrixSetRow, "library", "Sets a row of the matrix." )
{
    MatrixSetRow( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "row" ), LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "vector" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, Vector3dMultiply, "library", "Multiplies a vector by the matrix." )
{
    Vector vec;
    Vector3DMultiply( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ), vec );
    lua_pushvector( L, vec );
    return 1;
}
LUA_BINDING_END( "Vector", "The multiplied vector." )

LUA_BINDING_BEGIN( Matrixes, Vector3dMultiplyPositionProjective, "library", "Multiplies a vector by the matrix." )
{
    Vector vec;
    Vector3DMultiplyPositionProjective( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ), vec );
    lua_pushvector( L, vec );
    return 1;
}
LUA_BINDING_END( "Vector", "The multiplied vector." )

LUA_BINDING_BEGIN( Matrixes, Vector3dMultiplyProjective, "library", "Multiplies a vector by the matrix." )
{
    Vector vec;
    Vector3DMultiplyProjective( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ), vec );
    lua_pushvector( L, vec );
    return 1;
}
LUA_BINDING_END( "Vector", "The multiplied vector." )

LUA_BINDING_BEGIN( Matrixes, Vector3dMultiplyTranspose, "library", "Multiplies a vector by the matrix." )
{
    Vector vec;
    Vector3DMultiplyTranspose( LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ), LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ), vec );
    lua_pushvector( L, vec );
    return 1;
}
LUA_BINDING_END( "Vector", "The multiplied vector." )

LUA_BINDING_BEGIN( Matrixes, MatrixBuildTranslation, "library", "Builds a translation matrix." )
{
    VMatrix destination = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );

    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
        default:
            MatrixBuildTranslation(
                destination,
                LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "z" ) );
            break;
        case LUA_TUSERDATA:
            MatrixBuildTranslation(
                destination,
                LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "translation" ) );
            break;
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixTranslate, "library", "Translates the matrix." )
{
    VMatrix destination = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    MatrixTranslate( destination, LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixBuildRotationAboutAxis, "library", "Builds a rotation matrix." )
{
    VMatrix destination = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    MatrixBuildRotationAboutAxis(
        destination,
        LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "axisOfRotation" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "degrees" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixBuildRotateZ, "library", "Builds a rotation matrix." )
{
    VMatrix destination = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    MatrixBuildRotateZ(
        destination,
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "degrees" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixRotate, "library", "Rotates the matrix." )
{
    VMatrix destination = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    MatrixRotate(
        destination,
        LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "axisOfRotation" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "degrees" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixBuildRotation, "library", "Builds a rotation matrix." )
{
    VMatrix destination = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    MatrixBuildRotation(
        destination,
        LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "vector" ),
        LUA_BINDING_ARGUMENT( luaL_checkvector, 3, "vector" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixBuildScale, "library", "Builds a scale matrix." )
{
    VMatrix destination = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );

    switch ( lua_type( L, 2 ) )
    {
        case LUA_TNUMBER:
        default:
            MatrixBuildScale(
                destination,
                LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "x" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "y" ),
                LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "z" ) );
            break;
        case LUA_TUSERDATA:
            MatrixBuildScale(
                destination,
                LUA_BINDING_ARGUMENT( luaL_checkvector, 2, "scale" ) );
            break;
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixBuildPerspective, "library", "Builds a perspective matrix." )
{
    VMatrix destination = LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" );
    MatrixBuildPerspective(
        destination,
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "fovX" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "fovY" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "zNear" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "zFar" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, CalculateAabbFromProjectionMatrix, "library", "Calculates an AABB from a projection matrix." )
{
    Vector vecMin, vecMax;
    CalculateAABBFromProjectionMatrix(
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ),
        &vecMin,
        &vecMax );
    lua_pushvector( L, vecMin );
    lua_pushvector( L, vecMax );
    return 2;
}
LUA_BINDING_END( "Vector", "The minimum vector.", "Vector", "The maximum vector." )

LUA_BINDING_BEGIN( Matrixes, CalculateSphereFromProjectionMatrix, "library", "Calculates a sphere from a projection matrix." )
{
    Vector vecCenter;
    float flRadius;
    CalculateSphereFromProjectionMatrix(
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ),
        &vecCenter,
        &flRadius );
    lua_pushvector( L, vecCenter );
    lua_pushnumber( L, flRadius );
    return 2;
}
LUA_BINDING_END( "Vector", "The center vector.", "number", "The radius." )

LUA_BINDING_BEGIN( Matrixes, MatrixFromAngles, "library", "Sets the matrix to a rotation matrix." )
{
    MatrixFromAngles(
        LUA_BINDING_ARGUMENT( luaL_checkangle, 1, "angle" ),
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixToAngles, "library", "Sets the matrix to a rotation matrix." )
{
    MatrixToAngles(
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ),
        LUA_BINDING_ARGUMENT( luaL_checkangle, 2, "angle" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixInverseTranslation, "library", "Inverts the matrix." )
{
    MatrixInverseTR(
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ),
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixInverseGeneral, "library", "Inverts the matrix." )
{
    MatrixInverseGeneral(
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ),
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Matrixes, MatrixInverseTranspose, "library", "Inverts the matrix." )
{
    MatrixInverseTranspose(
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 1, "Matrix" ),
        LUA_BINDING_ARGUMENT( luaL_checkvmatrix, 2, "Matrix" ) );
    return 0;
}
LUA_BINDING_END()

/*
** Open VMatrix object
*/
LUALIB_API int luaopen_VMatrix( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_VMATRIXLIBNAME );

    LUA_REGISTRATION_COMMIT( Matrix );

    lua_pushstring( L, LUA_VMATRIXLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Matrix" */
    lua_pop( L, 2 );

    LUA_REGISTRATION_COMMIT_LIBRARY( Matrixes );

    return 1;
}
