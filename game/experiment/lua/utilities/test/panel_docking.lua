-- lua_dofile_menu utilities/test/panel_docking.lua
-- or: lua_dofile_cl utilities/test/panel_docking.lua

-- Lets create a panel and dock it to the right of the parent.
local parent = Gui.Create("EditablePanel")
parent:SetBgColor(Color(255, 0, 0, 255))
parent:SetSize(256, 256)
parent:SetPos(50, 50)

function parent:ApplySchemeSettings(pScheme)
	print(self, parent, self == parent, 123, self:GetParent())
    self:SetBgColor(Color(255, 0, 0, 255))
	self:InvalidateLayout()
end

parent:MakePopup()
print(parent)

local child = Gui.Create("Button", parent)
child:SetText("Docked to the right")
child:SetSize(128, 128)

function child:OnClick()
    print("Clicked", self)
    -- parent:DeletePanel() -- Will crash with access violation, as repaint will be called on release of button
    parent:MarkForDeletion() -- safer
end

function child:ApplySchemeSettings(pScheme)
    print(self, child, self == child, 123, self:GetParent())
    self:SetPos(50, 50)
	self:SetSize(128, 128)
	self:InvalidateLayout()
    -- self:SetPinCorner(PIN_CORNER.TOPRIGHT, 0, 0)
	-- self:SetDefaultColor(Color(0, 255, 0, 255), Color(255, 255, 255, 255))
end

