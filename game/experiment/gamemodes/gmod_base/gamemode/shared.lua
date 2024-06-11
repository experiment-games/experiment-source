--[[
	Note that most code here belongs to Facepunch Studios. It has been
	modified and adapted to provide compatibility with Experiment.
--]]

GM.Name = "Garry's Mod Base"
GM.Homepage = ""
GM.Developer = "Experiment" -- Most code by Facepunch Studios

--[[
	All our libraries are plural, while with Garry's Mod they vary between plural and singular.
--]]
concommand = require("concommands")
scripted_ents = require("entities")
gamemode = require("gamemodes")
hook = require("hooks")
weapons = require("weapons")

--[[
	Libraries taken from Garry's Mod.
	TODO: Write own libraries.
--]]
matproxy = include("../modules/matproxy.lua")
menubar = include("../modules/menubar.lua")
notification = include("../modules/notification.lua")
numpad = include("../modules/numpad.lua")
player_manager = include("../modules/player_manager.lua")
presets = include("../modules/presets.lua")
properties = include("../modules/properties.lua")
saverestore = include("../modules/saverestore.lua")
-- scripted_ents = include("../modules/scripted_ents.lua") -- We have our own scripted_ents library.
search = include("../modules/search.lua")
spawnmenu = include("../modules/spawnmenu.lua")
team = include("../modules/team.lua")
undo = include("../modules/undo.lua")
usermessage = include("../modules/usermessage.lua")
utf8 = include("../modules/utf8.lua")
-- weapons = include("../modules/weapons.lua") -- We have our own weapons library.
widget = include("../modules/widget.lua")
ai_schedule = include("../modules/ai_schedule.lua")
ai_task = include("../modules/ai_task.lua")
baseclass = include("../modules/baseclass.lua")
cleanup = include("../modules/cleanup.lua")
-- concommand = include("../modules/concommand.lua") -- We have our own concommand library.
constraint = include("../modules/constraint.lua")
construct = include("../modules/construct.lua")
controlpanel = include("../modules/controlpanel.lua")
cookie = include("../modules/cookie.lua")
cvars = include("../modules/cvars.lua")
draw = include("../modules/draw.lua")
drive = include("../modules/drive.lua")
duplicator = include("../modules/duplicator.lua")
effects = include("../modules/effects.lua")
-- gamemode = include("../modules/gamemode.lua") -- We have our own gamemode library.
halo = include("../modules/halo.lua")
-- hook = include("../modules/hook.lua") -- We have our own hook library.
http = include("../modules/http.lua")
killicon = include("../modules/killicon.lua")
list = include("../modules/list.lua")
markup = include("../modules/markup.lua")

--[[
	Other shared functions.
--]]
unpack = unpack or table.unpack

MsgC = ConDColorMsg

PrintTable = table.Print
