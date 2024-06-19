--[[
	Note that most code here belongs to Facepunch Studios. It has been
	modified and adapted to provide compatibility with Experiment.
--]]

Include("sh_enumerations.lua")
Include("sh_file.lua")

if (SERVER) then
	SendFile("sh_file.lua")
	SendFile("sh_init.lua")
	SendFile("cl_hooks.lua")
	SendFile("sh_enumerations.lua")
	SendFile("sh_util.lua")
	SendFile("util/sql.lua")
end

-- Add the gmod_compatibility path to the Lua include search path.
package.IncludePath ="lua/includes/modules/gmod_compatibility/;" .. package.IncludePath

--[[
	Renames and other polyfills
--]]
DeriveGamemode = InheritGamemode

include = Include

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

cvars = ConsoleVariables
engine = Engine
input = Input
render = Render
resource = Resources
surface = Surface
surface = Surface
system = System
vgui = Gui
VMatrix = Matrix

RealFrameTime = Globals.absoluteframetime
CurTime = Globals.curtime
FrameNumber = Globals.framecount
FrameTime = Globals.frametime
engine.TickCount = Globals.tickcount
engine.TickInterval = Globals.interval_per_tick
SoundDuration = Engine.GetSoundDuration

umsg = require("UserMessages")
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
end

function isvector(variable)
	return type(variable) == "Vector"
end

unpack = table.unpack

Material = Globals.FindMaterial
CreateMaterial = Globals.CreateMaterial
CreateConVar = function(name, value, flags, helpText, min, max)
    if (istable(flags)) then
		if (#flags == 0) then
			flags = 0
		elseif (#flags == 1) then
			flags = flags[1]
		else
			flags = bit.bor(table.unpack(flags))
		end
	end

	return ConsoleVariable(name, value, flags, helpText, min, max)
end

AddCSLuaFile = SendFile or function() end

TauntCamera = function()
    return {
        ShouldDrawLocalPlayer = function() return false end,
        CreateMove = function() end,
        CalcView = function() end,
    }
end

game = {
	IsDedicated = function() return Engine.IsDedicatedServer() end,
}

if (SERVER) then
	resource.AddWorkshop = function() end
else
	LocalPlayer = Util.GetLocalPlayer

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

	Gui._OriginalRegister = Gui._OriginalRegister or Gui.Register
    vgui.Register = function(panelName, panelTable, baseClassName)
		Gui._OriginalRegister(panelTable, panelName, baseClassName)
	end
end

--[[
	All our libraries are plural, while with Garry's Mod they vary between plural and singular.
--]]

bit = require("bitwise")
concommand = require("console_commands")
scripted_ents = require("entities")
gamemode = require("gamemodes")
hook = require("hooks")
net = require("networks")
timer = require("timers")
weapons = require("weapons")

Include("sh_util.lua")
Include("util/sql.lua")

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
-- Include("drive/drive_base.lua")
-- Include("drive/drive_noclip.lua")

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

ErrorNoHalt = function(...)
    Msg(...)
end

ErrorNoHaltWithStack = function(...)
    Msg(debug.traceback(table.concat({...}, " "), 2))
end

--[[
	Now that the compatibility libraries have been loaded, we can start changing hooks
--]]
Include("cl_hooks.lua")
