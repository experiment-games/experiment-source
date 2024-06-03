///////////// Copyright © 2008 LodleNet. All rights reserved. /////////////
//
//   Project     : Server
//   File        : luamanager.cpp
//   Description :
//      https://developer.valvesoftware.com/wiki/LuaManager.cpp
//
//   Created On: 3/5/2009 4:58:54 PM
//   Created By:  <mailto:admin@lodle.net>
//   Updated By:  <mailto:matt.shirleys@gmail.com>
////////////////////////////////////////////////////////////////////////////

#include "cbase.h"
#include "luamanager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void RegisterLuaFuncs(lua_State *L);
void RegisterLuaGlobals(lua_State *L);

CLuaManager gLuaMng;
CLuaManager *Lua() { return &gLuaMng; }

void RegPublicFunctions(lua_State *L) { RegisterLuaFuncs(L); }

void RegPublicGlobals(lua_State *L) { RegisterLuaGlobals(L); }

////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////

LuaHandle::LuaHandle() {
  m_bStarted = false;
  pL = NULL;
}

LuaHandle::~LuaHandle() { Lua()->DeRegisterLuaHandle(this); }

void LuaHandle::InitDll() {
  if (m_bStarted)
    return;

  // Create an instance; Load the core libs.
  pL = luaL_newstate();
  luaL_openlibs(pL);

  RegFunctions();
  RegGlobals();

  RegPublicFunctions(pL);
  RegPublicGlobals(pL);

  m_bStarted = true;
}

void LuaHandle::ShutdownDll() {
  if (!m_bStarted)
    return;

  Shutdown();
  lua_close(pL);

  m_bStarted = false;
}

void LuaHandle::Register() { Lua()->RegisterLuaHandle(this); }

////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////

CLuaManager::CLuaManager() { m_bInit = false; }

CLuaManager::~CLuaManager() {}

void CLuaManager::InitDll() {
  // Register our Lua Functions and Globals so we can call them
  // from .lua scripts
  if (m_bInit)
    return;

  for (size_t x = 0; x < m_vHandles.size(); x++) {
    m_vHandles[x]->InitDll();
  }

  m_bInit = true;
}

void CLuaManager::InitHandles(void) {
  for (size_t x = 0; x < m_vHandles.size(); x++) {
    m_vHandles[x]->Init();
  }
}

void CLuaManager::ShutdownDll() {
  if (!m_bInit)
    return;

  for (size_t x = 0; x < m_vHandles.size(); x++) {
    m_vHandles[x]->ShutdownDll();
  }

  m_bInit = false;
}

void CLuaManager::ShutdownHandles(void) {
  for (size_t x = 0; x < m_vHandles.size(); x++) {
    m_vHandles[x]->Shutdown();
  }
}

void CLuaManager::DeRegisterLuaHandle(LuaHandle *handle) {
  if (!handle)
    return;

  for (size_t x = 0; x < m_vHandles.size(); x++) {
    if (m_vHandles[x] == handle) {
      m_vHandles.erase(m_vHandles.begin() + x);
      break;
    }
  }
}

void CLuaManager::RegisterLuaHandle(LuaHandle *handle) {
  if (!handle)
    return;

  for (size_t x = 0; x < m_vHandles.size(); x++) {
    if (m_vHandles[x] == handle)
      return;
  }

  m_vHandles.push_back(handle);

  // if we are late to the game
  if (m_bInit)
    handle->InitDll();
}

void RegisterLuaFuncs(lua_State *L) {
  // TODO
}

void RegisterLuaGlobals(lua_State *L) {
  // TODO
}
