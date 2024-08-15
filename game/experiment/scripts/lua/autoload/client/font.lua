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

local ComputeStringWidth = Utilities.ComputeStringWidth
local GetFontName = _R.Scheme.GetFontName
local CreateFont = Surfaces.CreateFont
local DrawSetTextFont = Surfaces.DrawSetTextFont
local GetCharABCwide = Surfaces.GetCharABCwide
local GetCharacterWidth = Surfaces.GetCharacterWidth
local GetFontAscent = Surfaces.GetFontAscent
local GetFontTall = Surfaces.GetFontTall
local GetTextSize = Surfaces.GetTextSize
local IsFontAdditive = Surfaces.IsFontAdditive
local SetFontGlyphSet = Surfaces.SetFontGlyphSet

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

function Utilities.ComputeStringWidth(font, str)
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

function _R.Scheme.GetFontName(scheme, font)
    local resolvedFont = Surfaces.ResolveFont(font, scheme)

	return GetFontName(scheme, resolvedFont)
end

function Surfaces.CreateFont()
	local fontcontainer = FontHandleContainer()
	fontcontainer.index = FontHandleContainerIndex
	fontcontainer.font = CreateFont()
	FontHandleContainers[FontHandleContainerIndex] = fontcontainer
	FontHandleContainerIndex = FontHandleContainerIndex + 1
	return fontcontainer
end

function Surfaces.ResolveFont(font, scheme)
	local type = type(font)

    if (type ~= "string" and type ~= "FontHandle" and type ~= "FontHandleContainer") then
        error("bad argument #1 to 'ResolveFont' (string, font or fontcontainer expected, got " .. type .. ")", 2)
    end

	if (type == "string") then
		scheme = scheme or Schemes.GetScheme("ClientScheme")

		return scheme:GetFont(font)
	elseif (type == "FontHandle") then
		return font
	elseif (type == "FontHandleContainer") then
		return font.font
	end
end

function Surfaces.DrawSetTextFont(font)
    local resolvedFont = Surfaces.ResolveFont(font)

    return DrawSetTextFont(resolvedFont)
end

function Surfaces.GetCharABCwide(font, ch)
    local resolvedFont = Surfaces.ResolveFont(font)

	return GetCharABCwide(resolvedFont, ch)
end

function Surfaces.GetCharacterWidth(font, ch)
    local resolvedFont = Surfaces.ResolveFont(font)

	return GetCharacterWidth(resolvedFont, ch)
end

function Surfaces.GetFontAscent(font, ch)
    local resolvedFont = Surfaces.ResolveFont(font)

	return GetFontAscent(resolvedFont, ch)
end

function Surfaces.GetFontTall(font)
    local resolvedFont = Surfaces.ResolveFont(font)

	return GetFontTall(resolvedFont)
end

function Surfaces.GetTextSize(font, text)
    local resolvedFont = Surfaces.ResolveFont(font)

	return GetTextSize(resolvedFont, text)
end

function Surfaces.IsFontAdditive(font)
    local resolvedFont = Surfaces.ResolveFont(font)

	return IsFontAdditive(resolvedFont)
end

function Surfaces.SetFontGlyphSet(font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax)
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
