--[[
	Enumerations

	TODO: Move these enum definitions to the C-side so they don't misalign.
--]]

--- @enum PIN_CORNER
PIN_CORNER = {
	TOP_LEFT = 0,
	TOP_RIGHT = 1,
	BOTTOM_LEFT = 2,
	BOTTOM_RIGHT = 3,
	CENTER_TOP = 4,
	CENTER_RIGHT = 5,
	CENTER_BOTTOM = 6,
	CENTER_LEFT = 7,
	LAST = 8
}

--- @enum AUTO_RESIZE
AUTO_RESIZE = {
	NO = 0,
	RIGHT = 1,
	DOWN = 2,
	DOWN_AND_RIGHT = 3
}

--- @enum DOCK_TYPE
DOCK_TYPE = {
	NONE = 0,
	FILL = 1,
	LEFT = 2,
	RIGHT = 3,
	TOP = 4,
	BOTTOM = 5,
}

--[[
	Helper functions
--]]

--- Converts the given value to a Panel.
--- Original code by Team Sandbox:
--- 	Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.
--- Modified for Experiment.
--- @param pPanel any
--- @return unknown
function ToPanel(pPanel)
	if (not pPanel or type(pPanel) ~= "Panel") then
		return INVALID_PANEL
	end

	local success, hPanel = xpcall(_R.Panel.GetVPanel, debug.PrintError, pPanel)

	if (not success) then
		hPanel = INVALID_PANEL
	end

	if _DEBUG then
		assert(hPanel ~= INVALID_PANEL)
	end

	return hPanel
end

--[[
	Panel metatable extensions
--]]
local PANEL = _R.Panel

--[[

	-- These tests worked and show how the pinning and resizing works:

	local parent = Gui.Create("Frame")
	local parentWide, parentTall = 256, 256
	parent:SetBgColor(Color(255, 0, 0, 255))
	parent:SetSize(parentWide, parentTall)
	parent:SetPos(50, 50)
	parent:SetTitle("Parent panel")

	parent:MakePopup()
	parent:SetVisible(true)

	local child = Gui.Create("Button", parent)
	local dockPadding = 10
	local childWide, childTall = 128, 128
	child:SetText("Docked button")

	function child:OnClick()
		print("Clicked", self)
		-- parent:DeletePanel() -- Will crash with access violation, as repaint will be called on release of button
		parent:MarkForDeletion() -- safer
	end

	-- Attached to top left, without stretching (default, no docking):
	child:SetBounds(dockPadding, dockPadding, childWide, childTall)
	child:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.NO, dockPadding, dockPadding, 0, 0)

	-- Dock to top right, without stretching:
	-- child:SetBounds(parentWide - childWide - dockPadding, dockPadding, childWide, childTall)
	-- child:SetAutoResize(PIN_CORNER.TOP_RIGHT, AUTO_RESIZE.NO, -dockPadding, dockPadding, 0, 0)

	-- Dock to bottom right, without stretching:
	-- child:SetBounds(parentWide - childWide - dockPadding, parentTall - childTall - dockPadding, childWide, childTall)
	-- child:SetAutoResize(PIN_CORNER.BOTTOM_RIGHT, AUTO_RESIZE.NO, -dockPadding, -dockPadding, 0, 0)

	-- Dock to bottom left, without stretching:
	-- child:SetBounds(dockPadding, parentTall - childTall - dockPadding, childWide, childTall)
	-- child:SetAutoResize(PIN_CORNER.BOTTOM_LEFT, AUTO_RESIZE.NO, dockPadding, -dockPadding, 0, 0)

	-- Dock to top left, without stretching:
	-- child:SetBounds(dockPadding, dockPadding, childWide, childTall)
	-- child:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.NO, dockPadding, dockPadding, 0, 0)

	-- Dock to fill, stretching:
	-- child:SetBounds(dockPadding, dockPadding, parentWide - dockPadding * 2, parentTall - dockPadding * 2)
	-- child:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.DOWN_AND_RIGHT, dockPadding, dockPadding, -dockPadding, -dockPadding)

	-- Dock to left, stretching down:
	-- child:SetBounds(dockPadding, dockPadding, childWide, parentTall - dockPadding * 2)
	-- child:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.DOWN, dockPadding, dockPadding, 0, -dockPadding)

	-- Dock to right, stretching down:
	-- child:SetBounds(parentWide - childWide - dockPadding, dockPadding, childWide, parentTall - dockPadding * 2)
	-- child:SetAutoResize(PIN_CORNER.TOP_RIGHT, AUTO_RESIZE.DOWN, -dockPadding, dockPadding, 0, -dockPadding)

	-- Dock to top, stretching right:
	-- child:SetBounds(dockPadding, dockPadding, parentWide - dockPadding * 2, childTall)
	-- child:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.RIGHT, dockPadding, dockPadding, -dockPadding, 0)

	-- Dock to bottom, stretching right:
	-- child:SetBounds(dockPadding, parentTall - childTall - dockPadding, parentWide - dockPadding * 2, childTall)
	-- child:SetAutoResize(PIN_CORNER.BOTTOM_LEFT, AUTO_RESIZE.RIGHT, dockPadding, -dockPadding, -dockPadding, 0)

--]]

--- Set dock margins for the panel.
--- @param left number
--- @param top number
--- @param right number
--- @param bottom number
function PANEL:SetDockMargins(left, top, right, bottom)
    self._dockMarginLeft = left
    self._dockMarginTop = top
    self._dockMarginRight = right
    self._dockMarginBottom = bottom
end

--- Get the dock margins for the panel.
--- @return number, number, number, number
function PANEL:GetDockMargins()
    return self._dockMarginLeft, self._dockMarginTop, self._dockMarginRight, self._dockMarginBottom
end

--- Set the dock padding for the panel.
--- @param left number
--- @param top number
--- @param right number
--- @param bottom number
function PANEL:SetDockPadding(left, top, right, bottom)
    self._dockPaddingLeft = left
    self._dockPaddingTop = top
    self._dockPaddingRight = right
    self._dockPaddingBottom = bottom
end

--- Get the dock padding for the panel.
--- @return number, number, number, number
function PANEL:GetDockPadding()
	return self._dockPaddingLeft, self._dockPaddingTop, self._dockPaddingRight, self._dockPaddingBottom
end

--- Set the dock type for the panel.
--- @param dockType DOCK_TYPE
function PANEL:SetDock(dockType)
    self._dockType = dockType
end

--- Get the dock type for the panel.
--- @return DOCK_TYPE
function PANEL:GetDock()
    return self._dockType
end

--- This function is called to update the panel's position and size based on the dock settings.
function PANEL:UpdateDocking()
	local parent = self:GetParent()

	if (not parent) then
		return
	end

	local parentWide, parentTall = parent:GetSize()
	-- local parentX, parentY = parent:GetPos()
    -- local left, top, right, bottom = self:GetDockMargins()
	local leftPadding, topPadding, rightPadding, bottomPadding = self:GetDockPadding()
	local dockType = self:GetDock()
    local newWidth, newHeight
    local newX, newY

    -- Calculate the new position and size based on dock type
    if dockType == DOCK_TYPE.FILL then
        newX, newY = leftPadding, topPadding
        newWidth = parentWide - leftPadding - rightPadding
        newHeight = parentTall - topPadding - bottomPadding

		self:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.DOWN_AND_RIGHT, leftPadding, topPadding, -rightPadding, -bottomPadding)
    elseif dockType == DOCK_TYPE.LEFT then
        newX, newY = leftPadding, topPadding
        newWidth = (parentWide / 2) - leftPadding - rightPadding
        newHeight = parentTall - topPadding - bottomPadding

		self:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.DOWN, leftPadding, topPadding, 0, -bottomPadding)
    elseif dockType == DOCK_TYPE.RIGHT then
        newX = (parentWide / 2) + leftPadding
        newY = topPadding
        newWidth = (parentWide / 2) - leftPadding - rightPadding
        newHeight = parentTall - topPadding - bottomPadding

		self:SetAutoResize(PIN_CORNER.TOP_RIGHT, AUTO_RESIZE.DOWN, -rightPadding, topPadding, 0, -bottomPadding)
    elseif dockType == DOCK_TYPE.TOP then
        newX, newY = leftPadding, topPadding
        newWidth = parentWide - leftPadding - rightPadding
        newHeight = (parentTall / 2) - topPadding - bottomPadding

		self:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.RIGHT, leftPadding, topPadding, -rightPadding, 0)
    elseif dockType == DOCK_TYPE.BOTTOM then
        newX, newY = leftPadding, (parentTall / 2) + topPadding
        newWidth = parentWide - leftPadding - rightPadding
        newHeight = (parentTall / 2) - topPadding - bottomPadding

		self:SetAutoResize(PIN_CORNER.BOTTOM_LEFT, AUTO_RESIZE.RIGHT, leftPadding, -bottomPadding, -rightPadding, 0)
    else
        newX, newY = 0, 0
        newWidth = self:GetWidth()
        newHeight = self:GetHeight()

		self:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.NO, newX, newY, 0, 0)
    end

    -- Apply the calculated position and size
    self:SetBounds(newX, newY, newWidth, newHeight)
end
