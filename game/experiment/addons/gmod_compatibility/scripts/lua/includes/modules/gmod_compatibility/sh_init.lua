--[[
	Note that most code here belongs to Facepunch Studios. It has been
	modified and adapted to provide compatibility with Experiment.
--]]

Include("sh_enumerations.lua")
Include("sh_file.lua")

--[[
	All our libraries are plural, while with Garry's Mod they vary between plural and singular.
--]]

bit = require("bitwise")
gamemode = require("gamemodes")
hook = require("hooks")
net = require("networks")
timer = require("timers")
weapons = require("scripted_weapons")
-- scripted_ents = require("scripted_entities") -- Gmods scripted_ents is compatible with our ScriptedEntities

--[[
	Renames and other polyfills
--]]
DeriveGamemode = InheritGamemode

include = Include

function GetConVar_Internal(name)
	local consoleVariable = ConsoleVariables.Get(name)

	if (not IsValid(consoleVariable)) then
		debug.PrintError("GetConVar: Couldn't find convar '" .. name .. "'")
		return nil
	end

	return consoleVariable
end

util = Utilities
util.PrecacheModel = _R.Entity.PrecacheModel
util.PrecacheSound = _R.Entity.PrecacheSound

-- TODO: 	Things like the player manager and drive system depend on these three functions
--			returning sensible data. Find a way to implement it:
util.AddNetworkString = function(name) end        -- Not needed for us.
util.NetworkIDToString = function() return "" end -- Not needed for us.
util.NetworkStringToID = function() return 0 end  -- Not needed for us.

util.TraceLine = Traces.TraceLine
util.TraceHull = Traces.TraceHull
util.TraceEntity = Traces.TraceEntity

util.JSONToTable = function(json)
	return Json.Decode(json)
end

util.TableToJSON = function(table)
	return Json.Encode(table)
end

util.IsValidProp = util.IsValidPhysicsProp
util.CRC = function()
	return tostring(Serializers.Crc32)
end

ents = {
	Create = Entities.CreateByName,

	GetAll = Entities.GetAll,
	GetCount = Entities.GetCount,
	GetEdictCount = Entities.GetEdictCount,
	FindAlongRay = Entities.GetAlongRay,
	FindInBox = Entities.GetInBox,
	FindInSphere = Entities.GetInSphere,

	FindInPVS = function(viewOrigin)
		if (type(viewOrigin) == "Entity") then
			viewOrigin = viewOrigin:GetPosition()
		elseif (type(viewOrigin) == "Player") then
			local viewEntity = viewOrigin:GetViewEntity()

			if (IsValid(viewEntity)) then
				viewOrigin = viewEntity:GetPosition()
			else
				viewOrigin = viewOrigin:GetPosition()
			end
		end

		return Entities.GetInPvs(viewOrigin)
	end,
}

player = Players
player.GetByID = Players.FindByUserId
player.GetBySteamID = Players.FindBySteamId
player.GetBySteamID64 = Players.FindBySteamId64
player.GetByUniqueID = Players.FindByUniqueID

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
	if (name == "Vehicle") then
		-- We don't have vehicles in Experiment, so lets not waste time on it
		return {}
	elseif (name == "IMaterial") then
		name = "Material"
	elseif (name == "ITexture") then
		name = "Texture"
	elseif (name == "CEffectData") then
		name = "EffectData"
	elseif (name == "CMoveData") then
		name = "MoveData"
	elseif (name == "CRecipientFilter") then
		name = "RecipientFilter"
	elseif (name == "CTakeDamageInfo") then
		name = "TakeDamageInfo"
	elseif (name == "CUserCmd") then
		name = "UserCommand"
	elseif (name == "bf_read") then
		name = "UserMessageReader"
	-- elseif (name == "PhysObj") then
	-- 	name = "PhysicsObject"
	-- elseif (name == "PhysCollide") then
	-- 	name = "PhysicsCollide"
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
engine = Engines
input = Inputs
render = Renders
resource = Resources
surface = Surface
system = Systems
vgui = Gui
VMatrix = Matrix
concommand = ConsoleCommands

Angle = Angles.Create
Color = Colors.Create
ClientsideModel = Entities.CreateClientEntity
Entity = Entities.Find
Vector = Vectors.Create
HSVToColor = Colors.HsvToColor
ColorToHSV = Colors.ColorToHsv
ColorToHSL = Colors.ColorToHsl
HslToColor = Colors.HslToColor

RealFrameTime = Engines.GetAbsoluteFrameTime
CurTime = Engines.GetCurrentTime
SysTime = Engines.GetSystemTime
VGUIFrameTime = Engines.GetSystemTime
RealTime = Engines.GetRealTime
FrameNumber = Engines.GetFrameCount
FrameTime = Engines.GetFrameTime
engine.TickCount = Engines.GetTickCount
engine.TickInterval = Engines.GetIntervalPerTick
SoundDuration = Engines.GetSoundDuration
GetHostName = Engines.GetServerName

PrecacheParticleSystem = ParticleSystems.Precache

RecipientFilter = RecipientFilters.Create

system.AppTime = Systems.GetSecondsSinceAppActive
system.IsOSX = Systems.IsOsx
system.SteamTime = Systems.GetSteamServerRealTime
system.UpTime = Systems.GetSecondsSinceComputerActive

local originalConCommandAdd = concommand.Add

function concommand.Add(command, callback, autoCompleteHandler, helpText, flags)
	if (istable(flags)) then
		if (#flags == 0) then
			flags = 0
		elseif (#flags == 1) then
			flags = flags[1]
		else
			flags = bit.bor(table.unpack(flags))
		end
	end

	return originalConCommandAdd(command, callback, helpText, flags)
end

engine.ActiveGamemode = function()
	return Gamemodes.GetActiveName()
end

engine.GetGames = function()
	local games = Engines.GetMountableGames()
	local result = {}

	for i, game in ipairs(games) do
		result[i] = {
			depot = game.appId,
			title = game.name,
			folder = game.directoryName,

			owned = game.isOwned,
			mounted = game.isMounted,
			installed = game.isInstalled,
		}
	end

	return result
end

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
		return Localizations.Find(phrase)
	end,

	Add = function(key, value)
		Localizations.AddString(key, value)
	end,
}

local basePath = "resource/localization/en/"
local languageFiles = file.Find(basePath .. "*.properties", "GAME")

for _, languageFile in ipairs(languageFiles) do
	local fileContent = file.Read(basePath .. languageFile, "GAME")

	if (fileContent == nil) then
		continue
	end

	for line in fileContent:gmatch("[^\r\n]+") do
		-- Skip comment lines or empty lines
		if (line:match("^%s*#") or line:match("^%s*$")) then
			continue
		end

		local key, value = line:match("([^=]+)=(.*)")
		value = value:gsub("\\(.)", "%1")

		language.Add(key, value)
	end
end

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

	return ConsoleVariables.Create(name, value, flags, helpText, min, max)
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
		return Engines.IsDedicatedServer()
	end,

	SinglePlayer = function()
		return Engines.GetMaxClients() == 1
	end,

	MaxPlayers = Engines.GetMaxClients,

	ConsoleCommand = RunConsoleCommand,

	AddParticles = function(filePath)
		-- Remove particles/ from the start of the file path
		filePath = filePath:sub(11)

		return ParticleSystems.ReadConfigFile("particles/" .. tostring(filePath))
	end,

	GetMap = Engines.GetLevelName,
}

local VECTOR_META = FindMetaTable("Vector")
VECTOR_META.AngleEx = VECTOR_META.AngleWithUp
VECTOR_META.Distance = VECTOR_META.DistanceTo
VECTOR_META.DistToSqr = VECTOR_META.DistanceToAsSqr
VECTOR_META.Distance2D = VECTOR_META.DistanceTo2D
VECTOR_META.Distance2DSqr = VECTOR_META.DistanceToAsSqr2D
VECTOR_META.Div = VECTOR_META.Divide
VECTOR_META.Mul = VECTOR_META.Scale

local ANGLE_META = FindMetaTable("Angle")
ANGLE_META.Div = ANGLE_META.Divide
ANGLE_META.Mul = ANGLE_META.Scale

local ENTITY_META = FindMetaTable("Entity")
ENTITY_META.Health = ENTITY_META.GetHealth
ENTITY_META.GetPos = ENTITY_META.GetPosition
ENTITY_META.SetPos = ENTITY_META.SetPosition
ENTITY_META.EyePos = ENTITY_META.GetEyePosition
ENTITY_META.EyeAngles = ENTITY_META.GetEyeAngles
ENTITY_META.GetModel = ENTITY_META.GetModelName
ENTITY_META.GetTable = ENTITY_META.GetRefTable
ENTITY_META.NearestPoint = ENTITY_META.CalculateNearestPoint
ENTITY_META.OBBCenter = ENTITY_META.GetOBBCenter
ENTITY_META.OBBMaxs = ENTITY_META.GetOBBMaxs
ENTITY_META.OBBMins = ENTITY_META.GetOBBMins
ENTITY_META.LocalToWorld = ENTITY_META.EntityToWorldSpace
ENTITY_META.SkinCount = ENTITY_META.GetSkinCount
ENTITY_META.Alive = ENTITY_META.IsAlive
ENTITY_META.Widget = false
ENTITY_META.AddFlags = ENTITY_META.AddFlag
ENTITY_META.RemoveFlags = ENTITY_META.RemoveFlag
ENTITY_META.GetFlexNum = ENTITY_META.GetFlexCount
ENTITY_META.SetOwner = ENTITY_META.SetOwnerEntity
ENTITY_META.GetOwner = ENTITY_META.GetOwnerEntity
ENTITY_META.DeleteOnRemove = ENTITY_META.AddDeleteOnRemove
ENTITY_META.DontDeleteOnRemove = ENTITY_META.RemoveDeleteOnRemove
ENTITY_META.GetFlexIDByName = ENTITY_META.GetFlexIdByName
ENTITY_META.GetNumBodyGroups = ENTITY_META.GetBodyGroupsCount

function ENTITY_META:SetSpawnEffect(effect)
	-- TODO: Implement
	self.__spawnEffect = effect
end

function ENTITY_META:GetSpawnEffect()
	return self.__spawnEffect
end

function ENTITY_META:SetNoDraw(bBool)
	if (bBool) then
		self:AddEffects(_E.ENTITY_EFFECT.NO_DRAW)
	else
		self:RemoveEffects(_E.ENTITY_EFFECT.NO_DRAW)
	end
end

function ENTITY_META:IsFlagSet(flag)
	return self:GetFlags() & flag ~= 0
end

function ENTITY_META:IsOnGround()
	return self:IsFlagSet(_E.ENGINE_FLAG.ON_GROUND)
end

--[[
	We implement SetNotSolid and DrawShadow using engine flags/effects, but I'm not sure
	if that's the way gmod does it. If it isn't, these flags may be unexpectedly set by
	some scripts.
	TODO: Check if DrawShadow/SetNotSolid on gmod, causes any flags to be set.
--]]
function ENTITY_META:SetNotSolid(bBool)
	if (bBool) then
		self:AddSolidFlags(SOLID_NONE)
	else
		print("ENTITY_META:SetNotSolid - Not sure if SOLID_VPHYSICS is correct for SetNotSolid(false)")
		self:RemoveSolidFlags(SOLID_VPHYSICS)
	end
end

function ENTITY_META:DrawShadow(bBool)
	if (bBool) then
		self:RemoveEffects(_E.ENTITY_EFFECT.NO_SHADOW)
	else
		self:AddEffects(_E.ENTITY_EFFECT.NO_SHADOW)
	end
end

ENTITY_META.OnGround = ENTITY_META.IsOnGround
ENTITY_META.WaterLevel = ENTITY_META.GetWaterLevel

local PLAYER_META = FindMetaTable("Player")
PLAYER_META.GetShootPos = ENTITY_META.GetEyePosition
PLAYER_META.UserID = PLAYER_META.GetUserId
PLAYER_META.AccountID = PLAYER_META.GetAccountId
PLAYER_META.SteamID = PLAYER_META.GetSteamId
PLAYER_META.SteamID64 = PLAYER_META.GetSteamId64
PLAYER_META.UniqueID = PLAYER_META.GetUniqueId
PLAYER_META.InVehicle = PLAYER_META.IsInVehicle
PLAYER_META.GetVehicle = PLAYER_META.GetVehicleEntity
PLAYER_META.AnimResetGestureSlot = PLAYER_META.AnimationResetGestureSlot
PLAYER_META.AnimRestartGesture = PLAYER_META.AnimationRestartGesture
PLAYER_META.AnimRestartMainSequence = PLAYER_META.AnimationRestartMainSequence
PLAYER_META.AnimSetGestureSequence = PLAYER_META.AnimationSetGestureSequence
PLAYER_META.AnimSetGestureWeight = PLAYER_META.AnimationSetGestureWeight
PLAYER_META.GetName = PLAYER_META.GetPlayerName
PLAYER_META.Name = PLAYER_META.GetPlayerName
PLAYER_META.Nick = PLAYER_META.GetPlayerName

function PLAYER_META:GetInfo(consoleVariableName)
	return engine.GetClientConsoleVariableValue(self, consoleVariableName)
end

function PLAYER_META:GetInfoNum(consoleVariableName, default)
	return engine.GetClientConsoleVariableValueAsNumber(self, consoleVariableName) or default
end

function PLAYER_META:IsDrivingEntity()
	return false -- TODO: implement this
end

function PLAYER_META:IsPlayingTaunt()
	return false -- TODO: implement this
end

function PLAYER_META:IsTyping()
	return false -- TODO: implement this
end

-- TODO: Implement these correctly (I'm not sure what they do atm)
function PLAYER_META:SetHoveredWidget(widget)
	self.__hoveredWidget = widget
end

function PLAYER_META:GetHoveredWidget()
	return self.__hoveredWidget
end

function PLAYER_META:SetPressedWidget(widget)
	self.__pressedWidget = widget
end

function PLAYER_META:GetPressedWidget()
	return self.__pressedWidget
end

if (SERVER) then
	function PLAYER_META:SendLua(lua)
		net.Start("__PlayerLuaRun")
		net.WriteString(lua)
		net.Send(self)
	end

	PLAYER_META.Spectate = PLAYER_META.SetObserverMode
	PLAYER_META.SpectateEntity = PLAYER_META.SetObserverTarget

	function PLAYER_META:UnSpectate()
		self:SetObserverMode(OBS_MODE_NONE)
	end
else
	net.Receive("__PlayerLuaRun", function()
		local lua = net.ReadString()
		RunString(lua)
	end)
end

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

	return self == Engines.GetListenServerHost()
end

local MOVE_DATA_META = FindMetaTable("MoveData")
MOVE_DATA_META.KeyDown = MOVE_DATA_META.IsKeyDown
MOVE_DATA_META.KeyPressed = MOVE_DATA_META.WasKeyPressed
MOVE_DATA_META.KeyReleased = MOVE_DATA_META.WasKeyReleased
MOVE_DATA_META.KeyWasDown = MOVE_DATA_META.WasKeyDown
MOVE_DATA_META.GetAbsMoveAngles = MOVE_DATA_META.GetAbsoluteMoveAngles
MOVE_DATA_META.SetAbsMoveAngles = MOVE_DATA_META.SetAbsoluteMoveAngles

local TEXTURE_META = FindMetaTable("Texture")

function TEXTURE_META:Width()
	return self:GetActualWidth()
end

function TEXTURE_META:Height()
	return self:GetActualHeight()
end

local MATERIAL_META = FindMetaTable("Material")
MATERIAL_META.GetShader = MATERIAL_META.GetShaderName
MATERIAL_META.IsError = MATERIAL_META.IsErrorMaterial
MATERIAL_META.Recompute = MATERIAL_META.RecomputeStateSnapshots

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
			name = "" .. name
		end

		return Surface.FindMaterial(name)
	end

	-- Returns whether the currently focused panel is a child of the given one.
	function vgui.FocusedHasParent(panel)
		local focusedPanel = Inputs.GetFocus()

		if (not IsValid(focusedPanel)) then
			return false
		end

		while (IsValid(focusedPanel)) do
			if (focusedPanel == panel) then
				return true
			end

			focusedPanel = focusedPanel:GetParent()
		end

		return false
	end

	vgui.GetKeyboardFocus = Inputs.GetFocus
	vgui.GetHoveredPanel = Inputs.GetMouseOver
	vgui.CursorVisible = Surface.IsCursorVisible

	gui = {
		MouseX = function()
			local x, y = Inputs.GetCursorPosition()
			return x
		end,
		MouseY = function()
			local x, y = input.GetCursorPosition()
			return y
		end,
		SetMousePos = input.SetCursorPosition,
		ScreenToVector = input.ScreenToWorld,
		AimToVector = input.AimToVector,
	}

	cam = {
		Start3D = render.PushView3D,
		Start2D = render.PushView2D,
		End3D = render.PopView3D,
		End2D = render.PopView2D,
	}

	render.SetModelLighting = render.SetAmbientLightCube
	render.ResetModelLighting = render.ResetAmbientLightCube
	render.PushFilterMin = render.PushFilterMinification
	render.PopFilterMin = render.PopFilterMinification
	render.PushFilterMag = render.PushFilterMagnification
	render.PopFilterMag = render.PopFilterMagnification

	function render.Clear(r, g, b, a, clearDepth, clearStencil)
		render.ClearBuffers(true, clearDepth or false, clearStencil or false)
		render.ClearColor(r, g, b, a)
	end

	function render.ClearDepth(clearStencil)
		render.ClearBuffers(false, true, clearStencil or false)
	end

	function render.ClearStencil()
		render.ClearBuffers(false, false, true)
	end

	input.SetCursorPos = input.SetCursorPosition
	input.GetCursorPos = input.GetCursorPosition
	input.IsButtonDown = input.IsKeyDown
	input.StartKeyTrapping = engine.StartKeyTrapMode
	input.IsKeyTrapping = engine.IsKeyTrapping
	input.CheckKeyTrapping = engine.CheckKeyTrapping

	function input.IsShiftDown()
		return input.IsKeyDown(KEY_LSHIFT) or input.IsKeyDown(KEY_RSHIFT)
	end

	function input.IsControlDown()
		return input.IsKeyDown(KEY_LCONTROL) or input.IsKeyDown(KEY_RCONTROL)
	end

	CreateMaterial = Surface.CreateMaterial
	DisableClipping = Surface.DisableClipping

	local MODEL_IMAGE_PANEL_META = FindMetaTable("ModelImagePanel")
	MODEL_IMAGE_PANEL_META._OriginalRebuildSpawnIcon = MODEL_IMAGE_PANEL_META._OriginalRebuildSpawnIcon or
		MODEL_IMAGE_PANEL_META.RebuildSpawnIcon
	registry.ModelImage = MODEL_IMAGE_PANEL_META

	MODEL_IMAGE_PANEL_META.SetSpawnIcon = MODEL_IMAGE_PANEL_META.SetModelImage

	function MODEL_IMAGE_PANEL_META:RebuildSpawnIcon()
		local ent = ClientsideModel(self:GetModel(), RENDERGROUP_OTHER)
		local result = PositionSpawnIcon(ent, vector_origin)
		ent:Remove()

		self:_OriginalRebuildSpawnIcon({
			origin = result.origin,
			angles = result.angles,
			fieldOfView = result.fov,
			zNear = result.znear,
			zFar = result.zfar,
		})
	end

	function MODEL_IMAGE_PANEL_META:RebuildSpawnIconEx(tab)
		self:_OriginalRebuildSpawnIcon({
			origin = tab.cam_pos or tab.origin or Vector(0, 0, 0),
			angles = tab.cam_ang or tab.angles or Angle(0, 0, 0),
			fieldOfView = tab.cam_fov or tab.fieldOfView or 90,
			zNear = tab.nearz or tab.zNear or 1,
			zFar = tab.farz or tab.zFar or 1000,
		})
	end

	local PANEL_META = FindMetaTable("Panel")
	PANEL_META.GetPos = PANEL_META.GetPosition
	PANEL_META.SetPos = PANEL_META.SetPosition
	PANEL_META._OriginalSetCursor = PANEL_META._OriginalSetCursor or PANEL_META.SetCursor
	PANEL_META._OriginalGetParent = PANEL_META._OriginalGetParent or PANEL_META.GetParent

	PANEL_META.CursorPos = PANEL_META.GetLocalCursorPosition
	PANEL_META.Remove = PANEL_META.MarkForDeletion
	PANEL_META.GetTable = PANEL_META.GetRefTable
	PANEL_META.Dock = PANEL_META.SetDock
	PANEL_META.DockMargin = PANEL_META.SetDockMargin
	PANEL_META.DockPadding = PANEL_META.SetDockPadding
	PANEL_META.ChildCount = PANEL_META.GetChildCount
	PANEL_META.ChildrenSize = PANEL_META.GetChildrenSize
	PANEL_META.NoClipping = PANEL_META.SetPaintClippingEnabled

	function PANEL_META:SizeToContents(sizeWidth, sizeHeight)
		-- For some reason DTree_Node uses SizeToContents on a DListLayout, which doesn't have a SizeToContents function.
		-- :/
		self:SizeToChildren(sizeWidth, sizeHeight)
	end

	function PANEL_META:DrawFilledRect()
		local width, height = self:GetSize()
		surface.DrawRect(0, 0, width, height)
	end

	function PANEL_META:DrawOutlinedRect()
		local width, height = self:GetSize()
		surface.DrawOutlinedRect(0, 0, width, height)
	end

	function PANEL_META:DrawTexturedRect()
		local width, height = self:GetSize()
		surface.DrawTexturedRect(0, 0, width, height)
	end

	function PANEL_META:MouseCapture(doCapture)
		if (doCapture) then
			input.SetMouseCapture(self)
		else
			input.SetMouseCapture(nil)
		end
	end

	function PANEL_META:GetParent()
		local parent = self:_OriginalGetParent()

		if (not IsValid(parent)) then
			return nil
		end

		return parent
	end

	-- Change casing and add functionality to pass individual color components.
	function PANEL_META:SetBGColor(rOrColor, g, b, a)
		if (istable(rOrColor)) then
			self:SetBgColor(rOrColor)
		else
			self:SetBgColor(Colors.Create(rOrColor, g, b, a))
		end
	end

	-- Change casing and add functionality to pass individual color components.
	function PANEL_META:SetFGColor(rOrColor, g, b, a)
		if (istable(rOrColor)) then
			self:SetFgColor(rOrColor)
		else
			self:SetFgColor(Colors.Create(rOrColor, g, b, a))
		end
	end

	local LABEL_PANEL_META = FindMetaTable("Label")
	LABEL_PANEL_META._OriginalSetFont = LABEL_PANEL_META._OriginalSetFont or LABEL_PANEL_META.SetFont
	LABEL_PANEL_META._OriginalGetFont = LABEL_PANEL_META._OriginalGetFont or LABEL_PANEL_META.GetFont
	LABEL_PANEL_META._OriginalSetContentAlignment = LABEL_PANEL_META._OriginalSetContentAlignment or
		LABEL_PANEL_META.SetContentAlignment

	LABEL_PANEL_META.GetTextSize = LABEL_PANEL_META.GetContentSize

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

	--- Set content alignment based on numpad keys
	local NUMPAD_TO_ALIGNMENT_MAP = {
		[7] = 0,
		[8] = 1,
		[9] = 2,

		[4] = 3,
		[5] = 4,
		[6] = 5,

		[1] = 6,
		[2] = 7,
		[3] = 8,
	}
	function LABEL_PANEL_META:SetContentAlignment(numpadAlignment)
		local alignment = NUMPAD_TO_ALIGNMENT_MAP[numpadAlignment]

		if (not alignment) then
			error("attempt to set invalid content alignment \"" .. tostring(numpadAlignment) .. "\"", 2)
		end

		self:_OriginalSetContentAlignment(alignment)
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

	local CHECK_BUTTON_PANEL_META = FindMetaTable("CheckButton")
	CHECK_BUTTON_PANEL_META.GetValue = CHECK_BUTTON_PANEL_META.IsSelected

	function PANEL_META:GetValue()
		local className = self:GetClassName()

		if (className == "LCheckButton") then
			return CHECK_BUTTON_PANEL_META.GetValue(self)
		elseif (className == "LTextEntry") then
			return TEXT_ENTRY_PANEL_META.GetValue(self)
		elseif (className == "LLabel") then
			return LABEL_PANEL_META.GetValue(self)
		end

		error("attempt to get value of unsupported panel class \"" .. tostring(className) .. "\"", 2)
	end

	function PANEL_META:GetContentSize()
		local className = self:GetClassName()

		if (className == "LLabel") then
			return LABEL_PANEL_META.GetContentSize(self)
		end

		error("attempt to get content size of unsupported panel class \"" .. tostring(className) .. "\"", 2)
	end

	local EDITABLEPANEL_PANEL_META = FindMetaTable("EditablePanel")
	function PANEL_META:IsModal()
		local className = self:GetClassName()

		if (className == "LEditablePanel") then
			return EDITABLEPANEL_PANEL_META.IsModal(self)
		end
	end

	-- TODO: Does this need serious implementing?
	function PANEL_META:SetDrawOnTop(isOnTop)
		print("PANEL_META:SetDrawOnTop: Reminder to implement this function (or not).")
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

	ScrW = Utilities.GetScreenWidth
	ScrH = Utilities.GetScreenHeight

	Utilities.IsSkyboxVisibleFromPoint = Engines.IsSkyboxVisibleFromPoint

	GetRenderTargetEx = render.CreateRenderTargetTextureEx
	GetRenderTarget = render.CreateRenderTargetTextureEx
	EyePos = render.MainViewOrigin
	EyeAngles = render.MainViewAngles
	EyeVector = render.MainViewForward

	LocalPlayer = Players.GetLocalPlayer

	surface.SetDrawColor = surface.DrawSetColor
	surface.DrawRect = surface.DrawFilledRect

	local textureMap = {}

	surface.GetTextureID = function(name)
		if (not file.Exists("materials/" .. name .. ".vmt", "GAME") and not file.Exists("materials/" .. name, "GAME")) then
			name = "" .. name
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

	--[[
		We disable our own Gui.Create and Gui.Register logic, in favor of the GMod one.
		This Gui.Create will be used as vgui.CreateX by gmod to create internal panels.
	--]]
	function Gui.Create(panelClassName, parentPanel, name)
		parentPanel = parentPanel or getAppropriateBaseParent()

		if (panelClassName == "ModelImage") then
			panelClassName = "ModelImagePanel"
		end

		if (not Gui[panelClassName]) then
			error("attempt to create non-existing panel class \"" .. tostring(panelClassName) .. "\"", 2)
		end

		return Gui[panelClassName](parentPanel, name or panelClassName)
	end

	surface.SetDrawColor = surface.DrawSetColor
	surface.DrawRect = surface.DrawFilledRect
	surface.DrawTexturedRectUV = surface.DrawTexturedSubRect
	surface.GetTextPos = surface.DrawGetTextPos
	surface.SetTextPos = surface.DrawSetTextPos
	surface.SetTextColor = surface.DrawSetTextColor
	surface.DrawText = surface.DrawPrintText
	surface.SetTexture = surface.DrawSetTexture

	local currentFont
	surface.SetFont = function(font)
		currentFont = font
		surface.DrawSetTextFont(font)
	end

	local oldTextSize = Surface.GetTextSize

	surface.GetTextSize = function(text)
		return oldTextSize(currentFont, text)
	end

	surface.SetMaterial = function(material)
		local name = material:GetString("$basetexture")

		if (not textureMap[name]) then
			textureMap[name] = Surface.CreateNewTextureID(true)
			Surface.DrawSetTextureMaterial(textureMap[name], material)
		end

		Surface.DrawSetTexture(textureMap[name])
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

function baseclassGetCompatibility(name)
	if (name:sub(1, 9) == "gamemode_") then
		name = name:sub(10)

		return Gamemodes.Get(name)
	end

	return baseclass.Get(name)
end

if (CLIENT) then
	matproxy = {
		Add = function(name, data) end -- TODO: Implement
	}

	spawnmenu = {
		PopulateFromTextFiles = function(callback)
			local spawnlists = file.Find("settings/spawnlist/*.txt", "GAME")

			for _, spawnlistFileName in ipairs(spawnlists) do
				local spawnlistKeyValues = KeyValues.Create(spawnlistFileName)
				spawnlistKeyValues:LoadFromFile("settings/spawnlist/" .. spawnlistFileName,
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
				file.Write("settings/spawnlist/" .. filename, data.contents)
			end
		end,
	}

	function LoadPresets()
		local loadedPresets = {}
		local _, directories = file.Find("settings/presets/*", "GAME")

		for _, directory in ipairs(directories) do
			local presetFiles = file.Find("settings/presets/" .. directory .. "/*.txt", "GAME")

			loadedPresets[directory] = {}

			for _, presetFileName in ipairs(presetFiles) do
				local presetKeyValues = KeyValues.Create(presetFileName)
				presetKeyValues:LoadFromFile(
					"settings/presets/" .. directory .. "/" .. presetFileName,
					"GAME")
				local preset = presetKeyValues:ToTable()

				loadedPresets[directory][presetKeyValues:GetName()] = preset
			end
		end

		return loadedPresets
	end
end

--[[
	Other shared functions.
--]]
unpack = unpack or table.unpack

MsgC = function(...)
	local currentColor = debug.GetRealmColor()

	for k, stringOrColor in ipairs({ ... }) do
		if (IsColor(stringOrColor)) then
			currentColor = stringOrColor
		else
			debug.PrintDebugColorMessage(currentColor, tostring(stringOrColor))
		end
	end

	debug.PrintDebugColorMessage(currentColor, "\n")
end

PrintTable = table.Print

ErrorNoHalt = function(...)
	Msg(...)
end

ErrorNoHaltWithStack = function(...)
	Msg(debug.traceback(table.concat({ ... }, " "), 2))
end

--[[
	sbox_ convars that aren't normally defined in Lua
	TODO: Read these from the gamemodes/gmname/gmname.txt keyvalues file
--]]
local sbox_godmode = CreateConVar("sbox_godmode", "0",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"If enabled, all players will be invincible")
local sbox_maxballoons = CreateConVar("sbox_maxballoons", "100",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum balloons a single player can create")
local sbox_maxbuttons = CreateConVar("sbox_maxbuttons", "50",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum buttons a single player can create")
local sbox_maxcameras = CreateConVar("sbox_maxcameras", "10",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum cameras a single player can create")
local sbox_maxdynamite = CreateConVar("sbox_maxdynamite", "10",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum dynamites a single player can create")
local sbox_maxeffects = CreateConVar("sbox_maxeffects", "200",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum effect props a single player can create")
local sbox_maxemitters = CreateConVar("sbox_maxemitters", "20",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum emitters a single player can create")
local sbox_maxhoverballs = CreateConVar("sbox_maxhoverballs", "50",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum hoverballs a single player can create")
local sbox_maxlamps = CreateConVar("sbox_maxlamps", "3",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum lamps a single player can create")
local sbox_maxlights = CreateConVar("sbox_maxlights", "5",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum lights a single player can create")
local sbox_maxnpcs = CreateConVar("sbox_maxnpcs", "10",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum NPCs a single player can create")
local sbox_maxprops = CreateConVar("sbox_maxprops", "200",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum props a single player can create")
local sbox_maxragdolls = CreateConVar("sbox_maxragdolls", "10",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum ragdolls a single player can create")
local sbox_maxsents = CreateConVar("sbox_maxsents", "100",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum entities a single player can create")
local sbox_maxthrusters = CreateConVar("sbox_maxthrusters", "50",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum thrusters a single player can create")
local sbox_maxvehicles = CreateConVar("sbox_maxvehicles", "4",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum vehicles a single player can create")
local sbox_maxwheels = CreateConVar("sbox_maxwheels", "50",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Maximum wheels a single player can create")
local sbox_noclip = CreateConVar("sbox_noclip", "1",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"If enabled, players will be able to use noclip")
local sbox_persist = CreateConVar("sbox_persist", "1",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"If not empty, enables 'Make Persistent' option when you right click on props while holding C, allowing you to save them across")
local sbox_playershurtplayers = CreateConVar("sbox_playershurtplayers", "1",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"If enabled, players will be able to hurt each other")
local sbox_weapons = CreateConVar("sbox_weapons", "1",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"If enabled, each player will receive default Half-Life 2 weapons on each spawn")

--[[
	Now that the compatibility libraries have been loaded, we can start changing hooks
--]]
if (CLIENT) then
	Include("cl_hooks.lua")

	hook.Add("LevelInitPostEntity", "GModCompatibility.CallInitPostEntityHooks", function()
		hook.Run("InitPostEntity")
	end)
else
	-- Include("sv_hooks.lua")

	hook.Add("ServerActivate", "GModCompatibility.CallInitPostEntityHooks", function()
		hook.Run("InitPostEntity")
	end)
end

hook.Add("Initialize", "GModCompatibility.CallInitializeHooks", function()
	-- Copy ents from our system to the GMod system.
	local scriptedEntities = ScriptedEntities.GetList()
	for className, scriptedEntity in pairs(scriptedEntities) do
		scripted_ents.Register(scriptedEntity, className)
	end
	scripted_ents.OnLoaded()

	hook.Run("CreateTeams")
	hook.Run("PreGamemodeLoaded")
	hook.Run("OnGamemodeLoaded")
	hook.Run("PostGamemodeLoaded")
end)

-- Setup the Garry's Mod lua include path so Include can find scripts
-- No need to prefix search paths here, since Include already finds scripts using
-- the configured search paths.
package.IncludePath = "lua/;" .. package.IncludePath

-- Setup to search for modules in all search paths
local searchPathsString = Files.GetSearchPath("GAME")

for searchPath in searchPathsString:gmatch("([^;]+)") do
	if (searchPath:find("%.bsp$") or searchPath:find("%.vpk$")) then
		continue
	end

	if (searchPath:sub(-1) ~= "/" and searchPath:sub(-1) ~= "\\") then
		searchPath = searchPath .. "/"
	end

	-- Prepend the searchpath to the lua dirs
	package.path = searchPath .. "lua/includes/modules/?.lua;" .. package.path

	if (Systems.IsWindows()) then
		package.cpath = searchPath .. "lua/bin/?.dll;" .. package.cpath
	elseif (Systems.IsLinux()) then
		package.cpath = searchPath .. "lua/bin/?.so;" .. package.cpath
	end
end

-- Now that the entire compatibility setup is done, we can execute the scripts in 'autorun' (shared), 'autorun/client' and 'autorun/server'.
local function includeFolder(folder)
	local files = file.Find(folder .. "/*.lua", "GAME")

	for _, fileName in ipairs(files) do
		include(folder .. "/" .. fileName)
	end
end

-- We implement this ourselves, because the gmod util/color.lua messes up the metatable
include("sh_color.lua")

--[[
	Load the Garry's Mod init scripts and panels
--]]

-- Let's setup a temporary filter so we don't load the extensions and modules we implemented ourselves.
local filter = {
	require = {
		["concommand"] = true,
		["hook"] = true,
		["gamemode"] = true,
		["weapons"] = true,
		["usermessage"] = true,
	},
	include = {
		["extensions/net.lua"] = true,  -- We implement networking using luasocket
		["extensions/file.lua"] = true, -- Our filesystem works slightly different
		["util/color.lua"] = true,      -- In contrast with gmod, we should properly get metatables everywhere (so don't need this hack util)
	},
}

local originalRequire = require
local originalInclude = Include

require = function(name)
	if (not filter.require[name]) then
		return originalRequire(name)
	end
end

include = function(name)
	if (not filter.include[name]) then
		return originalInclude(name)
	end
end

include("includes/init.lua")

include = originalInclude
require = originalRequire

if (CLIENT) then
	--[[
		Load VGUI
	--]]

	include("derma/init.lua")

	-- include("includes/vgui_base.lua") -- I don't think this is actually loaded in Garry's Mod since it's missing a bunch of includes
	-- So let's just include all files inside lua/vgui/
	includeFolder("lua/vgui")

	include("cl_awesomium.lua")

	include("skins/default.lua")

	require("notification")
end

--[[
	Load the autorun scripts.
--]]
includeFolder("lua/autorun")

if (CLIENT) then
	includeFolder("lua/autorun/client")
else
	includeFolder("lua/autorun/server")
end
