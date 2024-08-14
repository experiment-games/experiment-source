--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

if (GAMEUI) then
	return
end

local printError = debug.PrintError

function ToBaseEntity(pEntity)
	if (not pEntity or type(pEntity) ~= "entity") then
		return NULL
	end

	local success, hEntity = xpcall(_R.Entity.GetBaseEntity, printError, pEntity)
	if (not success) then
		hEntity = NULL
	end
	if _DEBUG then
		assert(hEntity ~= NULL)
	end

	return hEntity
end

local ENTITY_META = _R.CBaseAnimating

-- Source has inconsistent naming conventions for these, lets just support both name styles.
ENTITY_META.FindBodygroupByName = ENTITY_META.FindBodyGroupByName
ENTITY_META.GetBodygroup = ENTITY_META.GetBodyGroup
ENTITY_META.GetBodygroupCount = ENTITY_META.GetBodyGroupCount
ENTITY_META.GetBodygroupName = ENTITY_META.GetBodyGroupName
ENTITY_META.GetBodygroups = ENTITY_META.GetBodyGroups
ENTITY_META.SetBodygroup = ENTITY_META.SetBodyGroup
ENTITY_META.SetBodygroups = ENTITY_META.SetBodyGroups
ENTITY_META.GetNumBodygroups = ENTITY_META.GetNumBodyGroups
