local TEXT_ENTRY_PANEL_META = _R.TextEntry

-- Without us storing these old functions, Garry's Mod compatibility will be broken.
-- TODO: Nicer fix
local originalSetFont = TEXT_ENTRY_PANEL_META.SetFont
local originalGetFont = TEXT_ENTRY_PANEL_META.GetFont

--- Sets a font by its name instead of a FontHandle. Gets the font from the scheme.
--- @param fontName string
function TEXT_ENTRY_PANEL_META:SetFontByName(fontName)
    local scheme = self:GetScheme()
	local font = scheme:GetFont(fontName)

	if (font) then
        -- self:SetFont(font)
		originalSetFont(self, font)
	end
end

--- Gets the font name from the font handle.
--- @return string
function TEXT_ENTRY_PANEL_META:GetFontName()
    local scheme = self:GetScheme()

    -- return scheme:GetFontName(self:GetFont())
    local font = originalGetFont(self)
	return scheme:GetFontName(font)
end
