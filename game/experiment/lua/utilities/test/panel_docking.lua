-- lua_dofile_menu utilities/test/panel_docking.lua
-- or: lua_dofile_cl utilities/test/panel_docking.lua

-- Lets create a panel and dock it to the right of the parent.
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
child:SetText("TOP")

function child:OnClick()
    print("Clicked", self)
    -- parent:DeletePanel() -- Will crash with access violation, as repaint will be called on release of button
    parent:MarkForDeletion() -- safer
end

-- child:SetDock(DOCK_TYPE.FILL)
-- child:SetDock(DOCK_TYPE.LEFT)
-- child:SetDock(DOCK_TYPE.RIGHT)
-- child:SetWide(childWide)
child:SetDock(DOCK_TYPE.TOP)
-- child:SetDock(DOCK_TYPE.BOTTOM)
child:SetTall(childTall)
child:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)
child:UpdateDocking()

-- Child for filling
local child2 = Gui.Create("Button", parent)
child2:SetText("FILL")
child2:SetDock(DOCK_TYPE.FILL)
child2:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)
child2:UpdateDocking()

--[[
	Experiments with SetBounds and SetAutoResize:
--]]

-- Attached to top left, without stretching (default, no docking):
-- child:SetBounds(dockPadding, dockPadding, childWide, childTall)
-- child:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.NO, dockPadding, dockPadding, 0, 0)

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

-- Dock to top, stretching bottom down and right:
-- child:SetBounds(dockPadding, dockPadding, parentWide - dockPadding * 2, childTall)
-- child:SetAutoResize(PIN_CORNER.TOP_LEFT, AUTO_RESIZE.DOWN_AND_RIGHT, dockPadding, dockPadding, -dockPadding, -childTall + dockPadding)

-- Dock to bottom, stretching down and right, but keeping the top pinned:
-- child:SetBounds(dockPadding, parentTall - childTall - dockPadding, parentWide - dockPadding * 2, childTall)
-- child:SetAutoResize(PIN_CORNER.BOTTOM_LEFT, AUTO_RESIZE.DOWN_AND_RIGHT, dockPadding, -dockPadding, -dockPadding, childTall - dockPadding)
