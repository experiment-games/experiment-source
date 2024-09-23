--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

GM.Name      = "Sandbox"
GM.Homepage  = "http://www.andrewmcwatters.com/hl2sb/"
GM.Developer = "Team Sandbox"
GM.Manual    = nil

function GM:Initialize()
	if (not CLIENT) then
		Engines.ServerCommand("mp_falldamage 1\n")
		Engines.ServerCommand("mp_flashlight 1\n")
	else
		-- Andrew; not the right place to put this, but oh well.
		self:CreateDefaultPanels()
	end
end

function GM:FlWeaponRespawnTime(targetWeapon)
	return -1
end

function GM:FlWeaponTryRespawn(targetWeapon)
	return 0
end

function GM:PlayerPlayStepSound(client, vecOrigin, psurface, fvol, force)
	if (Engines.GetMaxClients() > 1 and ConsoleVariables.FindVariable("sv_footsteps"):GetFloat() == 0) then
		return false
	end

	if (CLIENT) then
		-- during prediction play footstep sounds only once
		if (Predictions.InPrediction() and not Predictions.IsFirstTimePredicted()) then
			return false
		end
	end

	if (not psurface) then
		return false
	end

	local nSide = client:GetPlayerLocalData().m_nStepside
    local stepSoundName = nSide ~= 0 and psurface.sounds.stepleft or psurface.sounds.stepright

	if (not stepSoundName) then
		return false
	end

	client:SetPlayerLocalData("m_nStepside", nSide == 0 and 1 or 0)

	local params

	assert(nSide == 0 or nSide == 1)

	if (client:GetStepSoundCache()[nSide].m_usSoundNameIndex == stepSoundName) then
		params = client:GetStepSoundCache()[nSide].m_SoundParameters
	else
		local physprops = MoveHelpers.Create():GetSurfaceProperties()
		local pSoundName = physprops:GetString(stepSoundName)
		local b
		b, params = _R.Entity.GetParametersForSound(pSoundName)
		if (not b) then
			return false
		end

		-- Only cache if there's one option.  Otherwise we'd never here any other sounds
		if (params.count == 1) then
			client:SetStepSoundCache(nSide, "m_usSoundNameIndex", stepSoundName)
			client:SetStepSoundCache(nSide, "m_SoundParameters", params)
		end
	end

	local filter = RecipientFilters.Create()
	filter:AddRecipientsByPAS(vecOrigin)

	if not CLIENT then
		-- in MP, server removes all players in the vecOrigin's PVS, these players generate the footsteps client side
		if (Engines.GetMaxClients() > 1) then
			filter:RemoveRecipientsByPVS(vecOrigin)
		end
	end

	local ep = {}
	ep.m_nChannel = 4; -- CHAN_BODY
	ep.m_pSoundName = params.soundname
	ep.m_flVolume = fvol
	ep.m_SoundLevel = params.soundlevel
	ep.m_nFlags = 0
	ep.m_nPitch = params.pitch
	ep.m_pOrigin = vecOrigin

	_R.Entity.EmitSound(filter, client:GetEntityIndex(), ep)
	return false
end

function GM:WeaponShouldRespawn(pItem)
	pItem:AddSpawnFlags(2 ^ 30)
	-- return 2
end
