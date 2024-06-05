require("debug")

local ConDColorMsg = dbg.ConDColorMsg

debug.ColorClient = Color(255, 213, 107)
debug.ColorServer = Color(107, 186, 255)

local errorPrefix = "[SERVER] "
local errorColor = debug.ColorServer

if (_CLIENT) then
	errorPrefix = "[CLIENT] "
	errorColor = debug.ColorClient
end

function debug.PrintError(msg)
	ConDColorMsg(errorColor, debug.traceback(errorPrefix .. tostring(msg), 2) .. "\n")
end
