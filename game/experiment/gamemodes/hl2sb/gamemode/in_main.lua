--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: builds an intended command to send to the server
--]]

local function IN_BuildDown(client, pCmd, args)
	if (gBuildMenuInterface) then
		gBuildMenuInterface:ShowPanel(true)
	end
end

ConsoleCommands.Add("+menu", IN_BuildDown, nil)

local function IN_BuildUp(client, pCmd, args)
	if (gBuildMenuInterface) then
		gBuildMenuInterface:ShowPanel(false)
	end
end

ConsoleCommands.Add("-menu", IN_BuildUp, nil)
