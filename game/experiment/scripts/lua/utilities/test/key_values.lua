--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local gamePath
if CLIENT then
	gamePath = Engines.GetGameDirectory()
else
	gamePath = Engines.GetGameDir()
end

local pMainFile = KeyValues.Create("gameinfo.txt")
if (pMainFile:LoadFromFile(gamePath .. "/gameinfo.txt", "MOD")) then
	MsgN("KeyValues: ")
	pMainFile:Print(1)

	--[[
		print( "KeyValues -> Table: ")
		local tMainFile = pMainFile:ToTable()
		table.Print( tMainFile, false, 1 )

		print( "Table -> KeyValues: ")
		local pNewMainFile = table.ToKeyValues( tMainFile, "gameinfo.txt" )
		pNewMainFile:Print( 1 )
		pNewMainFile:DeleteThis()
	]]

	MsgN("KeyValues -> Table (Order preserved): ")
	local tMainFile = pMainFile:ToTable(true)
	table.Print(tMainFile, true, 1)

	MsgN("Table (Order preserved) -> KeyValues: ")
	local pNewMainFile = table.ToKeyValues(tMainFile, "gameinfo.txt", true)
	pNewMainFile:Print(1)
	pNewMainFile:DeleteThis()
end
pMainFile:DeleteThis()
