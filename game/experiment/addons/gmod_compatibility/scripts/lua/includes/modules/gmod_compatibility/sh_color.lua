local COLOR = FindMetaTable("Color")

--- Create the color copy with a different alpha value
--- @param color Color
--- @param alpha number
--- @return unknown
function ColorAlpha(color, alpha)
	return Color(color.r, color.g, color.b, alpha)
end

--- Check if the given object is a color
--- @param object any
--- @return boolean
function IsColor(object)
	return getmetatable(object) == COLOR
end

--- Creates a new color that lerps this color towards another color by the given fraction
--- @param targetColor Color
--- @param fraction number
--- @return Color
function COLOR:Lerp(targetColor, fraction)
	return Color(
		Lerp(fraction, self.r, targetColor.r),
		Lerp(fraction, self.g, targetColor.g),
		Lerp(fraction, self.b, targetColor.b),
		Lerp(fraction, self.a, targetColor.a)
	)
end

--- Convert the color to its components as 3 HSL values
--- @return number, number, number
function COLOR:ToHSL()
	return ColorToHSL(self)
end

--- Convert the color to its components as 3 HSV values
--- @return number, number, number
function COLOR:ToHSV()
	return ColorToHSV(self)
end

--- Converts the color to a vector (alpha is ignored)
--- @return Vector
function COLOR:ToVector()
	return Vector(self.r / 255, self.g / 255, self.b / 255)
end

--- Unpacks the color into 4 values (r, g, b, a)
--- @return number, number, number, number
function COLOR:Unpack()
	return self.r, self.g, self.b, self.a
end

--- Sets the color from the given components
--- @param r number
--- @param g number
--- @param b number
--- @param a number
function COLOR:SetUnpacked(r, g, b, a)
	self.r = r or 255
	self.g = g or 255
	self.b = b or 255
	self.a = a or 255
end

--- Converts the color to a table
--- @return table
function COLOR:ToTable()
	return { self.r, self.g, self.b, self.a }
end
