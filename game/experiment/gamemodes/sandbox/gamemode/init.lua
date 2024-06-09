--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

include("shared.lua")
sendfile("shared.lua")
sendfile("cl_init.lua")
sendfile("in_main.lua")
sendfile("scripted_controls/buildmenu.lua")

local tSpawnPointClassnames = {
	"info_player_deathmatch",
	"info_player_combine",
	"info_player_rebel",
	"info_player_terrorist",
	"info_player_counterterrorist",
	"info_player_axis",
	"info_player_allies",
	"info_player_start"
}

function GM:AddLevelDesignerPlacedObject(pEntity)
	return false
end

function GM:GiveDefaultItems(client)
	self.BaseClass.GiveDefaultItems(self, client)

	-- client:GiveNamedItem("weapon_test")
	-- client:Weapon_Switch(client:Weapon_OwnsThisType("weapon_test"))
end

function GM:ItemShouldRespawn(pItem)
	pItem:AddSpawnFlags(2 ^ 30)
	-- return 6
end

function GM:PlayerEntSelectSpawnPoint(pHL2MPPlayer)
	local tSpawnPoints = {}
	local pSpot = NULL
	for _, classname in ipairs(tSpawnPointClassnames) do
		pSpot = gEntList.FindEntityByClassname(NULL, classname)
		while (pSpot ~= NULL) do
			table.insert(tSpawnPoints, pSpot)
			pSpot = gEntList.FindEntityByClassname(pSpot, classname)
		end
	end
	return tSpawnPoints[math.random(1, #tSpawnPoints)]
end

function GM:PlayerPickupObject(pHL2MPPlayer, pObject, bLimitMassAndSize)
end
