require("math")

--- Clamps a number between a minimum and maximum value.
--- @param value number The value to clamp.
--- @param min number The minimum value.
--- @param max number The maximum value.
--- @return number
function math.Clamp(value, min, max)
    return math.min(math.max(value, min), max)
end

--- Checks if floating point numbers are almost equal.
--- @param a number The first number.
--- @param b number The second number.
--- @param epsilon? number The maximum difference between the two numbers.
--- @return boolean
function math.AlmostEquals(a, b, epsilon)
	epsilon = epsilon or 0.00001
	return math.abs(a - b) < epsilon
end
