include("shared.lua")

DEFINE_BASECLASS("gamemode_base")

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

	-- TODO: But here we can click the sheets :/ (see above)
	local sheet = vgui.Create( "DPropertySheet", testPanel )
	sheet:Dock( FILL )

	local panel1 = vgui.Create( "DPanel", sheet )
	panel1.Paint = function( self, w, h ) draw.RoundedBox( 4, 0, 0, w, h, Color( 0, 128, 255, self:GetAlpha() ) ) end
	sheet:AddSheet( "test", panel1, "icon16/cross.png" )

	local panel2 = vgui.Create( "DPanel", sheet )
	panel2.Paint = function( self, w, h ) draw.RoundedBox( 4, 0, 0, w, h, Color( 255, 128, 0, self:GetAlpha() ) ) end
	sheet:AddSheet( "test 2", panel2, "icon16/tick.png" )

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
