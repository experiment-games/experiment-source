--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

hook.Add("CalcPlayerView", "DumpPlayerView", function(pPlayer, eyeOrigin, eyeAngles, fov)
	MsgN("eyeOrigin: " .. tostring(eyeOrigin))
	MsgN("eyeAngles: " .. tostring(eyeAngles))
	MsgN("fov: " .. tostring(fov))
	hook.Remove("CalcPlayerView", "CalcPlayerView")
end)
