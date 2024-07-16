hook.Add("HudViewportPaint", "GModCompatibility.CallHUDPaint", function()
	hook.Run("HUDPaint")
end)

ConsoleCommands.Add("+buildmenu", function(client, pCmd, args)
	hook.Run("OnSpawnMenuOpen")
end)

ConsoleCommands.Add("-buildmenu", function(client, pCmd, args)
	hook.Run("OnSpawnMenuClose")
end)
