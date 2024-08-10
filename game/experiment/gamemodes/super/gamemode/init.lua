Include("shared.lua")

function GM:AddLevelDesignerPlacedObject(entity)
end

function GM:AllowDamage(victim, info)
end

function GM:CanEnterVehicle(client, vehicle, role)
end

function GM:CanHavePlayerItem(client, item)
end

function GM:CanPlayerHearPlayer(listener, talker, isNearby)
end

function GM:CheatImpulseCommands(client, impulse)
end

function GM:CheckChatForReadySignal(client, chatmsg)
end

function GM:CleanUpMap()
end

function GM:ClientConnected(entity, name, address, reject, maxRejectLength)
end

function GM:ClientDisconnected(client)
end

function GM:FlItemRespawnTime(item)
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
	client:GiveNamedItem("weapon_physcannon")

	local defaultWeaponName = Engine.GetClientConVarValue(Engine.IndexOfEdict(client), "cl_defaultweapon")
	local defaultWeapon = client:Weapon_OwnsThisType(defaultWeaponName)

	if (ToBaseEntity(defaultWeapon) ~= NULL) then
		client:Weapon_Switch(defaultWeapon)
	else
		client:Weapon_Switch(client:Weapon_OwnsThisType("weapon_physcannon"))
	end
end

function GM:PlayerSay(client, p, isTeamOnly)
end

function GM:InitHUD(client)
end

function GM:ItemShouldRespawn(item)
end

function GM:LevelInit(mapName, mapEntities, oldLevel, landmarkName, loadGame, background)
end

function GM:NetworkIDValidated(userName, networkID)
end

function GM:PlayerCanHearChat(listener, speaker)
end

function GM:PlayerCanPickupObject(object, massLimit, sizeLimit)
end

function GM:PlayerDeathSound(info)
end

function GM:PlayerDeathThink(client)
end

function GM:PlayerEntSelectSpawnPoint(client)
end

function GM:PlayerGotItem(client, item)
end

function GM:PlayerInitialSpawn(client)
end

function GM:PlayerPickupObject(client, object, isMassAndSizeLimited)
	return false
end

function GM:PlayerSpawn(client)
end

function GM:PlayerThink(client)
end

function GM:RemoveLevelDesignerPlacedObject(entity)
end

function GM:RestartGame()
end

function GM:ServerActivate(edictCount, clientMax)
end

function GM:ShouldHideServer()
end

function GM:VecItemRespawnSpot(item)
end

function GM:VecItemRespawnAngles(item)
end

function GM:WeaponShouldRespawn(targetWeapon)
end

function GM:Weapon_Equip(client, targetWeapon)
end
