--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

Include("shared.lua")

local AngleVectors = math.AngleVectors

-- Have the flashlight appear to come from the weapon's muzzle flash position
function GM:PlayerUpdateFlashlight(client, position, vecForward, vecRight, vecUp, nDistance)
	local vm = client:GetViewModel()

	if (vm == NULL) then
		return
	end

	local attachmentId = vm:LookupAttachment("muzzle")

	if (attachmentId < 0) then
		return
	end

	local origin = Vector()

	--Tony; EyeAngles will return proper whether it's local player or not.
	local eyeAngles = client:GetEyeAngles()

	vm:GetAttachment(attachmentId, origin, eyeAngles)

	AngleVectors(eyeAngles, vecForward, vecRight, vecUp)
	position = origin

	return position, vecForward, vecRight, vecUp, nDistance
end
