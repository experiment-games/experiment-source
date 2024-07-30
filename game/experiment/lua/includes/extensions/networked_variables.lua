--[[
	Implements network variables for global variables.

	For these datatypes: Angle, Bool, Entity, Float, Int, String, Vector, Var (sends type AND value)
--]]

if (GAMEUI) then
	return
end

local Networks = require("networks")
local Hooks = require("hooks")

local debugPrint = function(...)
	if (not EntityNetworkedVariablesDebug) then
		return
	end

	local prefix = CLIENT and "Client]" or "Server]"
	print("[EntityNetworking ", prefix, ...)
end

local globalVariables = {}

--- Sets a global networked variable
--- @param key string
--- @param value any
function SetGlobalVariable(key, value)
	globalVariables[key] = value

	if (not SERVER) then
		return
	end

	debugPrint("Setting global networked variable", key, value)

	Networks.Start("GlobalNetworkedVariable")
	Networks.WriteString(key)
	Networks.WriteType(value)
	Networks.Broadcast()
end

--- Gets a global networked variable
--- @param key string
--- @return any
function GetGlobalVariable(key)
	return globalVariables[key]
end

--- Sets up a callback for when a global networked variable changes
--- @param key string
--- @param callback fun(key: string, oldValue: any, newValue: any)
function SetGlobalVariableCallback(key, callback)
	if (not globalVariables.__callbacks) then
		globalVariables.__callbacks = {}
	end

	debugPrint("Setting global networked variable callback", key)

	globalVariables.__callbacks[key] = callback
end

--- Calls all callbacks for the provided key
--- @param key string
--- @param oldValue any
--- @param newValue any
function CallGlobalVariableCallbacks(key, oldValue, newValue)
	if (not globalVariables.__callbacks) then
		return
	end

	debugPrint("Calling global networked variable callbacks", key)

	for _, callback in pairs(globalVariables.__callbacks) do
		callback(key, oldValue, newValue)
	end
end

if (SERVER) then
	--- Sends a set of global networked variables to a player
	--- @param player Player
	--- @param networkedVariables table
	function SendGlobalVariables(player, networkedVariables)
		Networks.Start("GlobalNetworkedVariablesSet")
		Networks.WriteUInt(table.Count(networkedVariables), 16)

		for key, value in pairs(networkedVariables) do
			Networks.WriteString(key)
			Networks.WriteType(value)
		end

		Networks.Send(player)
	end
end

if (SERVER) then
	Hooks.Add("PlayerInitialSpawn", "__EntityNetworkGlobalVariableUpdate", function(player)
		SendGlobalVariables(player, globalVariables)
	end)
else
	Networks.Receive("GlobalNetworkedVariable", function(length, socketClient)
		local key = Networks.ReadString()
		local newValue = Networks.ReadType()

		debugPrint("Received global networked variable", key, newValue)

		local oldValue = GetGlobalVariable(key)

		CallGlobalVariableCallbacks(key, oldValue, newValue)

		Hooks.Call("GlobalNetworkedVarChanged", key, oldValue, newValue)

		SetGlobalVariable(key, newValue)
	end)

	Networks.Receive("GlobalNetworkedVariablesSet", function(length, socketClient)
		local count = Networks.ReadUInt(16)
		local networkedVariables = {}

		for i = 1, count do
			local key = Networks.ReadString()
			local value = Networks.ReadType()

			networkedVariables[key] = value
		end

		debugPrint("(set) Received global networked variables")

		for key, value in pairs(networkedVariables) do
			local oldValue = GetGlobalVariable(key)

			CallGlobalVariableCallbacks(key, oldValue, value)

			Hooks.Call("GlobalNetworkedVarChanged", key, oldValue, value)
		end

		for key, value in pairs(networkedVariables) do
			SetGlobalVariable(key, value)
		end
	end)
end

local dataTypes = {
	"Angle", "Bool", "Entity", "Float", "Int", "String", "Vector"
}

for _, dataType in ipairs(dataTypes) do
	_G["SetGlobal" .. dataType] = SetGlobalVariable
	_G["SetGlobal2" .. dataType] = SetGlobalVariable
	_G["GetGlobal" .. dataType] = GetGlobalVariable
	_G["GetGlobal2" .. dataType] = GetGlobalVariable
end

GetGlobalVar = GetGlobalVariable
SetGlobalVar = SetGlobalVariable
