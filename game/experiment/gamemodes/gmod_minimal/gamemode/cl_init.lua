include("shared.lua")

GM.TestAsd2 = Material("gmod_compatibility_content/gwenskin/gmoddefault.png")
DEFINE_BASECLASS("gamemode_base")

GM.TestAsd = Material("gmod_compatibility_content/gwenskin/gmoddefault.png")
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
	print("Loading ", "gmod_compatibility_content/gui/"..file)
	local mat = Material("gmod_compatibility_content/gui/" .. file)
	mats[file] = mat
end

function GM:Initialize()
    BaseClass.Initialize(self)
end

--[[
		Testing if Gui panel inheritance works as expected
--]]
local PANEL = {}
function PANEL:Init()
	print(self, getmetatable(self), self.GetText, self.SetText, "DCustomLabel")
end
derma.DefineControl("DCustomLabel", "", PANEL, "Label")
local PANEL = {}
function PANEL:Init()
	print(self, getmetatable(self), self.GetText, self.SetText, "DCustomLabelChild")
end
derma.DefineControl("DCustomLabelChild", "", PANEL, "DCustomLabel")

local PANEL = {}

function PANEL:Init()
	-- TODO: Why cant we click the sheets when initializing here? (see below)
	-- local sheet = vgui.Create( "DPropertySheet", self )
	-- sheet:Dock( FILL )

	-- local panel1 = vgui.Create( "DPanel", sheet )
	-- panel1.Paint = function( self, w, h ) draw.RoundedBox( 4, 0, 0, w, h, Color( 0, 128, 255, self:GetAlpha() ) ) end
	-- sheet:AddSheet( "test", panel1, "icon16/cross.png" )

	-- local panel2 = vgui.Create( "DPanel", sheet )
	-- panel2.Paint = function( self, w, h ) draw.RoundedBox( 4, 0, 0, w, h, Color( 255, 128, 0, self:GetAlpha() ) ) end
	-- sheet:AddSheet( "test 2", panel2, "icon16/tick.png" )
end

derma.DefineControl("TestPanel", "", PANEL, "DFrame")

-- lua_run_cl MakeTestPanel()
function MakeTestPanel()
	local testPanel = vgui.Create("TestPanel")
    testPanel:SetSize(512, 512)
	testPanel:Center()
    testPanel:MakePopup()
	print(testPanel:IsActive())

	-- TODO: But here we can click the sheets :/ (see above)
	local sheet = vgui.Create( "DPropertySheet", testPanel )
	sheet:Dock( FILL )

	local panel1 = vgui.Create( "DPanel", sheet )
	panel1.Paint = function( self, w, h ) draw.RoundedBox( 4, 0, 0, w, h, Color( 0, 128, 255, self:GetAlpha() ) ) end
	sheet:AddSheet( "test", panel1, "icon16/cross.png" )

	local panel2 = vgui.Create( "DPanel", sheet )
	panel2.Paint = function( self, w, h ) draw.RoundedBox( 4, 0, 0, w, h, Color( 255, 128, 0, self:GetAlpha() ) ) end
    sheet:AddSheet("test 2", panel2, "icon16/tick.png")

    -- Add all images as DImage to the first panel
	for file, mat in pairs(mats) do
		local img = vgui.Create("DImage", panel1)
		img:SetMaterial(mat)
		img:SetSize(16, 16)
        img:SetPos(0, 16 * #panel1:GetChildren())
	end

	return testPanel
end

ConsoleCommands.Add("+buildmenu", function(client, pCmd, args)
	MakeTestPanel()
end)

ConsoleCommands.Add("-buildmenu", function(client, pCmd, args)
end)
TEST_PANEL = MakeTestPanel()
TEST_PANEL2 = MakeTestPanel()
TEST_PANEL2:SetPos(512, 0)
