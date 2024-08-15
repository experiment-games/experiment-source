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
	gBuildMenuInterface = Panels.CBuildMenu(Panels.GetClientLuaRootPanel(), "build")

	Surfaces.AddCustomFontFile("DIN-Light", "resource/DINLi.ttf")

	gBuildMenuInterface.m_hFonts["BuildMenuTextLarge"] = Surfaces.CreateFont()
	Surfaces.SetFontGlyphSet(
		gBuildMenuInterface.m_hFonts["BuildMenuTextLarge"],
		"DIN-Light",
		64,
		0,
		0,
		0,
		FONTFLAG_ANTIALIAS | FONTFLAG_ADDITIVE | FONTFLAG_CUSTOM
	)
	gBuildMenuInterface.m_hFonts["BuildMenuTextLargeSelected"] = Surfaces.CreateFont()
	Surfaces.SetFontGlyphSet(
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

Surfaces.GetTextureID = function(name)
	if (not textureMap[name]) then
		textureMap[name] = Surfaces.CreateNewTextureID()
		Surfaces.DrawSetTextureFile(textureMap[name], name)
	end

	return textureMap[name]
end

Surfaces.SetMaterial = function(material)
	local name = material:GetString("$basetexture")

	if (not textureMap[name]) then
		textureMap[name] = Surfaces.CreateNewTextureID(true)
		Surfaces.DrawSetTextureMaterial(textureMap[name], material)
	end

	Surfaces.DrawSetTexture(textureMap[name])
end

testMat = Surfaces.FindMaterial("silkicons/accept.png")
testMatX = Surfaces.FindMaterial("silkicons/monkey.png") -- Unused materials caused a crash on exit earlier
testMatY = Surfaces.FindMaterial("silkicons/___non___existant_mat.png") -- Testing that non-existant materials don't crash
testTexture = Surfaces.GetTextureID("vgui/gfx/vgui/crosshair")

-- Having the ESC menu opened whilst finding a material seemed to cause a crash on exit
-- Having had the ESC menu open at some point does not cause a crash on exit
-- Having SetTextureRegenerator with a reference to the texture regen caused an error
-- This is now fixed, because we set the texture regenerator to null on shutdown
function test()
	print("testingTempMat test")
    local testingTempMat1 = Surfaces.FindMaterial("icon16/folder.png")
    local testingTempMat2 = Surfaces.FindMaterial("icon16/folder.png")
    local testingTempMat3 = Surfaces.FindMaterial("icon16/folder.png") -- With this line, we crashed on exit because we incorrectly re-loaded textures, even if the material and texture were found. That is now fixed using g_pMaterialSystem->IsTextureLoaded (cpng.cpp)
    -- local testingTempMat4 = Surfaces.FindMaterial("icon16/folder.png")
	-- Despite being fixed here, the problem still occurs in the gmod sandbox gamemode...
end
Timers.Simple(2, test)

function GM:HUDPaint()
	if (not STOP) then -- TODO: How to detect freed material textures?
		Surfaces.SetMaterial(testMat)
		Surfaces.DrawSetColor(255, 255, 255, 255)
		Surfaces.DrawTexturedRectangle(0, 0, 64, 64)
	end

	if (Surfaces.DrawGetTextureFile(testTexture) == true) then
		Surfaces.DrawSetTexture(testTexture)
		Surfaces.DrawSetColor(255, 255, 255, 255)
		Surfaces.DrawTexturedRectangle(64, 64, 64, 64)
	end
end
