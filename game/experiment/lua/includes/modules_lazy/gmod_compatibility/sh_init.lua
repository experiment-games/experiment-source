--[[
	Note that most code here belongs to Facepunch Studios. It has been
	modified and adapted to provide compatibility with Experiment.
--]]

include("sh_enumerations.lua")

if (SERVER) then
	sendfile("sh_init.lua")
	sendfile("cl_hooks.lua")
	sendfile("sh_enumerations.lua")
	sendfile("sh_util.lua")
	sendfile("util/sql.lua")
end

-- Add the gmod_compatibility path to the Lua include search path.
package.IncludePath ="lua/includes/modules_lazy/gmod_compatibility/;" .. package.IncludePath

--[[
	Renames and other polyfills
--]]
DeriveGamemode = InheritGamemode

util = {
	PrecacheModel = _R.CBaseEntity.PrecacheModel,
	PrecacheSound = _R.CBaseEntity.PrecacheSound,

	AddNetworkString = function(name) end, -- Not needed for us.
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

function isstring(variable)
	return type(variable) == "string"
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

AddCSLuaFile = sendfile or function() end

TauntCamera = function()
	return {
		ShouldDrawLocalPlayer = function() return false end,
		CreateMove = function() end,
		CalcView = function() end,
	}
end

if (SERVER) then
    resource = resources
	resource.AddWorkshop = function() end
else
	LocalPlayer = UTIL.GetLocalPlayer

	surface.SetDrawColor = surface.DrawSetColor
	surface.DrawRect = surface.DrawFilledRect

	local textureMap = {}

	surface.GetTextureID = function(name)
		if (not textureMap[name]) then
			textureMap[name] = surface.CreateNewTextureID()
		end

		return textureMap[name]
	end

	surface.GetTextureNameByID = function(id)
		for name, textureID in pairs(textureMap) do
			if (textureID == id) then
				return name
			end
		end
	end
end

--[[
	All our libraries are plural, while with Garry's Mod they vary between plural and singular.
--]]
concommand = require("concommands")
scripted_ents = require("entities")
gamemode = require("gamemodes")
hook = require("hooks")
weapons = require("weapons")

include("sh_util.lua")
include("util/sql.lua")

--[[
	Libraries taken from Garry's Mod.
	TODO: Write own libraries.
--]]

-- Shared modules.

require("gmod_compatibility/modules/baseclass")
-- require("gmod_compatibility/modules/concommand") -- We have our own concommand library.
require("gmod_compatibility/modules/saverestore")
-- require("gmod_compatibility/modules/hook") -- We have our own hook library.
-- require("gmod_compatibility/modules/gamemode") -- We have our own gamemode library.
-- require("gmod_compatibility/modules/weapons") -- We have our own weapons library.
-- require("gmod_compatibility/modules/scripted_ents") -- We have our own scripted_ents library.
require("gmod_compatibility/modules/player_manager")
-- require("gmod_compatibility/modules/numpad")
require("gmod_compatibility/modules/team")
-- require("gmod_compatibility/modules/undo")
require("gmod_compatibility/modules/cleanup")
require("gmod_compatibility/modules/duplicator")
-- require("gmod_compatibility/modules/constraint")
-- require("gmod_compatibility/modules/construct")
-- require("gmod_compatibility/modules/usermessage") -- We have our own usermessage library.
require("gmod_compatibility/modules/list")
require("gmod_compatibility/modules/cvars")
require("gmod_compatibility/modules/http")
require("gmod_compatibility/modules/properties")
require("gmod_compatibility/modules/widget")
require("gmod_compatibility/modules/cookie")
require("gmod_compatibility/modules/utf8")

require("gmod_compatibility/modules/drive")
-- include("drive/drive_base.lua")
-- include("drive/drive_noclip.lua")

if (SERVER) then
	-- Server-side modules.
	require("gmod_compatibility/modules/ai_task")
	require("gmod_compatibility/modules/ai_schedule")
end

if (CLIENT) then
	-- Client-side modules.
	require("gmod_compatibility/modules/draw")
	-- require("gmod_compatibility/modules/markup") -- Not implemented atm.
	require("gmod_compatibility/modules/effects")
	require("gmod_compatibility/modules/halo")
	require("gmod_compatibility/modules/killicon")
	require("gmod_compatibility/modules/spawnmenu")
	require("gmod_compatibility/modules/controlpanel")
	-- require("gmod_compatibility/modules/presets") -- Not implemented atm.
	require("gmod_compatibility/modules/menubar")
	require("gmod_compatibility/modules/matproxy")

	-- require("gmod_compatibility/modules/notification")
	-- require("gmod_compatibility/modules/search")
end

--[[
	Other shared functions.
--]]
unpack = unpack or table.unpack

MsgC = ConDColorMsg

PrintTable = table.Print

--[[
	Now that the compatibility libraries have been loaded, we can start changing hooks
--]]
include("cl_hooks.lua")
