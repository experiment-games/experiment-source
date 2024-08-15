-- lua_openscript_menu utilities/test/panel_docking.lua
-- or: lua_openscript_cl utilities/test/panel_docking.lua

Include("includes/extensions/panel.lua")

-- Lets create a panel and dock it to the right of the parent.
local parent = Gui.Create("Frame")
local parentWide, parentTall = 256, 256
parent:SetBgColor(Colors.Create(255, 0, 0, 255))
parent:SetSize(parentWide, parentTall)
parent:SetPosition(50, 50)
parent:SetTitle("Parent panel")

parent:MakePopup()
parent:SetVisible(true)

local dockPadding = 10
local childWide, childTall = 64, 64

-- Child for filling
local fillChild = Gui.Create("Button", parent)
fillChild:SetText("FILL")
fillChild:SetDock(_E.DOCK_TYPE.FILL)
fillChild:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)

function fillChild:OnClick()
    -- parent:DeletePanel() -- Will crash with access violation, as repaint will be called on release of button
    parent:MarkForDeletion() -- safer
end

local topChild = Gui.Create("Label", parent)
topChild:SetText("TOP")
topChild:SetDock(_E.DOCK_TYPE.TOP)
topChild:SetTall(childTall)
topChild:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)

local leftChild = Gui.Create("Button", parent)
leftChild:SetText("LEFT")
leftChild:SetDock(_E.DOCK_TYPE.LEFT)
leftChild:SetWide(childWide)
leftChild:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)

function leftChild:OnClick()
    parent:MarkForDeletion()
end

local bottomChild = Gui.Create("Panel", parent)
bottomChild:SetDock(_E.DOCK_TYPE.BOTTOM)
bottomChild:SetTall(childTall)
bottomChild:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)
bottomChild:SetBgColor(Colors.Create(0, 255, 0, 255))

local bottomAncestor = Gui.Create("Button", bottomChild)
bottomAncestor:SetText("bottom BOTTOM")
bottomAncestor:SetDock(_E.DOCK_TYPE.BOTTOM)
bottomAncestor:SetTall(childTall / 2)
bottomAncestor:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)

local bottomAncestor2 = Gui.Create("Button", bottomChild)
bottomAncestor2:SetText("bottom TOP")
bottomAncestor2:SetDock(_E.DOCK_TYPE.TOP)
bottomAncestor2:SetTall(childTall / 2)
bottomAncestor2:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)

local rightChild = Gui.Create("Button", parent)
rightChild:SetText("RIGHT")
rightChild:SetDock(_E.DOCK_TYPE.RIGHT)
rightChild:SetWide(childWide)
rightChild:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)

function rightChild:OnClick()
	parent:MarkForDeletion()
end

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
