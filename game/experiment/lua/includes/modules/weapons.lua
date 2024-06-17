--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

_BASE_WEAPON = "weapon_hl2mpbase_scriptedweapon"

local Warning = debug.Warning

local MODULE = {}
MODULE.registeredWeapons = MODULE.registeredWeapons or {}

--- Returns the registered weapon table for the given weapon class name.
--- @param weaponClassName string
--- @return table|nil
function MODULE.Get(weaponClassName)
	local weaponTable = MODULE.registeredWeapons[weaponClassName]

	if (not weaponTable) then
		return nil
	end

	weaponTable = table.Copy(weaponTable)

	if (weaponTable.Base ~= weaponClassName) then
		local baseWeaponTable = MODULE.Get(weaponTable.Base)

		if (not baseWeaponTable) then
			Warning("WARNING: Attempted to initialize weapon \"" ..
			weaponClassName .. "\" with non-existing base class!\n")
		else
			return table.Inherit(weaponTable, baseWeaponTable)
		end
	end

	return weaponTable
end

--- Returns all registered weapons.
--- Be careful, this returns a reference to the actual table.
--- @return table
function MODULE.GetStored()
	return MODULE.registeredWeapons
end

--- Returns a list of all registered weapons.
--- @return table
function MODULE.GetList()
	return table.Copy(MODULE.registeredWeapons)
end

--- Registers a weapon.
--- @param weaponTable table
--- @param className string
--- @param isReloading boolean
function MODULE.Register(weaponTable, className, isReloading)
	if (MODULE.Get(className) ~= nil and isReloading ~= true) then
		return
	end

	MODULE.registeredWeapons[className] = weaponTable
end

return MODULE
