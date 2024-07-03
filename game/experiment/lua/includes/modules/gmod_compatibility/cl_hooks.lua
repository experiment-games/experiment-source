-- TODO: Remove this, for testing only
testMat = Material("icon16/accept.png")
testTexture = surface.GetTextureID("vgui/gfx/vgui/crosshair")
-- lua_run_cl print(surface.GetTextureID(testMat:GetTexture("$basetexture")))

print(
	testMat,
	testMat:GetString("$basetexture"),
	testMat:GetTexture("$basetexture"),
	testMat:GetTexture("$basetexture"):GetActualHeight(),
	testMat:GetTexture("$basetexture"):GetActualWidth(),
	"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
)
-- TODO: End of 'Remove this, for testing only'

hook.Add("HudViewportPaint", "GModCompatibility.CallHUDPaint", function()
	hook.Run("HUDPaint")

	-- TODO: Remove this, for testing only
	surface.SetMaterial(testMat)
	surface.SetDrawColor(255, 255, 255, 255)
	surface.DrawTexturedRect(0, 0, 64, 64)

	surface.SetTexture(testTexture)
	surface.SetDrawColor(255, 255, 255, 255)
	surface.DrawTexturedRect(64, 64, 64, 64)
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
