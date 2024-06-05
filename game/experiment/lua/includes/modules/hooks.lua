--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

require("debug")

local pairs = pairs
local tostring = tostring
local unpack = table.unpack
local printError = debug.PrintError

local MODULE = {}

local registeredHooks = {}
local returnValues = {}

--- Adds a hook to the given GameRules function.
--- @param eventName string The name of the internal GameRules method.
--- @param hookIdentifier string The name of the hook.
--- @param callback fun(...): ... The function to call when the hook is called.
function MODULE.Add(eventName, hookIdentifier, callback)
	registeredHooks[eventName] = registeredHooks[eventName] or {}
	registeredHooks[eventName][hookIdentifier] = callback
end

--- Calls all hooks associated with a specific event.
--- @param eventName string The name of the internal GameRules method.
--- @param gamemodeTable table The table of the current gamemode.
--- @vararg any Arguments to pass to the hooks.
--- @return any # The return value(s) of the hook.
function MODULE.Call(eventName, gamemodeTable, ...)
	local callbacks = registeredHooks[eventName]

	if (callbacks ~= nil) then
		for hookIdentifier, callback in pairs(callbacks) do
			if (callback == nil) then
				printError("Hook Error! '" ..
					tostring(hookIdentifier) .. "' (" .. tostring(eventName) .. ") tried to call a nil function!\n")
				-- callbacks[hookIdentifier] = nil
				break
			end

			returnValues = { xpcall(callback, printError, ...) }

			if (returnValues[1] == false) then
				printError("Hook Error! '" ..
					tostring(hookIdentifier) .. "' (" .. tostring(eventName) .. ") Failed\n")
				-- callbacks[hookIdentifier] = nil
			elseif (returnValues[2] ~= nil) then
				return unpack(returnValues, 2)
			end
		end
	end

	if (gamemodeTable ~= nil) then
		local callback = gamemodeTable[eventName]

		if (callback == nil) then
			return nil
		end

		returnValues = { xpcall(callback, printError, gamemodeTable, ...) }

		if (returnValues[1] == false) then
			printError("Gamemode Error! '" ..
				tostring(eventName) .. "' Failed\n")
			-- gamemodeTable[eventName] = nil
			return nil
		end

		return unpack(returnValues, 2)
	end
end

--- Calls all hooks associated with a specific event, using the current gamemode.
--- @param eventName string The name of the internal GameRules method.
--- @vararg any Arguments to pass to the hooks.
--- @return any # The return value(s) of the hook.
function MODULE.Run(eventName, ...)
	return MODULE.Call(eventName, _G.GAMEMODE, ...)
end

--- Returns all of the registered hooks or only hooks pertaining to a specific event.
--- @param eventName string The name of the internal GameRules method.
--- @return table
function MODULE.GetTable(eventName)
	if (eventName) then
		return registeredHooks[eventName]
	end

	return registeredHooks
end

--- Removes a hook from the list of registered hooks.
--- @param eventName string The name of the internal GameRules method.
--- @param hookIdentifier string The name of the hook.
function MODULE.Remove(eventName, hookIdentifier)
	if (registeredHooks[eventName][hookIdentifier]) then
		registeredHooks[eventName][hookIdentifier] = nil
	end
end

hooks = MODULE

return MODULE
