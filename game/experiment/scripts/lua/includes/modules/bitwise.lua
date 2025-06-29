local MODULE = {}

function MODULE.band(first, ...)
	for _, v in ipairs({ ... }) do
		first = first & v
	end

	return first
end

function MODULE.bor(first, ...)
	for _, v in ipairs({ ... }) do
		first = first | v
	end

	return first
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
