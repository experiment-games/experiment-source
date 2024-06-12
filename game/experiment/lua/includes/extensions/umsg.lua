require("umsg")

function umsg.Hook(messageName, func, ...)
	umsg.Hooks[messageName] = {}

	umsg.Hooks[messageName].Function = func
	umsg.Hooks[messageName].PreArgs = { ... }
end

function umsg.OnMessageReceived(messageName, message)
	if (umsg.Hooks[messageName]) then
		umsg.Hooks[messageName].Function(message, unpack(umsg.Hooks[messageName].PreArgs))
		return
	end

	print("Warning: Unhandled usermessage '" .. messageName .. "'")
end
