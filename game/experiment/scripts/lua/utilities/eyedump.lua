--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

Hooks.Add("CalcView", "DumpPlayerView", function(client, eyeOrigin, eyeAngles, fov, zNear, zFar)
	MsgN("eyeOrigin: " .. tostring(eyeOrigin))
	MsgN("eyeAngles: " .. tostring(eyeAngles))
	MsgN("fov: " .. tostring(fov))
	Hooks.Remove("CalcView", "DumpPlayerView")
end)
