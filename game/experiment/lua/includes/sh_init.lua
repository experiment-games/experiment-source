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

Include("extensions/enumerations.lua")
Include("extensions/console_variables.lua")
Include("extensions/debug.lua")
Include("extensions/entity.lua")
Include("extensions/networked_variables.lua")
Include("extensions/entity_networked_variables.lua")
Include("extensions/key_values.lua")
Include("extensions/math.lua")
Include("extensions/string.lua")
Include("extensions/table.lua")
Include("extensions/user_messages.lua")
Include("extensions/gui.lua")
Include("extensions/weapon.lua")
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
	Entities = require("entities")
	Networks = require("networks")
	Weapons = require("weapons")
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
		return Engine.ClientCommand(commandString)
	else
		return Engine.ServerCommand(commandString)
	end
end
