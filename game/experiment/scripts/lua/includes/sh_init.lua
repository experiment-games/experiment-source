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

Metatables = require("metatables")
Bitwise = require("bitwise")
Gamemodes = require("gamemodes")
Hooks = require("hooks")

if (not GAMEUI) then
	ScriptedEntities = require("scripted_entities")
	ScriptedWeapons = require("scripted_weapons")
	Timers = require("timers")
end

if (CLIENT) then
	Include("extensions/panel.lua")
	Include("extensions/label.lua")
	Include("extensions/text_entry.lua")

	-- GUI elements
    Include("gui/cl_image_panel.lua")

	-- Annoyingly, Garry's Mod doesn't really have Label nor EditablePanel metatables.
	-- That messes with the expectation of things like vgui.Register, which for example depend on SetText
	-- existing in Panel, whilst for us it exists in Label.
    -- So we merge those metatables here into Panel for compatibility.
    -- For reference, in gmod the GetValue is the same function for TextEntry and Label. So perhaps it does some type
    -- checking internally. We'll resort to hacking about it in Lua for now.
    -- TODO: clean this. Rework inheritance completely. This is a mess that I keep adding more mess to.
	-- These duplicates are neccessary because TextEntry doesn't inherit from Label and we need to merge them into Panel.
	local collapsedMethods = {}

	local function addCollapsedMethod(methodName, metatable)
		if (not collapsedMethods[methodName]) then
			collapsedMethods[methodName] = {}
		end

        collapsedMethods[methodName][metatable] = metatable[methodName]
		metatable[methodName] = nil -- Clear it so we don't have duplicates.
	end

    addCollapsedMethod("GetText", _R.Label)
    addCollapsedMethod("GetText", _R.TextEntry)
    addCollapsedMethod("SetText", _R.Label)
    addCollapsedMethod("SetText", _R.TextEntry)
    addCollapsedMethod("GetValue", _R.Label)
    addCollapsedMethod("GetValue", _R.TextEntry)
    addCollapsedMethod("SetValue", _R.Label)
    addCollapsedMethod("SetValue", _R.TextEntry)
    addCollapsedMethod("SetFont", _R.Label)
    addCollapsedMethod("SetFont", _R.TextEntry)
    addCollapsedMethod("SetWrap", _R.Label)
    addCollapsedMethod("SetWrap", _R.TextEntry)
    addCollapsedMethod("SetFontByName", _R.Label)
    addCollapsedMethod("SetFontByName", _R.TextEntry)
    addCollapsedMethod("GetFontName", _R.Label)
    addCollapsedMethod("GetFontName", _R.TextEntry)

	for methodName, metatables in pairs(collapsedMethods) do
		_R.Panel[methodName] = function(self, ...)
			local metatable = getmetatable(self)

			if (metatables[metatable]) then
				return metatables[metatable](self, ...)
			end
		end
	end

	Metatables.CollapseSkipMetamethods(_R.Panel, _R.EditablePanel, _R.Label, _R.Button, _R.Frame, _R.Html, _R.CheckButton, _R.ModelImagePanel, _R.TextEntry)

    -- TODO: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH
    -- TODO: THINK ABOUT HOW WE WANT TO DO INHERITANCE IN A WAY THAT IS MAINTAINABLE >.<"
	-- TODO: This doesnt work!
	-- -- Setup our inheritance structures so the __index metamethods exist.
	-- -- Then we merge the vgui element metatables into a single panel metatable.
	-- -- The __index of merged metatables will be the __index of the metatable
	-- -- that is merged into.
	-- Metatables.SetupSpecialInheritance(_R.EditablePanel, _R.Panel)
    -- Metatables.SetupSpecialInheritance(_R.Frame, _R.EditablePanel)

    -- Metatables.SetupSpecialInheritance(_R.TextEntry, _R.Panel)
    -- Metatables.SetupSpecialInheritance(_R.Label, _R.Panel)
    -- Metatables.SetupSpecialInheritance(_R.Button, _R.Label)

    -- -- Metatables.SetupSpecialInheritance(_R.CheckButton, _R.ToggleButton)
    -- -- Metatables.SetupSpecialInheritance(_R.ToggleButton, _R.Button)
    -- Metatables.SetupSpecialInheritance(_R.CheckButton, _R.Button)

	-- Metatables.SetupSpecialInheritance(_R.Html, _R.Panel)
    -- Metatables.SetupSpecialInheritance(_R.ModelImagePanel, _R.EditablePanel)
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
		elseif (type(value) == "Entity") then
            table[key] = {
				__type = NET_TYPE_ENTITY,
				id = value:GetEntityIndex()
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
            elseif (value.__type == NET_TYPE_ENTITY) then
				table[key] = Entities.Find(value.id)
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
	-- Setup our inheritance structures so the __index metamethods exist.
	-- Then we merge the player metatables into a single metatable.
	-- Same goes for the entity metatables.
	-- The __index of merged metatables will be the __index of the metatable
	-- that is merged into.
	Metatables.SetupSpecialInheritance(_R.Player, _R.Entity)
	Metatables.SetupSpecialInheritance(_R.Weapon, _R.Entity)

	Metatables.Collapse(_R.Player, _R.CExperimentPlayer)
	Metatables.Collapse(_R.Entity, _R.CBaseAnimating, _R.CBaseFlex)

	-- Have all these classes take the metamethods of the base classes.
	for key, value in pairs(Metatables.METAMETHODS) do
		_R.CExperimentPlayer[key] = _R.CExperimentPlayer[key] or _R.Player[key] or _R.Entity[key]
		_R.CBaseAnimating[key] = _R.CBaseAnimating[key] or _R.Entity[key]
		_R.CBaseFlex[key] = _R.CBaseFlex[key] or _R.Entity[key]
		_R.Weapon[key] = _R.Weapon[key] or _R.Entity[key]
	end
end
