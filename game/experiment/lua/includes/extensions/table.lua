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

function table.Copy(t, tRecursive)
	if (t == nil) then
		return nil
	end
	local __copy = {}
	setmetatable(__copy, getmetatable(t))
	for i, v in pairs(t) do
		if (type(v) ~= "table") then
			__copy[i] = v
		else
			tRecursive = tRecursive or {}
			tRecursive[t] = __copy
			if (tRecursive[v]) then
				__copy[i] = tRecursive[v]
			else
				__copy[i] = table.Copy(v, tRecursive)
			end
		end
	end
	return __copy
end

function table.HasValue(t, val)
	for _, v in pairs(t) do
		if (v == val) then
			return true
		end
	end
	return false
end

function table.Inherit(t, BaseClass)
	for k, v in pairs(BaseClass) do
		if (t[k] == nil) then
			t[k] = v
		end
	end
	t.BaseClass = BaseClass
	return t
end

function table.Merge(dest, src)
	if (type(dest) ~= "table") then
		error("bad argument #1 to 'Merge' (table expected, got " .. type(dest) .. ")", 2)
	end
	if (type(src) ~= "table") then
		error("bad argument #2 to 'Merge' (table expected, got " .. type(src) .. ")", 2)
	end
	for k, v in pairs(src) do
		if (type(dest[k]) == "table" and type(v) == "table") then
			table.Merge(dest[k], v)
		else
			dest[k] = v
		end
	end
end

function table.Print(t, bOrdered, i, seen)
    i = i or 0
    seen = seen or {}
	local indent = ""

	for j = 1, i do
		indent = indent .. "\t"
	end

	if (i == 0) then
		print("Printing Table: " .. tostring(t))
	end

    if seen[t] then
        print(indent .. "*recursive*")
        return
    end
    seen[t] = true

    if (not bOrdered) then
        for k, v in pairs(t) do
            if (type(v) == "table") then
                print(indent .. k)
                table.Print(v, false, i + 1, seen)
            else
                print(indent .. k, v)
            end
        end
    else
        for j, pair in ipairs(t) do
            if (type(pair.value) == "table") then
                print(indent .. pair.key)
                table.Print(pair.value, true, i + 1, seen)
            else
                print(indent .. pair.key, pair.value)
            end
        end
    end

    seen[t] = nil
end

function table.ToKeyValues(t, setName, bOrdered)
	local pKV = KeyValues(setName)
	if (not bOrdered) then
		for k, v in pairs(t) do
			if (type(v) == "table") then
				pKV:AddSubKey(table.ToKeyValues(v, k))
			elseif (type(v) == "string") then
				pKV:SetString(k, v)
			elseif (type(v) == "number") then
				pKV:SetFloat(k, v)
			elseif (type(v) == "color") then
				pKV:SetColor(k, v)
			else
				pKV:SetString(k, tostring(v))
			end
		end
	else
		for i, pair in ipairs(t) do
			if (type(pair.value) == "table") then
				pKV:AddSubKey(table.ToKeyValues(pair.value, pair.key, true))
			else
				local pKey = pKV:CreateNewKey()
				pKey:SetName(pair.key)
				pKey:SetStringValue(tostring(pair.value))
			end
		end
	end
	return pKV
end
