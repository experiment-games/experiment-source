require("gmod_compatibility")

include("shared.lua")

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
    if IsValid(TEST_PANEL) then
        TEST_PANEL:Remove()
    end

	TEST_PANEL = vgui.Create("TestPanel")
	TEST_PANEL:SetSize(512, 512)
	TEST_PANEL:Center()
    TEST_PANEL:MakePopup()

	-- TODO: But here we can click the sheets :/ (see above)
	local sheet = vgui.Create( "DPropertySheet", TEST_PANEL )
	sheet:Dock( FILL )

	local panel1 = vgui.Create( "DPanel", sheet )
	panel1.Paint = function( self, w, h ) draw.RoundedBox( 4, 0, 0, w, h, Color( 0, 128, 255, self:GetAlpha() ) ) end
	sheet:AddSheet( "test", panel1, "icon16/cross.png" )

	local panel2 = vgui.Create( "DPanel", sheet )
	panel2.Paint = function( self, w, h ) draw.RoundedBox( 4, 0, 0, w, h, Color( 255, 128, 0, self:GetAlpha() ) ) end
	sheet:AddSheet( "test 2", panel2, "icon16/tick.png" )

	return TEST_PANEL
end

ConsoleCommands.Add("+buildmenu", function(client, pCmd, args)
	MakeTestPanel()
end)

ConsoleCommands.Add("-buildmenu", function(client, pCmd, args)
end)
