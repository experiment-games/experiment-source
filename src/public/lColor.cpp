//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "Color.h"
#include "fmtstr.h"
#include "lua.hpp"
#include "luasrclib.h"
#include "lColor.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
** access functions (stack -> C)
*/

LUA_API lua_Color &lua_tocolor( lua_State *L, int idx )
{
    lua_Color *clr = ( lua_Color * )luaL_checkudata( L, idx, "Color" );
    return *clr;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushcolor( lua_State *L, lua_Color &clr )
{
    lua_Color *pColor = ( lua_Color * )lua_newuserdata( L, sizeof( lua_Color ) );
    *pColor = clr;
    luaL_getmetatable( L, "Color" );
    lua_setmetatable( L, -2 );
}

LUA_API void lua_pushcolor( lua_State *L, Color &clr )
{
    lua_Color *pColor = ( lua_Color * )lua_newuserdata( L, sizeof( lua_Color ) );
    *pColor = lua_Color( clr.r(), clr.g(), clr.b(), clr.a() );
    luaL_getmetatable( L, "Color" );
    lua_setmetatable( L, -2 );
}

LUALIB_API lua_Color &luaL_checkcolor( lua_State *L, int narg )
{
    lua_Color *d = ( lua_Color * )luaL_checkudata( L, narg, "Color" );
    return *d;
}

static int Color_a( lua_State *L )
{
    lua_pushinteger( L, luaL_checkcolor( L, 1 ).a() );
    return 1;
}

static int Color_b( lua_State *L )
{
    lua_pushinteger( L, luaL_checkcolor( L, 1 ).b() );
    return 1;
}

static int Color_g( lua_State *L )
{
    lua_pushinteger( L, luaL_checkcolor( L, 1 ).g() );
    return 1;
}

static int Color_GetColor( lua_State *L )
{
    int r, g, b, a;
    luaL_checkcolor( L, 1 ).GetColor( r, g, b, a );
    lua_pushinteger( L, r );
    lua_pushinteger( L, g );
    lua_pushinteger( L, b );
    lua_pushinteger( L, a );
    return 4;
}

static int Color_GetRawColor( lua_State *L )
{
    lua_pushinteger( L, luaL_checkcolor( L, 1 ).GetRawColor() );
    return 1;
}

static int Color_r( lua_State *L )
{
    lua_pushinteger( L, luaL_checkcolor( L, 1 ).r() );
    return 1;
}

static int Color_SetColor( lua_State *L )
{
    luaL_checkcolor( L, 1 ).SetColor( luaL_checkint( L, 1 ), luaL_checkint( L, 2 ), luaL_checkint( L, 3 ), luaL_optint( L, 4, 255 ) );
    return 0;
}

static int Color_SetRawColor( lua_State *L )
{
    luaL_checkcolor( L, 1 ).SetRawColor( luaL_checkint( L, 1 ) );
    return 0;
}

static int Color___tostring( lua_State *L )
{
    lua_Color color = luaL_checkcolor( L, 1 );
    lua_pushfstring( L, "Color: %s", static_cast< const char * >( CFmtStr( "(%i, %i, %i, %i)", color.r(), color.g(), color.b(), color.a() ) ) );
    return 1;
}

static int Color___eq( lua_State *L )
{
    lua_pushboolean( L, luaL_checkcolor( L, 1 ) == luaL_checkcolor( L, 2 ) );
    return 1;
}

// Returns the color's r, g, b, a values or fallsback to the Color metatable
static int Color___index( lua_State *L )
{
    lua_Color *color = &luaL_checkcolor( L, 1 );
    LUA_METATABLE_INDEX_CHECK( L, color );

    LUA_GET_REF_TABLE( L, color );
    LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, color );

    if ( lua_getmetatable( L, 1 ) )
    {
        LUA_METATABLE_INDEX_CHECK_TABLE( L );
    }

    lua_pushnil( L );
    return 1;
}

static int Color___newindex( lua_State *L )
{
    lua_Color *color = &luaL_checkcolor( L, 1 );
    LUA_METATABLE_INDEX_CHECK( L, color );

    const char *field = luaL_checkstring( L, 2 );

    LUA_GET_REF_TABLE( L, color );
    lua_pushvalue( L, 3 );
    lua_setfield( L, -2, field );
    lua_pop( L, 1 );

    return 0;
}

static const luaL_Reg Colormeta[] = {
    { "GetAlpha", Color_a },
    { "GetBlue", Color_b },
    { "GetColor", Color_GetColor },
    { "GetGreen", Color_g },
    { "GetRawColor", Color_GetRawColor },
    { "GetRed", Color_r },
    { "SetColor", Color_SetColor },
    { "SetRawColor", Color_SetRawColor },
    { "__tostring", Color___tostring },
    { "__eq", Color___eq },
    { "__index", Color___index },
    { "__newindex", Color___newindex },
    { NULL, NULL } };

static int luasrc_Color( lua_State *L )
{
    lua_Color clr = lua_Color( luaL_checkint( L, 1 ), luaL_checkint( L, 2 ), luaL_checkint( L, 3 ), luaL_optint( L, 4, 255 ) );
    lua_pushcolor( L, clr );
    return 1;
}

static int luasrc_RGBToHSV( lua_State *L )
{
    lua_Color clr = luaL_checkcolor( L, 1 );
    Vector hsv;
    RGBtoHSV( Vector( clr.r(), clr.g(), clr.b() ), hsv );
    lua_pushnumber( L, hsv.x );
    lua_pushnumber( L, hsv.y );
    lua_pushnumber( L, hsv.z );
    return 3;
}

static int luasrc_HSVToRGB( lua_State *L )
{
    Vector hsv = Vector( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
    Vector rgb;
    HSVtoRGB( hsv, rgb );
    lua_pushcolor( L, lua_Color( rgb.x, rgb.y, rgb.z, 255 ) );
    return 1;
}

// These Hue & HSL functions are taken from the shadersdk @ src\materialsystem\stdshaders\common_ps_fxc.h
float HueToRGB( float v1, float v2, float vH )
{
    float fResult = v1;

    vH = fmod( vH + 1.0f, 1.0f );

    if ( ( 6.0f * vH ) < 1.0f )
    {
        fResult = ( v1 + ( v2 - v1 ) * 6.0f * vH );
    }
    else if ( ( 2.0f * vH ) < 1.0f )
    {
        fResult = ( v2 );
    }
    else if ( ( 3.0f * vH ) < 2.0f )
    {
        fResult = ( v1 + ( v2 - v1 ) * ( ( 2.0f / 3.0f ) - vH ) * 6.0f );
    }

    return fResult;
}

static int luasrc_RGBToHSL( lua_State *L )
{
    lua_Color clr = luaL_checkcolor( L, 1 );
    float h, s;
    float flMax = max( clr.r(), max( clr.g(), clr.b() ) );
    float flMin = min( clr.r(), min( clr.g(), clr.b() ) );

    float l = ( flMax + flMin ) / 2.0f;

    if ( flMax == flMin )  // achromatic case
    {
        s = h = 0;
    }
    else  // chromatic case
    {
        // Next, calculate the hue
        float delta = flMax - flMin;

        // First, calculate the saturation
        if ( l < 0.5f )  // If we're in the lower hexcone
        {
            s = delta / ( flMax + flMin );
        }
        else
        {
            s = delta / ( 2 - flMax - flMin );
        }

        if ( clr.r() == flMax )
        {
            h = ( clr.g() - clr.b() ) / delta;  // color between yellow and magenta
        }
        else if ( clr.g() == flMax )
        {
            h = 2 + ( clr.b() - clr.r() ) / delta;  // color between cyan and yellow
        }
        else  // blue must be max
        {
            h = 4 + ( clr.r() - clr.g() ) / delta;  // color between magenta and cyan
        }

        h *= 60.0f;

        if ( h < 0.0f )
        {
            h += 360.0f;
        }

        h /= 360.0f;
    }

    lua_pushnumber( L, h );
    lua_pushnumber( L, s );
    lua_pushnumber( L, l );

    return 3;
}

static int luasrc_HSLToRGB( lua_State *L )
{
    float r, g, b;
    float h = luaL_checknumber( L, 1 );
    float s = luaL_checknumber( L, 2 );
    float l = luaL_checknumber( L, 3 );

    if ( s == 0 )
    {
        r = g = b = l;
    }
    else
    {
        float v1, v2;

        if ( l < 0.5f )
            v2 = l * ( 1.0f + s );
        else
            v2 = ( l + s ) - ( s * l );

        v1 = 2 * l - v2;

        r = HueToRGB( v1, v2, h + ( 1.0f / 3.0f ) );
        g = HueToRGB( v1, v2, h );
        b = HueToRGB( v1, v2, h - ( 1.0f / 3.0f ) );
    }

    lua_pushcolor( L, lua_Color( r, g, b, 255 ) );

    return 1;
}

static const luaL_Reg Color_funcs[] = {
    { "Color", luasrc_Color },
    { "HSVToColor", luasrc_HSVToRGB },
    { "ColorToHSV", luasrc_RGBToHSV },
    { "ColorToHSL", luasrc_RGBToHSL },
    { "HSLToColor", luasrc_HSLToRGB },
    { NULL, NULL } };

/*
** Open Color object
*/
LUALIB_API int luaopen_Color( lua_State *L )
{
    luaL_newmetatable( L, LUA_COLORLIBNAME );
    luaL_register( L, NULL, Colormeta );
    lua_pushstring( L, "color" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "color" */
    luaL_register( L, LUA_GNAME, Color_funcs );
    lua_pop( L, 1 );
    return 1;
}
