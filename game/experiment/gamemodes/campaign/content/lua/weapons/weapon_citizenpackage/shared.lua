--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

SWEP.PrintName             = "#HL2_SMG1"
SWEP.ViewModel             = "models/weapons/w_package.mdl"
SWEP.WorldModel            = "models/weapons/w_package.mdl"
SWEP.AnimationPrefix       = "smg2"
SWEP.InventorySlot         = 2
SWEP.InventorySlotPosition = 0

SWEP.MaxClip               = -1
SWEP.MaxClip2              = -1

SWEP.DefaultClip          = -1
SWEP.DefaultClip2         = -1

SWEP.PrimaryAmmo          = "None"
SWEP.SecondaryAmmo        = "None"

SWEP.Weight                = 0
SWEP.WeaponFlags            = 0

SWEP.SoundData             =
{
}

SWEP.m_acttable            =
{
	{ 1, 307, false },
	{ 6, 308, false },
}

-------------------------------------------------------------------------------
-- Purpose:
-------------------------------------------------------------------------------
function SWEP:ItemPostFrame()
	-- Do nothing
end

-------------------------------------------------------------------------------
-- Purpose: Remove the citizen package if it's ever dropped
-------------------------------------------------------------------------------
function SWEP:Drop(vecVelocity)
	self:Drop(vecVelocity)
	if not CLIENT then
		UTIL.Remove(self)
	end
end
