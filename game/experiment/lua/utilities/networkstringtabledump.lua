--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: Dumps a list of all INetworkStringTables
--]]

local t
local i = 0
repeat
	t = NetworkStringTable.GetTable(i)
	if (t == nil) then
		break
	end
	MsgN(t:GetTableName())
	i = i + 1
until t == nil
