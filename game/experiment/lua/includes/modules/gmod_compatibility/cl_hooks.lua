-- TODO: Remove this, for testing only
testMat = Material("icon16/accept.png")
testTexture = surface.GetTextureID("vgui/gfx/vgui/crosshair")
-- lua_run_cl STOP = true surface.ClearAllTextures()
-- TODO: End of 'Remove this, for testing only'

hook.Add("HudViewportPaint", "GModCompatibility.CallHUDPaint", function()
	hook.Run("HUDPaint")

	-- TODO: Remove this, for testing only
	if (not STOP) then -- TODO: How to detect freed material textures?
		surface.SetMaterial(testMat)
		surface.SetDrawColor(255, 255, 255, 255)
		surface.DrawTexturedRect(0, 0, 64, 64)
	end

	if (surface.DrawGetTextureFile(testTexture) == true) then
		surface.SetTexture(testTexture)
		surface.SetDrawColor(255, 255, 255, 255)
		surface.DrawTexturedRect(64, 64, 64, 64)
	end
	-- TODO: End of 'Remove this, for testing only'
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
