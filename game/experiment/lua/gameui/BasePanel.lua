--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

Include("../includes/extensions/panel.lua")

local FCVAR_CLIENTDLL = _E.FCVAR.CLIENTDLL

local ConsoleCommands = require("console_commands")

local hContentDialog = INVALID_PANEL

local function PositionDialog(dlg)
	if (dlg == INVALID_PANEL) then
		return
	end

	local x, y, ww, wt, wide, tall
	x, y, ww, wt = Surface.GetWorkspaceBounds()
	wide, tall = dlg:GetSize()

	-- Center it, keeping requested size
	dlg:SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2))
end

local function OnOpenContentDialog()
	if (ToPanel(hContentDialog) == INVALID_PANEL) then
		hContentDialog = Gui.CContentDialog(Gui.GetGameUIPanel(), "ContentDialog")
		PositionDialog(hContentDialog)
	end
	hContentDialog:Activate()
end

ConsoleCommands.Add("OpenContentDialog", OnOpenContentDialog, "Open content dialog.", FCVAR_CLIENTDLL)
