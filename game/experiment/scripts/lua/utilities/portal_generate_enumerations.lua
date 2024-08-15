--[[
	Generates the markdown files for all Lua enumerations.

	Run in this order (since client wipes the files, which is needed to determine if it's shared):
	lua_openscript_cl utilities/portal_generate_enumerations.lua; lua_openscript utilities/portal_generate_enumerations.lua
--]]

require("Files")

local markdownTemplate = [[---
template: lua-enumeration.html
title: %title%
icon: lua-%realm%
tags:
  - lua
  - %realm%
  - needs-verification
  - needs-example
lua:
  library: %library%
  realm: %realm%
  description: ''
  enumerations:
%enumerations%
---

<div class="lua__search__keywords">
%searchKeywords%
</div>
]]

local allEnumerationsParent = _E

local function GenerateEnumerationMarkdown(enumerationName, enumerationTable)
    local enumerationMarkdown = ""
    local searchKeywords = enumerationName .. ": "
    local enumerationValues = ""

    local markdownDirectory = "docs/lua/enumerations"
    local markdownPath = markdownDirectory .. "/" .. enumerationName .. ".md"

    print("Generating " .. markdownPath)

    local realm = CLIENT and "client" or "server"

    -- If the file already exists, then the other realm also has it. Meaning this is a shared enumeration.
	if (Files.FileExists(markdownPath, "DATA")) then
		realm = "shared"
	end

    -- Let's sort by values and if they are the same, by keys
    local sortedKeys = {}
    for key, value in pairs(enumerationTable) do
        table.insert(sortedKeys, key)
    end
	table.sort(sortedKeys, function(a, b)
        if (enumerationTable[a] == enumerationTable[b]) then
            return a < b
        end

		return enumerationTable[a] < enumerationTable[b]
    end)

	for _, key in ipairs(sortedKeys) do
		local value = enumerationTable[key]
		enumerationValues = enumerationValues .. "    " .. key .. ": " .. tostring(value) .. "\n"
		searchKeywords = searchKeywords .. key .. "(" .. value .. ") "
	end

    enumerationMarkdown = string.gsub(markdownTemplate, "%%title%%", enumerationName)
    enumerationMarkdown = string.gsub(enumerationMarkdown, "%%realm%%", realm)
    enumerationMarkdown = string.gsub(enumerationMarkdown, "%%library%%", enumerationName)
    enumerationMarkdown = string.gsub(enumerationMarkdown, "%%enumerations%%", enumerationValues)
    enumerationMarkdown = string.gsub(enumerationMarkdown, "%%searchKeywords%%", searchKeywords:TrimEnd())

    local hFile = Files.Open(markdownPath, "w", "DATA")
    Files.Write(enumerationMarkdown, hFile)
    Files.Close(hFile)
end

local function GenerateAllEnumerationsMarkdown()
	if (CLIENT) then
		for _, file in ipairs(Files.Find("docs/lua/enumerations/*", "DATA")) do
			Files.RemoveFile("docs/lua/enumerations/" .. file, "DATA")
		end
	end

    for enumerationName, enumerationTable in pairs(allEnumerationsParent) do
        if (type(enumerationTable) == "table") then
            GenerateEnumerationMarkdown(enumerationName, enumerationTable)
        end
    end
end

GenerateAllEnumerationsMarkdown()
