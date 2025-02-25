local COLOR_META = _R.Color

local elements = {
	"r",
	"g",
	"b",
	"a",
}

--- Implements pairs operation to iterate the color elements.
-- @return Iterator function.
function COLOR_META:__pairs()
	local i = 0

	return function()
		i = i + 1

		if (elements[i]) then
			return elements[i], self[elements[i]]
		end
	end, self, nil
end
