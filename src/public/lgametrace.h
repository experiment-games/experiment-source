#ifndef LGAMETRACE_H
#define LGAMETRACE_H
#ifdef _WIN32
#pragma once
#endif

/* type for CGameTrace functions */
typedef CGameTrace lua_CGameTrace;

/*
** access functions (stack -> C)
*/

LUA_API lua_CGameTrace &( lua_totrace )( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUA_API void( lua_pushtrace )( lua_State *L, lua_CGameTrace &tr );

LUALIB_API lua_CGameTrace &( luaL_checktrace )( lua_State *L, int narg );

LUA_API void lua_pushtrace_t( lua_State *L, trace_t *trace, bool bNoNewTable = false );

class CTraceLuaFilter : public CTraceFilterSimple
{
    public:
    CTraceLuaFilter( lua_State *L, int filterArg, int collisionGroup, bool bIgnoreWorld, bool bFilterTableInverted = false );

    virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask );

    private:
    const IHandleEntity *m_pPassEntity;
    lua_State *m_pLuaState;
    int m_iFilterType;
    int m_iFilterArgIndex;
    bool m_bIgnoreWorld;
    bool m_bFilterTableInverted;

#ifdef CLIENT_DLL  // Server has its own GetWorldEntity in world.h
    C_BaseEntity *GetWorldEntity( void );
#endif
};

LUA_API void lua_checktracestruct( lua_State *L, int narg );
LUA_API void lua_checktracestruct( lua_State *L, int narg, Vector &start, Vector &end, int &mask, int &collisionGroup, bool &bIgnoreWorld, bool &bFilterTableInverted, bool &bOutput, CTraceLuaFilter **filter );

#endif  // LGAMETRACE_H
