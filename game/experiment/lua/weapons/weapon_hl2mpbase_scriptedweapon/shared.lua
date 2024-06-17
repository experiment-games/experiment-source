--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

SWEP.PrintName        = "#HL2_357Handgun"
SWEP.ViewModel        = "models/weapons/v_357.mdl"
SWEP.WorldModel      = "models/weapons/w_357.mdl"
SWEP.AnimationPrefix      = "python"
SWEP.InventorySlot           = 1
SWEP.InventorySlotPosition  = 1

SWEP.MaxClip        = 6
SWEP.MaxClip2       = -1
SWEP.DefaultClip     = 6
SWEP.DefaultClip2    = -1
SWEP.PrimaryAmmo     = "357"
SWEP.SecondaryAmmo   = "None"

SWEP.Weight           = 7
SWEP.WeaponFlags       = 0

SWEP.Damage           = 75

SWEP.SoundData        =
{
	empty       = "Weapon_Pistol.Empty",
	single_shot = "Weapon_357.Single"
}

SWEP.ShowUsageHint    = 0
SWEP.AutoSwitchTo     = 1
SWEP.AutoSwitchFrom   = 1
SWEP.BuiltRightHanded = 1
SWEP.AllowFlipping    = 1
SWEP.MeleeWeapon      = 0

-- TODO; implement Activity enum library!!
SWEP.m_acttable       =
{
	{ 1048, 977, false },
	{ 1049, 979, false },

	{ 1058, 978, false },
	{ 1061, 980, false },

	{ 1073, 981, false },
	{ 1077, 981, false },

	{ 1090, 982, false },
	{ 1093, 982, false },

	{ 1064, 983, false },
}

function SWEP:Initialize()
	self.m_bReloadsSingly = false
	self.m_bFiresUnderwater = false
end

function SWEP:Precache()
end

function SWEP:PrimaryAttack()
	-- Only the player fires this way so we can cast
	local client = self:GetOwner()

	if (ToBaseEntity(client) == NULL) then
		return
	end

	if (self.m_iClip1 <= 0) then
		if (not self.m_bFireOnEmpty) then
			self:Reload()
		else
			self:WeaponSound(0)
			self.m_flNextPrimaryAttack = 0.15
		end

		return
	end

	self:WeaponSound(1)
	client:DoMuzzleFlash()

	self:SendWeaponAnim(180)
	client:SetAnimation(5)
	ToPlayer(client):DoAnimationEvent(0)

	self.m_flNextPrimaryAttack   = Globals.curtime() + 0.75
	self.m_flNextSecondaryAttack = Globals.curtime() + 0.75

	self.m_iClip1                = self.m_iClip1 - 1

	local vecSrc                 = client:Weapon_ShootPosition()
	local vecAiming              = client:GetAutoaimVector(0.08715574274766)

	local info                   = { m_iShots = 1, m_vecSrc = vecSrc, m_vecDirShooting = vecAiming, m_vecSpread =
	vec3_origin, m_flDistance = MAX_TRACE_LENGTH, m_iAmmoType = self.m_iPrimaryAmmoType }
	info.m_pAttacker             = client

	-- Fire the bullets, and force the first shot to be perfectly accuracy
	client:FireBullets(info)

	--Disorient the player
	local angles = client:GetLocalAngles()

	angles.x = angles.x + random.RandomInt(-1, 1)
	angles.y = angles.y + random.RandomInt(-1, 1)
	angles.z = 0

	if not CLIENT then
		client:SnapEyeAngles(angles)
	end

	client:ViewPunch(Angle(-8, random.RandomFloat(-2, 2), 0))

	if (self.m_iClip1 == 0 and client:GetAmmoCount(self.m_iPrimaryAmmoType) <= 0) then
		-- HEV suit - indicate out of ammo condition
		client:SetSuitUpdate("!HEV_AMO0", 0, 0)
	end
end

function SWEP:SecondaryAttack()
end

function SWEP:Reload()
	local fRet = self:DefaultReload(self:GetMaxClip1(), self:GetMaxClip2(), 182)
	if (fRet) then
		--		self:WeaponSound( 6 )
		ToPlayer(self:GetOwner()):DoAnimationEvent(3)
	end
	return fRet
end

function SWEP:Think()
end

function SWEP:CanHolster()
end

function SWEP:Deploy()
end

function SWEP:GetDrawActivity()
	return 171
end

function SWEP:Holster(pSwitchingTo)
end

function SWEP:ItemPostFrame()
end

function SWEP:ItemBusyFrame()
end

function SWEP:DoImpactEffect()
end
