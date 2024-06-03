#include "cbase.h"
#include "baseluahandle.h"

class LuaHandle : CBaseLuaHandle {
public:
    LuaHandle();
    ~LuaHandle();

    void Init();
    void Shutdown();
    void RegisterFunctions();
    void RegisterGlobals();
};

CBaseLuaHandle *GetLuaHandle();
static lua_State *GetLuaState();
