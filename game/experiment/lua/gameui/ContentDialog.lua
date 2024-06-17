--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

Include("../includes/extensions/table.lua")
Include("../includes/extensions/gui.lua")


local CContentDialog = {}

-------------------------------------------------------------------------------
-- Purpose: Basic help dialog
-------------------------------------------------------------------------------
function CContentDialog:Init(parent, panelName)
	self:SetDeleteSelfOnClose(true)
	self:SetBounds(0, 0, 512, 406)
	self:SetSizeable(false)

	self:SetTitle("#GameUI_Content", true)

	-- debug timing code, this function takes too long
	--	local s4 = system.GetCurrentTime()

	self:AddPage(Gui.CContentSubGames(self, ""), "#GameUI_Games")
	-- self:AddPage(Gui.CContentSubAddons(self, ""), "#GameUI_Addons")

	--	local s5 = system.GetCurrentTime()
	--	Msg("CContentDialog:Init(): " .. (s5 - s4) * 1000.0 .. "ms\n")

	self:SetApplyButtonVisible(true)
	-- self:GetPropertySheet():SetTabWidth(84)
end

-------------------------------------------------------------------------------
-- Purpose: Opens the dialog
-------------------------------------------------------------------------------
function CContentDialog:Run()
	self:SetTitle("#GameUI_Content", true)
	self:Activate()
end

Gui.Register(CContentDialog, "CContentDialog", "PropertyDialog")
