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

baseclass = include("../modules/baseclass.lua")
-- concommand = include("../modules/concommand.lua") -- We have our own concommand library.
saverestore = include("../modules/saverestore.lua")
-- hook = include("../modules/hook.lua") -- We have our own hook library.
-- gamemode = include("../modules/gamemode.lua") -- We have our own gamemode library.
-- weapons = include("../modules/weapons.lua") -- We have our own weapons library.
-- scripted_ents = include("../modules/scripted_ents.lua") -- We have our own scripted_ents library.
player_manager = include("../modules/player_manager.lua")
numpad = include("../modules/numpad.lua")
team = include("../modules/team.lua")
undo = include("../modules/undo.lua")
cleanup = include("../modules/cleanup.lua")
duplicator = include("../modules/duplicator.lua")
constraint = include("../modules/constraint.lua")
construct = include("../modules/construct.lua")
usermessage = include("../modules/usermessage.lua")
list = include("../modules/list.lua")
cvars = include("../modules/cvars.lua")
http = include("../modules/http.lua")
properties = include("../modules/properties.lua")
widget = include("../modules/widget.lua")
cookie = include("../modules/cookie.lua")
utf8 = include("../modules/utf8.lua")

drive = include("../modules/drive.lua")
-- include("drive/drive_base.lua")
-- include("drive/drive_noclip.lua")

if (SERVER) then
	-- Server-side modules.
	ai_task = include("../modules/ai_task.lua")
	ai_schedule = include("../modules/ai_schedule.lua")
end

if (CLIENT) then
	-- Client-side modules.
	draw = include("../modules/draw.lua")
	-- markup = include("../modules/markup.lua") -- Not implemented atm.
	effects = include("../modules/effects.lua")
	halo = include("../modules/halo.lua")
	killicon = include("../modules/killicon.lua")
	spawnmenu = include("../modules/spawnmenu.lua")
	controlpanel = include("../modules/controlpanel.lua")
	-- presets = include("../modules/presets.lua") -- Not implemented atm.
	menubar = include("../modules/menubar.lua")
	matproxy = include("../modules/matproxy.lua")

	-- notification = include("../modules/notification.lua")
	-- search = include("../modules/search.lua")
end

--[[
	Other shared functions.
--]]
unpack = unpack or table.unpack

MsgC = ConDColorMsg

PrintTable = table.Print
