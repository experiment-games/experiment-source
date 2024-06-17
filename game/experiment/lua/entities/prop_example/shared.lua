--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local Warning = debug.Warning
local Msg = debug.Msg

function ENT:Initialize()
	if (CLIENT) then
		return
	end

	local allowPrecache = self.IsPrecacheAllowed()

	self.SetAllowPrecache(true)
	self.PrecacheModel("models/props_junk/wood_crate001a.mdl")
	self:SetModel("models/props_junk/wood_crate001a.mdl")
	self.SetAllowPrecache(allowPrecache)

	local pPhysicsObject = self:VPhysicsInitNormal(6, 0, false)

	if (not pPhysicsObject) then
		self:SetSolid(0)
		self:SetMoveType(0)
		Warning("ERROR!: Can't create physics object for " .. self:GetModelName() .. "\n")
	end
end

function ENT:StartTouch(pEntity)
end

function ENT:Touch(pEntity)
end

function ENT:EndTouch(pEntity)
end

function ENT:VPhysicsUpdate(pPhysics)
end
