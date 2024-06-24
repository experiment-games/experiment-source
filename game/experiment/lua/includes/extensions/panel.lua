--[[
	Enumerations

	TODO: Move this to the C-side so they don't misalign.
--]]

--- @enum PIN_CORNER
PIN_CORNER = {
	TOPLEFT = 0,
	TOPRIGHT = 1,
	BOTTOMLEFT = 2,
	BOTTOMRIGHT = 3,
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
	DOWNANDRIGHT = 3
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

--- Converts the given dock type to the corresponding pin corner and auto resize type.
local function convertDockTypeToAutoSize(dockType)
	if (dockType == DOCK_TYPE.FILL) then
		return PIN_CORNER.TOPLEFT, AUTO_RESIZE.DOWNANDRIGHT
	elseif (dockType == DOCK_TYPE.LEFT) then
		return PIN_CORNER.TOPLEFT, AUTO_RESIZE.DOWN
	elseif (dockType == DOCK_TYPE.RIGHT) then
		return PIN_CORNER.TOPRIGHT, AUTO_RESIZE.DOWN
	elseif (dockType == DOCK_TYPE.TOP) then
		return PIN_CORNER.TOPLEFT, AUTO_RESIZE.RIGHT
	elseif (dockType == DOCK_TYPE.BOTTOM) then
		return PIN_CORNER.BOTTOMLEFT, AUTO_RESIZE.RIGHT
	end

	return PIN_CORNER.TOPLEFT, AUTO_RESIZE.NO
end

--- This function simulates docking a panel to its parent.
--- @param dockType DOCK_TYPE
function PANEL:Dock(dockType)
	local pinCorner, autoResize = convertDockTypeToAutoSize(dockType)

	print("Docking to", dockType, "with pin corner", pinCorner, "and auto resize", autoResize)

	self:SetAutoResize(
		pinCorner,
		autoResize,
		0, -- TODO: implement pin offset, see how it relates to DockMargin/Padding
		0,
		0, -- TODO: implement resize delta, see how it relates to DockMargin/Padding
		0
	)
end
