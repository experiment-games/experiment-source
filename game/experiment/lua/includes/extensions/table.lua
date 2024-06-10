--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

require("table")

local setmetatable = setmetatable
local getmetatable = getmetatable
local pairs = pairs
local type = type
local print = print
local KeyValues = KeyValues

function table.Copy(target, shouldCopyRecursively)
	if (target == nil) then
		return nil
	end

	local __copy = {}
	setmetatable(__copy, getmetatable(target))

	for key, value in pairs(target) do
		if (type(value) ~= "table") then
			__copy[key] = value
		else
			shouldCopyRecursively = shouldCopyRecursively or {}
			shouldCopyRecursively[target] = __copy
			if (shouldCopyRecursively[value]) then
				__copy[key] = shouldCopyRecursively[value]
			else
				__copy[key] = table.Copy(value, shouldCopyRecursively)
			end
		end
	end

	return __copy
end

function table.HasValue(target, valueToFind)
	for key, value in pairs(target) do
		if (value == valueToFind) then
			return true, key
		end
	end

	return false, nil
end

function table.Inherit(target, tableToInherit)
	if (type(target) ~= "table") then
		error(debug.traceback("bad argument #1 to 'Inherit' (table expected, got " .. type(target) .. ")", 2))
	end

	if (type(tableToInherit) ~= "table") then
		error(debug.traceback("bad argument #2 to 'Inherit' (table expected, got " .. type(tableToInherit) .. ")", 2))
	end

	for k, v in pairs(tableToInherit) do
		if (target[k] == nil) then
			target[k] = v
		end
	end

	target.BaseClass = tableToInherit

	return target
end

function table.Merge(destination, source)
	if (type(destination) ~= "table") then
		error(debug.traceback("bad argument #1 to 'Merge' (table expected, got " .. type(destination) .. ")", 2))
	end

	if (type(source) ~= "table") then
		error(debug.traceback("bad argument #2 to 'Merge' (table expected, got " .. type(source) .. ")", 2))
	end

	for key, value in pairs(source) do
		if (type(destination[key]) == "table" and type(value) == "table") then
			table.Merge(destination[key], value)
		else
			destination[key] = value
		end
	end
end

function table.Print(target, isSequential, indentLevel, tablesVisited)
	indentLevel = indentLevel or 0
	tablesVisited = tablesVisited or {}
	local indent = ""

	for j = 1, indentLevel do
		indent = indent .. "\t"
	end

	if (indentLevel == 0) then
		print("Printing Table: " .. tostring(target))
	end

	if tablesVisited[target] then
		print(indent .. "*recursive*")
		return
	end

	tablesVisited[target] = true

	if (not isSequential) then
		for key, value in pairs(target) do
			if (type(value) == "table") then
				print(indent .. key)
				table.Print(value, false, indentLevel + 1, tablesVisited)
			else
				print(indent .. key, value)
			end
		end
	else
		for _, pair in ipairs(target) do
			if (type(pair.value) == "table") then
				print(indent .. pair.key)
				table.Print(pair.value, true, indentLevel + 1, tablesVisited)
			else
				print(indent .. pair.key, pair.value)
			end
		end
	end

	tablesVisited[target] = nil
end

function table.ToKeyValues(target, setName, isSequential)
	local keyValueSet = KeyValues(setName)

	if (not isSequential) then
		for key, value in pairs(target) do
			if (type(value) == "table") then
				keyValueSet:AddSubKey(table.ToKeyValues(value, key))
			elseif (type(value) == "string") then
				keyValueSet:SetString(key, value)
			elseif (type(value) == "number") then
				keyValueSet:SetFloat(key, value)
			elseif (type(value) == "color") then
				keyValueSet:SetColor(key, value)
			else
				keyValueSet:SetString(key, tostring(value))
			end
		end
	else
		for _, pair in ipairs(target) do
			if (type(pair.value) == "table") then
				keyValueSet:AddSubKey(table.ToKeyValues(pair.value, pair.key, true))
			else
				local pKey = keyValueSet:CreateNewKey()
				pKey:SetName(pair.key)
				pKey:SetStringValue(tostring(pair.value))
			end
		end
	end

	return keyValueSet
end
