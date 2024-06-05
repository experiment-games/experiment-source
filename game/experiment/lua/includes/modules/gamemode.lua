--[[
	Original code by Team Sandbox:
		Copyleft © 2010, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

_BASE_GAMEMODE = "deathmatch"

require("hook")

local hook = hook
local table = table
local print = MsgN
local _BASE_GAMEMODE = _BASE_GAMEMODE
local _G = _G

local MODULE = {}

local registeredGamemodes = {}
local activeGamemodeName = nil

--- Calls a gamemode hook, but only if it exists. Returns false if it doesn't.
--- @param eventName string Name of the internal GameRules method
--- @vararg any Arguments to pass to the hook
--- @return boolean|any # False if the hook doesn't exist, otherwise the return value of the hook
function MODULE.Call(eventName, ...)
	if (_G._GAMEMODE and _G._GAMEMODE[eventName] == nil) then
		return false
	end

	return hook.Call(eventName, _G._GAMEMODE, ...)
end

--- Returns a gamemode table object
--- @param gamemodeName string Name of the gamemode
--- @return table # The gamemode table object
function MODULE.Get(gamemodeName)
	return registeredGamemodes[gamemodeName]
end

function MODULE.InternalSetActiveName(gamemodeName)
	activeGamemodeName = gamemodeName
end

function MODULE.GetActiveName()
	return activeGamemodeName
end

--- Registers a gamemode, possibly inheriting from another gamemode.
--- @param gamemodeTable table Gamemode table object
--- @param gamemodeName string Name of the gamemode
--- @param baseGameMode string Name
function MODULE.Register(gamemodeTable, gamemodeName, baseGameMode)
	if (MODULE.Get(gamemodeName) ~= nil and _G._GAMEMODE ~= nil) then
		gamemodeTable = table.Inherit(gamemodeTable, _G._GAMEMODE)
	end

	if (gamemodeName ~= _BASE_GAMEMODE) then
		gamemodeTable = table.Inherit(gamemodeTable, MODULE.Get(baseGameMode))
	end

	registeredGamemodes[gamemodeName] = gamemodeTable
end

return MODULE
