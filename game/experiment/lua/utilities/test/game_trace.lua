--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local MASK_SHOT = _E.MASK.SHOT
local COLLISION_GROUP_NONE = 0

local client = Util.PlayerByIndex(1)

if (client == NULL) then
	return
end

local vForward = Vector()
client:EyeVectors(vForward)

local vecEye = client:EyePosition()
local tr = trace_t()
Util.TraceLine(vecEye, vecEye + vForward * 10000, MASK_SHOT, client, COLLISION_GROUP_NONE, tr)
MsgN(tostring(tr))
local pEntity = tr.m_pEnt
MsgN(tostring(pEntity) .. " (" .. (not CLIENT and pEntity:GetEntityName() or "") .. ")")
if (pEntity ~= NULL) then
	MsgN(tostring(pEntity:GetModelName()))
	pEntity:SetRenderColor(255, 0, 0, 255)
end
