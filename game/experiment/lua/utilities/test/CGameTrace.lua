--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local MASK_SHOT = _E.MASK.SHOT
local COLLISION_GROUP_NONE = 0

local pPlayer = UTIL.PlayerByIndex(1)

if (pPlayer == NULL) then
	return
end

local vForward = pPlayer:EyeVectors()

local vecEye = pPlayer:EyePosition()
local tr = trace_t()
UTIL.TraceLine(vecEye, vecEye + vForward * 10000, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, tr)
MsgN(tr)
local pEntity = tr.m_pEnt
MsgN(tostring(pEntity) .. " (" .. (not _CLIENT and pEntity:GetEntityName() or "") .. ")")
if (pEntity ~= NULL) then
	MsgN(pEntity:GetModelName())
	pEntity:SetRenderColor(255, 0, 0, 255)
end
