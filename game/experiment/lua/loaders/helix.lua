--[[
	If Experiment finds a file named {gamemodename}.lua in a the lua/loaders
	directory, it will load it before any file in the gamemode.

	This file must return a table containing the following possible methods:

	- PreProcessFile( string fileContent, string filePath ) : string
		This method is called before the file is loaded. It receives the file's
		path and content as arguments and must return the content as a string.

		You can use this to replace or modify the content of the file before it
		is loaded into the final Lua environment.

	Currently no other methods are supported.

	The loader environment only gets the following libraries and functions loaded:

		- Base Lua Functions: error, ipairs, next, pairs, print, select, tonumber,
			tostring, type
		- Lua Libraries with their functions:
			- table
			- string
			- utf8
			- math
		- Globals: _G, _VERSION

	Finally this file is always sent to the client, this is so that the client
	can also load the files with the same modifications as the server.
--]]

return {
	--- @param fileContent string
	--- @param filePath string
	--- @return string
	PreProcessFile = function(fileContent, filePath)
		fileContent = fileContent:gsub("DEFINE_BASECLASS", "local BaseClass = baseclass.Get")

		--- Adds GM.Base = "gmod_sandbox" to the end of the gamemode/shared.lua
		--- file to ensure the gamemode is loaded with Garry's Mod compatibility.
		if (not filePath:match("gamemode[/\\]shared%.lua$")) then
			return fileContent
		end

		return "require(\"gmod_compatibility\")\n\n" -- Add the gmod_compatibility module
			.. fileContent
			.. "\nGM.Base = \"gmod_sandbox\""
	end,
}
