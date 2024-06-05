--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

hook.Add("PlayerTraceAttack", "PlayerTraceTeslaHitboxesAttack", function(pPlayer, info, vecDir, ptr)
	local pEntity = ptr.m_pEnt
	if (pEntity ~= NULL) then
		-- This is taken straight out of CNPC_Hunter::TeslaThink()
		local data = CEffectData()
		if (CLIENT) then
			data.m_hEntity = pEntity
		else
			data.m_nEntIndex = pEntity:entindex()
		end
		data.m_flMagnitude = 3
		data.m_flScale = 0.5
		DispatchEffect("TeslaHitboxes", data)
	end
end)
