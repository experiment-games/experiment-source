--[[
	Generates the markdown files for all Lua enumerations.

	lua_openscript_cl utilities/portal_generate_enumerations.lua
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
  %enumerations%
---

<div class="lua__search__keywords">
%searchKeywords%
</div>
]]

local allEnumerationsParent = _E

local function GenerateEnumerationMarkdown(enumerationName, enumerationTable, realm)
    local enumerationMarkdown = ""
    local searchKeywords = ""
    local enumerationValues = ""

    for key, value in pairs(enumerationTable) do
        enumerationValues = enumerationValues .. "- `" .. key .. "`: " .. tostring(value) .. "\n"
        searchKeywords = searchKeywords .. key .. " "
    end

    enumerationMarkdown = string.gsub(markdownTemplate, "%%title%%", enumerationName)
    enumerationMarkdown = string.gsub(enumerationMarkdown, "%%realm%%", realm)
    enumerationMarkdown = string.gsub(enumerationMarkdown, "%%library%%", "GMod Compatibility")
    enumerationMarkdown = string.gsub(enumerationMarkdown, "%%enumerations%%", enumerationValues)
    enumerationMarkdown = string.gsub(enumerationMarkdown, "%%searchKeywords%%", searchKeywords)

    local markdownDirectory = "docs/lua/enumerations/"
	Files.CreateDirectoryHierarchy(markdownDirectory)
    local markdownPath = markdownDirectory .. "/" .. enumerationName .. ".md"
    local hFile = Files.Open(markdownPath, "w")
    Files.Write(enumerationMarkdown, hFile)
    Files.Close(hFile)
end

local function GenerateAllEnumerationsMarkdown()
    for enumerationName, enumerationTable in pairs(allEnumerationsParent) do
        if (type(enumerationTable) == "table") then
            GenerateEnumerationMarkdown(enumerationName, enumerationTable, "gmod_compatibility")
        end
    end
end

GenerateAllEnumerationsMarkdown()
