--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: Print the similarities between two tables.
--]]

-- Andrew; dump your table contents in the two following tables:
local t1 = {
	"foo"
}
local t2 = {
	"foo",
	"bar"
}

-- This is reserved to tell us what is the same - value comparison only.
local ts = {}

for _, cmp1 in pairs(t1) do
	for _, cmp2 in pairs(t2) do
		if (cmp1 == cmp2) then
			table.insert(ts, cmp1)
			-- We don't care about duplicates, break this loop
			break
		end
	end
end

table.sort(ts)

if (#ts == 0) then
	MsgN("Found no shared values!")
else
	MsgN("Found " .. #ts .. " shared values!")
	for _, v in pairs(ts) do
		MsgN(v)
	end
end
