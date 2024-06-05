--[[
	Original code by Team Sandbox:
		Copyleft © 2010, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local ConCommand = ConCommand
local Warning = dbg.Warning
local tostring = tostring
local pcall = pcall

local MODULE = {}

local didCallbackError, callbackError
local registeredCallbacks = {}

--- Creates a command that can be called from the console.
--- @param command string The name of the command that you type into the console.
--- @param callback fun(Player, string, string) The function that is called when the command is run.
--- @param helpText string The help text that is displayed when finding the command in the console.
--- @param flags number FCVAR flags.
function MODULE.Add(command, callback, helpText, flags)
	registeredCallbacks[command] = callback

	ConCommand(command, helpText, flags)
end

--- Dispatches a command to the appropriate callback.
--- @param client Player The player who ran the command.
--- @param command string The name of the command.
--- @param arguments string The arguments that were passed to the command.
--- @return boolean # Whether or not the command was dispatched.
function MODULE.Dispatch(client, command, arguments)
	local callback = registeredCallbacks[command]

	if (not callback) then
		return false
	end

	didCallbackError, callbackError = pcall(callback, client, command, arguments)

	if (didCallbackError == false) then
		Warning("ConCommand '" .. tostring(command) .. "' Failed: " .. tostring(callbackError) .. "\n")
	end

	return true
end

--- Removes a command from the list of registered commands.
--- @param command string The name of the command to remove.
function MODULE.Remove(command)
	if (registeredCallbacks[command]) then
		registeredCallbacks[command] = nil
	end
end

return MODULE
