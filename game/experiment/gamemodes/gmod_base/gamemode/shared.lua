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

umsg.Angle = umsg.WriteAngles
umsg.Bool = umsg.WriteBool
umsg.Char = umsg.WriteChar
umsg.Entity = umsg.WriteEntity
umsg.Float = umsg.WriteFloat
umsg.Long = umsg.WriteLong
umsg.Short = umsg.WriteShort
umsg.String = umsg.WriteString
umsg.Vector = umsg.WriteVector
umsg.VectorNormal = umsg.WriteNormal
umsg.Send = umsg.MessageEnd

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

require("../modules/baseclass")
-- require("../modules/concommand") -- We have our own concommand library.
require("../modules/saverestore")
-- require("../modules/hook") -- We have our own hook library.
-- require("../modules/gamemode") -- We have our own gamemode library.
-- require("../modules/weapons") -- We have our own weapons library.
-- require("../modules/scripted_ents") -- We have our own scripted_ents library.
require("../modules/player_manager")
require("../modules/numpad")
require("../modules/team")
require("../modules/undo")
require("../modules/cleanup")
require("../modules/duplicator")
require("../modules/constraint")
require("../modules/construct")
-- require("../modules/usermessage") -- We have our own usermessage library.
require("../modules/list")
require("../modules/cvars")
require("../modules/http")
require("../modules/properties")
require("../modules/widget")
require("../modules/cookie")
require("../modules/utf8")

require("../modules/drive")
-- include("drive/drive_base.lua")
-- include("drive/drive_noclip.lua")

if (SERVER) then
	-- Server-side modules.
	require("../modules/ai_task")
	require("../modules/ai_schedule")
end

if (CLIENT) then
	-- Client-side modules.
	require("../modules/draw")
	-- require("../modules/markup") -- Not implemented atm.
	require("../modules/effects")
	require("../modules/halo")
	require("../modules/killicon")
	require("../modules/spawnmenu")
	require("../modules/controlpanel")
	-- require("../modules/presets") -- Not implemented atm.
	require("../modules/menubar")
	require("../modules/matproxy")

	-- require("../modules/notification")
	-- require("../modules/search")
end

--[[
	Other shared functions.
--]]
unpack = unpack or table.unpack

MsgC = ConDColorMsg

PrintTable = table.Print
