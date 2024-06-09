_G.concommand = require("concommands")
_G.cvars = require("cvars")
_G.scripted_ents = require("entities")
_G.gamemode = require("gamemodes")
_G.hook = require("hooks")
_G.weapons = require("weapons")

AddCSLuaFile = sendfile

unpack = unpack or table.unpack

MsgC = ConDColorMsg

PrintTable = table.Print
LocalPlayer = UTIL.GetLocalPlayer

if (CLIENT) then
	surface.SetDrawColor = surface.DrawSetColor
    surface.DrawRect = surface.DrawFilledRect
elseif (SERVER) then
	resource = resources
end

-- TODO: The rest of the Garry's Mod compatibility layer
