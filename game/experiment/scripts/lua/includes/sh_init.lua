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

function isfunction(variable)
	return type(variable) == "function"
end

function ismatrix(variable)
	return type(variable) == "Matrix"
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
Include("extensions/console_variables.lua")
Include("extensions/entity.lua")
Include("extensions/weapon.lua")
Include("extensions/networked_variables.lua")
Include("extensions/entity_networked_variables.lua")
Include("extensions/user_messages.lua")
Include("extensions/gui.lua")
Include("extensions/color.lua")

if (CLIENT) then
	Include("extensions/panel.lua")
	Include("extensions/label.lua")
	Include("extensions/text_entry.lua")

	-- Gui elements
	Include("gui/cl_image_panel.lua")
end

Bitwise = require("bitwise")
ConsoleCommands = require("console_commands")
Gamemodes = require("gamemodes")
Hooks = require("hooks")
Timers = require("timers")

if (not GAMEUI) then
	ScriptedEntities = require("scripted_entities")
	Networks = require("networks")
	ScriptedWeapons = require("scripted_weapons")
end

local json = require("json")
Json = {
	Encode = json.encode,
	Decode = json.decode,
}

function RunConsoleCommand(command, ...)
	local commandString = command .. " " .. table.concat({ ... }, " ")
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

	--- Merges the provided metatables into base and returns it.
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
				-- TODO: Find a better way to inherit without having to exclude these keys.
				-- TODO: Because this feels hard to maintain.
				if (key == "__tostring" or key == "__eq" or key == "__gc") then
					continue
				end

				if (type(target[key]) == "table" and type(value) == "table") then
					table.Merge(target[key], value)
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
	setupMetatableMethods(_R.CBasePlayer, _R.CBaseEntity)
	setupMetatableMethods(_R.CBaseCombatWeapon, _R.CBaseEntity)

	collapseMetatables(_R.CBasePlayer, _R.CExperimentPlayer)
	collapseMetatables(_R.CBaseEntity, _R.CBaseAnimating, _R.CBaseFlex)

	_R.CExperimentPlayer.__index = _R.CBasePlayer.__index
	_R.CExperimentPlayer.__newindex = _R.CBaseEntity.__newindex

	_R.CBaseAnimating.__index = _R.CBaseEntity.__index
	_R.CBaseAnimating.__newindex = _R.CBaseEntity.__newindex

	_R.CBaseFlex.__index = _R.CBaseEntity.__index
	_R.CBaseFlex.__newindex = _R.CBaseEntity.__newindex
end
