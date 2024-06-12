--[[
	Note that most code here belongs to Facepunch Studios. It has been
	modified and adapted to provide compatibility with Experiment.
--]]

GM.Name = "Garry's Mod Base"
GM.Homepage = ""
GM.Developer = "Experiment" -- Most code by Facepunch Studios

include("enumerations.lua")

--[[
	Renames and other polyfills
--]]
util = {
	PrecacheModel = _R.CBaseEntity.PrecacheModel,
	PrecacheSound = _R.CBaseEntity.PrecacheSound,
}

local registry = debug.getregistry()
function FindMetaTable(name)
	if (name == "Entity") then
		name = "CBaseEntity"
	elseif (name == "Player") then
		name = "CBasePlayer"
	end

	return registry[name]
end

-- TODO: Actually implement SQLite
sql = {
	Begin = function() end,
	Commit = function() end,
	IndexExists = function() end,
	LastError = function() end,
	Query = function() end,
	QueryRow = function() end,
	QueryValue = function() end,
	SQLStr = function() end,
	TableExists = function() end,
}

Angle = QAngle

RealFrameTime = gpGlobals.absoluteframetime
CurTime = gpGlobals.curtime
FrameNumber = gpGlobals.framecount
FrameTime = gpGlobals.frametime
engine.TickCount = gpGlobals.tickcount
engine.TickInterval = gpGlobals.interval_per_tick

function istable(variable)
	return type(variable) == "table"
end
unpack = table.unpack

Material = gpGlobals.FindMaterial
CreateMaterial = gpGlobals.CreateMaterial
CreateConVar = function(name, value, flags, helpText, min, max)
	if (istable(flags)) then
		flags = bit.bor(table.unpack(flags))
	end

	return ConVar(name, value, flags, helpText, min, max)
end

--[[
	All our libraries are plural, while with Garry's Mod they vary between plural and singular.
--]]
concommand = require("concommands")
scripted_ents = require("entities")
gamemode = require("gamemodes")
hook = require("hooks")
weapons = require("weapons")

include("util.lua")
include("util/sql.lua")

--[[
	Libraries taken from Garry's Mod.
	TODO: Write own libraries.
--]]

-- Shared modules.

baseclass = require("../modules/baseclass")
-- concommand = require("../modules/concommand") -- We have our own concommand library.
saverestore = require("../modules/saverestore")
-- hook = require("../modules/hook") -- We have our own hook library.
-- gamemode = require("../modules/gamemode") -- We have our own gamemode library.
-- weapons = require("../modules/weapons") -- We have our own weapons library.
-- scripted_ents = require("../modules/scripted_ents") -- We have our own scripted_ents library.
player_manager = require("../modules/player_manager")
numpad = require("../modules/numpad")
team = require("../modules/team")
undo = require("../modules/undo")
cleanup = require("../modules/cleanup")
duplicator = require("../modules/duplicator")
constraint = require("../modules/constraint")
construct = require("../modules/construct")
usermessage = require("../modules/usermessage")
list = require("../modules/list")
cvars = require("../modules/cvars")
http = require("../modules/http")
properties = require("../modules/properties")
widget = require("../modules/widget")
cookie = require("../modules/cookie")
utf8 = require("../modules/utf8")

drive = require("../modules/drive")
-- include("drive/drive_base.lua")
-- include("drive/drive_noclip.lua")

if (SERVER) then
	-- Server-side modules.
	ai_task = require("../modules/ai_task")
	ai_schedule = require("../modules/ai_schedule")
end

if (CLIENT) then
	-- Client-side modules.
	draw = require("../modules/draw")
	-- markup = require("../modules/markup") -- Not implemented atm.
	effects = require("../modules/effects")
	halo = require("../modules/halo")
	killicon = require("../modules/killicon")
	spawnmenu = require("../modules/spawnmenu")
	controlpanel = require("../modules/controlpanel")
	-- presets = require("../modules/presets") -- Not implemented atm.
	menubar = require("../modules/menubar")
	matproxy = require("../modules/matproxy")

	-- notification = require("../modules/notification")
	-- search = require("../modules/search")
end

--[[
	Other shared functions.
--]]
unpack = unpack or table.unpack

MsgC = ConDColorMsg

PrintTable = table.Print
