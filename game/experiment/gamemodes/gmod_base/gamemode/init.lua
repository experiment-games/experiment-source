include("shared.lua")

sendfile("shared.lua")
sendfile("cl_init.lua")
sendfile("enumerations.lua")

AddCSLuaFile = sendfile

LocalPlayer = UTIL.GetLocalPlayer
resource = resources
