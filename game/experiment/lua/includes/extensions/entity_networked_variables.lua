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

local ENTITY_META = _R.CBaseEntity
local Networks = require("networks")
local Hooks = require("hooks")

local debugPrint = function(...)
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

    Networks.Start("EntityNetworkedVar")
    Networks.WriteEntity(self)
    Networks.WriteString(key)
    Networks.WriteType(value)
    Networks.BroadcastPVS(self)
end

--- Gets a networked variable from the entity
--- @param key string
--- @return any
function ENTITY_META:GetNetworkedVariable(key)
    if (not self.__networkedVariables) then
        return nil
    end

    return self.__networkedVariables[key]
end

--- Sets up a callback for when a networked variable changes
--- @param key string
--- @param callback fun(entity: CBaseEntity, key: string, oldValue: any, newValue: any)
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

--[[
	Receiver for entity networked variables, calls
	'EntityNetworkedVarChanged' hook with the entity, key,
	old value and new value.
--]]
Networks.Receive("EntityNetworkedVar", function(length, socketClient)
	local entity = Networks.ReadEntity()
	local key = Networks.ReadString()
    local newValue = Networks.ReadType()

	debugPrint("Received networked variable", key, newValue, "for", entity)

    if (not IsValid(entity)) then
        -- TODO: Should this happen? We should only be sending to players that are in the PVS of the entity
		-- Erroring for debug, but perhaps we should just ignore it
		error("Received networked variable for invalid entity")
		return
	end

    local oldValue = entity:GetNetworkedVariable(key)

	entity:CallNetworkedVariableCallbacks(key, oldValue, newValue)

	Hooks.Call("EntityNetworkedVarChanged", entity, key, oldValue, newValue)

	entity:SetNetworkedVariable(key, newValue)
end)
