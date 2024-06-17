--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local EYE_PRESET_ORIGIN = Vector(27.044645, -906.119995, 165.712967)
local EYE_PRESET_ANGLES = Angle(-7.722018, 102.617867, 0.000000)
local EYE_PRESET_FOV = 90

local r_eyeset = ConVar("r_eyeset", "0", FCVAR_CLIENTDLL)

Hooks.Add("CalcPlayerView", "SetPlayerView", function(client, eyeOrigin, eyeAngles, fov)
	if (r_eyeset:GetBool() == true) then
		-- FIXME: eyeOrigin isn't getting set (overriden) correctly.
		eyeOrigin = EYE_PRESET_ORIGIN
		eyeAngles = EYE_PRESET_ANGLES
		fov = EYE_PRESET_FOV
	end
	return eyeOrigin, eyeAngles, fov
end)
