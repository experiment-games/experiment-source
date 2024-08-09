# Hooks Library

/*

--- Adds a hook to the given GameRules function.
--- @param eventName string The name of the internal GameRules method.
--- @param hookIdentifier string The name of the hook.
--- @param callback fun(...): ... The function to call when the hook is called.
function Hooks.Add(eventName, hookIdentifier, callback)
	Hooks.registeredHooks[eventName] = Hooks.registeredHooks[eventName] or {}
	Hooks.registeredHooks[eventName][hookIdentifier] = callback
end

--- Calls all hooks associated with a specific event.
--- @param eventName string The name of the internal GameRules method.
--- @param gamemodeTable table The table of the current gamemode.
--- @vararg any Arguments to pass to the Hooks.
--- @return any # The return value(s) of the hook.
function Hooks.Call(eventName, gamemodeTable, ...)
	local callbacks = Hooks.registeredHooks[eventName]

	if (callbacks ~= nil) then
		for hookIdentifier, callback in pairs(callbacks) do
			if (callback == nil) then
				printError("Hook Error! '" ..
					tostring(hookIdentifier) .. "' (" .. tostring(eventName) .. ") tried to call a nil function!\n")
				-- callbacks[hookIdentifier] = nil
				break
			end

			-- String identifiers are handled normally
			if (isstring(hookIdentifier)) then
				returnValues = { xpcall(callback, printError, ...) }

				if (returnValues[1] == false) then
					printError("Hook Error! '" ..
						tostring(hookIdentifier) .. "' (" .. tostring(eventName) .. ") Failed\n")
					-- callbacks[hookIdentifier] = nil
				elseif (returnValues[2] ~= nil) then
					return unpack(returnValues, 2)
				end
			else
				-- Non-string identifiers are expected to be an object which responds to IsValid
				-- In that case we check if they're still valid and insert them before all other arguments
				if (IsValid(hookIdentifier)) then
					returnValues = { xpcall(callback, printError, hookIdentifier, ...) }

					if (returnValues[1] == false) then
						printError("Hook Error! '" ..
							tostring(hookIdentifier) .. "' (" .. tostring(eventName) .. ") Failed\n")
						-- callbacks[hookIdentifier] = nil
					elseif (returnValues[2] ~= nil) then
						return unpack(returnValues, 2)
					end
				else
					-- Since they're no longer valid, unset them for future calls
					callbacks[hookIdentifier] = nil
				end
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
--- @vararg any Arguments to pass to the Hooks.
--- @return any # The return value(s) of the hook.
function Hooks.Run(eventName, ...)
	return Hooks.Call(eventName, _G.GAMEMODE, ...)
end

--- Returns all of the registered hooks or only hooks pertaining to a specific event.
--- @param eventName string The name of the internal GameRules method.
--- @return table
function Hooks.GetTable(eventName)
	if (eventName) then
		return Hooks.registeredHooks[eventName]
	end

	return Hooks.registeredHooks
end

--- Removes a hook from the list of registered Hooks.
--- @param eventName string The name of the internal GameRules method.
--- @param hookIdentifier string The name of the hook.
function Hooks.Remove(eventName, hookIdentifier)
	if (not Hooks.registeredHooks[eventName]) then
		return
	end

	if (Hooks.registeredHooks[eventName][hookIdentifier]) then
		Hooks.registeredHooks[eventName][hookIdentifier] = nil
	end
end

*/

The Hooks library is implemented in Lua as a [module](../../modules/standard.md) that allows you to attach code to certain events in the game. This allows you to run custom code when certain things happen in the game.
