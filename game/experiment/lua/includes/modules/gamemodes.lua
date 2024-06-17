--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

_BASE_GAMEMODE = "base"

local hooks = require("hooks")
local debugTraceback = debug.traceback

local MODULE = {}
MODULE.registeredGamemodes = MODULE.registeredGamemodes or {}
MODULE.activeGamemodeName = MODULE.activeGamemodeName or nil

--- Calls a gamemode hook, but only if it exists. Returns false if it doesn't.
--- @param eventName string Name of the internal GameRules method
--- @vararg any Arguments to pass to the hook
--- @return boolean|any # False if the hook doesn't exist, otherwise the return value of the hook
function MODULE.Call(eventName, ...)
	if (_G.GAMEMODE and _G.GAMEMODE[eventName] == nil) then
		return false
	end

	return hooks.Call(eventName, _G.GAMEMODE, ...)
end

--- Returns a gamemode table object
--- @param gamemodeName string Name of the gamemode
--- @return table # The gamemode table object
function MODULE.Get(gamemodeName)
	return MODULE.registeredGamemodes[gamemodeName]
end

function MODULE.InternalSetActiveName(gamemodeName)
	MODULE.activeGamemodeName = gamemodeName
end

function MODULE.GetActiveName()
	return MODULE.activeGamemodeName
end

--- Registers a gamemode, possibly inheriting from another gamemode.
--- @param gamemodeTable table Gamemode table object
--- @param gamemodeName string Name of the gamemode
--- @param baseGameMode string Name
function MODULE.Register(gamemodeTable, gamemodeName, baseGameMode)
    local showError = function(message)
        error(
            debugTraceback(
                "Error registering gamemode " .. tostring(gamemodeName) .. ": " .. tostring(message),
                3
            )
        )
    end

	if (_G.GAMEMODE ~= nil) then
		showError("Cannot register gamemode when a gamemode has already been set!")
		return
	end

	if (MODULE.Get(gamemodeName) ~= nil) then
		showError("Gamemode " .. gamemodeName .. " is already registered!")
		return
	end

	if (gamemodeName ~= _BASE_GAMEMODE) then
		baseGameMode = baseGameMode or _BASE_GAMEMODE
        local baseGamemodeTable = MODULE.Get(baseGameMode)

		if (baseGamemodeTable == nil) then
			showError("Base gamemode " .. tostring(baseGameMode) .. " does not exist!")
			return
		end

		gamemodeTable = table.Inherit(gamemodeTable, baseGamemodeTable)
	end

	MODULE.registeredGamemodes[gamemodeName] = gamemodeTable
end

return MODULE
