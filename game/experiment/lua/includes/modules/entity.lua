--[[
	Original code by Team Sandbox:
		Copyleft © 2010, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

_BASE_ENTITY_CLASS = "prop_scripted"

local table = table
local Warning = dbg.Warning

local MODULE = {}

local registeredEntities = {}

--- Returns an entity table
--- @param className string Name of the entity
--- @return table
function MODULE.Get(className)
	local foundEntity = registeredEntities[className]

	if (not foundEntity) then
		return nil
	end

	foundEntity = table.copy(foundEntity)

	if (foundEntity.__base ~= className) then
		local tBaseEntity = MODULE.Get(foundEntity.__base)

		if (not tBaseEntity) then
			Warning("WARNING: Attempted to initialize entity \"" .. className .. "\" with non-existing base class!\n")
		else
			return table.inherit(foundEntity, tBaseEntity)
		end
	end

	return foundEntity
end

--- Returns all registered entities
--- @return table
function MODULE.GetList()
	return registeredEntities
end

--- Registers an entity
--- @param entityTable table Entity table
--- @param className string Name of the entity
--- @param isReloading boolean Whether or not we're reloading this entity data
function MODULE.Register(entityTable, className, isReloading)
	if (MODULE.Get(className) ~= nil and isReloading ~= true) then
		return
	end

	registeredEntities[className] = entityTable
end

return MODULE
