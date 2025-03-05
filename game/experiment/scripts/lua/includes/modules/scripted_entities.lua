_BASE_ENTITY_CLASS = "prop_scripted"

local Hooks = require("hooks")
local MODULE = {}
MODULE.registeredEntities = MODULE.registeredEntities or {}

-- TODO: This file has a lot of similar code to scripted_weapons.lua, reduce duplication

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
			debug.PrintWarning("WARNING: Attempted to initialize entity \"" .. className .. "\" with non-existing base class \"" .. tostring(foundEntity.Base) .. "\"!\n")
		else
			return table.Inherit(foundEntity, baseEntityTable)
		end
	end

	return foundEntity
end

--- Returns all registered entities.
--- Be careful, this returns a reference to the actual table.
--- @return table
function MODULE.GetStored()
	return MODULE.registeredEntities
end

--- Returns a list of all registered entities.
--- @return table
function MODULE.GetList()
	return table.Copy(MODULE.registeredEntities)
end

--- Checks if the entity class is based on another entity class
--- @param className string Name of the entity
--- @param potentialBaseClassName string Name of the potential base entity
--- @return boolean
function MODULE.IsBasedOn(className, potentialBaseClassName)
	local entityTable = MODULE.Get(className)

	if (not entityTable) then
		return false
	end

	if (entityTable.Base == potentialBaseClassName) then
		return true
	end

	if (entityTable.Base ~= className) then
		return MODULE.IsBasedOn(entityTable.Base, potentialBaseClassName)
	end

	return false
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

	Hooks.Run("ScriptedEntityRegistered", className, entityTable)
end

return MODULE
