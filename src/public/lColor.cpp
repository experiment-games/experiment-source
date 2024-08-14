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
    lua_Color *clr = ( lua_Color * )luaL_checkudata( L, idx, LUA_COLORMETANAME );
    return *clr;
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushcolor( lua_State *L, lua_Color &clr )
{
    lua_Color *pColor = ( lua_Color * )lua_newuserdata( L, sizeof( lua_Color ) );
    *pColor = clr;
    LUA_SAFE_SET_METATABLE( L, LUA_COLORMETANAME );
}

LUA_API void lua_pushcolor( lua_State *L, Color &clr )
{
    lua_Color *pColor = ( lua_Color * )lua_newuserdata( L, sizeof( lua_Color ) );
    *pColor = lua_Color( clr.r(), clr.g(), clr.b(), clr.a() );
    LUA_SAFE_SET_METATABLE( L, LUA_COLORMETANAME );
}

LUALIB_API lua_Color &luaL_checkcolor( lua_State *L, int narg )
{
    if ( lua_isuserdata(L, narg) )
    {
        lua_Color *d = ( lua_Color * )luaL_checkudata( L, narg, LUA_COLORMETANAME );
        return *d;
    }

    // HACK:    For some reason some code is copying colors as a table, messing up at luaL_checkudata
    //          As a temporary fix we will first check if the metatable has a __type field and if it is "Color"
    //          If it is we will assume it is a color and return it
    if ( lua_istable( L, narg ) )
    {
        lua_getmetatable( L, narg );
        lua_getfield( L, -1, "__type" );

        if ( lua_isstring( L, -1 ) && !strcmp( lua_tostring( L, -1 ), LUA_COLORMETANAME ) )
        {
            lua_pop( L, 2 );
            lua_pushcolor( L, lua_Color( lua_tonumber( L, narg ), lua_tonumber( L, narg + 1 ), lua_tonumber( L, narg + 2 ), lua_tonumber( L, narg + 3 ) ) );
            return lua_tocolor( L, -1 );
        }
        lua_pop( L, 2 );
    }

    luaL_typerror( L, narg, LUA_COLORMETANAME );
    return *static_cast< lua_Color * >( 0 );
}

LUA_REGISTRATION_INIT( Color )

LUA_BINDING_BEGIN( Color, GetAlpha, "class", "Gets the alpha value of the color." )
{
    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    lua_pushinteger( L, color.a() );
    return 1;
}
LUA_BINDING_END( "number", "The alpha value." )

LUA_BINDING_BEGIN( Color, GetBlue, "class", "Gets the blue value of the color." )
{
    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    lua_pushinteger( L, color.b() );
    return 1;
}
LUA_BINDING_END( "number", "The blue value." )

LUA_BINDING_BEGIN( Color, GetColor, "class", "Gets the color values." )
{
    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    lua_pushinteger( L, color.r() );
    lua_pushinteger( L, color.g() );
    lua_pushinteger( L, color.b() );
    lua_pushinteger( L, color.a() );
    return 4;
}
LUA_BINDING_END( "number", "The red value.", "number", "The green value.", "number", "The blue value.", "number", "The alpha value." )

LUA_BINDING_BEGIN( Color, GetGreen, "class", "Gets the green value of the color." )
{
    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    lua_pushinteger( L, color.g() );
    return 1;
}
LUA_BINDING_END( "number", "The green value." )

LUA_BINDING_BEGIN( Color, GetRawColor, "class", "Gets the raw color value." )
{
    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    lua_pushinteger( L, color.GetRawColor() );
    return 1;
}
LUA_BINDING_END( "number", "The raw color value." )

LUA_BINDING_BEGIN( Color, GetRed, "class", "Gets the red value of the color." )
{
    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    lua_pushinteger( L, color.r() );
    return 1;
}
LUA_BINDING_END( "number", "The red value." )

LUA_BINDING_BEGIN( Color, SetColor, "class", "Sets the color values." )
{
    lua_Color &color = luaL_checkcolor( L, 1 );
    int red = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "red" );
    int green = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "green" );
    int blue = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "blue" );
    int alpha = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 255, "alpha" );
    color.SetColor( red, green, blue, alpha );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Color, SetRawColor, "class", "Sets the raw color value." )
{
    lua_Color &color = luaL_checkcolor( L, 1 );
    int rawColor = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "rawColor" );
    color.SetRawColor( rawColor );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Color, __tostring, "class", "Returns a string representation of the color." )
{
    lua_Color color = luaL_checkcolor( L, 1 );
    lua_pushfstring( L, "Color: %s", static_cast< const char * >( CFmtStr( "(%i, %i, %i, %i)", color.r(), color.g(), color.b(), color.a() ) ) );
    return 1;
}
LUA_BINDING_END( "string", "The string representation of the color." )

LUA_BINDING_BEGIN( Color, __eq, "class", "Compares two colors." )
{
    lua_Color colorA = LUA_BINDING_ARGUMENT( lua_tocolor, 1, "colorA" );
    lua_Color colorB = LUA_BINDING_ARGUMENT( lua_tocolor, 2, "colorB" );
    lua_pushboolean( L, colorA == colorB );
    return 1;
}
LUA_BINDING_END( "boolean", "Whether the colors are equal." )

LUA_BINDING_BEGIN( Color, __index, "class", "Metamethod for when the index field doesn't exist. Returns values when indexing r, g, b, a or 1, 2, 3, 4." )
{
    lua_Color *color = &LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    LUA_METATABLE_INDEX_CHECK( L, color );

    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

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
        if ( lua_getmetatable( L, 1 ) )
        {
            LUA_METATABLE_INDEX_CHECK_TABLE( L );
        }

        lua_pushnil( L );
    }

    return 1;
}
LUA_BINDING_END( "any", "The value of the field." )

LUA_BINDING_BEGIN( Color, __newindex, "class", "Metamethod for when the newindex field doesn't exist. Sets values when indexing r, g, b, a or 1, 2, 3, 4." )
{
    lua_Color *color = &LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    LUA_METATABLE_INDEX_CHECK( L, color );

    const char *field = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "field" );

    if ( Q_strcmp( field, "r" ) == 0 || strcmp( field, "1" ) == 0 )
        color->SetColor( LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "red" ), color->g(), color->b(), color->a() );
    else if ( Q_strcmp( field, "g" ) == 0 || strcmp( field, "2" ) == 0 )
        color->SetColor( color->r(), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "green" ), color->b(), color->a() );
    else if ( Q_strcmp( field, "b" ) == 0 || strcmp( field, "3" ) == 0 )
        color->SetColor( color->r(), color->g(), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "blue" ), color->a() );
    else if ( Q_strcmp( field, "a" ) == 0 || strcmp( field, "4" ) == 0 )
        color->SetColor( color->r(), color->g(), color->b(), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "alpha" ) );
    else
    {
        luaL_error( L, "attempt to set a field in a read-only table" );
    }

    return 0;
}
LUA_BINDING_END()

LUA_REGISTRATION_INIT( _G );

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

LUA_BINDING_BEGIN( _G, Color, "library", "Creates a new color." )
{
    int red = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "red" );
    int green = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "green" );
    int blue = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "blue" );
    int alpha = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 255, "alpha" );
    lua_Color color = lua_Color( red, green, blue, alpha );
    lua_pushcolor( L, color );
    return 1;
}
LUA_BINDING_END( "color", "The created color." )

LUA_BINDING_BEGIN( _G, HSVToColor, "library", "Converts HSV to RGB." )
{
    float hue = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "hue" );
    float saturation = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "saturation" );
    float value = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "value" );
    Vector rgb;
    HSVtoRGB( Vector( hue, saturation, value ), rgb );
    lua_pushcolor( L, lua_Color( rgb.x, rgb.y, rgb.z, 255 ) );
    return 1;
}
LUA_BINDING_END( "color", "The converted color." )

LUA_BINDING_BEGIN( _G, ColorToHSV, "library", "Converts RGB to HSV." )
{
    lua_Color clr = luaL_checkcolor( L, 1 );
    Vector hsv;
    RGBtoHSV( Vector( clr.r(), clr.g(), clr.b() ), hsv );
    lua_pushnumber( L, hsv.x );
    lua_pushnumber( L, hsv.y );
    lua_pushnumber( L, hsv.z );
    return 3;
}
LUA_BINDING_END( "number", "The hue.", "number", "The saturation.", "number", "The value." )

LUA_BINDING_BEGIN( _G, ColorToHSL, "library", "Converts RGB to HSL." )
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
LUA_BINDING_END( "number", "The hue.", "number", "The saturation.", "number", "The lightness." )

LUA_BINDING_BEGIN( _G, HSLToColor, "library", "Converts HSL to RGB." )
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
LUA_BINDING_END( "color", "The converted color." )

/*
** Open Color object
*/
LUALIB_API int luaopen_Color( lua_State *L )
{
    LUA_PUSH_NEW_METATABLE( L, LUA_COLORMETANAME );

    LUA_REGISTRATION_COMMIT( Color );

    lua_pushstring( L, LUA_COLORMETANAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Color" */

    LUA_REGISTRATION_COMMIT_LIBRARY( _G );
    lua_pop( L, 1 );  // pop metatable

    return 1;
}
