--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local r_eyeset = CreateConsoleVariable("r_eyeset", "1", FCVAR_CLIENTDLL)

-- lua_openscript_cl utilities/eyeset.lua
Hooks.Add("CalcView", "SetPlayerView", function(client, eyeOrigin, eyeAngles, fov, zNear, zFar)
    if (r_eyeset:GetBool() == true) then
        -- Draws the LocalPlayer and sets the view behind.
		return {
			origin = eyeOrigin - (eyeAngles:Forward() * 100),
			angles = eyeAngles,
			fov = fov,
			drawviewer = true
		}
    end
end)
