--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

Include("shared.lua")
Include("in_main.lua")
Include("scripted_controls/buildmenu.lua")

local FONTFLAG_ANTIALIAS = _E.FONTFLAG.ANTIALIAS
local FONTFLAG_ADDITIVE = _E.FONTFLAG.ADDITIVE
local FONTFLAG_CUSTOM = _E.FONTFLAG.CUSTOM

function GM:CreateDefaultPanels()
	gBuildMenuInterface = Gui.CBuildMenu(Gui.GetClientLuaRootPanel(), "build")

	Surface.AddCustomFontFile("DIN-Light", "resource/DINLi.ttf")

	gBuildMenuInterface.m_hFonts["BuildMenuTextLarge"] = Surface.CreateFont()
	Surface.SetFontGlyphSet(
		gBuildMenuInterface.m_hFonts["BuildMenuTextLarge"],
		"DIN-Light",
		64,
		0,
		0,
		0,
		FONTFLAG_ANTIALIAS | FONTFLAG_ADDITIVE | FONTFLAG_CUSTOM
	)
	gBuildMenuInterface.m_hFonts["BuildMenuTextLargeSelected"] = Surface.CreateFont()
	Surface.SetFontGlyphSet(
		gBuildMenuInterface.m_hFonts["BuildMenuTextLargeSelected"],
		"DIN-Light",
		64,
		0,
		5,
		2,
		FONTFLAG_ANTIALIAS | FONTFLAG_ADDITIVE | FONTFLAG_CUSTOM
	)
end
