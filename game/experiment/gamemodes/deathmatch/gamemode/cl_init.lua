--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

include("shared.lua")

local AngleVectors = mathlib.AngleVectors

function GM:ActivateClientUI()
end

function GM:AdjustEngineViewport( x, y, width, height )
end

function GM:CanShowSpeakerLabels()
end

function GM:CreateDefaultPanels()
end

function GM:DrawHeadLabels( pPlayer )
end

function GM:GetPlayerTextColor( entindex, r, g, b )
end

function GM:HideClientUI()
end

function GM:HudElementShouldDraw( pElementName )
end

function GM:HudViewportPaint()
end

function GM:KeyInput( down, keynum, pszCurrentBinding )
end

function GM:LevelInitPreEntity()
end

function GM:LevelInitPostEntity()
end

function GM:OnScreenSizeChanged( iOldWide, iOldTall )
end

function GM:PlayerUpdateFlashlight(client, position, vecForward, vecRight, vecUp, nDistance)
	local vm = client:GetViewModel()

	if (vm == NULL) then
		return
	end

	local iAttachment = vm:LookupAttachment("muzzle")

	if (iAttachment < 0) then
		return
	end

	local vecOrigin = Vector()

	--Tony; EyeAngles will return proper whether it's local player or not.
	local eyeAngles = client:EyeAngles()

	vm:GetAttachment(iAttachment, vecOrigin, eyeAngles)

	local vForward = Vector()
	AngleVectors(eyeAngles, vecForward, vecRight, vecUp)
	position = vecOrigin

	return position, vecForward, vecRight, vecUp, nDistance
end

function GM:ShouldDrawCrosshair()
end

function GM:ShouldDrawDetailObjects()
end

function GM:ShouldDrawEntity( pEnt )
end

function GM:ShouldDrawFog()
end

function GM:ShouldDrawLocalPlayer()
end

function GM:ShouldDrawParticles()
end

function GM:ShouldDrawViewModel()
end
