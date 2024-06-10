/*
 * Compatibility with Lua 5.1
 */

#include "cbase.h"
#include "lua.hpp"

lua_State *lua_open() {
    return luaL_newstate();
}

void luaL_register(lua_State *L, const char *libname, const luaL_Reg *l) {
    if (libname) {
        luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
        lua_getfield(L, -1, libname);  // get package.loaded[libname]
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);  // remove previous result
            lua_newtable(L);
            lua_pushvalue(L, -1);
            lua_setfield(L, -3,
                         libname);  // package.loaded[libname] = new table
        }
        lua_remove(L, -2);  // remove package.loaded
        lua_pushvalue(L, -1);
        lua_setglobal(L, libname);  // _G[libname] = new table
    }

    for (; l->name != NULL; l++) {
        lua_pushcfunction(L, l->func);
        lua_setfield(L, -2, l->name);
    }
}

int luaL_typerror(lua_State *L, int narg, const char *tname) {
    const char *msg = lua_pushfstring(L, "%s expected, got %s", tname,
                                      luaL_typename(L, narg));
    return luaL_argerror(L, narg, msg);
}

#define luaL_checkint(L, n) ((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L, n, d) ((int)luaL_optinteger(L, (n), (d)))
#define luaL_checklong(L, n) ((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L, n, d) ((long)luaL_optinteger(L, (n), (d)))

void *luaL_checkudata(lua_State *L, int ud, const char *tname) {
    void *p = lua_touserdata(L, ud);
    if (p == NULL)
        luaL_typerror(L, ud, tname);
    return p;
}

const char *luaL_optlstring(lua_State *L, int numArg, const char *def,
                            size_t *l) {
    if (lua_isnoneornil(L, numArg)) {
        if (l)
            *l = (def ? strlen(def) : 0);
        return def;
    }
    return lua_tolstring(L, numArg, l);
}

void lua_getref(lua_State *L, int ref) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
}

void lua_unref(lua_State *L, int ref) {
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
}

bool lua_isrefvalid(lua_State *L, int ref) {
    // ref being 0 indicates we forgot to set `m_nTableReference = LUA_NOREF`
    // in a constructor somewhere.
    Assert(ref != 0);

    return ref != LUA_REFNIL && ref != LUA_NOREF;
}

// The following base functions are copied from
// https://github.com/lua/lua/blob/bdc85357aa41a9610498232c2cffe7aa191e5cf6/lbaselib.c#L432
// since they're static in the original file
// TODO: Instead of copying the functions, we could just include the original
// source file in the project and recompile Lua to get the functions.
#define luaL_intop( op, v1, v2 ) \
    ( ( lua_Integer )( ( lua_Unsigned )( v1 )op( lua_Unsigned )( v2 ) ) )

int luaB_error( lua_State *L )
{
    int level = ( int )luaL_optinteger( L, 2, 1 );
    lua_settop( L, 1 );
    if ( lua_type( L, 1 ) == LUA_TSTRING && level > 0 )
    {
        luaL_where( L, level ); /* add extra information */
        lua_pushvalue( L, 1 );
        lua_concat( L, 2 );
    }
    return lua_error( L );
}

static int ipairsaux( lua_State *L )
{
    lua_Integer i = luaL_checkinteger( L, 2 );
    i = luaL_intop( +, i, 1 );
    lua_pushinteger( L, i );
    return ( lua_geti( L, 1, i ) == LUA_TNIL ) ? 1 : 2;
}

int luaB_ipairs( lua_State *L )
{
    luaL_checkany( L, 1 );
    lua_pushcfunction( L, ipairsaux ); /* iteration function */
    lua_pushvalue( L, 1 );             /* state */
    lua_pushinteger( L, 0 );           /* initial value */
    return 3;
}

int luaB_next( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TTABLE );
    lua_settop( L, 2 ); /* create a 2nd argument if there isn't one */
    if ( lua_next( L, 1 ) )
        return 2;
    else
    {
        lua_pushnil( L );
        return 1;
    }
}

static int pairscont( lua_State *L, int status, lua_KContext k )
{
    ( void )L;
    ( void )status;
    ( void )k; /* unused */
    return 3;
}

int luaB_pairs( lua_State *L )
{
    luaL_checkany( L, 1 );
    if ( luaL_getmetafield( L, 1, "__pairs" ) == LUA_TNIL )
    {                                      /* no metamethod? */
        lua_pushcfunction( L, luaB_next ); /* will return generator, */
        lua_pushvalue( L, 1 );             /* state, */
        lua_pushnil( L );                  /* and initial value */
    }
    else
    {
        lua_pushvalue( L, 1 );              /* argument 'self' to metamethod */
        lua_callk( L, 1, 3, 0, pairscont ); /* get 3 values from metamethod */
    }
    return 3;
}

int luaB_select( lua_State *L )
{
    int n = lua_gettop( L );
    if ( lua_type( L, 1 ) == LUA_TSTRING && *lua_tostring( L, 1 ) == '#' )
    {
        lua_pushinteger( L, n - 1 );
        return 1;
    }
    else
    {
        lua_Integer i = luaL_checkinteger( L, 1 );
        if ( i < 0 )
            i = n + i;
        else if ( i > n )
            i = n;
        luaL_argcheck( L, 1 <= i, 1, "index out of range" );
        return n - ( int )i;
    }
}

#define SPACECHARS " \f\n\r\t\v"

static const char *b_str2int( const char *s, int base, lua_Integer *pn )
{
    lua_Unsigned n = 0;
    int neg = 0;
    s += strspn( s, SPACECHARS ); /* skip initial spaces */
    if ( *s == '-' )
    {
        s++;
        neg = 1;
    } /* handle sign */
    else if ( *s == '+' )
        s++;
    if ( !isalnum( ( unsigned char )*s ) ) /* no digit? */
        return NULL;
    do
    {
        int digit = ( isdigit( ( unsigned char )*s ) ) ? *s - '0'
                                                       : ( toupper( ( unsigned char )*s ) - 'A' ) + 10;
        if ( digit >= base ) return NULL; /* invalid numeral */
        n = n * base + digit;
        s++;
    } while ( isalnum( ( unsigned char )*s ) );
    s += strspn( s, SPACECHARS ); /* skip trailing spaces */
    *pn = ( lua_Integer )( ( neg ) ? ( 0u - n ) : n );
    return s;
}

int luaB_tonumber( lua_State *L )
{
    if ( lua_isnoneornil( L, 2 ) )
    { /* standard conversion? */
        if ( lua_type( L, 1 ) == LUA_TNUMBER )
        {                       /* already a number? */
            lua_settop( L, 1 ); /* yes; return it */
            return 1;
        }
        else
        {
            size_t l;
            const char *s = lua_tolstring( L, 1, &l );
            if ( s != NULL && lua_stringtonumber( L, s ) == l + 1 )
                return 1; /* successful conversion to number */
            /* else not a number */
            luaL_checkany( L, 1 ); /* (but there must be some parameter) */
        }
    }
    else
    {
        size_t l;
        const char *s;
        lua_Integer n = 0; /* to avoid warnings */
        lua_Integer base = luaL_checkinteger( L, 2 );
        luaL_checktype( L, 1, LUA_TSTRING ); /* no numbers as strings */
        s = lua_tolstring( L, 1, &l );
        luaL_argcheck( L, 2 <= base && base <= 36, 2, "base out of range" );
        if ( b_str2int( s, ( int )base, &n ) == s + l )
        {
            lua_pushinteger( L, n );
            return 1;
        }               /* else not a number */
    }                   /* else not a number */
    luaL_pushfail( L ); /* not a number */
    return 1;
}

int luaB_tostring( lua_State *L )
{
    luaL_checkany( L, 1 );
    luaL_tolstring( L, 1, NULL );
    return 1;
}

int luaB_type( lua_State *L )
{
    int t = lua_type( L, 1 );
    luaL_argcheck( L, t != LUA_TNONE, 1, "value expected" );
    lua_pushstring( L, lua_typename( L, t ) );
    return 1;
}
