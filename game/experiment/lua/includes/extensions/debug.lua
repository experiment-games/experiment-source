require("debug")

debug.ColorClient = Color(255, 213, 107)
debug.ColorServer = Color(107, 186, 255)

local errorPrefix = "[SERVER] "
local errorColor = debug.ColorServer

if (CLIENT) then
	errorPrefix = "[CLIENT] "
	errorColor = debug.ColorClient
end

function debug.PrintError(message)
	message = debug.traceback(message or "<error without message>", 2)
    debug.ConDColorMsg(errorColor, errorPrefix .. message .. "\n")
	LuaLogToFile(message .. "\n")
end
