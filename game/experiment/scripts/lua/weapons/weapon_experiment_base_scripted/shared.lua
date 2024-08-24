SWEP.PrintName             = "#HL2_357Handgun"
SWEP.ViewModel             = "models/weapons/v_pistol.mdl"
SWEP.WorldModel            = "models/weapons/w_357.mdl"
SWEP.AnimationPrefix       = "python"
SWEP.InventorySlot         = 1
SWEP.InventorySlotPosition = 1

SWEP.MaxClip               = 6
SWEP.MaxClip2              = -1
SWEP.DefaultClip           = 6
SWEP.DefaultClip2          = -1
SWEP.PrimaryAmmo           = "Pistol"
SWEP.SecondaryAmmo         = "None"

SWEP.Weight                = 7
SWEP.WeaponFlags           = 0

SWEP.Damage                = 75

SWEP.SoundData             =
{
	empty       = "Weapon_Pistol.Empty",
	single_shot = "Weapon_357.Single"
}

SWEP.ShowUsageHint         = 0
SWEP.AutoSwitchTo          = 1
SWEP.AutoSwitchFrom        = 1
SWEP.BuiltRightHanded      = 1
SWEP.AllowFlipping         = 1
SWEP.MeleeWeapon           = 0

-- TODO; implement Activity enum library!!
SWEP.m_acttable            =
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
	-- TODO: implement these
	-- self.m_bReloadsSingly = false
	-- self.m_bFiresUnderwater = false
end

function SWEP:Precache()
end

--[[
	Here for Gmod compatibility
--]]
function SWEP:ShootEffects()
	local owner = self:GetOwner()

	self:SendWeaponAnimation(_E.ACTIVITY.ACT_VM_PRIMARYATTACK)
	owner:DoMuzzleFlash()
	owner:SetAnimation(_E.PLAYER_ANIMATION.ATTACK1)
end

function SWEP:ShootBullet(damage, amountOfBullets, spread, ammoType, force, tracer)
	local owner = self:GetOwner()
	local bulletInfo = {
		Src = owner:GetWeaponShootPosition(),
		Dir = owner:GetAimVector(0.08715574274766),
		Spread = Vector(spread.x, spread.y, 0),

		Num = amountOfBullets,
		AmmoType = ammoType or self.Primary.Ammo,
		Tracer = tracer or 5,

		Damage = damage,
		Force = force or 1,
	}

	owner:FireBullets(bulletInfo)

	self:ShootEffects()
end

function SWEP:OnRemove()
end

function SWEP:OwnerChanged()
end

function SWEP:Ammo1()
	return self:GetOwner():GetAmmoCount(self:GetPrimaryAmmoType())
end

function SWEP:Ammo2()
	return self:GetOwner():GetAmmoCount(self:GetSecondaryAmmoType())
end

function SWEP:DoImpactEffect(trace, damageType)
	return false
end

function SWEP:CanPrimaryAttack()
	if (self:Clip1() <= 0) then -- TODO: implement: and not self.m_bFireOnEmpty) then
		return false
	end

	return true
end

function SWEP:PrimaryAttack()
	if (not self:CanPrimaryAttack()) then
		self:EmitSound("Weapon_Pistol.Empty")
		self:SetNextPrimaryFire(Engines.GetCurrentTime() + 0.2)
		self:Reload()

		return
	end

	local owner = self:GetOwner()

	if (owner == NULL) then
		return
	end

	self:EmitWeaponSound(1)

	owner:DoAnimationEvent(_E.PLAYER_ANIMATION_EVENT.PLAYERANIMEVENT_ATTACK_PRIMARY)

	self:SetNextPrimaryFire(Engines.GetCurrentTime() + 0.75)
	self:SetNextSecondaryFire(Engines.GetCurrentTime() + 0.75)

	self:TakePrimaryAmmo(1)

	self:ShootBullet(self.Damage, 1, 0.01, self.Primary.Ammo)

	-- Disorient the player
	local angles = owner:GetLocalAngles()

	angles.x = angles.x + Randoms.RandomInt(-1, 1)
	angles.y = angles.y + Randoms.RandomInt(-1, 1)
	angles.z = 0

	if (not CLIENT) then
		owner:SnapEyeAngles(angles)
	end

	owner:ViewPunch(Angles.Create(-8, Randoms.RandomFloat(-2, 2), 0))

	if (self:Clip1() == 0 and owner:GetAmmoCount(self:GetPrimaryAmmoType()) <= 0) then
		-- HEV suit - indicate out of ammo condition
		owner:SetSuitUpdate("!HEV_AMO0", 0, 0)
	end
end

function SWEP:SecondaryAttack()
end

function SWEP:TakePrimaryAmmo(num)
	if (self:Clip1() <= 0) then
		if (self:Ammo1() <= 0) then
			return
		end
		self:GetOwner():RemoveAmmo(num, self:GetPrimaryAmmoType())

		return
	end

	self:SetClip1(self:Clip1() - num)
end

function SWEP:TakeSecondaryAmmo(num)
	if (self:Clip2() <= 0) then
		if (self:Ammo2() <= 0) then
			return
		end

		self:GetOwner():RemoveAmmo(num, self:GetSecondaryAmmoType())

		return
	end

	self:SetClip2(self:Clip2() - num)
end

function SWEP:Reload()
	local fRet = self:DefaultReload(self:GetMaxClip1(), self:GetMaxClip2(), _E.ACTIVITY.ACT_VM_RELOAD)

	if (fRet) then
		--		self:WeaponSound( 6 )
		self:GetOwner():DoAnimationEvent(_E.PLAYER_ANIMATION_EVENT.PLAYERANIMEVENT_RELOAD)
	end

	return fRet
end

function SWEP:Think()
end

function SWEP:CanHolster()
end

function SWEP:Deploy()
	return true
end

function SWEP:GetDrawActivity()
	return _E.ACTIVITY.ACT_VM_DRAW
end

function SWEP:Holster(pSwitchingTo)
	return true
end

function SWEP:ItemPostFrame()
end

function SWEP:ItemBusyFrame()
end

function SWEP:DoImpactEffect()
end
