--[[
	Note that most code here belongs to Facepunch Studios. It has been
	modified and adapted to provide compatibility with Experiment.
--]]

GM.Name = "Garry's Mod Base"
GM.Homepage = ""
GM.Developer = "Experiment" -- Most code by Facepunch Studios

--[[
	All our libraries are plural, while with Garry's Mod they vary between plural and singular.
--]]
concommand = require("concommands")
scripted_ents = require("entities")
gamemode = require("gamemodes")
hook = require("hooks")
weapons = require("weapons")

--[[
	Libraries taken from Garry's Mod.
	TODO: Write own libraries.
--]]
cleanup = include("modules/cleanup.lua")
player_manager = include("modules/player_manager.lua")

--[[
	Other shared functions.
--]]
unpack = unpack or table.unpack

MsgC = ConDColorMsg

PrintTable = table.Print
