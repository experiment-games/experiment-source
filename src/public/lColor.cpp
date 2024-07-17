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
    if ( lua_isuserdata(L, narg) )
    {
        lua_Color *d = ( lua_Color * )luaL_checkudata( L, narg, "Color" );
        return *d;
    }

    // HACK:    For some reason some code is copying colors as a table, messing up at luaL_checkudata
    //          As a temporary fix we will first check if the metatable has a __type field and if it is "Color"
    //          If it is we will assume it is a color and return it
    if ( lua_istable( L, narg ) )
    {
        lua_getmetatable( L, narg );
        lua_getfield( L, -1, "__type" );

        if ( lua_isstring( L, -1 ) && !strcmp( lua_tostring( L, -1 ), "Color" ) )
        {
            lua_pop( L, 2 );
            lua_pushcolor( L, lua_Color( lua_tonumber( L, narg ), lua_tonumber( L, narg + 1 ), lua_tonumber( L, narg + 2 ), lua_tonumber( L, narg + 3 ) ) );
            return lua_tocolor( L, -1 );
        }
        lua_pop( L, 2 );
    }

    luaL_typerror( L, narg, "Color" );
    return *static_cast< lua_Color * >( 0 );
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
    luaL_checkcolor( L, 1 ).SetColor( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ), luaL_checknumber( L, 4 ), luaL_optnumber( L, 5, 255 ) );
    return 0;
}

static int Color_SetRawColor( lua_State *L )
{
    luaL_checkcolor( L, 1 ).SetRawColor( luaL_checknumber( L, 2 ) );
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

    const char *field = luaL_checkstring( L, 2 );

    if ( Q_strcmp( field, "r" ) == 0 || strcmp( field, "1" ) == 0 )
        lua_pushinteger( L, color->r() );
    else if ( Q_strcmp( field, "g" ) == 0 || strcmp( field, "2" ) == 0 )
        lua_pushinteger( L, color->g() );
    else if ( Q_strcmp( field, "b" ) == 0 || strcmp( field, "3" ) == 0 )
        lua_pushinteger( L, color->b() );
    else if ( Q_strcmp( field, "a" ) == 0 || strcmp( field, "4" ) == 0 )
        lua_pushinteger( L, color->a() );
    else
    {
        LUA_GET_REF_TABLE( L, color );
        LUA_METATABLE_INDEX_CHECK_REF_TABLE( L, color );

        if ( lua_getmetatable( L, 1 ) )
        {
            LUA_METATABLE_INDEX_CHECK_TABLE( L );
        }

        lua_pushnil( L );
    }

    return 1;
}

static int Color___newindex( lua_State *L )
{
    lua_Color *color = &luaL_checkcolor( L, 1 );
    LUA_METATABLE_INDEX_CHECK( L, color );

    const char *field = luaL_checkstring( L, 2 );

    if ( Q_strcmp( field, "r" ) == 0 || strcmp( field, "1" ) == 0 )
        color->SetColor( luaL_checknumber( L, 3 ), color->g(), color->b(), color->a() );
    else if ( Q_strcmp( field, "g" ) == 0 || strcmp( field, "2" ) == 0 )
        color->SetColor( color->r(), luaL_checknumber( L, 3 ), color->b(), color->a() );
    else if ( Q_strcmp( field, "b" ) == 0 || strcmp( field, "3" ) == 0 )
        color->SetColor( color->r(), color->g(), luaL_checknumber( L, 3 ), color->a() );
    else if ( Q_strcmp( field, "a" ) == 0 || strcmp( field, "4" ) == 0 )
        color->SetColor( color->r(), color->g(), color->b(), luaL_checknumber( L, 3 ) );
    else
    {
        LUA_GET_REF_TABLE( L, color );
        lua_pushvalue( L, 3 );
        lua_setfield( L, -2, field );
        lua_pop( L, 1 );
    }

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
    lua_Color clr = lua_Color( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ), luaL_optnumber( L, 4, 255 ) );
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
    LUA_PUSH_NEW_METATABLE( L, LUA_COLORLIBNAME );
    luaL_register( L, NULL, Colormeta );
    lua_pushstring( L, "Color" );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Color" */
    luaL_register( L, LUA_GNAME, Color_funcs );
    lua_pop( L, 1 );
    return 1;
}
