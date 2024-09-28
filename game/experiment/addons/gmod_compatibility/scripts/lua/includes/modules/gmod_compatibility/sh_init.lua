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
timer = require("timers")
net = Networks

-- weapons = require("scripted_weapons")
-- scripted_ents = require("scripted_entities") -- Gmods scripted_ents is compatible with our ScriptedEntities

--[[
	Renames and other polyfills
--]]
DeriveGamemode = InheritGamemode

include = Include
Msg = PrintMessage
MsgN = PrintMessageLine

OriginalPrintMessage = PrintMessage
PrintMessage = Players.ClientPrintToAll

function GetConVar_Internal(name)
	local consoleVariable = ConsoleVariables.Get(name)

	if (not IsValid(consoleVariable)) then
		-- debug.PrintError("GetConVar: Couldn't find convar '" .. name .. "'")
		return nil
	end

	return consoleVariable
end

-- Before Lua 5.3 math.random(1, #emptyTable) would return 1, but it errors now
local originalMathRandom = math.random

function math.random(min, max)
	if (not min and not max) then
		return originalMathRandom()
	end

	if (not max) then
		max = min
		min = 1
	end

	if (max < min) then
		return originalMathRandom(max, min)
	end

	return originalMathRandom(min, max)
end

function math.pow(base, exponent)
	return base ^ exponent
end

-- Since Lua 5.3 non-integers give an error
local originalRandomSeed = math.randomseed

function math.randomseed(seed)
	originalRandomSeed(math.floor(seed))
end

util = Utilities
util.PrecacheModel = _R.Entity.PrecacheModel
util.PrecacheSound = _R.Entity.PrecacheSound

util.AddNetworkString = Networks.AddNetworkString
util.NetworkIDToString = Networks.NetworkIdToString
util.NetworkStringToID = Networks.NetworkStringToId

util.TraceLine = Traces.TraceLine
util.TraceHull = Traces.TraceHull
util.TraceEntity = Traces.TraceEntity
util.PointContents = Traces.PointContents

util.Compress = Serializers.LzmaCompress
util.Decompress = Serializers.LzmaDecompress

util.JSONToTable = function(json)
	-- wraped in a pcall, because gmod returns nothing on failure (without error)
	local success, result = pcall(Json.Decode, json)

	if (not success) then
		return
	end

	return result
end

util.TableToJSON = function(table)
	return Json.Encode(table)
end

util.IsValidProp = util.IsValidPhysicsProp
util.CRC = function(unencoded)
	return tostring(Serializers.Crc32(unencoded))
end

util.SharedRandom = Randoms.SharedRandomFloat

ents = {
	Create = Entities.CreateByName,

	GetAll = Entities.GetAll,
	GetCount = Entities.GetCount,
	GetEdictCount = Entities.GetEdictCount,
	FindAlongRay = Entities.GetAlongRay,
	FindInBox = Entities.GetInBox,
	FindInSphere = Entities.GetInSphere,
	FindByClass = Entities.GetByClass,

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
player.GetByID = Players.FindByIndex
player.GetBySteamID = Players.FindBySteamId
player.GetBySteamID64 = Players.FindBySteamId64
player.GetByUniqueID = Players.FindByUniqueID
player.GetHumans = Players.GetAllHumans
player.GetBots = Players.GetAllBots

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
    elseif (name == "IGModAudioChannel") then
		name = "AudioChannel"
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
	elseif (name == "VMatrix") then
		name = "Matrix"
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
surface = Surfaces
system = Systems
vgui = Panels
concommand = ConsoleCommands
physenv = PhysicsEnvironments
gameevent = GameEvents
sound = Sounds
debugoverlay = DebugOverlays

Angle = Angles.Create
Color = Colors.Create
ClientsideModel = Entities.CreateClientEntity
Entity = Entities.Find
Vector = Vectors.Create
Matrix = Matrixes.Create
HSVToColor = Colors.HsvToColor
ColorToHSV = Colors.ColorToHsv
ColorToHSL = Colors.ColorToHsl
HslToColor = Colors.HslToColor

SetClipboardText = Systems.SetClipboardText

RealFrameTime = Engines.GetAbsoluteFrameTime
CurTime = Engines.GetCurrentTime
UnPredictedCurTime = Engines.GetCurrentTime -- TODO: Use actually un-predicted time
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
GetPredictionPlayer = Predictions.GetPredictionPlayer
IsFirstTimePredicted = Predictions.IsFirstTimePredicted
LerpVector = Vectors.Lerp

RecipientFilter = RecipientFilters.Create
EffectData = Effects.Create
util.Effect = Effects.Dispatch

system.AppTime = Systems.GetSecondsSinceAppActive
system.IsOSX = Systems.IsOsx
system.SteamTime = Systems.GetSteamServerRealTime
system.UpTime = Systems.GetSecondsSinceComputerActive

local function convertPlaySoundFlags(flagsAsString)
    local flags = _E.PLAY_SOUND_FLAG.STREAM_BLOCK

	if (not flagsAsString or type(flagsAsString) ~= "string") then
		return flags
	end

	for flag in flagsAsString:gmatch("%S+") do
		if (flag == "3d") then
			flags = bit.bor(flags, _E.PLAY_SOUND_FLAG.SAMPLE_3D)
		elseif (flag == "mono") then
			flags = bit.bor(flags, _E.PLAY_SOUND_FLAG.SAMPLE_MONO)
		elseif (flag == "noplay") then
			flags = bit.bor(flags, _E.PLAY_SOUND_FLAG.DONT_PLAY)
		elseif (flag == "noblock") then
            -- Remove the STREAM_BLOCK flag
			flags = bit.band(flags, bit.bnot(_E.PLAY_SOUND_FLAG.STREAM_BLOCK))
		end
	end
end

sound.PlayURL = function(path, flagsAsString, callback)
    return Sounds.PlayUrl(path, convertPlaySoundFlags(flagsAsString), callback)
end
local oldPlayFile = Sounds.PlayFile
sound.PlayFile = function(path, flagsAsString, callback)
	return oldPlayFile(path, convertPlaySoundFlags(flagsAsString), callback)
end

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

function MsgAll(...)
	local message = table.concat({ ... }, " ")
	Players.ShowMessageToAll(message)
end

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

function physenv.AddSurfaceData(surfaceDataKeyValuesString)
	-- ParseSurfaceData expects a unique file name, so we generate one
	local fakeFileName = "physenv.AddSurfaceData" .. tostring(os.time()) .. ".txt"

	return PhysicsSurfaceProperties.ParseSurfaceData(fakeFileName, surfaceDataKeyValuesString)
end

local MESSAGE_READER_META = FindMetaTable("MessageReader")

function MESSAGE_READER_META:ReadInt(bitCount)
	return self:ReadBitLong(bitCount, true)
end

function MESSAGE_READER_META:ReadUInt(bitCount)
	return self:ReadBitLong(bitCount, false)
end

local MESSAGE_WRITER_META = FindMetaTable("MessageWriter")

function MESSAGE_WRITER_META:WriteInt(value, bitCount)
	return self:WriteBitLong(value, bitCount, true)
end

function MESSAGE_WRITER_META:WriteUInt(value, bitCount)
	return self:WriteBitLong(value, bitCount, false)
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

debugoverlay.Axis = function(origin, angle, size, lifetime, ignoreZ)
	DebugOverlays.AddLine(origin, origin + angle:Forward() * size, lifetime, Color(0, 255, 0), ignoreZ)
	DebugOverlays.AddLine(origin, origin + angle:Right() * size, lifetime, Color(255, 0, 0), ignoreZ)
	DebugOverlays.AddLine(origin, origin + angle:Up() * size, lifetime, Color(0, 0, 255), ignoreZ)
end
debugoverlay.Box = function(origin, mins, maxs, lifetime, color)
	DebugOverlays.AddBox(origin, mins, maxs, Angle(), color, lifetime)
end
debugoverlay.BoxAngles = DebugOverlays.AddBox
debugoverlay.Cross = function(origin, size, lifetime, color, ignoreZ)
	DebugOverlays.AddLine(origin + Vector(size, 0, 0), origin - Vector(size, 0, 0), lifetime, color, ignoreZ)
	DebugOverlays.AddLine(origin + Vector(0, size, 0), origin - Vector(0, size, 0), lifetime, color, ignoreZ)
	DebugOverlays.AddLine(origin + Vector(0, 0, size), origin - Vector(0, 0, size), lifetime, color, ignoreZ)
end
debugoverlay.EntityTextAtPosition = DebugOverlays.AddEntityTextOverlay
debugoverlay.Grid = DebugOverlays.AddGrid
debugoverlay.Line = DebugOverlays.AddLine
debugoverlay.ScreenText = DebugOverlays.AddScreenText
-- debugoverlay.Sphere = DebugOverlays.AddSphere -- not yet implemented
debugoverlay.SweptBox = DebugOverlays.AddSweptBox
debugoverlay.Text = DebugOverlays.AddText
debugoverlay.Triangle = DebugOverlays.AddTriangle

local MATRIX_META = FindMetaTable("Matrix")
MATRIX_META.Translate = MATRIX_META.PostTranslate

local EFFECT_DATA_META = FindMetaTable("EffectData")
EFFECT_DATA_META.GetEntIndex = EFFECT_DATA_META.GetEntityIndex
EFFECT_DATA_META.SetEntIndex = EFFECT_DATA_META.SetEntityIndex
EFFECT_DATA_META.GetAttachment = EFFECT_DATA_META.GetAttachmentIndex
EFFECT_DATA_META.SetAttachment = EFFECT_DATA_META.SetAttachmentIndex

local USER_COMMAND_META = FindMetaTable("UserCommand")

function USER_COMMAND_META:IsForced()
	return not self:IsNew()
end

local VECTOR_META = FindMetaTable("Vector")
VECTOR_META.AngleEx = VECTOR_META.AngleWithUp
VECTOR_META.Distance = VECTOR_META.DistanceTo
VECTOR_META.DistToSqr = VECTOR_META.DistanceToAsSqr
VECTOR_META.Distance2D = VECTOR_META.DistanceTo2D
VECTOR_META.Distance2DSqr = VECTOR_META.DistanceToAsSqr2D
VECTOR_META.Div = VECTOR_META.Divide
VECTOR_META.Mul = VECTOR_META.Scale
VECTOR_META.Normalize = VECTOR_META.NormalizeInPlace
VECTOR_META.SetUnpacked = VECTOR_META.Initialize

function VECTOR_META:Set(vectorToCopy)
	self:Initialize(vectorToCopy.x, vectorToCopy.y, vectorToCopy.z)
end

function VECTOR_META:GetNormalized()
	local copy = Vector(self)
	copy:NormalizeInPlace()
	return copy
end

local ANGLE_META = FindMetaTable("Angle")
ANGLE_META.Div = ANGLE_META.Divide
ANGLE_META.Mul = ANGLE_META.Scale
ANGLE_META.SetUnpacked = ANGLE_META.Initialize

function ANGLE_META:Set(angleToCopy)
	self:Initialize(angleToCopy.p, angleToCopy.y, angleToCopy.r)
end

local ENTITY_META = FindMetaTable("Entity")
ENTITY_META.EntIndex = ENTITY_META.GetEntityIndex
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
ENTITY_META.IsNPC = ENTITY_META.IsNpc
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
ENTITY_META.WaterLevel = ENTITY_META.GetWaterLevel
ENTITY_META.SetMaterial = ENTITY_META.SetMaterialOverride
ENTITY_META.GetMaterial = ENTITY_META.GetMaterialOverride
ENTITY_META.SetSubMaterial = ENTITY_META.SetSubMaterialOverride
ENTITY_META.GetSubMaterial = ENTITY_META.GetSubMaterialOverride
ENTITY_META.GetAbsVelocity = ENTITY_META.GetLocalVelocity
ENTITY_META.SetAbsVelocity = ENTITY_META.SetLocalVelocity
ENTITY_META.Team = ENTITY_META.GetTeamNumber
ENTITY_META.GetNoCollideWithTeammates = ENTITY_META.GetNoCollidingWithTeammates
ENTITY_META.SetNoCollideWithTeammates = ENTITY_META.SetNoCollidingWithTeammates
ENTITY_META.SetColor = ENTITY_META.SetRenderColor
ENTITY_META.GetColor = ENTITY_META.GetRenderColor
ENTITY_META.GetSequenceList = ENTITY_META.GetSequences

function ENTITY_META:GetDTAngle(index)
	return self:GetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.ANGLE, index)
end

function ENTITY_META:GetDTBool(index)
	return self:GetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.BOOLEAN, index)
end

function ENTITY_META:GetDTEntity(index)
	return self:GetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.ENTITY, index)
end

function ENTITY_META:GetDTFloat(index)
	return self:GetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.FLOAT, index)
end

function ENTITY_META:GetDTInt(index)
	return self:GetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.INTEGER, index)
end

function ENTITY_META:GetDTString(index)
	return self:GetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.STRING, index)
end

function ENTITY_META:GetDTVector(index)
	return self:GetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.VECTOR, index)
end

function ENTITY_META:SetDTAngle(index, value)
	self:SetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.ANGLE, index, value)
end

function ENTITY_META:SetDTBool(index, value)
	self:SetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.BOOLEAN, index, value)
end

function ENTITY_META:SetDTEntity(index, value)
	self:SetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.ENTITY, index, value)
end

function ENTITY_META:SetDTFloat(index, value)
	self:SetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.FLOAT, index, value)
end

function ENTITY_META:SetDTInt(index, value)
	self:SetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.INTEGER, index, value)
end

function ENTITY_META:SetDTString(index, value)
	self:SetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.STRING, index, value)
end

function ENTITY_META:SetDTVector(index, value)
	self:SetNetworkDataValue(_E.NETWORK_VARIABLE_TYPE.VECTOR, index, value)
end

function ENTITY_META:IsNextBot()
	return false -- TODO: Implement (low priority)
end

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

function ENTITY_META:GetNoDraw()
	return self:IsEffectActive(_E.ENTITY_EFFECT.NO_DRAW)
end

function ENTITY_META:IsFlagSet(flag)
	return self:GetFlags() & flag ~= 0
end

function ENTITY_META:IsOnGround()
	return self:IsFlagSet(_E.ENGINE_FLAG.ON_GROUND)
end

ENTITY_META.OnGround = ENTITY_META.IsOnGround

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

-- TODO: Actually implement these bone manipulation functions
-- TODO: We should probably override SetupBones, call the baseclass, and then apply our manipulations?
-- TODO: Or we need to use a function from bone_setup.h. I'm not sure which one.
function ENTITY_META:GetManipulateBoneAngles()
	return Angle()
end

function ENTITY_META:GetManipulateBoneJiggle()
	return 0
end

function ENTITY_META:GetManipulateBonePosition()
	return Vector()
end

function ENTITY_META:GetManipulateBoneScale()
	return Vector(1, 1, 1)
end

function ENTITY_META:ManipulateBoneAngles(boneID, angle, isNetworked)
end

function ENTITY_META:ManipulateBoneJiggle(boneID, jiggle)
end

function ENTITY_META:ManipulateBonePosition(boneID, position, isNetworked)
end

function ENTITY_META:ManipulateBoneScale(boneID, scale, isNetworked)
end

function ENTITY_META:HasBoneManipulations()
	return false
end

local WEAPON_META = FindMetaTable("Weapon")
WEAPON_META.GetHoldType = WEAPON_META.GetAnimationPrefix
WEAPON_META.SetHoldType = WEAPON_META.SetAnimationPrefix
WEAPON_META.Ammo1 = WEAPON_META.GetPrimaryAmmoCount
WEAPON_META.Ammo2 = WEAPON_META.GetSecondaryAmmoCount

local PLAYER_META = FindMetaTable("Player")
PLAYER_META.GetShootPos = PLAYER_META.GetWeaponShootPosition
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
PLAYER_META.SetSlowWalkSpeed = PLAYER_META.SetWalkSpeed
PLAYER_META.GetSlowWalkSpeed = PLAYER_META.GetWalkSpeed
PLAYER_META.SetWalkSpeed = PLAYER_META.SetNormalSpeed
PLAYER_META.GetWalkSpeed = PLAYER_META.GetNormalSpeed
PLAYER_META.GetCrouchedWalkSpeed = PLAYER_META.GetCrouchWalkFraction
PLAYER_META.SetCrouchedWalkSpeed = PLAYER_META.SetCrouchWalkFraction
PLAYER_META.Ping = PLAYER_META.GetPing
PLAYER_META.KeyDown = PLAYER_META.IsKeyDown
PLAYER_META.KeyDownLast = PLAYER_META.WasKeyDown
PLAYER_META.KeyPressed = PLAYER_META.WasKeyPressed
PLAYER_META.KeyReleased = PLAYER_META.WasKeyReleased
PLAYER_META.GetFOV = PLAYER_META.GetFov
PLAYER_META.SetFOV = PLAYER_META.SetFov
PLAYER_META.SetUnDuckSpeed = PLAYER_META.SetUnDuckFraction
PLAYER_META.GetUnDuckSpeed = PLAYER_META.GetUnDuckFraction
PLAYER_META.SetDSP = PLAYER_META.SetDsp
PLAYER_META.ShouldDropWeapon = PLAYER_META.SetDropActiveWeaponOnDeath
PLAYER_META.Armor = PLAYER_META.GetArmor

function PLAYER_META:LagCompensation(shouldStart)
	if (shouldStart) then
		self:StartLagCompensation()
	else
		self:FinishLagCompensation()
	end
end

function PLAYER_META:Crouching()
	return self:IsFlagSet(_E.ENGINE_FLAG.DUCKING)
end

--[[
	TODO: Challenge: 	FCVAR_USERINFO convars are sent to the server so it knows about them.
	TODO:				However, only those sent before the player is fully connected are stored.
	TODO:				- Can we somehow have the server store them after the player has connected?
	TODO:				- Do we need to rewrite these engine functions to support this?`
	ANSWER:				See gameinterface.cpp DetourClientConnect for a hacky solution.

	Test commands:

	hack_reset_cvars

	lua_run_cl ConsoleVariables.Create("supertest", "0", _E.FCVAR.USERINFO)

	lua_run_cl print(ConsoleVariables.Get("supertest"):SetValue(124))`

	lua_run print(player.GetByID(1):GetInfo("supertest"))
--]]
function PLAYER_META:GetInfo(consoleVariableName)
	return engine.GetClientConsoleVariableValue(self, consoleVariableName)
end

function PLAYER_META:GetInfoNum(consoleVariableName, default)
	return engine.GetClientConsoleVariableValueAsNumber(self, consoleVariableName) or default
end

function PLAYER_META:PrintMessage(type, message)
	Players.ClientPrint(self, type, message)
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

function PLAYER_META:GetCanZoom()
	return self.__canZoom or false
end

function PLAYER_META:SetCanZoom(canZoom)
	self.__canZoom = canZoom
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

function PLAYER_META:FlashlightIsOn()
	return self:IsFlashlightOn()
end

function PLAYER_META:SetWeaponColor(color)
	-- Not implemented
end

function PLAYER_META:GetWeaponColor()
	return Color(255, 255, 255)
end

if (SERVER) then
    function PLAYER_META:SelectWeapon(weaponClass)
        self:SwitchWeapon(self:GetWeapon(weaponClass))
    end

	function PLAYER_META:StripAmmo()
		self:RemoveAllAmmo()
	end

	function PLAYER_META:StripWeapon(weaponClass)
		self:RemoveWeapon(self:GetWeapon(weaponClass))
	end

	function PLAYER_META:StripWeapons()
		local shouldRemoveSuit = false
		-- shouldRemoveSuit is false by default, but here for clarity.
		-- TODO: What does Garry's Mod do with this?
		self:RemoveAllItems(shouldRemoveSuit)
	end

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

	function PLAYER_META:CrosshairDisable()
		self:ShowCrosshair(false)
	end

	function PLAYER_META:CrosshairEnable()
		self:ShowCrosshair(true)
	end

	function PLAYER_META:Flashlight(isOn)
		if (isOn) then
			self:TurnFlashlightOn()
		else
			self:TurnFlashlightOff()
		end
	end
else
	net.Receive("__PlayerLuaRun", function()
		local lua = net.ReadString()
		RunString(lua)
	end)
end

function PLAYER_META:SetClassID(id)
	self:SetNWInt("__classId", id)
end

function PLAYER_META:GetClassID()
	return self:GetNWInt("__classId", 0)
end

function PLAYER_META:IsListenServerHost()
	if (CLIENT) then
		if (Engines.GetMaxClients() == 1) then
			-- Singleplayer always returns true
			return true
		else
			return self == Players.FindByIndex(1)
		end
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

local CONSOLE_VARIABLE_META = FindMetaTable("ConsoleVariable")
CONSOLE_VARIABLE_META.GetInt = CONSOLE_VARIABLE_META.GetInteger
CONSOLE_VARIABLE_META.SetInt = CONSOLE_VARIABLE_META.SetValue
CONSOLE_VARIABLE_META.SetFloat = CONSOLE_VARIABLE_META.SetValue
CONSOLE_VARIABLE_META.SetBool = CONSOLE_VARIABLE_META.SetValue
CONSOLE_VARIABLE_META.SetString = CONSOLE_VARIABLE_META.SetValue

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
MATERIAL_META.SetInt = MATERIAL_META.SetInteger
MATERIAL_META.GetInt = MATERIAL_META.GetInteger

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

Material = function(name)
	return Materials.Find(name)
end

if (SERVER) then
	resource.AddWorkshop = function() end -- TODO: Implement (low priority)
else
	-- Empty functions so we can easily add resource files without errors in shared code
	resource = {}
	resource.AddFile = function() end
	resource.AddWorkshop = function() end
	resource.AddSingleFile = function() end

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

	vgui.GetWorldPanel = Panels.GetClientLuaRootPanel
	GetHUDPanel = Panels.GetClientLuaRootHudPanel

	vgui.GetKeyboardFocus = Inputs.GetFocus
	vgui.GetHoveredPanel = Inputs.GetMouseOver
	vgui.CursorVisible = Surfaces.IsCursorVisible

	gui = {
		MouseX = function()
			local x, y = Inputs.GetCursorPosition()
			return x
		end,

		MouseY = function()
			local x, y = input.GetCursorPosition()
			return y
		end,

		EnableScreenClicker = function(visible)
			Surfaces.SetCursorAlwaysVisible(visible)
		end,

		SetMousePos = input.SetCursorPosition,
		ScreenToVector = input.ScreenToWorld,
		AimToVector = input.AimToVector,
		IsConsoleVisible = Engines.IsConsoleVisible,
	}

	cam = {
		Start3D = Renders.PushView3D,
		Start2D = Renders.PushView2D,
		End3D = Renders.PopView3D,
		End2D = Renders.PopView2D,

		IgnoreZ = function(bBool)
			if (bBool) then
				Renders.DepthRange(0, 0.01)
			else
				Renders.DepthRange(0, 1)
			end
		end,

		GetModelMatrix = Renders.GetModelMatrix,
        PushModelMatrix = Renders.PushModelMatrix,
		PopModelMatrix = Renders.PopModelMatrix,
	}

	render.SetModelLighting = Renders.SetAmbientLightCube
	render.ResetModelLighting = Renders.ResetAmbientLightCube
	render.PushFilterMin = Renders.PushFilterMinification
	render.PopFilterMin = Renders.PopFilterMinification
	render.PushFilterMag = Renders.PushFilterMagnification
	render.PopFilterMag = Renders.PopFilterMagnification
	render.SetScissorRect = Renders.SetScissorRectangle
    render.SetWriteDepthToDestAlpha = Renders.SetWriteDepthToDestinationAlpha
	render.EnableClipping = Renders.SetClippingEnabled

	function render.Clear(r, g, b, a, clearDepth, clearStencil)
		Renders.ClearBuffers(true, clearDepth or false, clearStencil or false)
		Renders.ClearColor(r, g, b, a)
	end

	function render.ClearDepth(clearStencil)
		Renders.ClearBuffers(false, true, clearStencil or false)
	end

	function render.ClearStencil()
		Renders.ClearBuffers(false, false, true)
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

	CreateMaterial = Materials.Create
	DisableClipping = Surfaces.DisableClipping
	GetViewEntity = Renders.GetViewEntity

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
	PANEL_META.GetZPos = PANEL_META.GetZIndex
	PANEL_META.SetZPos = PANEL_META.SetZIndex
	PANEL_META.ParentToHUD = PANEL_META.ParentToHud
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
			self:SetBackgroundColor(rOrColor)
		else
			self:SetBackgroundColor(Colors.Create(rOrColor, g, b, a))
		end
	end

	-- Change casing and add functionality to pass individual color components.
	function PANEL_META:SetFGColor(rOrColor, g, b, a)
		if (istable(rOrColor)) then
			self:SetForegroundColor(rOrColor)
		else
			self:SetForegroundColor(Colors.Create(rOrColor, g, b, a))
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

	local HTML_PANEL_META = FindMetaTable("Html")
	HTML_PANEL_META.OpenURL = HTML_PANEL_META.OpenUrl
	HTML_PANEL_META.SetHTML = HTML_PANEL_META.SetHtml

	local TEXT_ENTRY_PANEL_META = FindMetaTable("TextEntry")
	TEXT_ENTRY_PANEL_META._OriginalSetFont = TEXT_ENTRY_PANEL_META._OriginalSetFont or TEXT_ENTRY_PANEL_META.SetFont
	TEXT_ENTRY_PANEL_META._OriginalGetFont = TEXT_ENTRY_PANEL_META._OriginalGetFont or TEXT_ENTRY_PANEL_META.GetFont
	TEXT_ENTRY_PANEL_META.DrawTextEntryText = TEXT_ENTRY_PANEL_META.PaintText
	TEXT_ENTRY_PANEL_META.GetCaretPos = TEXT_ENTRY_PANEL_META.GetCursorPosition
	TEXT_ENTRY_PANEL_META.SetCaretPos = TEXT_ENTRY_PANEL_META.SetCursorPosition

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
	GetRenderTarget = function(name, width, height)
		return Renders.CreateRenderTargetTextureEx(name, width, height)
	end
	EyePos = render.MainViewOrigin
	EyeAngles = render.MainViewAngles
	EyeVector = render.MainViewForward

	LocalPlayer = Players.GetLocalPlayer

	surface.SetDrawColor = Surfaces.DrawSetColor
    surface.DrawRect = Surfaces.DrawFilledRectangle
	surface.SetAlphaMultiplier = Surfaces.DrawSetAlphaMultiplier

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
			return Panels.GetGameUIPanel()
		end

		return Panels.GetClientLuaRootPanel()
	end

	--[[
		We disable our own Panels.Create and Panels.Register logic, in favor of the GMod one.
		This Panels.Create will be used as vgui.CreateX by gmod to create internal panels.
	--]]
	function Panels.Create(panelClassName, parentPanel, name)
		parentPanel = parentPanel or getAppropriateBaseParent()

		if (panelClassName == "ModelImage") then
			panelClassName = "ModelImagePanel"
		end

		if (not Panels[panelClassName]) then
			error("attempt to create non-existing panel class \"" .. tostring(panelClassName) .. "\"", 2)
		end

		return Panels[panelClassName](parentPanel, name or panelClassName)
	end

	surface.SetDrawColor = Surfaces.DrawSetColor
	surface.DrawRect = Surfaces.DrawFilledRectangle
	surface.DrawOutlinedRect = Surfaces.DrawOutlinedRectangle
	surface.DrawTexturedRect = Surfaces.DrawTexturedRectangle
	-- TODO: surface.DrawTexturedRectRotated = Surfaces.DrawTexturedRotatedRectangle
	surface.DrawTexturedRectUV = Surfaces.DrawTexturedSubRectangle
	surface.GetTextPos = Surfaces.DrawGetTextPosition
	surface.SetTextPos = Surfaces.DrawSetTextPosition
	surface.SetTextColor = Surfaces.DrawSetTextColor
	surface.DrawText = Surfaces.DrawPrintText
	surface.SetTexture = Surfaces.DrawSetTexture

	local currentFont
	surface.SetFont = function(font)
		currentFont = font
		surface.DrawSetTextFont(font)
	end

	local oldTextSize = Surfaces.GetTextSize

	surface.GetTextSize = function(text)
		return oldTextSize(currentFont, text)
	end

	surface.SetMaterial = function(material)
		local name = material:GetString("$basetexture")

		if (not textureMap[name]) then
			textureMap[name] = Surfaces.CreateNewTextureID(true)
			Surfaces.DrawSetTextureMaterial(textureMap[name], material)
		end

		Surfaces.DrawSetTexture(textureMap[name])
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
	chat = Chats

	chat.GetChatBoxPos = Chats.GetChatBoxPosition
	chat.Open = Chats.StartMessageMode
	chat.Close = Chats.StopMessageMode

	ProjectedTexture = ProjectedTextures.Create

	local PROJECTED_TEXTURE_META = FindMetaTable("ProjectedTexture")
	PROJECTED_TEXTURE_META.GetPos = PROJECTED_TEXTURE_META.GetPosition
	PROJECTED_TEXTURE_META.SetPos = PROJECTED_TEXTURE_META.SetPosition
	PROJECTED_TEXTURE_META.GetFOV = PROJECTED_TEXTURE_META.GetFov
	PROJECTED_TEXTURE_META.SetFOV = PROJECTED_TEXTURE_META.SetFov
	PROJECTED_TEXTURE_META.GetHorizontalFOV = PROJECTED_TEXTURE_META.GetHorizontalFov
	PROJECTED_TEXTURE_META.SetHorizontalFOV = PROJECTED_TEXTURE_META.SetHorizontalFov
	PROJECTED_TEXTURE_META.GetVerticalFOV = PROJECTED_TEXTURE_META.GetVerticalFov
	PROJECTED_TEXTURE_META.SetVerticalFOV = PROJECTED_TEXTURE_META.SetVerticalFov

	local AUDIO_CHANNEL_META = FindMetaTable("AudioChannel")
	AUDIO_CHANNEL_META.FFT = AUDIO_CHANNEL_META.GetFft
	AUDIO_CHANNEL_META.Is3D = AUDIO_CHANNEL_META.Is3d
	AUDIO_CHANNEL_META.Set3DCone = AUDIO_CHANNEL_META.Set3dCone
	AUDIO_CHANNEL_META.Get3DCone = AUDIO_CHANNEL_META.Get3dCone
	AUDIO_CHANNEL_META.Set3DEnabled = AUDIO_CHANNEL_META.Set3dEnabled
	AUDIO_CHANNEL_META.Get3DEnabled = AUDIO_CHANNEL_META.Get3dEnabled
	AUDIO_CHANNEL_META.Set3DFadeDistance = AUDIO_CHANNEL_META.Set3dFadeDistance
	AUDIO_CHANNEL_META.Get3DFadeDistance = AUDIO_CHANNEL_META.Get3dFadeDistance
	AUDIO_CHANNEL_META.GetPos = AUDIO_CHANNEL_META.GetPosition
	AUDIO_CHANNEL_META.SetPos = AUDIO_CHANNEL_META.SetPosition
	AUDIO_CHANNEL_META.GetTagsHTTP = AUDIO_CHANNEL_META.GetTagsOfHttp
	AUDIO_CHANNEL_META.GetTagsID3 = AUDIO_CHANNEL_META.GetTagsOfId3
	AUDIO_CHANNEL_META.GetTagsMeta = AUDIO_CHANNEL_META.GetTagsOfMeta
	AUDIO_CHANNEL_META.GetTagsOGG = AUDIO_CHANNEL_META.GetTagsOfOgg
	AUDIO_CHANNEL_META.GetTagsVendor = AUDIO_CHANNEL_META.GetTagsOfVendor

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
local gmod_language = CreateConVar("gmod_language", "en",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Language to use in GMod")

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

local sv_defaultdeployspeed = CreateConVar("sv_defaultdeployspeed", "4",
	{ FCVAR_ARCHIVE, FCVAR_NOTIFY, FCVAR_REPLICATED, FCVAR_SERVER_CAN_EXECUTE },
	"Default deploy speed for weapons")

--[[
	Now that the compatibility libraries have been loaded, we can start changing hooks
	and adding commands
--]]
if (CLIENT) then
	ConsoleCommands.Add("+menu", function(client, command, arguments)
		hook.Run("OnSpawnMenuOpen")
	end)

	ConsoleCommands.Add("-menu", function(client, command, arguments)
		hook.Run("OnSpawnMenuClose")
	end)

	ConsoleCommands.Add("+menu_context", function(client, command, arguments)
		hook.Run("OnContextMenuOpen")
	end)

	ConsoleCommands.Add("-menu_context", function(client, command, arguments)
		hook.Run("OnContextMenuClose")
	end)

	hook.Add("LevelInitPostEntity", "GModCompatibility.CallInitPostEntityHooks", function()
		hook.Run("InitPostEntity")
	end)
else
	hook.Add("ServerActivate", "GModCompatibility.CallInitPostEntityHooks", function()
		hook.Run("InitPostEntity")
	end)

	hook.Add("ClientConnected", "GModCompatibility.CallPlayerConnectHook", function(name, ip)
		hook.Run("PlayerConnect", name, ip)
	end)
end

hook.Add("Initialize", "GModCompatibility.CallInitializeHooks", function()
	-- Copy ents from our system to the GMod system.
	local scriptedEntities = ScriptedEntities.GetList()
	for className, scriptedEntity in pairs(scriptedEntities) do
		scripted_ents.Register(scriptedEntity, className)
	end
	scripted_ents.OnLoaded()

	local ScriptedWeapons = ScriptedWeapons.GetList()
	for className, scriptedWeapon in pairs(ScriptedWeapons) do
		weapons.Register(scriptedWeapon, className)
	end
	weapons.OnLoaded()

	hook.Run("CreateTeams")
	hook.Run("PreGamemodeLoaded")
	hook.Run("OnGamemodeLoaded")
	hook.Run("PostGamemodeLoaded")
end)

hook.Add("PreEntityInitialize", "GModCompatibility.CallSetupDataTables", function(entity)
	if (entity.InstallDataTable) then
		entity:InstallDataTable()
	end

	if (entity.SetupDataTables) then
		entity:SetupDataTables()
	end
end)

-- Setup the Garry's Mod lua include path so Include can find scripts
-- No need to prefix search paths here, since Include already finds scripts using
-- the configured search paths.
package.IncludePath = "lua/;" .. package.IncludePath

-- Setup to search for modules in all search paths
local searchPathsString = Files.GetSearchPath("GAME")
local searchPaths = {}

for searchPath in searchPathsString:gmatch("([^;]+)") do
	searchPaths[#searchPaths + 1] = searchPath

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
		-- ["weapons"] = true,
		-- ["scripted_ents"] = true,
		["usermessage"] = true,
	},
	include = {
		["extensions/net.lua"] = true, -- We implement networking using luasocket
		["extensions/file.lua"] = true, -- Our filesystem works slightly different
		["util/color.lua"] = true, -- In contrast with gmod, we should properly get metatables everywhere (so don't need this hack util)
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
	Panels.HTML = Panels.Html

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

if (SERVER) then
	PLAYER_META.IPAddress = PLAYER_META.GetIpAddress
    PLAYER_META.UnLock = PLAYER_META.Unlock

	-- AllowFlashlight and CanUseFlashlight override default gmod implementations, which is why its placed after include("includes/init.lua")
	-- https://github.com/Facepunch/garrysmod/blob/1ce3b6fec3417e4798ade0862540da74ce612483/garrysmod/lua/includes/extensions/player.lua#L182C70-L183C71
	function PLAYER_META:AllowFlashlight(canFlashlight)
		self:SetFlashlightEnabled(canFlashlight)
	end

	function PLAYER_META:CanUseFlashlight()
		return self:IsFlashlightEnabled()
	end
end

--[[
	Load the autorun scripts for Garry's Mod addons/etc.
--]]
includeFolder("lua/autorun")

if (CLIENT) then
	includeFolder("lua/autorun/client")
else
	includeFolder("lua/autorun/server")
end

--[[
	For all search paths, ensure the entities and weapons are loaded, after all autorun scripts have been loaded.
--]]

local pathsToLoad = {
	entities = {
		"lua/entities/",
		-- "../entities/entities/", -- Gamemode entities are loaded by the loader
	},
	weapons = {
		"lua/weapons/",
		-- "../entities/weapons/", -- Gamemode weapons are loaded by the loader
	},
}

for _, searchPath in ipairs(searchPaths) do
	local separator = (searchPath:sub(-1) == "/" or searchPath:sub(-1) == "\\") and "" or "/"

	for __, path in pairs(pathsToLoad.entities) do
		local entitiesPath = searchPath .. separator .. path

		if (file.Exists(entitiesPath, "GAME")) then
			print("[GMOD Compatibility] Loading entities from " .. entitiesPath)
			ScriptedEntities.LoadFromDirectory(entitiesPath)
		end
	end

	for __, path in pairs(pathsToLoad.weapons) do
		local weaponsPath = searchPath .. separator .. path

		if (file.Exists(weaponsPath, "GAME")) then
			print("[GMOD Compatibility] Loading weapons from " .. weaponsPath)
			ScriptedWeapons.LoadFromDirectory(weaponsPath)
		end
	end
end
