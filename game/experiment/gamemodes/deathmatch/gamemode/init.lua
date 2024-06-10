--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

include("shared.lua")

local PLAYER_SOUNDS_CITIZEN = 0
local PLAYER_SOUNDS_COMBINESOLDIER = 1
local PLAYER_SOUNDS_METROPOLICE = 2

function GM:CanHavePlayerItem(client, pItem)
	if (cvars.FindVar("mp_weaponstay"):GetInt() > 0) then
		if (client:Weapon_OwnsThisType(pItem:GetClassname(), pItem:GetSubType())) then
			return false
		end
	end

	return true
end

function GM:FlItemRespawnTime(pItem)
	return cvars.FindVar("sv_hl2mp_item_respawn_time"):GetFloat()
end

function GM:GiveDefaultItems(client)
	client:EquipSuit()

	_R.CBasePlayer.GiveAmmo(client, 255, "Pistol")
	_R.CBasePlayer.GiveAmmo(client, 45, "SMG1")
	_R.CBasePlayer.GiveAmmo(client, 1, "grenade")
	_R.CBasePlayer.GiveAmmo(client, 6, "Buckshot")
	_R.CBasePlayer.GiveAmmo(client, 6, "357")

	if (client:GetPlayerModelType() == PLAYER_SOUNDS_METROPOLICE or client:GetPlayerModelType() == PLAYER_SOUNDS_COMBINESOLDIER) then
		client:GiveNamedItem("weapon_stunstick")
	elseif (client:GetPlayerModelType() == PLAYER_SOUNDS_CITIZEN) then
		client:GiveNamedItem("weapon_crowbar")
	end

	client:GiveNamedItem("weapon_pistol")
	client:GiveNamedItem("weapon_smg1")
	client:GiveNamedItem("weapon_frag")
	client:GiveNamedItem("weapon_physcannon")

	local szDefaultWeaponName = engine.GetClientConVarValue(engine.IndexOfEdict(client), "cl_defaultweapon")

	local pDefaultWeapon = client:Weapon_OwnsThisType(szDefaultWeaponName)

	if (ToBaseEntity(pDefaultWeapon) ~= NULL) then
		client:Weapon_Switch(pDefaultWeapon)
	else
		client:Weapon_Switch(client:Weapon_OwnsThisType("weapon_physcannon"))
	end
end

function GM:PlayerPickupObject(pHL2MPPlayer, pObject, bLimitMassAndSize)
	return self.BaseClass.PlayerPickupObject(self, pHL2MPPlayer, pObject, bLimitMassAndSize)
end
