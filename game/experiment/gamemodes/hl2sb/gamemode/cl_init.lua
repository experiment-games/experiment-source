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

-- TODO: Remove this all, for testing only
local textureMap = {}

Surface.GetTextureID = function(name)
	if (not textureMap[name]) then
		textureMap[name] = Surface.CreateNewTextureID()
		Surface.DrawSetTextureFile(textureMap[name], name)
	end

	return textureMap[name]
end

Surface.SetMaterial = function(material)
	local name = material:GetString("$basetexture")

	if (not textureMap[name]) then
		textureMap[name] = Surface.CreateNewTextureID(true)
	end

	Surface.DrawSetTextureMaterial(textureMap[name], material)
end

testMat = Globals.FindMaterial("silkicons/accept.png")
testTexture = Surface.GetTextureID("vgui/gfx/vgui/crosshair")
-- lua_run_cl STOP = true surface.ClearAllTextures()
function GM:HudViewportPaint()
	if (not STOP) then -- TODO: How to detect freed material textures?
		Surface.SetMaterial(testMat)
		Surface.DrawSetColor(255, 255, 255, 255)
		Surface.DrawTexturedRect(0, 0, 64, 64)
	end

	if (Surface.DrawGetTextureFile(testTexture) == true) then
		Surface.DrawSetTexture(testTexture)
		Surface.DrawSetColor(255, 255, 255, 255)
		Surface.DrawTexturedRect(64, 64, 64, 64)
	end
end

-- uncomment the following line, then go in-game into the hl2sb gamemode. Press quit in the main menu. The game will crash with an access violation.
-- TODO: perhaps we are leaving something on the stack, or popping too much when showing error messages?
-- error("errors cause issues on quitting?")
