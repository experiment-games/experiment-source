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

local model = "models/stalkertnb/exo_free.mdl"-- "models/props_junk/PlasticCrate01a.mdl"
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

local buttonByHelper = vgui.Create("DButton", spawnIconFrame)
buttonByHelper:SetText("Set by Helper")
buttonByHelper:SetSize(100, 32)
buttonByHelper:SetPos(4 + 100 * 4, 512 + 30 + 32)
buttonByHelper.DoClick = function()
	local ent = ClientsideModel(model, RENDERGROUP_OTHER)
    local result = PositionSpawnIcon(ent, vector_origin)
	ent:Remove()

	PrintTable(result)

	origin = result.origin
	angles = result.angles
	fieldOfView = result.fov
	zNear = result.znear
	zFar = result.zfar

	RebuildTheSpawnIcon()
end
