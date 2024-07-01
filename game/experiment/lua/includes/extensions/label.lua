local LABEL_PANEL_META = _R.Label

-- Without us storing these old functions, Garry's Mod compatibility will be broken.
-- TODO: Nicer fix
LABEL_PANEL_META._OriginalSetFont = LABEL_PANEL_META._OriginalSetFont or LABEL_PANEL_META.SetFont
LABEL_PANEL_META._OriginalGetFont = LABEL_PANEL_META._OriginalGetFont or LABEL_PANEL_META.GetFont

--- Sets a font by its name instead of a FontHandle. Gets the font from the scheme.
--- @param fontName string
function LABEL_PANEL_META:SetFontByName(fontName)
    local scheme = self:GetScheme()
	local iScheme = Scheme.GetIScheme(scheme)
	local font = iScheme:GetFont(fontName)

	if (font) then
        -- self:SetFont(font)
		self._OriginalSetFont(self, font)
	end
end

--- Gets the font name from the font handle.
--- @return string
function LABEL_PANEL_META:GetFontName()
    local scheme = self:GetScheme()
    local iScheme = Scheme.GetIScheme(scheme)

    -- return iScheme:GetFontName(self:GetFont())
    local font = self._OriginalGetFont(self)
	return iScheme:GetFontName(font)
end

print("Loaded extensions/label.lua")
