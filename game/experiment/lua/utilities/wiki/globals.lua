--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: Dumps a list of globals in wiki format for documentation.
--]]

local blocklist = {
	"_VERSION",
	"assert",
	"collectgarbage",
	"dofile",
	"error",
	"gcinfo",
	"getfenv",
	"getmetatable",
	"ipairs",
	"load",
	"loadfile",
	"loadstring",
	"module",
	"newproxy",
	"next",
	"pairs",
	"pcall",
	"print",
	"rawequal",
	"rawget",
	"rawset",
	"require",
	"select",
	"setfenv",
	"setmetatable",
	"tonumber",
	"tostring",
	"type",
	"unpack",
	"xpcall"
}

local bBlacklisted = false
local globals = {}

for global, fn in pairs(_G) do
	for _, v in pairs(blocklist) do
		if (global == v) then
			bBlacklisted = true
		end
	end
	if (not bBlacklisted and type(fn) ~= "number" and type(fn) ~= "table") then
		table.insert(globals, global)
	end
	bBlacklisted = false
end

table.sort(globals)

local file = assert(io.open("globals.txt", "wb"))
for i, global in ipairs(globals) do
	file:write("*[[_G." .. global .. "|" .. global .. "]]\r\n")
end
assert(io.close(file))
