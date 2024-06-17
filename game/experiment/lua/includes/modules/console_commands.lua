--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local printError = debug.PrintError

local MODULE = {}
MODULE.registeredCallbacks = MODULE.registeredCallbacks or {}

local didCallbackError, callbackError

--- Creates a command that can be called from the console.
--- @param command string The name of the command that you type into the console.
--- @param callback fun(Player, string, string) The function that is called when the command is run.
--- @param helpText? string The help text that is displayed when finding the command in the console.
--- @param flags? number FCVAR flags.
function MODULE.Add(command, callback, helpText, flags)
	MODULE.registeredCallbacks[command] = callback

	ConsoleCommand(command, helpText, flags or 0)
end

--- Dispatches a command to the appropriate callback.
--- @param client Player The player who ran the command.
--- @param command string The name of the command.
--- @param arguments string The arguments that were passed to the command.
--- @return boolean # Whether or not the command was dispatched.
function MODULE.Dispatch(client, command, arguments)
	local callback = MODULE.registeredCallbacks[command]

	if (not callback) then
		return false
	end

	didCallbackError, callbackError = xpcall(callback, printError, client, command, arguments)

	if (didCallbackError == false) then
		printError("ConsoleCommand Error! '" .. tostring(command) .. "' Failed: " .. tostring(callbackError) .. "\n")
	end

	return true
end

--- Removes a command from the list of registered commands.
--- @param command string The name of the command to remove.
function MODULE.Remove(command)
	if (MODULE.registeredCallbacks[command]) then
		MODULE.registeredCallbacks[command] = nil
	end
end

return MODULE
