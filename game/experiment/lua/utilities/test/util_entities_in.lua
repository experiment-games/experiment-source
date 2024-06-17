--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

-- Andrew; Here, I just declare locals in the order I use them, and store global
-- functions because we use them in a Think hook, which should cut some
-- execution time.
local vecOrigin
local count
local pList
local Util_EntitiesInSphere = Util.EntitiesInSphere
local MAX_ENTITYARRAY = 1024
local SPHERE_RADIUS_OUTER = 128
local pairs = pairs
local SPHERE_RADIUS_INNER = 64

hooks.Add("PlayerThink", "Util.EntityIn*", function(client)
	vecOrigin = client:GetAbsOrigin()
	count, pList = Util_EntitiesInSphere(MAX_ENTITYARRAY, vecOrigin, SPHERE_RADIUS_OUTER, 0)
	for _, pEntity in pairs(pList) do
		if (pEntity ~= NULL) then
			pEntity:SetRenderColor(255, 255, 255, 255)
		end
	end
	count, pList = Util_EntitiesInSphere(MAX_ENTITYARRAY, vecOrigin, SPHERE_RADIUS_INNER, 0)
	for _, pEntity in pairs(pList) do
		if (pEntity ~= NULL) then
			pEntity:SetRenderColor(0, 255, 0, 255)
		end
	end
end)
