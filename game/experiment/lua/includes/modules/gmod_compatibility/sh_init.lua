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

GetConVar_Internal = GetConsoleVariable

util = {
	PrecacheModel = _R.CBaseEntity.PrecacheModel,
	PrecacheSound = _R.CBaseEntity.PrecacheSound,

	-- TODO: 	Things like the player manager and drive system depend on these three functions
	--			returning sensible data. Find a way to implement it:
	AddNetworkString = function(name) end,     -- Not needed for us.
	NetworkIDToString = function() return "" end, -- Not needed for us.
	NetworkStringToID = function() return 0 end, -- Not needed for us.

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
system = System
vgui = Gui
VMatrix = Matrix

RealFrameTime = Globals.AbsoluteFrameTime
CurTime = Globals.CurrentTime
SysTime = Globals.SystemTime
RealTime = Globals.RealTime
FrameNumber = Globals.FrameCount
FrameTime = Globals.FrameTime
engine.TickCount = Globals.TickCount
engine.TickInterval = Globals.IntervalPerTick
SoundDuration = Engine.GetSoundDuration

PrecacheParticleSystem = ParticleSystem.Precache

engine.ActiveGamemode = function()
	return Gamemodes.GetActiveName()
end
engine.GetGames = function() return {} end
engine.GetAddons = function() return {} end    -- TODO: Implement with our addon system
engine.GetGamemodes = function() return {} end -- TODO: Implement with our gamemode system
engine.GetUserContent = function() return {} end

gmod = {
	GetGamemode = function()
		return _G.GAMEMODE
	end,
}

language = {
	GetPhrase = function(phrase)
		return Localization.Find(phrase)
	end,

	Add = function(key, value)
		Localization.AddString(key, value)
	end,
}

notification = {
	AddLegacy = function(text, type, length)
		print("Notification: " .. text)
	end,

	AddProgress = function(id, text, frac)
		print("Progress Notification: " .. text)
	end,

	Kill = function(uid)
		print("Killing notification with UID: " .. uid)
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

-- Have table.insert return the position where the value was inserted
table._OriginalInsert = table._OriginalInsert or table.insert

---@overload fun(list: table, value: any)
---@param list table
---@param positionOrValue integer|any
---@param value? any|nil
---@return integer
---@diagnostic disable-next-line: duplicate-set-field
function table.insert(list, positionOrValue, value)
	if (not value) then
		table._OriginalInsert(list, positionOrValue)
		return #list
	end

	table._OriginalInsert(list, positionOrValue, value)

	return positionOrValue
end

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

	return CreateConsoleVariable(name, value, flags, helpText, min, max)
end

AddCSLuaFile = SendFile or function() end

TauntCamera = function()
	return {
		ShouldDrawLocalPlayer = function() return false end,
		CreateMove = function() end,
		CalcView = function() end,
	}
end

-- We don't use the workshop
WorkshopFileBase = function(namespace, requiredTags)
	return {}
end

game = {
	IsDedicated = function()
		return Engine.IsDedicatedServer()
	end,

	SinglePlayer = function()
		return Globals.MaxClients == 1
	end,

	MaxPlayers = function()
		return Globals.MaxClients
	end,

	ConsoleCommand = RunConsoleCommand,

	AddParticles = function(filePath)
		-- Remove particles/ from the start of the file path
		filePath = filePath:sub(11)

		return ParticleSystem.ReadConfigFile("particles/gmod_compatibility_content/" .. tostring(filePath))
	end,

	GetMap = Engine.GetLevelName,
}

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

local TEXTURE_META = FindMetaTable("ITexture")

function TEXTURE_META:Width()
	return self:GetActualWidth()
end

function TEXTURE_META:Height()
	return self:GetActualHeight()
end

local MATERIAL_META = FindMetaTable("IMaterial")
MATERIAL_META.GetShader = MATERIAL_META.GetShaderName

function MATERIAL_META:Width()
	local baseTexture = self:GetTexture("$basetexture")

	if (baseTexture) then
		return baseTexture:GetActualWidth()
	end

	return 0
end

function MATERIAL_META:Height()
	local baseTexture = self:GetTexture("$basetexture")

	if (baseTexture) then
		return baseTexture:GetActualHeight()
	end

	return 0
end

--[[
	We don't use achievements:
--]]
achievements = {
	BalloonPopped = function() end,
	Count = function() return 0 end,
	EatBall = function() end,
	GetCount = function() return 0 end,
	GetDesc = function() return "" end,
	GetGoal = function() return 0 end,
	GetName = function() return "" end,
	IncBaddies = function() end,
	IncBystander = function() end,
	IncGoodies = function() end,
	IsAchieved = function() return false end,
	Remover = function() end,
	SpawnedNPC = function() end,
	SpawnedProp = function() end,
	SpawnedRagdoll = function() end,
	SpawnMenuOpen = function() end,
}

if (SERVER) then
    resource.AddWorkshop = function() end

	Material = function(name) end
else
	Material = function(name)
		if (not Surface.DoesMaterialExist(name)) then
			name = "gmod_compatibility_content/" .. name
		end

		return Surface.FindMaterial(name)
	end

    CreateMaterial = Surface.CreateMaterial

	local PANEL_META = FindMetaTable("Panel")
	PANEL_META._OriginalSetCursor = PANEL_META._OriginalSetCursor or PANEL_META.SetCursor
	PANEL_META._OriginalGetParent = PANEL_META._OriginalGetParent or PANEL_META.GetParent

	PANEL_META.Remove = PANEL_META.DeletePanel
	PANEL_META.GetTable = PANEL_META.GetRefTable
	PANEL_META.Dock = PANEL_META.SetDock
	PANEL_META.DockMargin = PANEL_META.SetDockMargin
	PANEL_META.DockPadding = PANEL_META.SetDockPadding
    PANEL_META.ChildCount = PANEL_META.GetChildCount

	function PANEL_META:GetParent()
		local parent = self:_OriginalGetParent()

		if (not IsValid(parent)) then
			return nil
		end

		return parent
	end

	function PANEL_META:Prepare()
		-- Installs Lua defined functions into the panel.
		-- TODO: What does that mean? That all functions are only here stored into the C panel?

		-- ! HACK! This is a workaround for the following problem:
		-- ! 1. We Gui.Create("SubPanel") which is based on "ParentPanel"
		-- ! 2. Gui.Create will :Init() and :MarkAsInitialized() the parent panel
		-- ! 3. Gui.Create will merge the parent panel's functions with the child panel
		-- ! 4. If the child panel does something to trigger panel hooks inside :Init() those
		-- ! 	will be called.
		-- ! 	This can cause issues because the child panel may not be fully initialized yet
		-- ! 	(e.g: another child panel may not have been setup yet inside :Init())
		-- TODO: Fix this weird hack
		local _self = self
		timer.Simple(0, function()
			if (not IsValid(_self)) then
				return
			end
			_self:MarkAsInitialized()
			_self:InvalidateLayout(true)
		end)
	end

	function PANEL_META:SetWorldClicker(isEnabled)
		-- TODO: Implement this in Lua
	end

	-- Change casing and add functionality to pass individual color components.
	function PANEL_META:SetBGColor(rOrColor, g, b, a)
		if (istable(rOrColor)) then
			self:SetBgColor(rOrColor)
		else
			self:SetBgColor(Color(rOrColor, g, b, a))
		end
	end

	-- Change casing and add functionality to pass individual color components.
	function PANEL_META:SetFGColor(rOrColor, g, b, a)
		if (istable(rOrColor)) then
			self:SetFgColor(rOrColor)
		else
			self:SetFgColor(Color(rOrColor, g, b, a))
		end
	end

	local LABEL_PANEL_META = FindMetaTable("Label")
	LABEL_PANEL_META._OriginalSetFont = LABEL_PANEL_META._OriginalSetFont or LABEL_PANEL_META.SetFont
	LABEL_PANEL_META._OriginalGetFont = LABEL_PANEL_META._OriginalGetFont or LABEL_PANEL_META.GetFont

	function LABEL_PANEL_META:SetFontInternal(font)
		self:SetFontByName(font)
	end

	function LABEL_PANEL_META:SetFont(font)
		self:SetFontByName(font)
	end

	function LABEL_PANEL_META:GetFont()
		return self:GetFontName()
	end

	-- TODO: Actually draw the drop shadow
	function LABEL_PANEL_META:SetExpensiveShadow(distance, color)
		self._expensiveShadow = { distance = distance, color = color }
	end

	local TEXT_ENTRY_PANEL_META = FindMetaTable("TextEntry")
	TEXT_ENTRY_PANEL_META._OriginalSetFont = TEXT_ENTRY_PANEL_META._OriginalSetFont or TEXT_ENTRY_PANEL_META.SetFont
	TEXT_ENTRY_PANEL_META._OriginalGetFont = TEXT_ENTRY_PANEL_META._OriginalGetFont or TEXT_ENTRY_PANEL_META.GetFont
	TEXT_ENTRY_PANEL_META.DrawTextEntryText = TEXT_ENTRY_PANEL_META.PaintText

	function TEXT_ENTRY_PANEL_META:SetFontInternal(font)
		self:SetFontByName(font)
	end

	function TEXT_ENTRY_PANEL_META:SetFont(font)
		self:SetFontByName(font)
	end

	function TEXT_ENTRY_PANEL_META:GetFont()
		return self:GetFontName()
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

	GetRenderTargetEx = render.CreateRenderTargetTextureEx
	GetRenderTarget = render.CreateRenderTargetTextureEx

	LocalPlayer = _R.CBasePlayer.GetLocalPlayer

	surface.SetDrawColor = surface.DrawSetColor
	surface.DrawRect = surface.DrawFilledRect

	local textureMap = {}

	surface.GetTextureID = function(name)
		if (not file.Exists("materials/" .. name .. ".vmt", "GAME") and not file.Exists("materials/" .. name, "GAME")) then
			name = "gmod_compatibility_content/" .. name
		end

		if (not textureMap[name]) then
			textureMap[name] = surface.CreateNewTextureID()
			surface.DrawSetTextureFile(textureMap[name], name)
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

	local function getAppropriateBaseParent()
		if (GAMEUI) then
			return Gui.GetGameUIPanel()
		end

		return Gui.GetClientLuaRootPanel()
	end

	function Gui.Create(panelName, parentPanel, name)
		parentPanel = parentPanel or getAppropriateBaseParent()

		if (not Gui[panelName]) then
			error("attempt to create non-existing panel class \"" .. tostring(panelName) .. "\"", 2)
		end

		return Gui[panelName](parentPanel, name or panelName)
	end

	Gui._OriginalRegister = Gui._OriginalRegister or Gui.RegisterWithMetatable
	vgui.Register = function(panelName, panelTable, baseClassName)
		Gui._OriginalRegister(panelTable, panelName, baseClassName)
	end

	surface.SetDrawColor = surface.DrawSetColor
	surface.DrawRect = surface.DrawFilledRect
	surface.DrawTexturedRectUV = surface.DrawTexturedSubRect -- TODO: Not sure if this is the correct function
	surface.GetTextPos = surface.DrawGetTextPos
	surface.SetFont = surface.DrawSetTextFont
	surface.SetTextPos = surface.DrawSetTextPos
	surface.SetTextColor = surface.DrawSetTextColor
	surface.DrawText = surface.DrawPrintText
	surface.SetTexture = surface.DrawSetTexture

	surface.SetMaterial = function(material)
		local name = material:GetString("$basetexture")

		if (not textureMap[name]) then
			textureMap[name] = surface.CreateNewTextureID(true)
		end

		surface.DrawSetTextureMaterial(textureMap[name], material)
	end

	-- TODO: Implement
	surface.DrawTexturedRectRotated = function(x, y, w, h, rotation)
		-- See src/game/client/hl2/hud_zoom.cpp for an example of how to possibly implement this:
		--[[
			// draw the darkened edges, with a rotated texture in the four corners
			CMatRenderContextPtr pRenderContext( materials );
			pRenderContext->Bind( m_ZoomMaterial );
			IMesh *pMesh = pRenderContext->GetDynamicMesh( true, NULL, NULL, NULL );

			float x0 = 0.0f, x1 = fX, x2 = wide;
			float y0 = 0.0f, y1 = fY, y2 = tall;

			float uv1 = 1.0f - (1.0f / 255.0f);
			float uv2 = 0.0f + (1.0f / 255.0f);

			struct coord_t
			{
				float x, y;
				float u, v;
			};
			coord_t coords[16] =
			{
				// top-left
				{ x0, y0, uv1, uv2 },
				{ x1, y0, uv2, uv2 },
				{ x1, y1, uv2, uv1 },
				{ x0, y1, uv1, uv1 },

				// top-right
				{ x1, y0, uv2, uv2 },
				{ x2, y0, uv1, uv2 },
				{ x2, y1, uv1, uv1 },
				{ x1, y1, uv2, uv1 },

				// bottom-right
				{ x1, y1, uv2, uv1 },
				{ x2, y1, uv1, uv1 },
				{ x2, y2, uv1, uv2 },
				{ x1, y2, uv2, uv2 },

				// bottom-left
				{ x0, y1, uv1, uv1 },
				{ x1, y1, uv2, uv1 },
				{ x1, y2, uv2, uv2 },
				{ x0, y2, uv1, uv2 },
			};

			CMeshBuilder meshBuilder;
			meshBuilder.Begin( pMesh, MATERIAL_QUADS, 4 );

			for (int i = 0; i < 16; i++)
			{
				meshBuilder.Color4f( 0.0, 0.0, 0.0, alpha );
				meshBuilder.TexCoord2f( 0, coords[i].u, coords[i].v );
				meshBuilder.Position3f( coords[i].x, coords[i].y, 0.0f );
				meshBuilder.AdvanceVertex();
			}

			meshBuilder.End();
			pMesh->Draw();
		--]]
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
require("gmod_compatibility/modules/numpad")
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

-- Include("extensions/file.lua")
Include("extensions/angle.lua")
Include("extensions/debug.lua")
Include("extensions/entity.lua")
Include("extensions/ents.lua")
Include("extensions/math.lua")
Include("extensions/player.lua")
Include("extensions/player_auth.lua")
Include("extensions/string.lua")
Include("extensions/table.lua")
Include("extensions/util.lua")
Include("extensions/vector.lua")
Include("extensions/game.lua")
Include("extensions/motionsensor.lua")
Include("extensions/weapon.lua")
Include("extensions/coroutine.lua")

-- Experiment; we load our table lib again to overwrite the GMod one.
Include("../../extensions/table.lua")

if (CLIENT) then
	spawnmenu = {
		PopulateFromTextFiles = function(callback)
			local spawnlists = file.Find("settings/gmod_compatibility_content/spawnlist/*.txt", "GAME")

			for _, spawnlistFileName in ipairs(spawnlists) do
				local spawnlistKeyValues = KeyValues(spawnlistFileName)
				spawnlistKeyValues:LoadFromFile("settings/gmod_compatibility_content/spawnlist/" .. spawnlistFileName,
					"GAME")
				local spawnlist = spawnlistKeyValues:ToTable()

				callback(
					spawnlistFileName,
					spawnlist.name,
					spawnlist.contents,
					spawnlist.icon,
					spawnlist.id,
					spawnlist.parentid,
					spawnlist.needsapp
				)
			end
		end,

		DoSaveToTextFiles = function(props)
			for filename, data in pairs(props) do
				file.Write("settings/gmod_compatibility_content/spawnlist/" .. filename, data.contents)
			end
		end,
	}

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

	Include("extensions/client/entity.lua")
	Include("extensions/client/globals.lua")
	Include("extensions/client/panel.lua")
	Include("extensions/client/player.lua")
	Include("extensions/client/render.lua")

	Include("derma/init.lua")
	Include("vgui_base.lua")

	Include("skins/default.lua")
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
if (CLIENT) then
	Include("cl_hooks.lua")
end

hook.Add("Initialize", "GModCompatibility.CallInitializeHooks", function()
	hook.Run("PreGamemodeLoaded")

	hook.Run("OnGamemodeLoaded")
	hook.Run("PostGamemodeLoaded")
end)
