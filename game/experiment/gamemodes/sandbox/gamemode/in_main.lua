--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: builds an intended command to send to the server
--]]

require("concommand")

local function IN_BuildDown(pPlayer, pCmd, args)
	if (gBuildMenuInterface) then
		gBuildMenuInterface:ShowPanel(true)
	end
end

concommand.Add("+buildmenu", IN_BuildDown, nil)

local function IN_BuildUp(pPlayer, pCmd, args)
	if (gBuildMenuInterface) then
		gBuildMenuInterface:ShowPanel(false)
	end
end

concommand.Add("-buildmenu", IN_BuildUp, nil)
