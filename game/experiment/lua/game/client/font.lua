--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.


	Purpose: Wraps the font type so handles are persistent through screen size
			changes, and fonts work properly without having to manually
			recreate them.
--]]

if (not CLIENT) then
	return
end

local ComputeStringWidth = Util.ComputeStringWidth
local GetFontName = _R.IScheme.GetFontName
local CreateFont = Surface.CreateFont
local DrawSetTextFont = Surface.DrawSetTextFont
local GetCharABCwide = Surface.GetCharABCwide
local GetCharacterWidth = Surface.GetCharacterWidth
local GetFontAscent = Surface.GetFontAscent
local GetFontTall = Surface.GetFontTall
local GetTextSize = Surface.GetTextSize
local IsFontAdditive = Surface.IsFontAdditive
local SetFontGlyphSet = Surface.SetFontGlyphSet

-------------------------------------------------------------------------------
-- _R.HFontContainer
-- Purpose: Class metatable
-------------------------------------------------------------------------------
_R.HFontContainer = {
	__index = {},
	__type = "fontcontainer"
}

-------------------------------------------------------------------------------
-- HFontContainerIndex
-- Purpose: Internal container index
-------------------------------------------------------------------------------
local HFontContainerIndex = 1

-------------------------------------------------------------------------------
-- HFontContainers
-- Purpose: Internal containers index
-------------------------------------------------------------------------------
local HFontContainers = {}

-------------------------------------------------------------------------------
-- HFontContainer()
-- Purpose: Creates a new HFontContainer
-- Output: HFontContainer
-------------------------------------------------------------------------------
function HFontContainer()
	local t = {
		index = 0,
		font = INVALID_FONT,
		windowsFontName = "",
		tall = 0,
		weight = 0,
		blur = 0,
		scanlines = 0,
		flags = 0,
		nRangeMin = nil,
		nRangeMax = nil
	}
	setmetatable(t, _R.HFontContainer)
	return t
end

-------------------------------------------------------------------------------
-- HFontContainer:__tostring()
-- Purpose: __tostring metamethod for HFontContainer
-------------------------------------------------------------------------------
function _R.HFontContainer:__tostring()
	return "HFontContainer: " .. self.index
end

Hooks.Add("OnScreenSizeChanged", "HFontContainerManager", function()
	for i, fontcontainer in ipairs(HFontContainers) do
		fontcontainer.font = CreateFont()
		SetFontGlyphSet(fontcontainer.font,
			fontcontainer.windowsFontName,
			fontcontainer.tall,
			fontcontainer.weight,
			fontcontainer.blur,
			fontcontainer.scanlines,
			fontcontainer.flags,
			fontcontainer.nRangeMin,
			fontcontainer.nRangeMax)
	end
end)

function Util.ComputeStringWidth(font, str)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'ComputeStringWidth' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return ComputeStringWidth(font, str)
	elseif (type == "fontcontainer") then
		return ComputeStringWidth(font.font, str)
	end
end

function _R.IScheme.GetFontName(font)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'GetFontName' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return GetFontName(font)
	elseif (type == "fontcontainer") then
		return GetFontName(font.font)
	end
end

function Surface.CreateFont()
	local fontcontainer = HFontContainer()
	fontcontainer.index = HFontContainerIndex
	fontcontainer.font = CreateFont()
	HFontContainers[HFontContainerIndex] = fontcontainer
	HFontContainerIndex = HFontContainerIndex + 1
	return fontcontainer
end

function Surface.DrawSetTextFont(font)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'DrawSetTextFont' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return DrawSetTextFont(font)
	elseif (type == "fontcontainer") then
		return DrawSetTextFont(font.font)
	end
end

function Surface.GetCharABCwide(font, ch)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'GetCharABCwide' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return GetCharABCwide(font, ch)
	elseif (type == "fontcontainer") then
		return GetCharABCwide(font.font, ch)
	end
end

function Surface.GetCharacterWidth(font, ch)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'GetCharacterWidth' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return GetCharacterWidth(font, ch)
	elseif (type == "fontcontainer") then
		return GetCharacterWidth(font.font, ch)
	end
end

function Surface.GetFontAscent(font, ch)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'GetFontAscent' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return GetFontAscent(font, ch)
	elseif (type == "fontcontainer") then
		return GetFontAscent(font.font, ch)
	end
end

function Surface.GetFontTall(font)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'GetFontTall' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return GetFontTall(font)
	elseif (type == "fontcontainer") then
		return GetFontTall(font.font)
	end
end

function Surface.GetTextSize(font, text)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'GetTextSize' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return GetTextSize(font, text)
	elseif (type == "fontcontainer") then
		return GetTextSize(font.font, text)
	end
end

function Surface.IsFontAdditive(font)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'IsFontAdditive' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return IsFontAdditive(font)
	elseif (type == "fontcontainer") then
		return IsFontAdditive(font.font)
	end
end

function Surface.SetFontGlyphSet(font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax)
	local type = type(font)
	if (type ~= "font" and type ~= "fontcontainer") then
		error("bad argument #1 to 'SetFontGlyphSet' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "font") then
		return SetFontGlyphSet(font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax)
	elseif (type == "fontcontainer") then
		font.windowsFontName = windowsFontName
		font.tall = tall
		font.weight = weight
		font.blur = blur
		font.scanlines = scanlines
		font.flags = flags
		font.nRangeMin = nRangeMin
		font.nRangeMax = nRangeMax
		return SetFontGlyphSet(font.font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax)
	end
end
