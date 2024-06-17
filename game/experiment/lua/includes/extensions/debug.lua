require("Debug")

local ConDColorMsg = dbg.ConDColorMsg

debug.ColorClient = Color(255, 213, 107)
debug.ColorServer = Color(107, 186, 255)

local errorPrefix = "[SERVER] "
local errorColor = debug.ColorServer

if (CLIENT) then
	errorPrefix = "[CLIENT] "
	errorColor = debug.ColorClient
end

function debug.PrintError(msg)
	msg = msg and (errorPrefix .. tostring(msg)) or nil
	ConDColorMsg(errorColor, debug.traceback(msg, 2) .. "\n")
end
