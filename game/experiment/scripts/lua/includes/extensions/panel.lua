--[[
	Enumerations

	TODO: Move these enum definitions to the C-side so they don't misalign.
--]]

local Hooks = require("hooks")

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

-- function PANEL:SizeToChildren(sizeWidth, sizeHeight)
-- 	sizeWidth = sizeWidth or false
-- 	sizeHeight = sizeHeight or false

-- 	local width, height = self:GetChildrenSize()
-- 	local currentWidth, currentHeight = self:GetSize()

-- 	if (sizeWidth and width ~= currentWidth) then
-- 		self:SetWide(width)
-- 	end

-- 	if (sizeHeight and height ~= currentHeight) then
-- 		self:SetTall(height)
-- 	end
-- end

-- function PANEL:SizeToContents(sizeWidth, sizeHeight)
-- 	sizeWidth = sizeWidth or false
-- 	sizeHeight = sizeHeight or false

-- 	self.__sizeToContents = {
-- 		width = sizeWidth,
-- 		height = sizeHeight
-- 	}
-- end

-- --- We override PerformLayout so we can update docking when the panel is resized.
-- Hooks.Add("OnPanelPerformLayout", "OnPanelPerformLayoutInternal", function(panel)
-- 	if (panel.__sizeToContents) then
-- 		panel:SizeToChildren(panel.__sizeToContents.width, panel.__sizeToContents.height)
-- 	end

-- 	if (panel.__dockType ~= DOCK_TYPE.NONE) then
-- 		panel:UpdateDockingParent()
-- 		panel:UpdateDocking()
-- 	end
-- end)

-- --- Get all children of the panel.
-- --- @return table
-- function PANEL:GetChildren()
-- 	local children = {}

-- 	for i = 1, self:GetChildCount() do
-- 		table.insert(children, self:GetChild(i))
-- 	end

-- 	return children
-- end

-- --- Checks if the panel has any children.
-- --- @return boolean
-- function PANEL:HasChildren()
-- 	return self:GetChildCount() > 0
-- end

-- --- Set dock margins for the panel.
-- --- @param left number
-- --- @param top number
-- --- @param right number
-- --- @param bottom number
-- function PANEL:SetDockMargin(left, top, right, bottom)
-- 	self.__dockMarginLeft = left or 0
-- 	self.__dockMarginTop = top or 0
-- 	self.__dockMarginRight = right or 0
-- 	self.__dockMarginBottom = bottom or 0

-- 	-- self:UpdateDockingParent()
-- end

-- --- Get the dock margins for the panel.
-- --- @return number, number, number, number
-- function PANEL:GetDockMargin()
-- 	return self.__dockMarginLeft or 0, self.__dockMarginTop or 0, self.__dockMarginRight or 0,
-- 		self.__dockMarginBottom or 0
-- end

-- --- Set the dock padding for the panel.
-- --- @param left number
-- --- @param top number
-- --- @param right number
-- --- @param bottom number
-- function PANEL:SetDockPadding(left, top, right, bottom)
-- 	self.__dockPaddingLeft = left or 0
-- 	self.__dockPaddingTop = top or 0
-- 	self.__dockPaddingRight = right or 0
-- 	self.__dockPaddingBottom = bottom or 0

-- 	-- self:UpdateDockingParent()
-- end

-- --- Get the dock padding for the panel.
-- --- @return number, number, number, number
-- function PANEL:GetDockPadding()
-- 	return self.__dockPaddingLeft or 0, self.__dockPaddingTop or 0, self.__dockPaddingRight or 0,
-- 		self.__dockPaddingBottom or 0
-- end

-- --- Set the dock type for the panel.
-- --- @param dockType DOCK_TYPE
-- function PANEL:SetDock(dockType)
-- 	self.__dockType = dockType

-- 	-- self:UpdateDockingParent()
-- end

-- --- Get the dock type for the panel.
-- --- @return DOCK_TYPE
-- function PANEL:GetDock()
-- 	return self.__dockType
-- end

-- --- Update the docking of the parent panel.
-- function PANEL:UpdateDockingParent()
-- 	local parent = self:GetParent()

-- 	if (IsValid(parent)) then
-- 		parent:UpdateDocking()
-- 	end
-- end

-- --- Update the docking of child panels.
-- function PANEL:UpdateDocking()
-- 	local parentWidth, parentHeight = self:GetSize()
-- 	local leftPadding, topPadding, rightPadding, bottomPadding = self:GetDockPadding()
-- 	local occupiedSpace = {
-- 		left = leftPadding,
-- 		top = topPadding,
-- 		right = rightPadding,
-- 		bottom = bottomPadding
-- 	}

-- 	for i = 1, self:GetChildCount() do
-- 		local child = self:GetChild(i)

--         if (not IsValid(child)) then
--             -- TODO: Fix and find out why this happens with DMenu
--             print("Invalid child panel found in UpdateDocking", child, self, i, self:GetChildCount())
--             continue
--         end

-- 		local leftPadding, topPadding, rightPadding, bottomPadding = child:GetDockPadding()
-- 		local dockType = child:GetDock()

-- 		if (dockType == DOCK_TYPE.LEFT) then
-- 			child:SetBounds(
-- 				occupiedSpace.left + leftPadding,
-- 				occupiedSpace.top + topPadding,
-- 				child:GetWide(),
-- 				parentHeight - occupiedSpace.top - topPadding - occupiedSpace.bottom - bottomPadding
-- 			)

-- 			child:SetAutoResize(
-- 				PIN_CORNER.TOP_LEFT,
-- 				AUTO_RESIZE.DOWN,
-- 				occupiedSpace.left + leftPadding,
-- 				occupiedSpace.top + topPadding,
-- 				0,
-- 				0 - occupiedSpace.bottom - bottomPadding
-- 			)

-- 			occupiedSpace.left = occupiedSpace.left + leftPadding + child:GetWide()
-- 		elseif (dockType == DOCK_TYPE.TOP) then
-- 			child:SetBounds(
-- 				occupiedSpace.left + leftPadding,
-- 				occupiedSpace.top + topPadding,
-- 				parentWidth - occupiedSpace.left - leftPadding - occupiedSpace.right - rightPadding,
-- 				child:GetTall()
-- 			)

-- 			child:SetAutoResize(
-- 				PIN_CORNER.TOP_LEFT,
-- 				AUTO_RESIZE.RIGHT,
-- 				occupiedSpace.left + leftPadding,
-- 				occupiedSpace.top + topPadding,
-- 				0 - occupiedSpace.right - rightPadding,
-- 				0
-- 			)

-- 			occupiedSpace.top = occupiedSpace.top + topPadding + child:GetTall()
-- 		elseif (dockType == DOCK_TYPE.RIGHT) then
-- 			child:SetBounds(
-- 				parentWidth - rightPadding - child:GetWide(),
-- 				occupiedSpace.top + topPadding,
-- 				child:GetWide(),
-- 				parentHeight - occupiedSpace.top - topPadding - occupiedSpace.bottom - bottomPadding
-- 			)

-- 			child:SetAutoResize(
-- 				PIN_CORNER.TOP_RIGHT,
-- 				AUTO_RESIZE.DOWN,
-- 				0 - occupiedSpace.right - rightPadding,
-- 				occupiedSpace.top + topPadding,
-- 				0,
-- 				0 - occupiedSpace.bottom - bottomPadding
-- 			)

-- 			occupiedSpace.right = occupiedSpace.right + rightPadding + child:GetWide()
-- 		elseif (dockType == DOCK_TYPE.BOTTOM) then
-- 			child:SetBounds(
-- 				occupiedSpace.left + leftPadding,
-- 				parentHeight - bottomPadding - child:GetTall(),
-- 				parentWidth - occupiedSpace.left - leftPadding - occupiedSpace.right - rightPadding,
-- 				child:GetTall()
-- 			)

-- 			child:SetAutoResize(
-- 				PIN_CORNER.BOTTOM_LEFT,
-- 				AUTO_RESIZE.RIGHT,
-- 				occupiedSpace.left + leftPadding,
-- 				0 - occupiedSpace.bottom - bottomPadding,
-- 				0 - occupiedSpace.right - rightPadding,
-- 				0
-- 			)

-- 			occupiedSpace.bottom = occupiedSpace.bottom + bottomPadding + child:GetTall()
-- 		end
-- 	end

-- 	-- Put any FILL panels in the remaining space (overlapping, not distributed)
-- 	for i = 1, self:GetChildCount() do
-- 		local child = self:GetChild(i)
-- 		local leftPadding, topPadding, rightPadding, bottomPadding = child:GetDockPadding()
-- 		local dockType = child:GetDock()

-- 		if dockType == DOCK_TYPE.FILL then
-- 			child:SetBounds(
-- 				occupiedSpace.left + leftPadding,
-- 				occupiedSpace.top + topPadding,
-- 				parentWidth - occupiedSpace.left - leftPadding - occupiedSpace.right - rightPadding,
-- 				parentHeight - occupiedSpace.top - topPadding - occupiedSpace.bottom - bottomPadding
-- 			)

-- 			child:SetAutoResize(
-- 				PIN_CORNER.TOP_LEFT,
-- 				AUTO_RESIZE.DOWN_AND_RIGHT,
-- 				occupiedSpace.left + leftPadding,
-- 				occupiedSpace.top + topPadding,
-- 				0 - occupiedSpace.right - rightPadding,
-- 				0 - occupiedSpace.bottom - bottomPadding
-- 			)
-- 		end
-- 	end
-- end