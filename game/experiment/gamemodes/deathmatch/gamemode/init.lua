--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

include("shared.lua")

local PLAYER_SOUNDS_CITIZEN = 0
local PLAYER_SOUNDS_COMBINESOLDIER = 1
local PLAYER_SOUNDS_METROPOLICE = 2

function GM:AddLevelDesignerPlacedObject(pEntity)
end

function GM:AllowDamage(pVictim, info)
end

function GM:CanEnterVehicle(client, pVehicle, nRole)
end

function GM:CanHavePlayerItem(client, pItem)
	if (cvars.FindVar("mp_weaponstay"):GetInt() > 0) then
		if (client:Weapon_OwnsThisType(pItem:GetClassname(), pItem:GetSubType())) then
			return false
		end
	end
end

function GM:CanPlayerHearPlayer(pListener, pTalker, bProximity)
end

function GM:CheatImpulseCommands(client, iImpulse)
end

function GM:CheckChatForReadySignal(client, chatmsg)
end

function GM:CleanUpMap()
end

function GM:ClientConnected(pEntity, pszName, pszAddress, reject, maxrejectlen)
end

function GM:ClientDisconnected(pClient)
end

function GM:FlItemRespawnTime(pItem)
	return cvars.FindVar("sv_hl2mp_item_respawn_time"):GetFloat()
end

function GM:FlPlayerFallDamage(client)
end

function GM:FlPlayerSpawnTime(client)
end

function GM:FPlayerCanRespawn(client)
end

function GM:FPlayerCanTakeDamage(client, attacker)
end

function GM:FShouldSwitchWeapon(client, targetWeapon)
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

function GM:Host_Say(client, p, isTeamOnly)
end

function GM:InitHUD(client)
end

function GM:ItemShouldRespawn(pItem)
end

function GM:LevelInit(strMapName, strMapEntities, strOldLevel, strLandmarkName, loadGame, background)
end

function GM:NetworkIDValidated(strUserName, strNetworkID)
end

function GM:PlayerCanHearChat(pListener, pSpeaker)
end

function GM:PlayerCanPickupObject(pObject, massLimit, sizeLimit)
end

function GM:PlayerDeathSound(info)
end

function GM:PlayerDeathThink(client)
end

function GM:PlayerEntSelectSpawnPoint(pHL2MPPlayer)
end

function GM:PlayerGotItem(client, pItem)
end

function GM:PlayerInitialSpawn(client)
end

function GM:PlayerPickupObject(pHL2MPPlayer, pObject, bLimitMassAndSize)
	return false
end

function GM:PlayerSpawn(client)
end

function GM:PlayerThink(client)
end

function GM:RemoveLevelDesignerPlacedObject(pEntity)
end

function GM:RestartGame()
end

function GM:ServerActivate(edictCount, clientMax)
end

function GM:ShouldHideServer()
end

function GM:VecItemRespawnSpot(pItem)
end

function GM:VecItemRespawnAngles(pItem)
end

function GM:WeaponShouldRespawn(targetWeapon)
end

function GM:Weapon_Equip(client, targetWeapon)
end
