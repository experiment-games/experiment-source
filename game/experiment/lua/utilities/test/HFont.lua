--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	lua_dofile_cl utilities/test/HFont.lua
--]]

if (not CLIENT) then
	return
end

local bor = bit.bor
local Frame = Gui.Frame
local Panel = Gui.Panel

Surface.AddCustomFontFile("DIN-Light", "gamemodes\\sandbox\\content\\resource\\DINLi.ttf")
local hTestFont = Surface.CreateFont()
Surface.SetFontGlyphSet(hTestFont,
	"DIN-Light",
	32,
	0,
	0,
	0,
	bor(0x010, 0x100, 0x400))

local strTextSample = "The five boxing wizards jump quickly."

g_hFontTestFrame = Frame()
g_hFontTestFrame:SetBounds(0, 0, 408, 120)
g_hFontTestFrame:SetSizeable(false)
g_hFontTestFrame:SetTitle("Font Test", true)
g_hFontTestFrame:SetVisible(true)

g_hFontTestFrame.m_hFontSamples = Panel(g_hFontTestFrame, "FontSamples")
g_hFontTestFrame.m_hFontSamples:SetProportional(true)

local iFontWide, iFontTall = Surface.GetTextSize(hTestFont, strTextSample)
g_hFontTestFrame.m_hFontSamples:SetPos(0, 120 / 2 - iFontTall / 2 + 4)
g_hFontTestFrame.m_hFontSamples:SetSize(408, iFontTall + 8)

function g_hFontTestFrame.m_hFontSamples:Paint()
	Surface.DrawSetTextFont(hTestFont)
	Surface.DrawPrintText(strTextSample)
end

-- move to the middle of the screen
g_hFontTestFrame:MoveToCenterOfScreen()
g_hFontTestFrame:InvalidateLayout()
g_hFontTestFrame:Activate()
