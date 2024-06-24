-- lua_dofile_menu utilities/test/panel_docking.lua

-- Lets create a panel and dock it to the right of the parent.
local parent = Gui.Create("EditablePanel", VGui_GetGameUIPanel())

function parent:ApplySchemeSettings(pScheme)
    self:SetBgColor(Color(255, 0, 0, 80))
	self:SetSize(256, 256)
	self:SetPos(50, 50)
	print("ApplySchemeSettings", self, pScheme)
end

local child = Gui.Create("Button", parent)
child:SetText("Docked to the right")
child:SetSize(64, 64)
child:Dock(DOCK_TYPE.RIGHT)
child.OnClick = function(self)
	print("Clicked", self)
	-- parent:DeletePanel() -- Will crash with access violation, as repaint will be called on release of button
	parent:MarkForDeletion() -- safer
end

parent:MakePopup()

print(parent)
