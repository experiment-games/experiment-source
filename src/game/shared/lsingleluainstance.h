#ifndef L_SINGLELUAINSTANCE_H
#define L_SINGLELUAINSTANCE_H
#ifdef _WIN32
#pragma once
#endif

#include <lua.hpp>

/*
 * Source for the code below:
 *    https://github.com/katlogic/pudata/blob/e731cae1725c372ecdfef799d541a12bcccc864e/pudata.h
 * Modified to work for Experiment with changes like:
 * - Renaming the macros so they do not conflict with the original lua_newuserdata
 * - Splitting it into a .h and .cpp file
 */

/*
 * lua_pushuserdata() polyfill (slow).
 * (C) Copyright 2014, Karel Tuma <kat@lua.cz>, All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define PUDATA_RIDX "__pudata_field"

/* Resolve actual userdata instance we've stored. */
void lua_pushuserdatainstance( lua_State *L, void *p );

/* Associate userdata at top of stack with p. */
void lua_pushuserdata_associateinstance( lua_State *L, void *p );

/* Creates and pushes new user data like normally, except it now stores the instance
in the registry for later retrieval. */
void *lua_newuserdatainstance( lua_State *L, size_t sz );

/* Destroys the userdata instance by removing it from the registry. */
void lua_destroyuserdatainstance( lua_State *L, void *p );

/* When userdata are resurrected in finalizer, the weak value
 * in PUDATA_RIDX is already gone. Use this to explicitly
 * resurrect. */
#define lua_pushuserdata_resurrect( L, idx )                               \
    {                                                                      \
        lua_pushvalue( L, idx );                                           \
        lua_pushuserdata_associateinstance( L, lua_touserdata( L, idx ) ); \
    }

// Apply this to a class to mark it as having a single Lua instance that will
// have the specified metatable applied to its userdata.
#define LUA_DECLARE_SINGLE_LUA_INSTANCE( ClassName, MetatableName )                                                        \
   protected:                                                                                                              \
    void *m_pLuaInstance = nullptr;                                                                                        \
                                                                                                                           \
    /* Creates a new pointer to the instance for this class and pushes it                                                  \
     to the stack. The pointer will be stored with the instance so the                                                     \
     same userdata instance can be pushed again later.                                                                     \
     This should not apply any metatable to the instance, as that will be                                                  \
     done by PushLuaInstance. */                                                                                           \
    static void *CreateLuaInstance( lua_State *L, ClassName *pInstance );                                                  \
                                                                                                                           \
   public:                                                                                                                 \
    virtual const char *GetMetatableName() const                                                                           \
    {                                                                                                                      \
        return MetatableName;                                                                                              \
    }                                                                                                                      \
                                                                                                                           \
    /*  Pushes the instance to the Lua stack. It will check if there's already an                                          \
     instance in the registry, and if there is, it will push that instead.                                                 \
     If no instance is found, it will create a new one and store it in the                                                 \
     registry. It will set the metatable for the instance based on the metatable                                           \
     name returned by GetMetatableName(). */                                                                               \
    void PushLuaInstance( lua_State *L )                                                                                   \
    {                                                                                                                      \
        if ( m_pLuaInstance != nullptr )                                                                                   \
        {                                                                                                                  \
            lua_pushuserdatainstance( L, m_pLuaInstance );                                                                 \
            return;                                                                                                        \
        }                                                                                                                  \
                                                                                                                           \
        m_pLuaInstance = CreateLuaInstance( L, this );                                                                     \
        lua_pushuserdata_associateinstance( L, m_pLuaInstance );                                                           \
        luaL_getmetatable( L, GetMetatableName() );                                                                        \
        AssertMsg( !lua_isnil( L, -1 ), "Invalid MetatableName! Must return a valid metatable on client and server" );     \
        lua_setmetatable( L, -2 );                                                                                         \
    }                                                                                                                      \
                                                                                                                           \
    /* Safely pushes the lua instance, checking if it exists or pushing a NULL instead */                                  \
    static void PushLuaInstanceSafe( lua_State *L, ClassName *pInstance )                                                  \
    {                                                                                                                      \
        if ( !pInstance )                                                                                                  \
        {                                                                                                                  \
            void *pLuaInstance = CreateLuaInstance( L, pInstance );                                                        \
            lua_pushuserdata_associateinstance( L, pLuaInstance );                                                         \
            luaL_getmetatable( L, MetatableName ); /* We fall back to the MetatableName we know from the baseclass*/       \
            AssertMsg( !lua_isnil( L, -1 ), "Invalid MetatableName! Must return a valid metatable on client and server" ); \
            lua_setmetatable( L, -2 );                                                                                     \
            return;                                                                                                        \
        }                                                                                                                  \
                                                                                                                           \
        pInstance->PushLuaInstance( L );                                                                                   \
    }

// After applying LUA_DECLARE_SINGLE_LUA_INSTANCE to a baseclass, use this to
// specify that this subclass overrides the name of the metatable with the specified
// metatable name.
#define LUA_OVERRIDE_SINGLE_LUA_INSTANCE_METATABLE( MetaTableName ) \
    const char *GetMetatableName() const override                   \
    {                                                               \
        return MetaTableName;                                       \
    }

// Safely pushes an entity instance, pushing a NULL entity if no valid entity
// is provided
#define LUA_SAFE_PUSH_ENTITY_INSTANCE( L, EntityStatement )                                  \
    if ( !EntityStatement )                                                                  \
    {                                                                                        \
        CBaseHandle *hEntity = ( CBaseHandle * )lua_newuserdata( L, sizeof( CBaseHandle ) ); \
        hEntity->Set( pEntity );                                                             \
        luaL_getmetatable( L, LUA_BASEENTITYLIBNAME );                                       \
        lua_setmetatable( L, -2 );                                                           \
    }                                                                                        \
    else                                                                                     \
        EntityStatement->PushLuaInstance( L );

#endif
