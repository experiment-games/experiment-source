--[[
	This file will be automatically included on the server and client
	to bootstrap extensions and modules.
--]]

function IsValid(target)
	if (not target) then
		return false
	end

	local isValidChecker = target.IsValid

	if (not isValidChecker) then
		return false
	end

	return isValidChecker(object)
end

function isangle(variable)
	return type(variable) == "Angle"
end

function isbool(variable)
	return type(variable) == "boolean"
end

function IsColor(variable)
	return type(variable) == "Color"
end

function ispanel(variable)
	return type(variable) == "Panel"
end

function isentity(variable)
    return type(variable) == "Entity"
end
IsEntity = isentity

function isfunction(variable)
	return type(variable) == "function"
end

function ismatrix(variable)
	return type(variable) == "Matrix" or type(variable) == "Matrix3x4"
end

function isnumber(variable)
	return type(variable) == "number"
end

function isstring(variable)
	return type(variable) == "string"
end

function istable(variable)
	return type(variable) == "table"
		or type(variable) == "Color"
end

function isvector(variable)
	return type(variable) == "Vector"
end

Include("extensions/enumerations.lua")
Include("extensions/debug.lua")
Include("extensions/key_values.lua")
Include("extensions/math.lua")
Include("extensions/string.lua")
Include("extensions/table.lua")
Include("extensions/console_commands.lua")
Include("extensions/console_variables.lua")
Include("extensions/entity.lua")
Include("extensions/weapon.lua")
Include("extensions/networks.lua")
Include("extensions/networked_variables.lua")
Include("extensions/entity_networked_variables.lua")
Include("extensions/user_messages.lua")
Include("extensions/panels.lua")
Include("extensions/color.lua")

if (CLIENT) then
	Include("extensions/panel.lua")
	Include("extensions/label.lua")
	Include("extensions/text_entry.lua")

	-- GUI elements
	Include("gui/cl_image_panel.lua")
end

Bitwise = require("bitwise")
Gamemodes = require("gamemodes")
Hooks = require("hooks")
Timers = require("timers")

if (not GAMEUI) then
	ScriptedEntities = require("scripted_entities")
	ScriptedWeapons = require("scripted_weapons")
end

local json = require("json")

-- Turn Color, Vector, and Angle into tables with a special type identifier
local encodeTableSpecialTypes
encodeTableSpecialTypes = function(table)
	for key, value in pairs(table) do
		if (type(value) == "Color") then
			table[key] = {
				__type = NET_TYPE_COLOR,
				r = value.r,
				g = value.g,
				b = value.b,
				a = value.a,
			}
		elseif (type(value) == "Vector") then
			table[key] = {
				__type = NET_TYPE_VECTOR,
				x = value.x,
				y = value.y,
				z = value.z,
			}
		elseif (type(value) == "Angle") then
			table[key] = {
				__type = NET_TYPE_ANGLE,
				pitch = value.pitch,
				yaw = value.yaw,
				roll = value.roll,
			}
		elseif (type(value) == "table") then
			encodeTableSpecialTypes(value)
		end
	end
end

local decodeTableSpecialTypes
decodeTableSpecialTypes = function(table)
	for key, value in pairs(table) do
		if (type(value) == "table") then
			if (value.__type == NET_TYPE_COLOR) then
				table[key] = Colors.Create(value.r, value.g, value.b, value.a)
			elseif (value.__type == NET_TYPE_VECTOR) then
				table[key] = Vectors.Create(value.x, value.y, value.z)
			elseif (value.__type == NET_TYPE_ANGLE) then
				table[key] = Angles.Create(value.pitch, value.yaw, value.roll)
			else
				decodeTableSpecialTypes(value)
			end
		end
	end
end

Json = {
    Encode = function(value)
        local copy = table.Copy(value)
        encodeTableSpecialTypes(copy)
		return json.encode(copy)
    end,
	Decode = function(value)
		local decoded = json.decode(value)
		decodeTableSpecialTypes(decoded)
		return decoded
	end
}

function RunConsoleCommand(command, ...)
	local commandSplit = command:Split(" ")
	local arguments = { ... }

	if (#commandSplit > 1) then
		command = commandSplit[1]
		arguments = table.Merge(commandSplit, arguments)
	end

	local consoleVariable = ConsoleVariables.Get(command)

	if (IsValid(consoleVariable)) then
		consoleVariable:SetValue(table.unpack(arguments))
		return
	end

	local commandString = command .. " " .. table.concat(arguments, " ")
	commandString = commandString:Trim()

	if (CLIENT) then
		return Engines.ClientCommand(commandString)
	else
		return Engines.ServerCommand(commandString)
	end
end

if (not GAMEUI) then
	local function setupMetatableMethods(meta, metaToInherit)
		-- The newindex will be the same as the base metatable.
		meta.__newindex = metaToInherit.__newindex
		meta.__index = function(self, key)
			-- Search the metatable first.
			local value = meta[key]
			if (value ~= nil) then
				return value
			end

			-- Search the metatable to inherit.
			local valueToInherit = metaToInherit[key]

			if (valueToInherit ~= nil) then
				return valueToInherit
			end

			-- If there's a GetRefTable function, use it and see if it has the key.
			local refTableFunction = meta.GetRefTable or metaToInherit.GetRefTable

			if (refTableFunction) then
				local refTable = refTableFunction(self)

				if (refTable) then
					return refTable[key]
				end
			end

			return nil
		end
	end

	local metaMethods = {
		["__tostring"] = true,
		["__eq"] = true,
		["__gc"] = true,
		["__mode"] = true,
		["__metatable"] = true,
		["__index"] = true,
		["__newindex"] = true,
		["__call"] = true,
		["__len"] = true,
		["__pairs"] = true,
		["__ipairs"] = true,
		["__unm"] = true,
		["__add"] = true,
		["__sub"] = true,
		["__mul"] = true,
		["__div"] = true,
		["__mod"] = true,
		["__pow"] = true,
		["__concat"] = true,
		["__lt"] = true,
		["__le"] = true,
	}

	--- Merges the provided metatables into base and returns it.
	--- This will ensure that you can call subclass methods on the base class.
	--- This can be useful for NULL players for example, since you will be able
	--- to call Player methods on them.
	--- @param base any
	--- @param ... unknown
	--- @return table # The metatable merged into.
	local function collapseMetatables(base, ...)
		local target = base

		for _, metatableToMergeFrom in ipairs({ ... }) do
			if (not metatableToMergeFrom) then
				continue
			end

			for key, value in pairs(metatableToMergeFrom) do
				-- Only the 'Entity' baseclass should have metamethods.
				if (metaMethods[key]) then
					debug.PrintError("Attempted to merge a metatable with a key (" .. key .. ") that is a metamethod.")
					continue
				end

				-- Skip __name and __type so we don't overwrite the name of the metatable.
				if (key == "__name" or key == "__type") then
					continue
				end

				if (type(target[key]) == "table" and type(value) == "table") then
					table.Merge(target[key], value)
				elseif (target[key] ~= nil) then
					-- Catch mistakes where we double defined methods
					debug.PrintError("Attempted to merge a metatable (" .. tostring(target) .. ") with a key (" .. key .. ") that already exists.")
					continue
				else
					target[key] = value
				end
			end
		end

		return target
	end

	-- Setup our inheritance structures so the __index metamethods exist.
	-- Then we merge the player metatables into a single metatable.
	-- Same goes for the entity metatables.
	-- The __index of merged metatables will be the __index of the metatable
	-- that is merged into.
	setupMetatableMethods(_R.Player, _R.Entity)
	setupMetatableMethods(_R.Weapon, _R.Entity)

	collapseMetatables(_R.Player, _R.CExperimentPlayer)
	collapseMetatables(_R.Entity, _R.CBaseAnimating, _R.CBaseFlex)

	-- Have all these classes take the metamethods of the base classes.
	for key, value in pairs(metaMethods) do
		_R.CExperimentPlayer[key] = _R.CExperimentPlayer[key] or _R.Player[key] or _R.Entity[key]
		_R.CBaseAnimating[key] = _R.CBaseAnimating[key] or _R.Entity[key]
		_R.CBaseFlex[key] = _R.CBaseFlex[key] or _R.Entity[key]
		_R.Weapon[key] = _R.Weapon[key] or _R.Entity[key]
	end
end
