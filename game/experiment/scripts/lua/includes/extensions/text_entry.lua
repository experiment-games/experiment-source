local TEXT_ENTRY_PANEL_META = _R.TextEntry

-- Without us storing these old functions, Garry's Mod compatibility will be broken.
-- TODO: Nicer fix
TEXT_ENTRY_PANEL_META._OriginalSetFont = TEXT_ENTRY_PANEL_META._OriginalSetFont or TEXT_ENTRY_PANEL_META.SetFont
TEXT_ENTRY_PANEL_META._OriginalGetFont = TEXT_ENTRY_PANEL_META._OriginalGetFont or TEXT_ENTRY_PANEL_META.GetFont

--- Sets a font by its name instead of a FontHandle. Gets the font from the scheme.
--- @param fontName string
function TEXT_ENTRY_PANEL_META:SetFontByName(fontName)
    local scheme = self:GetScheme()
	local font = scheme:GetFont(fontName)

	if (font) then
        -- self:SetFont(font)
		self._OriginalSetFont(self, font)
	end
end

--- Gets the font name from the font handle.
--- @return string
function TEXT_ENTRY_PANEL_META:GetFontName()
    local scheme = self:GetScheme()

    -- return scheme:GetFontName(self:GetFont())
    local font = self._OriginalGetFont(self)
	return scheme:GetFontName(font)
end
