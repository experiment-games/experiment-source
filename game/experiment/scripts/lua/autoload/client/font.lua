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
-- _R.FontHandleContainer
-- Purpose: Class metatable
-------------------------------------------------------------------------------
_R.FontHandleContainer = {
	__index = {},
	__type = "FontHandleContainer"
}

-------------------------------------------------------------------------------
-- FontHandleContainerIndex
-- Purpose: Internal container index
-------------------------------------------------------------------------------
local FontHandleContainerIndex = 1

-------------------------------------------------------------------------------
-- FontHandleContainers
-- Purpose: Internal containers index
-------------------------------------------------------------------------------
local FontHandleContainers = {}

-------------------------------------------------------------------------------
-- FontHandleContainer()
-- Purpose: Creates a new FontHandleContainer
-- Output: FontHandleContainer
-------------------------------------------------------------------------------
function FontHandleContainer()
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
	setmetatable(t, _R.FontHandleContainer)
	return t
end

-------------------------------------------------------------------------------
-- FontHandleContainer:__tostring()
-- Purpose: __tostring metamethod for FontHandleContainer
-------------------------------------------------------------------------------
function _R.FontHandleContainer:__tostring()
	return "FontHandleContainer: " .. self.index
end

Hooks.Add("OnScreenSizeChanged", "HFontContainerManager", function()
	for i, fontcontainer in ipairs(FontHandleContainers) do
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
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'ComputeStringWidth' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return ComputeStringWidth(font, str)
	elseif (type == "FontHandleContainer") then
		return ComputeStringWidth(font.font, str)
	end
end

function _R.IScheme.GetFontName(font)
	local type = type(font)
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'GetFontName' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return GetFontName(font)
	elseif (type == "FontHandleContainer") then
		return GetFontName(font.font)
	end
end

function Surface.CreateFont()
	local fontcontainer = FontHandleContainer()
	fontcontainer.index = FontHandleContainerIndex
	fontcontainer.font = CreateFont()
	FontHandleContainers[FontHandleContainerIndex] = fontcontainer
	FontHandleContainerIndex = FontHandleContainerIndex + 1
	return fontcontainer
end

function Surface.DrawSetTextFont(font)
	local type = type(font)
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'DrawSetTextFont' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return DrawSetTextFont(font)
	elseif (type == "FontHandleContainer") then
		return DrawSetTextFont(font.font)
	end
end

function Surface.GetCharABCwide(font, ch)
	local type = type(font)
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'GetCharABCwide' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return GetCharABCwide(font, ch)
	elseif (type == "FontHandleContainer") then
		return GetCharABCwide(font.font, ch)
	end
end

function Surface.GetCharacterWidth(font, ch)
	local type = type(font)
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'GetCharacterWidth' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return GetCharacterWidth(font, ch)
	elseif (type == "FontHandleContainer") then
		return GetCharacterWidth(font.font, ch)
	end
end

function Surface.GetFontAscent(font, ch)
	local type = type(font)
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'GetFontAscent' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return GetFontAscent(font, ch)
	elseif (type == "FontHandleContainer") then
		return GetFontAscent(font.font, ch)
	end
end

function Surface.GetFontTall(font)
	local type = type(font)
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'GetFontTall' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return GetFontTall(font)
	elseif (type == "FontHandleContainer") then
		return GetFontTall(font.font)
	end
end

function Surface.GetTextSize(font, text)
	local type = type(font)
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'GetTextSize' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return GetTextSize(font, text)
	elseif (type == "FontHandleContainer") then
		return GetTextSize(font.font, text)
	end
end

function Surface.IsFontAdditive(font)
	local type = type(font)
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'IsFontAdditive' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return IsFontAdditive(font)
	elseif (type == "FontHandleContainer") then
		return IsFontAdditive(font.font)
	end
end

function Surface.SetFontGlyphSet(font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax)
	local type = type(font)
	if (type ~= "FontHandle" and type ~= "FontHandleContainer") then
		error("bad argument #1 to 'SetFontGlyphSet' (font or fontcontainer expected, got " .. type .. ")", 2)
	end
	if (type == "FontHandle") then
		return SetFontGlyphSet(font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax)
	elseif (type == "FontHandleContainer") then
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