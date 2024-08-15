local LABEL_PANEL_META = _R.Label

-- Without us storing these old functions, Garry's Mod compatibility will be broken.
-- TODO: Nicer fix
LABEL_PANEL_META._OriginalSetFont = LABEL_PANEL_META._OriginalSetFont or LABEL_PANEL_META.SetFont
LABEL_PANEL_META._OriginalGetFont = LABEL_PANEL_META._OriginalGetFont or LABEL_PANEL_META.GetFont

--- Sets a font by its name instead of a FontHandle. Gets the font from the scheme.
--- @param fontName string
function LABEL_PANEL_META:SetFontByName(fontName)
    local scheme = self:GetScheme()
	local font = scheme:GetFont(fontName)

	self._OriginalSetFont(self, font)
end

--- Gets the font name from the font handle.
--- @return string
function LABEL_PANEL_META:GetFontName()
    local scheme = self:GetScheme()
    local font = self._OriginalGetFont(self, scheme)

	return scheme:GetFontName(font)
end
