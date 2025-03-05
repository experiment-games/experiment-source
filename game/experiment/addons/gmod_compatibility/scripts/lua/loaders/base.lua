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
		-- Replace DEFINE_BASECLASS with local BaseClass = baseclass
		fileContent = fileContent:gsub("DEFINE_BASECLASS", "local BaseClass = baseclassGetCompatibility")

        if (not filePath:match("gamemode[/\\]cl_init%.lua$") and not filePath:match("gamemode[/\\]init%.lua$")) then
            return fileContent
        end

        -- Split the paths at \ or / and get the last part
        local parts = {}

        for part in filePath:gmatch("([^/\\]+)[/\\]") do
            table.insert(parts, part)
        end

        -- Folder where gamemode folder (which contains cl_init.lua and init.lua) is located
        local rootFolder = ""

		-- -1 tp not get the last part (cl_init.lua or init.lua) nor the gamemode/ folder
		for i = 1, #parts - 1 do
			rootFolder = rootFolder .. parts[i] .. "/"
		end

        local isBaseGamemode = filePath:match("[/\\]base[/\\]gamemode[/\\]cl_init%.lua$")
			or filePath:match("[/\\]base[/\\]gamemode[/\\]init%.lua$")

		if (isBaseGamemode) then
			-- The base gamemode explicitly loads its entities and sweps before any other entity or swep is loaded, since
			-- those likely depend on the base gamemode's entities (base_entity, base_ai, base_nextbot) and weapons (weapon_base).
			return "require(\"gmod_compatibility\")\n\n"
				.. fileContent
				.. "\nScriptedEntities.LoadFromDirectory( \""..rootFolder.."entities/entities\" ) ScriptedWeapons.LoadFromDirectory( \""..rootFolder.."entities/weapons\" )\n"
		end

		-- Load the gmod_compatibility module to make Garry's Mod code compatible with Experiment
		return "require(\"gmod_compatibility\")\n\n"
            .. fileContent
			.. "\nFiles.AddSearchPath( \""..rootFolder.."entities\", \"LUA\" )\n" -- Ensure entities and weapons are loaded by Experiment
	end,
}
