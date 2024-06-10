--[[
	If Experiment finds a file named _loader.lua in a gamemode's directory,
	it will load it before any other file in the directory within its own
	Lua environment.

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
    --- Will modify the file so calls to "module(<something>, [package.seeall])"
    --- are replaced with:
	--- ```lua
    --- local MODULE = {}
    --- <something> = MODULE
    --- ```
    --- Any global function definitions in the file will be prefixed with
    --- "MODULE." to avoid polluting the global namespace.
    --- Finally the file will return MODULE at the end
    --- @param fileContent string
    --- @param filePath string
	--- @return string
    PreProcessFile = function(fileContent, filePath)
        local moduleFileName = filePath:match("([^/\\]+)%.lua$")

        if (not moduleFileName) then
            return fileContent
        end

		local moduleContent = {}

		local function replaceModuleCall(line)
            local moduleName = line:match("^%s*module%(([^,]+)")

            if (not moduleName) then
                return line
            end

            -- Trim whitespace and quotes
			moduleName = moduleName:match("^%s*(.-)%s*$"):gsub("\"", "")

            local replacement =
				moduleName .. " = " .. moduleName .. " or {}\n"
				.. "local MODULE = " .. moduleName .. "\n"

			return replacement, moduleName
		end

        local function replaceGlobalFunction(line, modulePrefix)
            local funcName = line:match("^%s*function ([%w_]+)")

            if (not funcName) then
                return line
            end

            return line:gsub("function ", "function " .. modulePrefix)
        end

		local moduleName, modulePrefix

        for line in fileContent:gmatch("[^\n]+") do
            if (not modulePrefix) then
                line, moduleName = replaceModuleCall(line)

				if (moduleName) then
					modulePrefix = moduleName .. "."
				end
			end

            if (modulePrefix) then
                line = replaceGlobalFunction(line, modulePrefix)
            end

            table.insert(moduleContent, line)
        end

		if (modulePrefix) then
			table.insert(moduleContent, "return MODULE")
		end

		return table.concat(moduleContent, "\n")
	end
}
