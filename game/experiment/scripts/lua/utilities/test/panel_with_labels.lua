-- lua_openscript_menu utilities/test/panel_with_labels.lua
-- or: lua_openscript_cl utilities/test/panel_with_labels.lua

Include("includes/extensions/panel.lua")
Include("includes/extensions/label.lua")
Include("includes/extensions/text_entry.lua")

local parent = Gui.Create("Frame")
local parentWide, parentTall = 256, 256
parent:SetBgColor(Color(255, 0, 0, 255))
parent:SetSize(parentWide, parentTall)
parent:SetPosition(50, 50)
parent:SetTitle("A panel with labels using fonts")

parent:MakePopup()
parent:SetVisible(true)

local dockPadding = 10
local labels = {
    {
        text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
		font = "Default",
    },
	{
		text = "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
		font = "DefaultSmall",
    },
	{
		text = "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.",
		font = "Marlett",
	},
	{
		text = "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.",
		font = "non-existing",
	},
}

firstSpacing = 22

for i, label in ipairs(labels) do
    local topChild = Gui.Create("Label", parent)
    topChild:SetText(label.text)
    topChild:SetDock(_E.DOCK_TYPE.TOP)
    topChild:SetDockPadding(dockPadding, firstSpacing + dockPadding, dockPadding, dockPadding)
    topChild:SetFontByName(label.font)

    print("child font name", topChild:GetFontName())

    firstSpacing = 0
end

-- Add a text entry
local textEntry = Gui.Create("TextEntry", parent)
textEntry:SetDock(_E.DOCK_TYPE.FILL)
textEntry:SetDockPadding(dockPadding, dockPadding, dockPadding, dockPadding)
textEntry:SetFontByName("Default")
textEntry:SetMultiline(true)
textEntry:SetEditable(false)

local closeButton = Gui.Create("Button", parent)
closeButton:SetText("Close")
closeButton:SetDock(_E.DOCK_TYPE.BOTTOM)
closeButton:SetTall(32)

function closeButton:OnClick()
	parent:MarkForDeletion()
end

local enableButton = Gui.Create("Button", parent)
enableButton:SetText("Toggle TextEntry IsEditable")
enableButton:SetDock(_E.DOCK_TYPE.BOTTOM)
enableButton:SetTall(32)

function enableButton:OnClick()
	textEntry:SetEditable(not textEntry:IsEditable())
end
