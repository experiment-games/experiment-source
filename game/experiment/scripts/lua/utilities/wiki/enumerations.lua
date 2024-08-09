--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: Dumps a list of all enumerations in _E in wiki format for documentation.
--]]

local enumerations = {}

for prefix, t in pairs(_E) do
	for name, n in pairs(t) do
		table.insert(enumerations, prefix .. "_" .. name)
	end
end

table.sort(enumerations)

local file = assert(io.open("enumerations.txt", "wb"))
for i, enumeration in ipairs(enumerations) do
	file:write("*[[_E." .. enumeration .. "|" .. enumeration .. "]]\r\n")
end
assert(io.close(file))
