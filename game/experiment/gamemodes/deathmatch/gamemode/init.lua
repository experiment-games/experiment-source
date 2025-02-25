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
	if (ConsoleVariables.FindVariable("mp_weaponstay"):GetInt() > 0) then
		if (client:OwnsWeaponOfType(item:GetClass(), item:GetSubType())) then
			return false
		end
	end

	return true
end

function GM:FlItemRespawnTime(pItem)
	return ConsoleVariables.FindVariable("sv_experiment_item_respawn_time"):GetFloat()
end

function GM:GiveDefaultItems(client)
	client:EquipSuit()

	-- TODO: Just do client:GiveAmmo(255, "Pistol") etc.
	_R.Player.GiveAmmo(client, 255, "Pistol")
	_R.Player.GiveAmmo(client, 45, "SMG1")
	_R.Player.GiveAmmo(client, 1, "grenade")
	_R.Player.GiveAmmo(client, 6, "Buckshot")
	_R.Player.GiveAmmo(client, 6, "357")

	if (client:GetPlayerModelType() == PLAYER_SOUNDS_METROPOLICE or client:GetPlayerModelType() == PLAYER_SOUNDS_COMBINESOLDIER) then
		client:GiveItem("weapon_stunstick")
	elseif (client:GetPlayerModelType() == PLAYER_SOUNDS_CITIZEN) then
		client:GiveItem("weapon_crowbar")
	end

	client:GiveItem("weapon_pistol")
	client:GiveItem("weapon_smg1")
	client:GiveItem("weapon_frag")
	client:GiveItem("weapon_physcannon")

	local defaultWeaponName = Engines.GetClientConsoleVariableValue(Engines.IndexOfEdict(client), "cl_defaultweapon")
	local defaultWeapon = client:OwnsWeaponOfType(defaultWeaponName)

	if (defaultWeapon ~= NULL) then
		client:SwitchWeapon(defaultWeapon)
	else
		client:SwitchWeapon(client:OwnsWeaponOfType("weapon_physcannon"))
	end
end

function GM:PlayerPickupObject(client, object, isLimitedByMassAndSize)
	return self.BaseClass.PlayerPickupObject(self, client, object, isLimitedByMassAndSize)
end
