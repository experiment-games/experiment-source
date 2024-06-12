bit = bit or {}
local MODULE = bit

function MODULE.band(a, b)
	return a & b
end

function MODULE.bor(a, b)
	return a | b
end

function MODULE.bxor(a, b)
	return a ~ b
end

function MODULE.bnot(a)
	return ~a
end

function MODULE.lshift(a, b)
	return a << b
end

function MODULE.rshift(a, b)
	return a >> b
end

return MODULE
