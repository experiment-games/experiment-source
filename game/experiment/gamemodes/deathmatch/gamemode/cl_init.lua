--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

include("shared.lua")

local AngleVectors = mathlib.AngleVectors

-- Have the flashlight appear to come from the weapon's muzzle flash position
function GM:PlayerUpdateFlashlight(client, position, vecForward, vecRight, vecUp, nDistance)
	local vm = client:GetViewModel()

	if (vm == NULL) then
		return
	end

	local iAttachment = vm:LookupAttachment("muzzle")

	if (iAttachment < 0) then
		return
	end

	local vecOrigin = Vector()

	--Tony; EyeAngles will return proper whether it's local player or not.
	local eyeAngles = client:EyeAngles()

	vm:GetAttachment(iAttachment, vecOrigin, eyeAngles)

	local vForward = Vector()
	AngleVectors(eyeAngles, vecForward, vecRight, vecUp)
	position = vecOrigin

	return position, vecForward, vecRight, vecUp, nDistance
end
