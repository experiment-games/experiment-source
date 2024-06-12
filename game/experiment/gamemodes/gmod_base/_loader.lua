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

local isModuleSimulated = false
local isInMultilineString = false

local transformers = {
	--[[
		Helper to only track whether we are in a multiline string or not
	--]]
	{
		OnFileStart = function()
			isInMultilineString = false
		end,

		LineHandlers = {
			function(line, allLinesBefore)
				if (isInMultilineString) then
					local _, countEnd = line:gsub("%]%]", "")

					if (countEnd % 2 == 1) then
						isInMultilineString = false
					end
				end

				return line
			end,
		},
	},

	-- --[[
	-- 	Transformers to simulate the 'module' function from Lua 5.1

	-- 	Will modify the file so calls to "module(<something>)" or "module(<something>, package.seeall)"
	-- 	are replaced with:
	-- 	```lua
	-- 	local MODULE = {}
	-- 	<something> = MODULE
	-- 	```
	-- 	Any global function definitions in the file will be prefixed with
	-- 	"MODULE." to avoid polluting the global namespace.
	-- 	Finally the file will return MODULE at the end
	-- --]]
	-- {
	-- 	OnFileStart = function()
	-- 		isModuleSimulated = false
	-- 	end,

	-- 	LineHandlers = {
	-- 		-- Transforms the 'module' function call into a local variable and a module table
	-- 		function(line, allLinesBefore)
	-- 			local moduleName = line:match("^%s*module%(([^,%)]+)")

	-- 			if (not moduleName) then
	-- 				return line
	-- 			end

	-- 			local indent
	-- 			indent, moduleName = moduleName:match("^(%s*)(.-)%s*$")

	-- 			isModuleSimulated = true

	-- 			moduleName = moduleName:gsub("\"", "")

	-- 			-- Create the module table and make it available in the global scope
	-- 			local replacement = indent
	-- 				.. "local MODULE = {} "

	-- 			-- Trap new global functions into the module
	-- 			replacement = replacement
	-- 				.. "local __originalMetatableG = getmetatable(_G) "
	-- 				.. "setmetatable(_G, {"
	-- 				.. " __newindex = function(table, key, value) MODULE[key] = value end, "
	-- 				.. " __index = function(table, key) return MODULE[key] end, "
	-- 				.. "}) "

	-- 			return replacement
	-- 		end,
	-- 	},

	-- 	OnFileEnd = function(preProcessedContent)
	-- 		if (not isModuleSimulated) then
	-- 			return preProcessedContent
	-- 		end

	-- 		local restoreGlobalSnippet = "\n\n"
	-- 			.. "setmetatable(_G, __originalMetatableG) "

	-- 		return preProcessedContent .. restoreGlobalSnippet .. "\n\nreturn MODULE"
	-- 	end,
	-- },

	--[[
		Transforms C-style operators into Lua-style operators
	--]]
	{
		LineHandlers = {
			function(line, allLinesBefore)
				-- line = line:gsub("!=", "~=")
				-- line = line:gsub("&&", " and ")
				-- line = line:gsub("||", " or ")
				-- line = line:gsub("//", "--")
				-- line = line:gsub("/*", "--[[")
				-- line = line:gsub("*/", "]]")
				-- The above can cause problems with paths like "test/*" or "test//test", so let's be more specific
				-- about finding the symbols, only if they are not inside a string on the line
				if (isInMultilineString) then
					return line
				end

				local newLine = ""
				local lastChar = ""
				local inString = false
				local replacements = {
					["!="] = "~=",
					["&&"] = " and ",
					["||"] = " or ",
					["//"] = "--",
					["/*"] = "--[[",
					["*/"] = "]]",
				}

				for i = 1, #line do
					local char = line:sub(i, i)
					local nextChar = i < #line and line:sub(i + 1, i + 1) or ""

					if (char == "\"" and lastChar ~= "\\") then
						inString = not inString
					end

					if (not inString) then
						local replacement = replacements[lastChar .. char]

						if (replacement) then
							newLine = newLine:sub(1, -2) .. replacement
						elseif (char == "!" and nextChar ~= "=") then
							-- replace ! with not
							newLine = newLine .. " not "
						else
							newLine = newLine .. char
						end
					else
						newLine = newLine .. char
					end

					lastChar = char
				end

				return newLine
			end,
		},
	},
}

return {
	--- @param fileContent string
	--- @param filePath string
	--- @return string
	PreProcessFile = function(fileContent, filePath)
		local isLuaFile = filePath:match("%.lua$")

		if (not isLuaFile) then
			return fileContent
		end

		local moduleContent = {}

		for _, transformer in ipairs(transformers) do
			if (transformer.OnFileStart) then
				transformer.OnFileStart()
			end
		end

		for line in fileContent:gmatch("([^\n]*)\n?") do
			for _, transformer in ipairs(transformers) do
				for _, lineHandler in ipairs(transformer.LineHandlers) do
					line = lineHandler(line, moduleContent)
				end
			end

			table.insert(moduleContent, line)
		end

		local preProcessedContent = table.concat(moduleContent, "\n")

		for _, transformer in ipairs(transformers) do
			if (transformer.OnFileEnd) then
				preProcessedContent = transformer.OnFileEnd(preProcessedContent)
			end
		end

		-- print("====================================")
		-- print("Preprocessed " .. filePath)
		-- for line in preProcessedContent:gmatch("([^\n]*)\n?") do
		-- 	print(line)
		-- end
		-- print("====================================")

		return preProcessedContent
	end,
}
