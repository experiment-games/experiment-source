hook.Add("HudViewportPaint", "GModCompatibility.CallHUDPaint", function()
	hook.Run("HUDPaint")
end)

ConsoleCommands.Add("+menu", function(client, pCmd, args)
	hook.Run("OnSpawnMenuOpen")
end)

ConsoleCommands.Add("-menu", function(client, pCmd, args)
	hook.Run("OnSpawnMenuClose")
end)

ConsoleCommands.Add("+menu_context", function(client, pCmd, args)
	hook.Run("OnContextMenuOpen")
end)

ConsoleCommands.Add("-menu_context", function(client, pCmd, args)
	hook.Run("OnContextMenuClose")
end)
