_G.concommand = require("concommand")
_G.cvars = require("cvar")
_G.scripted_ents = require("entity")
_G.gamemode = require("gamemode")
_G.hook = require("hook")
_G.weapons = require("weapon")

unpack = unpack or table.unpack

CLIENT = _CLIENT
SERVER = _SERVER
GAMEMODE = _GAMEMODE

MsgC = ConDColorMsg

PrintTable = table.Print
LocalPlayer = UTIL.GetLocalPlayer

-- TODO: The rest of the Garry's Mod compatibility layer
