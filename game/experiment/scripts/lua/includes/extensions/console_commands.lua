require("ConsoleCommands")

local printError = debug.PrintError
local registeredCallbacks = {}

local wasCallbackSuccessful, callbackError

--- Creates a command that can be called from the console.
--- @param command string The name of the command that you type into the console.
--- @param callback fun(Player, string, string) The function that is called when the command is run.
--- @param helpText? string The help text that is displayed when finding the command in the console.
--- @param flags? number FCVAR flags.
function ConsoleCommands.Add(command, callback, helpText, flags)
	registeredCallbacks[command] = callback

	ConsoleCommands.Create(command, helpText, flags or 0)
end

--- Dispatches a command to the appropriate callback.
--- @param client Player The player who ran the command.
--- @param command string The name of the command.
--- @param arguments table The arguments that were passed to the command.
--- @return boolean # Whether or not the command was dispatched.
function ConsoleCommands.Dispatch(client, command, arguments)
    local callback = registeredCallbacks[command]

	if (not callback) then
		return false
	end

    wasCallbackSuccessful, callbackError = xpcall(callback, printError, client, command, arguments)

	if (not wasCallbackSuccessful) then
		printError("ConsoleCommand Error! '" .. tostring(command) .. "' Failed: " .. tostring(callbackError) .. "\n")
	end

	return true
end

--- Removes a command from the list of registered commands.
--- @param command string The name of the command to remove.
function ConsoleCommands.Remove(command)
	if (registeredCallbacks[command]) then
		registeredCallbacks[command] = nil
	end
end
