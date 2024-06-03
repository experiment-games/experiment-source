#include "cbase.h"
#include "luahandle.h"
#include "filesystem.h"

// Get the LuaHandle instance
// This will be initialized in CGameRules::CGameRules()
static CBaseLuaHandle *g_LuaHandle = NULL;

LuaHandle::LuaHandle() {
    g_LuaHandle = this;
    Register();
}

LuaHandle::~LuaHandle() {
}

void LuaHandle::Init() {
    const char *luaFile = "myLuaFile.lua";

    // Load into buffer
    FileHandle_t f = filesystem->Open(luaFile, "rb", "MOD");
    if (!f)
        return;

    // load file into a null-terminated buffer
    int fileSize = filesystem->Size(f);
    unsigned bufSize =
        ((IFileSystem *)filesystem)->GetOptimalReadSize(f, fileSize + 1);

    char *buffer =
        (char *)((IFileSystem *)filesystem)->AllocOptimalReadBuffer(f, bufSize);
    Assert(buffer);

    ((IFileSystem *)filesystem)
        ->ReadEx(buffer, bufSize, fileSize, f);  // read into local buffer
    buffer[fileSize] = '\0';                     // null terminate file as EOF
    filesystem->Close(f);                        // close file after reading

    int error = luaL_loadbuffer(GetLua(), buffer, fileSize, luaFile);
    if (error) {
        Warning("[LUA-ERR] %s\n", lua_tostring(GetLua(), -1));
        lua_pop(GetLua(), 1); /* pop error message from the stack */
        Warning(
            "[LUA-ERR] One or more errors occured while loading lua script!\n");
        return;
    }
    CallLua(GetLua(), 0, LUA_MULTRET, 0, luaFile);
    m_bLuaLoaded = true;
}

void LuaHandle::Shutdown() {
}

void LuaHandle::RegisterGlobals() {
    // LG_DEFINE_INT for int
    // LG_DEFINE_STRING for string
    // LG_DEFINE_BOOL for boolean
    // e.g: LG_DEFINE_INT("MAX_PLAYERS", gpGlobals->maxClients);
}

// Note:
// All functions that are registered in "RegFunctions" must have their
// actual function definition names prefixed with "lua".
void LuaHandle::RegisterFunctions() {
    REG_FUNCTION(Msg);
}

int luaMsg(lua_State *L) {
    Msg("%s\n", lua_tostring(L, 1));
    return 0;
}

CBaseLuaHandle *GetLuaHandle() {
    Assert(g_LuaHandle);

    return g_LuaHandle;
}

// Handy for getting the state in one go.
// Use GetLuaHandle if more control is needed.
lua_State *GetLuaState() {
    CBaseLuaHandle *luaHandle = GetLuaHandle();

    return luaHandle->GetLua();
}

static bool RunLuaString(const char *string) {
    lua_State *L = GetLuaState();

    int error = luaL_loadstring(L, string);

    if (error) {
        Warning("[Lua Error] %s\n", lua_tostring(L, -1));
        lua_pop(L, 1); /* pop error message from the stack */
        return false;
    }

    CallLua(L, 0, LUA_MULTRET, 0, string);
    return true;
}

#ifdef CLIENT_DLL
CON_COMMAND(lua_run_cl, "Run a Lua string in the client realm") {
    RunLuaString(args.ArgS());
}
#else
CON_COMMAND(lua_run, "Run a Lua string in the server realm") {
    if (!UTIL_IsCommandIssuedByServerAdmin()) {
        cvar->ConsoleColorPrintf(LUA_PROHIBIT_PRINT_COLOUR,
                                 "[Lua] Admin-only command!\n");
        return;
    }

    RunLuaString(args.ArgS());
}
#endif
