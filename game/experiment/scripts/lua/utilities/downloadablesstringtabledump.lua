--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: Dumps a list of all userdata in the downloadables string table
--]]

local t = NetworkStringTable.FindTable("downloadables")
MsgN(t)
for i = 1, t:GetNumStrings() do
	MsgN("\t" .. i .. ": " .. tostring(t:GetString(i)))
end
