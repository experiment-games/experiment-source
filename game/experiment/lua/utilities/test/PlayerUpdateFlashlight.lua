--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

if (not CLIENT) then
	return
end

local AngleVectors = mathlib.AngleVectors

hook.Add("PlayerUpdateFlashlight", "PlayerUpdateWeaponFlashlight",
	function(client, position, vecForward, vecRight, vecUp, nDistance)
		--if ( not input.CAM_IsThirdPerson() ) then
		local vm = client:GetViewModel()
		if (vm ~= NULL) then
			local iAttachment = vm:LookupAttachment("muzzle")

			if (iAttachment >= 0) then
				local vecOrigin = Vector()

				--Tony; EyeAngles will return proper whether it's local player or not.
				local eyeAngles = client:EyeAngles()

				print("before", vecOrigin, eyeAngles)
				vm:GetAttachment(iAttachment, vecOrigin, eyeAngles)
				print("after", vecOrigin, eyeAngles)

				local vForward = Vector()
				AngleVectors(eyeAngles, vecForward, vecRight, vecUp)
				position = vecOrigin

				return position, vecForward, vecRight, vecUp, nDistance
			end
		end
		--end
	end
)
