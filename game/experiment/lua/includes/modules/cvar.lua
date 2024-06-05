--[[
	Original code by Team Sandbox:
		Copyleft © 2010, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local pairs = pairs
local printError = debug.PrintError
local tostring = tostring
local xpcall = xpcall

local MODULE = {}

local didCallbackError, callbackError
local registeredCallbacks = {}

--- Adds a callback to a ConVar so that when it changes, the callback is called.
--- @param conVarName string The name of the ConVar.
--- @param callbackIdentifier string The name of the callback, so it can be removed later.
--- @param callback fun(ConVar, string, string) The function to call when the ConVar changes.
function MODULE.AddChangeCallback(conVarName, callbackIdentifier, callback)
	registeredCallbacks[conVarName] = registeredCallbacks[conVarName] or {}
	registeredCallbacks[conVarName][callbackIdentifier] = callback
end

--- Calls all of the callbacks for a ConVar.
--- @param conVar ConVar The ConVar that has changed.
--- @param oldValueAsString string The old value of the ConVar.
--- @param oldValueAsNumber number The old value of the ConVar.
function MODULE.CallGlobalChangeCallbacks(conVar, oldValueAsString, oldValueAsNumber)
	local callbacks = registeredCallbacks[conVar:GetName()]

	if (callbacks == nil) then
		return
	end

	for callbackIdentifier, callback in pairs(callbacks) do
		if (callback == nil) then
			printError(
				"ConVar Callback Error! '" ..
				tostring(callbackIdentifier) .. "' (" .. tostring(conVar:GetName()) .. ") tried to call a nil function!\n"
			)
			callbacks[callbackIdentifier] = nil

			break
		end

		didCallbackError, callbackError = xpcall(callback, printError, conVar, oldValueAsString, oldValueAsNumber)

		if (didCallbackError == false) then
			printError("ConVar Callback Error! '" ..
			tostring(callbackIdentifier) ..
			"' (" .. tostring(conVar:GetName()) .. ") Failed: " .. tostring(callbackError) .. "\n")
			callbacks[callbackIdentifier] = nil
		end
	end
end

--- Returns all of the registered callbacks or only callbacks pertaining to a specific ConVar.
--- @param conVarName string The name of the ConVar.
--- @return table
function MODULE.GetChangeCallbacks(conVarName)
	if (conVarName) then
		return registeredCallbacks[conVarName]
	end

	return registeredCallbacks
end

--- Removes a callback from the list of registered callbacks.
--- @param conVarName string The name of the ConVar.
--- @param callbackIdentifier string The name of the callback.
function MODULE.RemoveChangeCallback(conVarName, callbackIdentifier)
	if (registeredCallbacks[conVarName][callbackIdentifier]) then
		registeredCallbacks[conVarName][callbackIdentifier] = nil
	end
end

return MODULE
