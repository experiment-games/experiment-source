--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

Include("../includes/extensions/panel.lua")

local FCVAR_CLIENTDLL = _E.FCVAR.CLIENTDLL
local hContentDialog = INVALID_PANEL

local function PositionDialog(dlg)
	if (dlg == INVALID_PANEL) then
		return
	end

	local x, y, ww, wt, wide, tall
	x, y, ww, wt = Surface.GetWorkspaceBounds()
	wide, tall = dlg:GetSize()

	-- Center it, keeping requested size
	dlg:SetPosition(x + ((ww - wide) / 2), y + ((wt - tall) / 2))
end

