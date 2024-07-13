//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef LCOLOR_H
#define LCOLOR_H

#ifdef _WIN32
#pragma once
#endif

class LColor : public Color
{
   public:
    LColor() : Color() {}
    LColor( int _r, int _g, int _b ) : Color( _r, _g, _b ) {}
    LColor( int _r, int _g, int _b, int _a ) : Color( _r, _g, _b, _a ) {}

    int m_nTableReference = LUA_NOREF;

    void SetupRefTable( lua_State *L )
    {
        lua_newtable( L );
        lua_pushinteger( L, r() );
        lua_setfield( L, -2, "r" );
        lua_pushinteger( L, g() );
        lua_setfield( L, -2, "g" );
        lua_pushinteger( L, b() );
        lua_setfield( L, -2, "b" );
        lua_pushinteger( L, a() );
        lua_setfield( L, -2, "a" );
        m_nTableReference = luaL_ref( L, LUA_REGISTRYINDEX );
    }
};

/* type for Color functions */
typedef LColor lua_Color;

/*
** access functions (stack -> C)
*/

LUA_API lua_Color &( lua_tocolor )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushcolor )( lua_State *L, lua_Color &clr );
LUA_API void( lua_pushcolor )( lua_State *L, Color &clr );

LUALIB_API lua_Color &( luaL_checkcolor )( lua_State *L, int narg );


#endif // LCOLOR_H
