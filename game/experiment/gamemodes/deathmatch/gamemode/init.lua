--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

Include("shared.lua")

local PLAYER_SOUNDS_CITIZEN = 0
local PLAYER_SOUNDS_COMBINESOLDIER = 1
local PLAYER_SOUNDS_METROPOLICE = 2

function GM:CanHavePlayerItem(client, item)
	if (ConsoleVariables.FindVar("mp_weaponstay"):GetInt() > 0) then
		if (client:Weapon_OwnsThisType(item:GetClass(), item:GetSubType())) then
			return false
		end
	end

	return true
end

function GM:FlItemRespawnTime(pItem)
	return ConsoleVariables.FindVar("sv_experiment_item_respawn_time"):GetFloat()
end

function GM:GiveDefaultItems(client)
	client:EquipSuit()

	-- TODO: Just do client:GiveAmmo(255, "Pistol") etc.
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

	local defaultWeaponName = Engines.GetClientConsoleVariableValue(Engines.IndexOfEdict(client), "cl_defaultweapon")
	local defaultWeapon = client:Weapon_OwnsThisType(defaultWeaponName)

	if (ToBaseEntity(defaultWeapon) ~= NULL) then
		client:Weapon_Switch(defaultWeapon)
	else
		client:Weapon_Switch(client:Weapon_OwnsThisType("weapon_physcannon"))
	end
end

function GM:PlayerPickupObject(client, object, isLimitedByMassAndSize)
	return self.BaseClass.PlayerPickupObject(self, client, object, isLimitedByMassAndSize)
end
