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

function GetConVar_Internal(name)
	local consoleVariable = GetConsoleVariable(name)

	if (not IsValid(consoleVariable)) then
		return nil
	end

	return consoleVariable
end

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
VGUIFrameTime = Globals.SystemTime
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

local basePath = "resource/gmod_compatibility_content/localization/en/"
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

		language.Add(key, value)
	end
end

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

local VECTOR_META = FindMetaTable("Vector")
VECTOR_META.AngleEx = VECTOR_META.AngleWithUp

local ENTITY_META = FindMetaTable("Entity")
ENTITY_META.GetModel = ENTITY_META.GetModelName
ENTITY_META.GetTable = ENTITY_META.GetRefTable

function ENTITY_META:SetNoDraw(bBool)
    if (bBool) then
        self:SetRenderMode(10) -- kRenderNone
    else
        self:SetRenderMode(0) -- kRenderNormal
    end
end

function ENTITY_META:SetIK(bBool)
	print("TODO: Implement ENTITY_META:SetIK")
end

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
MATERIAL_META.IsError = MATERIAL_META.IsErrorMaterial

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

	-- Returns whether the currently focused panel is a child of the given one.
	function vgui.FocusedHasParent(panel)
		local focusedPanel = Input.GetFocus()

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

	vgui.GetKeyboardFocus = Input.GetFocus
	vgui.GetHoveredPanel = Input.GetMouseOver

	gui = {
		MouseX = function()
			local x, y = input.GetCursorPosition()
			return x
		end,
		MouseY = function()
			local x, y = input.GetCursorPosition()
			return y
		end,
		SetMousePos = input.SetCursorPosition,
    }

    cam = {
        Start3D = render.Push3DView,
		End3D = render.PopView,
    }

    render.SetModelLighting = render.SetAmbientLightCube
    render.ResetModelLighting = render.ResetAmbientLightCube

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

	function input.IsShiftDown()
		return input.IsKeyDown(KEY_LSHIFT) or input.IsKeyDown(KEY_RSHIFT)
	end

	function input.IsControlDown()
		return input.IsKeyDown(KEY_LCONTROL) or input.IsKeyDown(KEY_RCONTROL)
	end

	CreateMaterial = Surface.CreateMaterial
	DisableClipping = Surface.DisableClipping

	local MODEL_IMAGE_PANEL_META = FindMetaTable("ModelImagePanel")
	MODEL_IMAGE_PANEL_META._OriginalRebuildSpawnIcon = MODEL_IMAGE_PANEL_META._OriginalRebuildSpawnIcon or MODEL_IMAGE_PANEL_META.RebuildSpawnIcon
	registry.ModelImage = MODEL_IMAGE_PANEL_META

	MODEL_IMAGE_PANEL_META.SetSpawnIcon = MODEL_IMAGE_PANEL_META.SetModelImage

	function MODEL_IMAGE_PANEL_META:RebuildSpawnIcon()
		-- TODO: sensible defaults
		self:_OriginalRebuildSpawnIcon({
			origin = Vector(0, 0, 0),
			angles = Angle(0, 0, 0),
			fieldOfView = 90,
			zNear = 1,
			zFar = 1000,
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
	LABEL_PANEL_META._OriginalSetContentAlignment = LABEL_PANEL_META._OriginalSetContentAlignment or LABEL_PANEL_META.SetContentAlignment

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
	surface.SetFont = surface.DrawSetTextFont
	surface.SetTextPos = surface.DrawSetTextPos
	surface.SetTextColor = surface.DrawSetTextColor
	surface.DrawText = surface.DrawPrintText
	surface.SetTexture = surface.DrawSetTexture

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
