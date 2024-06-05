include("shared.lua")

function GM:AddLevelDesignerPlacedObject(pEntity)
end

function GM:AllowDamage(pVictim, info)
end

function GM:CanEnterVehicle(client, pVehicle, nRole)
end

function GM:CanHavePlayerItem(client, pItem)
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
