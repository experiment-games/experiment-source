--[[
	This tests whether the networked variables for entities (through datatables)
	are working as expected.

	First run:

	lua_openscript utilities/test/entity_networked_data_table.lua

	Then run:

	lua_openscript_cl utilities/test/entity_networked_data_table.lua
--]]

local client = player.GetByID(2)

if (SERVER) then
    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.BOOLEAN, 0, true)
    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.BOOLEAN, 31, false)

    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.INTEGER, 0, -123)
    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.INTEGER, 31, 456789)

    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.FLOAT, 0, 3.14159)
    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.FLOAT, 31, -2.71828)

    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.STRING, 0, "Hello, world!")
    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.STRING, 3,
        "World, I've got a long string for you to read, but at most 512 characters. Ain't that a mouthful?")

    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.VECTOR, 0, Vector(1, 2, 3))
    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.VECTOR, 31, Vector(456, 789, 101112))

    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.ANGLE, 0, Angle(1, 2, 3))
    client:SetDataTableValue(_E.NETWORK_VARIABLE_TYPE.ANGLE, 31, Angle(456, 789, 101112))
end

-- Let's assert that the values are correct.
assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.BOOLEAN, 0) == true)
assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.BOOLEAN, 31) == false)

assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.INTEGER, 0) == -123)
assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.INTEGER, 31) == 456789)

assert(math.AlmostEquals(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.FLOAT, 0), 3.14159))
assert(math.AlmostEquals(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.FLOAT, 31), -2.71828))

assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.STRING, 0) == "Hello, world!")
assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.STRING, 3) ==
    "World, I've got a long string for you to read, but at most 512 characters. Ain't that a mouthful?")

assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.VECTOR, 0) == Vector(1, 2, 3))
assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.VECTOR, 31) == Vector(456, 789, 101112))

assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.ANGLE, 0) == Angle(1, 2, 3))
assert(client:GetDataTableValue(_E.NETWORK_VARIABLE_TYPE.ANGLE, 31) == Angle(456, 789, 101112))

print("All tests passed!")
