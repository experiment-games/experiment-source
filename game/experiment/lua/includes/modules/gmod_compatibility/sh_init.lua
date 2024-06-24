--[[
	Note that most code here belongs to Facepunch Studios. It has been
	modified and adapted to provide compatibility with Experiment.
--]]

Include("sh_enumerations.lua")
Include("sh_file.lua")

-- Add the gmod_compatibility path to the Lua include search path.
package.IncludePath = "lua/includes/modules/gmod_compatibility/;" .. package.IncludePath

--[[
	Renames and other polyfills
--]]
DeriveGamemode = InheritGamemode

include = Include

util = {
	PrecacheModel = _R.CBaseEntity.PrecacheModel,
	PrecacheSound = _R.CBaseEntity.PrecacheSound,

	-- TODO: 	Things like the player manager and drive system depend on these three functions
	--			returning sensible data. Find a way to implement it:
	AddNetworkString = function(name) end,        -- Not needed for us.
	NetworkIDToString = function() return "" end, -- Not needed for us.
	NetworkStringToID = function() return 0 end,  -- Not needed for us.

	JSONToTable = function(json)
		return Json.Decode(json)
	end,

	TableToJSON = function(table)
		return Json.Encode(table)
	end,
}

ents = {
	FindAlongRay = function(...)
		return select(2, Util.EntitiesAlongRay(...))
	end,
	FindInBox = function(...)
		return select(2, Util.EntitiesInBox(...))
	end,
	FindInSphere = function(...)
		return select(2, Util.EntitiesInSphere(...))
	end,
}

player = {
	GetAll = Util.GetAllPlayers,
    GetBots = Util.GetAllBots,
	GetHumans = Util.GetAllHumans,
	GetBySteamID = Util.PlayerBySteamID,
	GetBySteamID64 = Util.PlayerBySteamID64,
}

-- We don't have LuaJIT
jit = {
	opt = function() end,
	status = function() return false end,
	version = function() return "Lua 5.4" end,
	versionnum = 0,
	arch = "x86",
}

local registry = debug.getregistry()
function FindMetaTable(name)
    if (name == "Entity") then
        name = "CBaseEntity"
    elseif (name == "Player") then
        name = "CBasePlayer"
    elseif (name == "Vehicle") then
        -- We don't have vehicles, so lets not waste time on it
        return {}
    end

    return registry[name]
end

local ENTITY_META = FindMetaTable("Entity")

ENTITY_META.GetTable = ENTITY_META.GetRefTable

local PLAYER_META = FindMetaTable("Player")

function PLAYER_META:SetClassID(id)
	-- We don't use this function, so we just store the class ID in the player.
	self.__classId = id
end

function PLAYER_META:GetClassID()
    return self.__classId
end

function PLAYER_META:IsListenServerHost()
    if (CLIENT) then
        ErrorNoHalt("IsListenServerHost has not yet been implemented on the client.")
    end

	return self == Util.GetListenServerHost()
end

if (CLIENT) then
    local PANEL_META = FindMetaTable("Panel")
	PANEL_META._OriginalSetCursor = PANEL_META._OriginalSetCursor or PANEL_META.SetCursor

    PANEL_META.GetTable = PANEL_META.GetRefTable
    PANEL_META.Dock = PANEL_META.SetDock
    PANEL_META.DockMargin = PANEL_META.SetDockMargin
	PANEL_META.DockPadding = PANEL_META.SetDockPadding

    function PANEL_META:Prepare()
        -- Installs Lua defined functions into the panel.
        -- TODO: What does that mean?
    end

    -- Maps cursor strings to cursor codes.
	local cursorMap = {
		-- ["user"] = CURSOR_CODE.USER,
		["none"] = CURSOR_CODE.NONE,
		["arrow"] = CURSOR_CODE.ARROW,
		["beam"] = CURSOR_CODE.I_BEAM,
		["hourglass"] = CURSOR_CODE.HOURGLASS,
		["waitarrow"] = CURSOR_CODE.WAIT_ARROW,
		["crosshair"] = CURSOR_CODE.CROSSHAIR,
		["up"] = CURSOR_CODE.UP,
		["sizenwse"] = CURSOR_CODE.SIZE_NWSE,
		["sizenesw"] = CURSOR_CODE.SIZE_NESW,
		["sizewe"] = CURSOR_CODE.SIZE_WE,
		["sizens"] = CURSOR_CODE.SIZE_NS,
		["sizeall"] = CURSOR_CODE.SIZE_ALL,
		["no"] = CURSOR_CODE.NO,
		["hand"] = CURSOR_CODE.HAND,
		["blank"] = CURSOR_CODE.BLANK,
		-- ["last"] = CURSOR_CODE.LAST,
		-- ["alwaysvisiblepush"] = CURSOR_CODE.ALWAYS_VISIBLE_PUSH,
		-- ["alwaysvisiblepop"] = CURSOR_CODE.ALWAYS_VISIBLE_POP,
	}

	function PANEL_META:SetCursor(cursor)
		local cursorCode = cursorMap[cursor]

		if (not cursorCode) then
			cursorCode = CURSOR_CODE.NONE
		end

		self:_OriginalSetCursor(cursorCode)
	end

    ScrW = Util.ScreenWidth
	ScrH = Util.ScreenHeight
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

engine.ActiveGamemode = function()
	return Gamemodes.GetActiveName()
end

gmod = {
	GetGamemode = function()
		return _G.GAMEMODE
	end,
}

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
		or type(variable) == "Color"
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
	IsDedicated = function()
		return Engine.IsDedicatedServer()
    end,

	SinglePlayer = function()
		return Globals.maxClients == 1
    end,

	MaxPlayers = function()
		return Globals.maxClients
	end,

	ConsoleCommand = RunConsoleCommand,
}

if (SERVER) then
	resource.AddWorkshop = function() end
else
	LocalPlayer = _R.CBasePlayer.GetLocalPlayer

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

	Gui._OriginalRegister = Gui._OriginalRegister or Gui.RegisterWithMetatable
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

function baseclassGetCompatibility(name)
	if (name:sub(1, 9) == "gamemode_") then
		name = name:sub(10)

		return Gamemodes.Get(name)
	end

	return baseclass.Get(name)
end

if (SERVER) then
	-- Server-side modules.
	require("gmod_compatibility/modules/ai_task")
	require("gmod_compatibility/modules/ai_schedule")
end

-- include("extensions/file.lua")
include("extensions/angle.lua")
include("extensions/debug.lua")
include("extensions/entity.lua")
include("extensions/ents.lua")
include("extensions/math.lua")
include("extensions/player.lua")
include("extensions/player_auth.lua")
include("extensions/string.lua")
include("extensions/table.lua")
include("extensions/util.lua")
include("extensions/vector.lua")
include("extensions/game.lua")
include("extensions/motionsensor.lua")
include("extensions/weapon.lua")
include("extensions/coroutine.lua")

-- Experiment; we load our table lib again to overwrite the GMod one.
include("../../extensions/table.lua")

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
	require("gmod_compatibility/modules/search")

	include("extensions/client/entity.lua")
	include("extensions/client/globals.lua")
	include("extensions/client/panel.lua")
	include("extensions/client/player.lua")
	include("extensions/client/render.lua")

	Include("derma/init.lua")
	Include("vgui_base.lua")
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
	Msg(debug.traceback(table.concat({ ... }, " "), 2))
end

--[[
	Now that the compatibility libraries have been loaded, we can start changing hooks
--]]
Include("cl_hooks.lua")

hook.Add("Initialize", "GModCompatibility.CallInitializeHooks", function()
	hook.Run("PreGamemodeLoaded")
	hook.Run("OnGamemodeLoaded")
	hook.Run("PostGamemodeLoaded")
end)
