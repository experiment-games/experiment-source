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
local GetInSphere = Entities.GetInSphere
local MAX_ENTITYARRAY = 1024
local SPHERE_RADIUS_OUTER = 128
local SPHERE_RADIUS_INNER = 64

Hooks.Add("PlayerThink", "Utilities.EntityIn*", function(client)
	vecOrigin = client:GetPosition()
	pList = GetInSphere(MAX_ENTITYARRAY, vecOrigin, SPHERE_RADIUS_OUTER, 0)
	for _, pEntity in pairs(pList) do
		if (pEntity ~= NULL) then
			pEntity:SetRenderColor(255, 255, 255, 255)
		end
	end
	pList = GetInSphere(MAX_ENTITYARRAY, vecOrigin, SPHERE_RADIUS_INNER, 0)
	for _, pEntity in pairs(pList) do
		if (pEntity ~= NULL) then
			pEntity:SetRenderColor(0, 255, 0, 255)
		end
	end
end)
