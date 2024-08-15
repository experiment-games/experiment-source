include("shared.lua")

GM.TestAsd2 = Material("gwenskin/gmoddefault.png")

GM.TestAsd = Material("gwenskin/gmoddefault.png")
print(GM.TestAsd, GM.TestAsd2)

-- Test loading many materials at once
local files = {
	"alpha_grid.png",
	"colors_dark.png",
	"colors_light.png",
	"contenticon-hovered.png",
	"colors.png",
	"contenticon-normal.png",
	"contenticon-normal.psd",
	"cross.png",
	"crosshair.png",
	"dupe_bg.png",
	"e.png",
	-- "crosshair_bg.png", -- TODO: support non power of 4 textures
	-- "crosshair_bg2.png", -- TODO: support non power of 4 textures
	"hand_human_left.png",
	"key.png",
	"lmb.png",
	"noicon.png",
	"npc.png",
	"numberscratch_cover.png",
	"numberscratch_under.png",
	"progress_cog.png",
	"point.png",
	"r.png",
	"ps_hover.png",
	"rmb.png",
	"sm_hover.png",
	"spawnicon-npc.png",
	"steamworks_header.png",
	"tool.png",
	"workshop_rocket.png"
}
local mats = {}

for _, file in ipairs(files) do
	print("Loading ", "gui/"..file)
	local mat = Material("gui/" .. file)
	mats[file] = mat
end

DEFINE_BASECLASS("gamemode_base")
function GM:Initialize()
    BaseClass.Initialize(self)
end

local PANEL = {}

local forAllChildren
forAllChildren = function(panels, func, recursive)
    if not istable(panels) then
        panels = { panels }
    end

	for _, panel in pairs(panels) do
		for _, child in pairs(panel:GetChildren()) do
            func(child)

			if recursive then
				forAllChildren(child, func, recursive)
			end
		end
	end
end

DEFINE_BASECLASS("DFrame")
function PANEL:Init()
    -- TODO: Why cant we click the sheets when MakePopup happens after init?
	if (self.WithTestEarlyMakePopup) then
        self:MakePopup() -- This the sheets clickable, wut :/
		print("MakePopup called early")
	end
    local sheet = vgui.Create("DPropertySheet", self)
    sheet:Dock(FILL)

    local panel1 = vgui.Create("DPanel", sheet)
    panel1.Paint = function(self, w, h) draw.RoundedBox(4, 0, 0, w, h, Color(0, 128, 255, self:GetAlpha())) end
    sheet:AddSheet("test", panel1, "icon16/cross.png")

    local panel2 = vgui.Create("DPanel", sheet)
    panel2.Paint = function(self, w, h) draw.RoundedBox(4, 0, 0, w, h, Color(255, 128, 0, self:GetAlpha())) end
    sheet:AddSheet("test 2", panel2, "icon16/tick.png")

    -- Add all images as DImage to the first panel
	for file, mat in pairs(mats) do
		local img = vgui.Create("DImage", panel1)
		img:SetMaterial(mat)
		img:SetSize(16, 16)
        img:SetPosition(0, 16 * #panel1:GetChildren())
	end
end

function PANEL:PerformLayout(w, h)
	BaseClass.PerformLayout(self, w, h)
end

local PANEL2 = table.Copy(PANEL)
PANEL2.WithTestEarlyMakePopup = true
derma.DefineControl("TestPanel", "", PANEL, "DFrame")
derma.DefineControl("TestPanel2", "", PANEL2, "DFrame")

-- lua_run_cl MakeTestPanel()
function MakeTestPanel(title)
	local testPanel = vgui.Create("TestPanel")
	testPanel:SetTitle(title)
    testPanel:SetSize(512, 512)
	testPanel:Center()

	return testPanel
end

ConsoleCommands.Add("+menu", function(client, pCmd, args)
	local testPanel = vgui.Create("TestPanel2")
	testPanel:SetTitle("TestPanel2 WithTestEarlyMakePopup")
    testPanel:SetSize(512, 512)
	testPanel:Center()
end)

ConsoleCommands.Add("-menu", function(client, pCmd, args)
end)
TEST_PANEL = MakeTestPanel("TestPanel (1)")
TEST_PANEL:MakePopup()
