--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: builds an intended command to send to the server
--]]

require("concommands")

local function IN_BuildDown(client, pCmd, args)
	if (gBuildMenuInterface) then
		gBuildMenuInterface:ShowPanel(true)
	end
end

concommands.Add("+buildmenu", IN_BuildDown, nil)

local function IN_BuildUp(client, pCmd, args)
	if (gBuildMenuInterface) then
		gBuildMenuInterface:ShowPanel(false)
	end
end

concommands.Add("-buildmenu", IN_BuildUp, nil)
