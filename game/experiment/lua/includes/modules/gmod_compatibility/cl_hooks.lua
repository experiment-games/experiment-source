hook.Add("HudViewportPaint", "GModCompatibility.CallHUDPaint", function()
	hook.Run("HUDPaint")
end)

ConsoleCommands.Add("+buildmenu", function(client, pCmd, args)
	hook.Run("OnSpawnMenuOpen")
end)

ConsoleCommands.Add("-buildmenu", function(client, pCmd, args)
	hook.Run("OnSpawnMenuClose")
end)

-- A single DFrame with a spawnicon, we call RebuildSpawnIcon on it
local spawnIconFrame = vgui.Create("DFrame")
spawnIconFrame:SetSize(512 + 8, 512 + 30 + 32 + 4 + 32 + 4)
spawnIconFrame:SetTitle("SpawnIcon")
spawnIconFrame:Center()
spawnIconFrame:MakePopup()

local model = "models/props_junk/PlasticCrate01a.mdl"--"models/stalkertnb/exo_free.mdl")
TheSpawnIcon = vgui.Create("SpawnIcon", spawnIconFrame)
TheSpawnIcon:SetModel(model)
TheSpawnIcon:SetSize(512, 512)
TheSpawnIcon:SetPos(4, 30)

local origin = Vector(265.463013, 222.722046, 161.557571)
local angles = Angle(25.000000, 220.000000, 0.000000)
local fieldOfView = 5.0196311198984
local zNear = 1
local zFar = 433.3431892957

function RebuildTheSpawnIcon()
    TheSpawnIcon:RebuildSpawnIconEx({
		origin = origin,
		angles = angles,
		fieldOfView = fieldOfView,
		zNear = zNear,
		zFar = zFar
	})
end

local buttonToRebuildSpawnIcon = vgui.Create("DButton", spawnIconFrame)
buttonToRebuildSpawnIcon:SetText("Rebuild SpawnIcon")
buttonToRebuildSpawnIcon:SetSize(512, 32)
buttonToRebuildSpawnIcon:SetPos(4, 512 + 30)
buttonToRebuildSpawnIcon.DoClick = function()
    RebuildTheSpawnIcon()
end

local function RenderSpawnIcon_Prop( model, pos, middle, size )

	if ( size < 900 ) then
		size = size * ( 1 - ( size / 900 ) )
	else
		size = size * ( 1 - ( size / 4096 ) )
	end

	size = math.Clamp( size, 5, 1000 )

	local ViewAngle = Angle( 25, 220, 0 )
	local ViewPos = pos + ViewAngle:Forward() * size * -15
	local view = {}

	view.fov		= 4 + size * 0.04
	view.origin		= ViewPos + middle
	view.znear		= 1
	view.zfar		= ViewPos:Distance( pos ) + size * 2
	view.angles		= ViewAngle

	return view

end

local function RenderSpawnIcon_Ragdoll( model, pos, middle, size )

	local at = model:GetAttachment( model:LookupAttachment( "eyes" ) )
	if ( !at ) then at = { Pos = model:GetPos(), Ang = model:GetAngles() } end

	local ViewAngle = at.Ang + Angle( -10, 160, 0 )
	local ViewPos = at.Pos + ViewAngle:Forward() * -60 + ViewAngle:Up() * -2
	local view = {}

	view.fov		= 10
	view.origin		= ViewPos
	view.znear		= 0.1
	view.zfar		= 100
	view.angles		= ViewAngle

	return view

end

--
-- For some TF2 ragdolls which do not have "eye" attachments
--
local function RenderSpawnIcon_Ragdoll_Head( model, pos, middle, size )

	local at = model:GetAttachment( model:LookupAttachment( "head" ) )
	if ( !at ) then at = { Pos = model:GetPos(), Ang = model:GetAngles() } end

	local ViewAngle = at.Ang + Angle( -10, 160, 0 )
	local ViewPos = at.Pos + ViewAngle:Forward() * -67 + ViewAngle:Up() * -7 + ViewAngle:Right() * 1.5
	local view = {}

	view.fov		= 10
	view.origin		= ViewPos
	view.znear		= 0.1
	view.zfar		= 100
	view.angles		= ViewAngle

	return view

end

local function RenderSpawnIcon_Ragdoll_Facemask( model, pos, middle, size )

	local at = model:GetAttachment( model:LookupAttachment( "facemask" ) )
	if ( !at ) then at = { Pos = model:GetPos(), Ang = model:GetAngles() } end

	local ViewAngle = at.Ang
	ViewAngle:RotateAroundAxis( ViewAngle:Right(), -10 )
	ViewAngle:RotateAroundAxis( ViewAngle:Up(), 160 )
	local ViewPos = at.Pos + ViewAngle:Forward() * -67 + ViewAngle:Up() * -2 + ViewAngle:Right() * -1
	local view = {}

	view.fov		= 10
	view.origin		= ViewPos
	view.znear		= 0.1
	view.zfar		= 100
	view.angles		= ViewAngle

	return view

end

local function RenderSpawnIcon_Ragdoll_Forward( model, pos, middle, size )

	local at = model:GetAttachment( model:LookupAttachment( "forward" ) )
	if ( !at ) then at = { Pos = model:GetPos(), Ang = model:GetAngles() } end

	local ViewAngle = at.Ang + Angle( 10, -20, 0 )
	local ViewPos = at.Pos + ViewAngle:Forward() * -67 + ViewAngle:Up() * -1 + ViewAngle:Right() * 2.5
	local view = {}

	view.fov		= 10
	view.origin		= ViewPos
	view.znear		= 0.1
	view.zfar		= 100
	view.angles		= ViewAngle

	return view

end

local function RenderSpawnIcon_DOD( model, pos, middle, size )

	local ViewAngle = Angle( 0, 160, 0 )
	local ViewPos = pos + ViewAngle:Forward() * -67 + ViewAngle:Up() * 30 + ViewAngle:Right() * 2.5
	local view = {}

	view.fov		= 10
	view.origin		= ViewPos + middle
	view.znear		= 1
	view.zfar		= ViewPos:Distance( pos ) + size * 2
	view.angles		= ViewAngle

	return view

end

local function RenderSpawnIcon_CS( model, pos, middle, size )

	local ViewAngle = Angle( 0, 160, 0 )
	local ViewPos = pos + ViewAngle:Forward() * -70 + ViewAngle:Up() * 32.4 + ViewAngle:Right() * 1.5
	local view = {}

	view.fov		= 10
	view.origin		= ViewPos + middle
	view.znear		= 1
	view.zfar		= ViewPos:Distance( pos ) + size * 2
	view.angles		= ViewAngle

	return view

end

local function RenderSpawnIcon_Special( model, pos, middle, size, x, y, z )

	local ViewAngle = Angle( 15, 140, 0 )
	local ViewPos = pos + ViewAngle:Forward() * x + ViewAngle:Up() * y + ViewAngle:Right() * z
	local view = {}

	view.fov		= 20
	view.origin		= ViewPos + middle
	view.znear		= 1
	view.zfar		= ViewPos:Distance( pos ) + size * 2
	view.angles		= ViewAngle

	return view

end

SpawniconGenFunctions = {}

function PositionSpawnIcon( model, pos, noAngles )

	local mn, mx = model:GetRenderBounds()
	local middle = ( mn + mx ) * 0.5
	local size = 0
	size = math.max( size, math.abs( mn.x ) + math.abs( mx.x ) )
	size = math.max( size, math.abs( mn.y ) + math.abs( mx.y ) )
	size = math.max( size, math.abs( mn.z ) + math.abs( mx.z ) )

	model:SetPos( pos )
	if ( !noAngles ) then model:SetAngles( angle_zero ) end

	local ModelName = model:GetModel()
	ModelName = string.Replace( ModelName, "--", "/" )
	ModelName = string.Replace( ModelName, "\\", "/" )
	ModelName = string.Replace( ModelName, "//", "/" )
	ModelName = string.Replace( ModelName, "\\\\", "/" )

	local fnc = SpawniconGenFunctions[ ModelName ]
	if ( fnc ) then return fnc( model, pos, middle, size ) end

	if ( model:LookupAttachment( "eyes" ) > 0 ) then
		return RenderSpawnIcon_Ragdoll( model, pos, middle, size )
	end

	if ( model:LookupAttachment( "head" ) > 0 ) then
		return RenderSpawnIcon_Ragdoll_Head( model, pos, middle, size )
	end

	-- CS:GO Player Models
	if ( model:LookupAttachment( "facemask" ) > 0 ) then
		return RenderSpawnIcon_Ragdoll_Facemask( model, pos, middle, size )
	end

	-- CS:GO Hostages
	if ( model:LookupAttachment( "forward" ) > 0 ) then
		return RenderSpawnIcon_Ragdoll_Forward( model, pos, middle, size )
	end

	return RenderSpawnIcon_Prop( model, pos, middle, size )

end

local buttonByHelper = vgui.Create("DButton", spawnIconFrame)
buttonByHelper:SetText("Set by Helper")
buttonByHelper:SetSize(100, 32)
buttonByHelper:SetPos(4 + 100 * 4, 512 + 30 + 32)
buttonByHelper.DoClick = function()
	local ent = ClientsideModel( model, RENDERGROUP_OTHER )
    local result = PositionSpawnIcon(ent, vector_origin)

    PrintTable(result)

    origin = result.origin
    angles = result.angles
    fieldOfView = result.fov
    zNear = result.znear
	zFar = result.zfar

	RebuildTheSpawnIcon()
end
