--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: Dumps a list of globals from Lua to be used as a blocklist.
--]]

local globals = {}

for global, v in pairs(_G) do
	if (type(v) ~= "number" and type(v) ~= "table") then
		table.insert(globals, global)
	end
end

table.sort(globals)

local file = assert(io.open("globals.txt", "wb"))
file:write("local blocklist = {\r\n")
for i, global in ipairs(globals) do
	file:write("  \"" .. global .. "\"" .. ((i ~= #globals) and "," or "") .. "\r\n")
end
file:write("}\r\n")
assert(io.close(file))
