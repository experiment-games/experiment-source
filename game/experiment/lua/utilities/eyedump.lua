--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

Hooks.Add("CalcPlayerView", "DumpPlayerView", function(client, eyeOrigin, eyeAngles, fov)
	MsgN("eyeOrigin: " .. tostring(eyeOrigin))
	MsgN("eyeAngles: " .. tostring(eyeAngles))
	MsgN("fov: " .. tostring(fov))
	Hooks.Remove("CalcPlayerView", "CalcPlayerView")
end)
