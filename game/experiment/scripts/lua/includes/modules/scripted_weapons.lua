_BASE_WEAPON = "weapon_experiment_base_scripted"

local Hooks = require("hooks")
local MODULE = {}
MODULE.registeredWeapons = MODULE.registeredWeapons or {}

--- Returns the registered weapon table for the given weapon class name.
--- @param weaponClassName string
--- @return table?
function MODULE.Get(weaponClassName)
	local weaponTable = MODULE.registeredWeapons[weaponClassName]

	if (not weaponTable) then
		return
	end

	weaponTable = table.Copy(weaponTable)

	--- @cast weaponTable table
	weaponTable.Base = weaponTable.Base or _BASE_WEAPON

	if (weaponTable.Base ~= weaponClassName) then
		local baseWeaponTable = MODULE.GetStored(weaponTable.Base)

		if (not baseWeaponTable) then
			debug.PrintWarning("WARNING: Attempted to initialize weapon \"" ..
				weaponClassName .. "\" with non-existing base class \"" .. tostring(weaponTable.Base) .. "\"!\n")
		else
			return table.Inherit(weaponTable, baseWeaponTable)
		end
	end

	return weaponTable
end

--- Returns the registered weapon table for the given weapon class name.
--- Be careful, this returns a reference to the actual table.
--- @param weaponClassName string
--- @return table?
function MODULE.GetStored(weaponClassName)
	return MODULE.registeredWeapons[weaponClassName]
end

--- Returns a list of all registered weapons.
--- @return table
function MODULE.GetStoredList()
	return MODULE.registeredWeapons
end

--- Returns a list of all registered weapons.
--- @return table
function MODULE.GetList()
	return table.Copy(MODULE.registeredWeapons)
end

--- Checks if the entity class is based on another entity class
--- @param className string Name of the entity
--- @param potentialBaseClassName string Name of the potential base entity
--- @return boolean
function MODULE.IsBasedOn(className, potentialBaseClassName)
	local entityTable = MODULE.GetStored(className)

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

--- Registers a weapon, will not overwrite existing weapons unless isReloading is true.
--- @param weaponTable table
--- @param className string
--- @param isReloading boolean
function MODULE.Register(weaponTable, className, isReloading)
	if (MODULE.GetStored(className) ~= nil and isReloading ~= true) then
		return
	end

    if (type(weaponTable.ClassNameOverride) == "string") then
        className = weaponTable.ClassNameOverride
    end

	weaponTable.ClassName = className

    MODULE.registeredWeapons[className] = weaponTable

	Hooks.Run("ScriptedWeaponRegistered", className, weaponTable)
end

--- Initializes the given reference table for a weapon.
--- @param refTable table
--- @param className string
function MODULE.InitializeRefTable(refTable, className)
	local weaponTable = MODULE.GetStored(className)

	if (not weaponTable) then
		debug.PrintWarning("WARNING: Attempted to initialize weapon \""
			.. className .. "\" with non-existing class!\n")
		return
	end

    table.CopyMerge(refTable, weaponTable)
end

return MODULE
