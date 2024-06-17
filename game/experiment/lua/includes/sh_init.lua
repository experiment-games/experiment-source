--[[
	This file will be automatically included on the server and client
	to bootstrap extensions and modules.
--]]

Include("extensions/console_variables.lua")
Include("extensions/debug.lua")
Include("extensions/entity.lua")
Include("extensions/key_values.lua")
Include("extensions/math.lua")
Include("extensions/panel.lua")
Include("extensions/table.lua")
Include("extensions/user_messages.lua")
Include("extensions/gui.lua")
Include("extensions/weapon.lua")

Bitwise = require("bitwise")
ConsoleCommands = require("console_commands")
Entities = require("entities")
Gamemodes = require("gamemodes")
Hooks = require("hooks")
Networks = require("networks")
Timers = require("timers")
Weapons = require("weapons")
