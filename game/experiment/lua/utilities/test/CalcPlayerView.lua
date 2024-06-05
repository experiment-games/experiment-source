--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

if (not CLIENT) then
	return
end

local sin = math.sin
local curtime = gpGlobals.curtime

hooks.Add("CalcPlayerView", "CalcPlayerView", function(client, eyeOrigin, eyeAngles, fov)
	-- Give a subtle breathing effect.
	eyeAngles = eyeAngles + QAngle(sin(curtime()) / 2, 0, 0)

	-- FIXME: Field of view is just plain broken here. Use client:SetFOV() to
	-- get around this.

	-- Although ideally you should just return nil for values you don't modify,
	-- however if we return nil for the first value, nothing will change at all.
	-- This is due to our hook system's mechanics (similar to the way Garry's
	-- Mod's hook system works, so left for compatiblity).
	return eyeOrigin, eyeAngles, fov
end)
