--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: Dump the size of _G..
--]]

local n = 0
for k, v in pairs(_G) do
	n = n + 1
end

MsgN(n)
