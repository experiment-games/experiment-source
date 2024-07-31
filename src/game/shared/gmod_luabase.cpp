#include "cbase.h"
#include "gmod_luabase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

lua_StateWithCompat *CreateLuaStateWithCompat( lua_State *L )
{
    lua_StateWithCompat *ext = ( lua_StateWithCompat * )malloc( sizeof( lua_StateWithCompat ) );
    ext->luabase = &CLuaBase::GetInstance( L );
    ext->L = L;

    return ext;
}

lua_StateWithCompat *ToLuaStateWithCompat( lua_State *L )
{
    return ( lua_StateWithCompat * )L;
}

CLuaBase &CLuaBase::GetInstance( lua_State *L )
{
    static CLuaBase instance;

    instance.L = L;

    return instance;
}

void CLuaBase::SetState( lua_StateWithCompat *LCompat )
{
    this->L = LCompat->L;
}

/// <summary>
/// Wraps the Lua function so the compat state can be passed to it
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int CompatFunctionWrapper( lua_State *L )
{
    GarrysMod::Lua::CFunc originalFunc = *static_cast< GarrysMod::Lua::CFunc * >( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
    lua_StateWithCompat *compatState = CreateLuaStateWithCompat( L );

    int result = originalFunc( compatState );

    free(compatState);

    return result;
}

void CLuaBase::PushCFunction( GarrysMod::Lua::CFunc val )
{
    // Store the function pointer in a userdata
    GarrysMod::Lua::CFunc *userData = static_cast< GarrysMod::Lua::CFunc * >( lua_newuserdata( L, sizeof( GarrysMod::Lua::CFunc ) ) );
    *userData = val;

    // Push the wrapper function with the userdata as an upvalue
    lua_pushcclosure( L, CompatFunctionWrapper, 1 );
}
