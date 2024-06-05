--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

_BASE_WEAPON = "weapon_hl2mpbase_scriptedweapon"

local table = table
local Warning = dbg.Warning

local MODULE = {}

local tWeapons = {}

-------------------------------------------------------------------------------
-- Purpose: Returns a weapon table
-- Input  : strName - Name of the weapon
-- Output : table
-------------------------------------------------------------------------------
function MODULE.Get(strClassname)
	local tWeapon = tWeapons[strClassname]

	if (not tWeapon) then
		return nil
	end

	tWeapon = table.Copy(tWeapon)

	if (tWeapon.__base ~= strClassname) then
		local tBaseWeapon = get(tWeapon.__base)

		if (not tBaseWeapon) then
			Warning("WARNING: Attempted to initialize weapon \"" .. strClassname .. "\" with non-existing base class!\n")
		else
			return table.Inherit(tWeapon, tBaseWeapon)
		end
	end

	return tWeapon
end

-------------------------------------------------------------------------------
-- Purpose: Returns all registered weapons
-- Input  :
-- Output : table
-------------------------------------------------------------------------------
function MODULE.GetStored()
	return tWeapons
end

function MODULE.GetList()
	return table.Copy(tWeapons)
end

-------------------------------------------------------------------------------
-- Purpose: Registers a weapon
-- Input  : tWeapon - Weapon table
--          strClassname - Name of the weapon
--          bReload - Whether or not we're reloading this weapon data
-- Output :
-------------------------------------------------------------------------------
function MODULE.Register(tWeapon, strClassname, bReload)
	if (MODULE.Get(strClassname) ~= nil and bReload ~= true) then
		return
	end
	tWeapons[strClassname] = tWeapon
end

return MODULE
