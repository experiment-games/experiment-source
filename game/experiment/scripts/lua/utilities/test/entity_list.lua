--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

if (not SERVER) then
	return
end

local pEntity = Entities.FindByClass("prop_physics*")

while (pEntity ~= NULL) do
	pEntity:SetRenderColor(255, 0, 0, 255)
	pEntity = Entities.FindByClass("prop_physics*", pEntity)
end
