hook.Add("HudViewportPaint", "GModCompatibility.CallHUDPaint", function()
	hook.Run("HUDPaint")
end)

hook.Add("KeyInput", "GModCompatibility.CallSpawnMenuHooks", function(down, keyNumber, currentBinding)
	if (keyNumber ~= KEY_Q) then
		return
	end

	if (down == 1) then
		hook.Run("OnSpawnMenuOpen")
	elseif (not down) then
		hook.Run("OnSpawnMenuClose")
	end
end)
