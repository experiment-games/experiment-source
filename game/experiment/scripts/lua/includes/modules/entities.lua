--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

_BASE_ENTITY_CLASS = "prop_scripted"

local Warning = debug.Warning

local MODULE = {}
MODULE.registeredEntities = MODULE.registeredEntities or {}

--- Returns an entity table
--- @param className string Name of the entity
--- @return table
function MODULE.Get(className)
	local foundEntity = MODULE.registeredEntities[className]

	if (not foundEntity) then
		return nil
	end

	foundEntity = table.Copy(foundEntity)

	if (foundEntity.Base ~= className) then
		local baseEntityTable = MODULE.Get(foundEntity.Base)

		if (not baseEntityTable) then
			Warning("WARNING: Attempted to initialize entity \"" .. className .. "\" with non-existing base class!\n")
		else
			return table.Inherit(foundEntity, baseEntityTable)
		end
	end

	return foundEntity
end

--- Returns all registered entities
--- @return table
function MODULE.GetList()
	return MODULE.registeredEntities
end

--- Registers an entity
--- @param entityTable table Entity table
--- @param className string Name of the entity
--- @param isReloading boolean Whether or not we're reloading this entity data
function MODULE.Register(entityTable, className, isReloading)
	if (MODULE.Get(className) ~= nil and isReloading ~= true) then
		return
	end

	MODULE.registeredEntities[className] = entityTable
end

return MODULE
