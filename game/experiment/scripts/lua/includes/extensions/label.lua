local LABEL_PANEL_META = _R.Label

-- Without us storing these old functions, Garry's Mod compatibility will be broken.
-- TODO: Nicer fix
local originalSetFont = LABEL_PANEL_META.SetFont
local originalGetFont = LABEL_PANEL_META.GetFont

--- Sets a font by its name instead of a FontHandle. Gets the font from the scheme.
--- @param fontName string
function LABEL_PANEL_META:SetFontByName(fontName)
    local scheme = self:GetScheme()
	local font = scheme:GetFont(fontName)

	originalSetFont(self, font)
end

--- Gets the font name from the font handle.
--- @return string
function LABEL_PANEL_META:GetFontName()
    local scheme = self:GetScheme()
    local font = originalGetFont(self, scheme)

	return scheme:GetFontName(font)
end
