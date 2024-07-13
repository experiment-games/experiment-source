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
spawnIconFrame:SetSize(512 + 8, 512 + 30 + 32 + 4)
spawnIconFrame:SetTitle("SpawnIcon")
spawnIconFrame:Center()
spawnIconFrame:MakePopup()

local spawnIcon = vgui.Create("SpawnIcon", spawnIconFrame)
spawnIcon:SetModel("models/stalkertnb/exo_free.mdl")
spawnIcon:SetSize(512, 512)
spawnIcon:SetPos(4, 30)

local buttonToRebuildSpawnIcon = vgui.Create("DButton", spawnIconFrame)
buttonToRebuildSpawnIcon:SetText("Rebuild SpawnIcon")
buttonToRebuildSpawnIcon:SetSize(512, 32)
buttonToRebuildSpawnIcon:SetPos(4, 512 + 30)
buttonToRebuildSpawnIcon.DoClick = function()
	spawnIcon:RebuildSpawnIcon()
end
