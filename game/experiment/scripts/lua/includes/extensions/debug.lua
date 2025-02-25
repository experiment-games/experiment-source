require("debug")

local errorPrefix = "[SERVER] "

if (CLIENT) then
	errorPrefix = "[CLIENT] "
end

function debug.PrintError(message)
	message = debug.traceback(message or "<error without message>", 2)
    debug.PrintDebugColorMessage(debug.GetRealmColor(), errorPrefix .. message .. "\n")
	LuaLogToFile(message .. "\n")
end
