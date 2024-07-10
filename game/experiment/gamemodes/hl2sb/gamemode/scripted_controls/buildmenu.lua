--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local CBuildSubMenu = Gui.CBuildSubMenu
local ScreenWidth = Util.ScreenWidth
local ScreenHeight = Util.ScreenHeight

local CBuildMenu = {
	m_hFonts = {},
	m_lastx = nil,
	m_lasty = nil,
}

-------------------------------------------------------------------------------
-- Purpose: Constructor
-------------------------------------------------------------------------------
function CBuildMenu:Init(pViewPort)
	self:SetProportional(true)

	self:SetDock( DOCK_TYPE.FILL )
	self:SetAutoDelete(false)

	self.m_pViewPort = pViewPort

	self.m_pImage = Gui.CImagePanel(self, "test_image")
	self.m_pImage:SetImage("silkicons/monkey.png")
	self.m_pImage:SizeToContents()
	self.m_pImage:SetPos(
		(ScreenWidth() - self.m_pImage:GetWide()) * .5,
		(ScreenHeight() - self.m_pImage:GetTall()) * .5
	)

	if false then -- TODO: Implement this
		self.m_pMainMenu = Gui.CBuildSubMenu(self, "mainmenu")
		self.m_pMainMenu:LoadControlSettings("Resource/UI/MainBuyMenu.res")
		self.m_pMainMenu:SetVisible(false)
	end

	self:SetVisible(false)
end

function CBuildMenu:ApplySchemeSettings(pScheme)
	self:SetBgColor(Color(0, 0, 0, 80))
end

-------------------------------------------------------------------------------
-- Purpose: shows/hides the build menu
-------------------------------------------------------------------------------
function CBuildMenu:ShowPanel(bShow)
	if (self:IsVisible() == bShow) then
		return
	end

	if (bShow) then
		self:Update()

		self:SetMouseInputEnabled(true)
		self:SetVisible(true)
		self:MakePopup()

		if (self.m_lastx and self.m_lasty) then
			Input.SetCursorPos(self.m_lastx, self.m_lasty)
		end
	else
		self.m_lastx, self.m_lasty = Input.GetCursorPos()

		self:SetVisible(false)
		self:SetMouseInputEnabled(false)
	end
end

function CBuildMenu:Update()
end

function CBuildMenu:OnClose()
	self.BaseClass.OnClose(self)
end

Gui.Register(CBuildMenu, "CBuildMenu", "Panel")
