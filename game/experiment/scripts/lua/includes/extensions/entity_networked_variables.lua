--[[
	Implements network variables for entities like
	Entity:SetNetworked* and Entity:GetNetworked*

	For these datatypes: Angle, Bool, Entity, Float, Int, String, Vector, Var (sends type AND value)

	Summary of how it implements the network variables:
	- When a player connects, they are sent all networked variables for all entities in their PVS
	- When a networked variable is set, they are sent to all networked players in the PVS
	- When a player moves into the PVS of an entity, they are sent all networked variables for that entity (and vice versa)
	- Networked variables are stored in the ENTITY.__networkedVariables table
--]]

if (GAMEUI) then
	return
end

if (SERVER) then
	Networks.AddNetworkString("EntityNetworkedVariable")
	Networks.AddNetworkString("EntityNetworkedVariableSet")
end

local ENTITY_META = _R.Entity
local Hooks = require("hooks")

local debugPrint = function(...)
	if (not EntityNetworkedVariablesDebug) then
		return
	end

	local prefix = CLIENT and "Client]" or "Server]"
	print("[EntityNetworking ", prefix, ...)
end

--- Sets a networked variable on the entity
--- @param key string
--- @param value any
function ENTITY_META:SetNetworkedVariable(key, value)
	if (not self.__networkedVariables) then
		self.__networkedVariables = {}
	end

	self.__networkedVariables[key] = value

	if (not SERVER) then
		-- TODO: Okay so the server should be updating the client sometimes,
		-- otherwise clientside SetNetworkedVariables will differ from the server for too long.
		return
	end

	debugPrint("Setting networked variable", self, key, value)

	Networks.Start("EntityNetworkedVariable")
	Networks.WriteEntity(self)
	Networks.WriteString(key)
	Networks.WriteType(value)
	Networks.BroadcastPvs(self)
end

--- Gets a networked variable from the entity
--- @param key string
--- @param defaultValue any
--- @return any
function ENTITY_META:GetNetworkedVariable(key, defaultValue)
	if (not self.__networkedVariables) then
		return defaultValue
	end

	if (self.__networkedVariables[key] == nil) then
		return defaultValue
	end

	return self.__networkedVariables[key]
end

--- Sets up a callback for when a networked variable changes
--- @param key string
--- @param callback fun(entity: Entity, key: string, oldValue: any, newValue: any)
function ENTITY_META:SetNetworkedVariableCallback(key, callback)
	if (not self.__networkedVarCallbacks) then
		self.__networkedVarCallbacks = {}
	end

	debugPrint("Setting networked variable callback", key)

	self.__networkedVarCallbacks[key] = callback
end

--- Calls all callbacks for the provided key on the entity
--- @param key string
--- @param oldValue any
--- @param newValue any
function ENTITY_META:CallNetworkedVariableCallbacks(key, oldValue, newValue)
	if (not self.__networkedVarCallbacks) then
		return
	end

	debugPrint("Calling networked variable callbacks", key)

	for _, callback in pairs(self.__networkedVarCallbacks) do
		callback(self, key, oldValue, newValue)
	end
end

if (SERVER) then
	Networks.AddNetworkString("EntityNetworkedVariablesSet")

	--- Sends a set of networked variables to a player
	--- @param player Player
	--- @param networkedVariables table
	function ENTITY_META:SendNetworkedVariables(player, networkedVariables)
		Networks.Start("EntityNetworkedVariablesSet")
		Networks.WriteEntity(self)

		Networks.WriteBitLong(table.Count(networkedVariables), 16, false)

		for key, value in pairs(networkedVariables) do
			Networks.WriteString(key)
			Networks.WriteType(value)
		end

		Networks.Send(player)
	end
end

--[[
	For now we simply use [SG]etNetworkedVar for all datatypes, but we should optimize this
	at some point to not need to send the type of the variable every time.

	However since the Networks library is already quite inefficient (sending the whole
	message name as a string every time), this is not a priority.
--]]
local dataTypes = {
	"Angle", "Bool", "Entity", "Float", "Int", "String", "Vector"
}

for _, dataType in ipairs(dataTypes) do
	ENTITY_META["SetNetworked" .. dataType] = ENTITY_META.SetNetworkedVariable
	ENTITY_META["SetNetworked2" .. dataType] = ENTITY_META.SetNetworkedVariable
	ENTITY_META["SetNW" .. dataType] = ENTITY_META.SetNetworkedVariable
	ENTITY_META["SetNW2" .. dataType] = ENTITY_META.SetNetworkedVariable

	ENTITY_META["GetNetworked" .. dataType] = ENTITY_META.GetNetworkedVariable
	ENTITY_META["GetNetworked2" .. dataType] = ENTITY_META.GetNetworkedVariable
	ENTITY_META["GetNW" .. dataType] = ENTITY_META.GetNetworkedVariable
	ENTITY_META["GetNW2" .. dataType] = ENTITY_META.GetNetworkedVariable
end

ENTITY_META.SetNetworkedVarProxy = ENTITY_META.SetNetworkedVariableCallback
ENTITY_META.SetNetworked2VarProxy = ENTITY_META.SetNetworkedVariableCallback
ENTITY_META.SetNWVarProxy = ENTITY_META.SetNetworkedVariableCallback
ENTITY_META.SetNW2VarProxy = ENTITY_META.SetNetworkedVariableCallback

if (SERVER) then
	local bit = require("bitwise")
	local playerForcedUpdateConvar = ConsoleVariables.Create(
		"sv_playerforcedupdate",
		10,
		bit.bor(FCVAR_REPLICATED, FCVAR_ARCHIVE),
		"Time in seconds between forced updates of networked variables for players",
		0
	)
	local nextForcedUpdate = 0

	Hooks.Add("Tick", "__EntityNetworkVariableUpdate", function()
		local curTime = Engines.GetCurrentTime()

		if (curTime < nextForcedUpdate) then
			return
		end

		nextForcedUpdate = curTime + playerForcedUpdateConvar:GetFloat()

		local players = Players.GetAll()

		for _, player in ipairs(players) do
			local entities = Entities.GetInPvs(player:GetPosition())

			for _, entity in ipairs(entities) do
				if (not entity.__networkedVariables) then
					continue
				end

				entity:SendNetworkedVariables(player, entity.__networkedVariables)
			end
		end

		debugPrint("Forced update of networked variables for players")
	end)
else
	--[[
		Receiver for entity networked variables, calls
		'EntityNetworkedVarChanged' hook with the entity, key,
		old value and new value.
	--]]
	Networks.Receive("EntityNetworkedVariable", function(length, socketClient)
		local entity = Networks.ReadEntity()
		local key = Networks.ReadString()
		local newValue = Networks.ReadType()

		debugPrint("Received networked variable", key, newValue, "for", entity)

		if (not IsValid(entity)) then
			-- TODO: Should this happen? We should only be sending to players that are in the PVS of the entity
			-- Erroring for debug, but perhaps we should just ignore it
			debugPrint("Received networked variable for invalid entity")
			return
		end

		local oldValue = entity:GetNetworkedVariable(key)

		entity:CallNetworkedVariableCallbacks(key, oldValue, newValue)

		Hooks.Run("EntityNetworkedVarChanged", entity, key, oldValue, newValue)

		entity:SetNetworkedVariable(key, newValue)
	end)

	Networks.Receive("EntityNetworkedVariablesSet", function(length, socketClient)
		local entity = Networks.ReadEntity()
		local count = Networks.ReadBitLong(16, false)
		local networkedVariables = {}

		for i = 1, count do
			local key = Networks.ReadString()
			local value = Networks.ReadType()

			networkedVariables[key] = value
		end

		debugPrint("(set) Received networked variables for", entity)

		for key, value in pairs(networkedVariables) do
			local oldValue = entity:GetNetworkedVariable(key)

			entity:CallNetworkedVariableCallbacks(key, oldValue, value)

			Hooks.Run("EntityNetworkedVarChanged", entity, key, oldValue, value)
		end

		for key, value in pairs(networkedVariables) do
			entity:SetNetworkedVariable(key, value)
		end
	end)
end
