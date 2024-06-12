include("shared.lua")

sendfile("shared.lua")
sendfile("cl_init.lua")
sendfile("enumerations.lua")
sendfile("util.lua")
sendfile("util/sql.lua")

AddCSLuaFile = sendfile

LocalPlayer = UTIL.GetLocalPlayer
resource = resources
