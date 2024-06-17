if (not CLIENT) then
	return
end

require("UserMessages")

UserMessages.Hooks = UserMessages.Hooks or {}

function UserMessages.Hook(messageName, func, ...)
	UserMessages.Hooks[messageName] = {}

	UserMessages.Hooks[messageName].Function = func
	UserMessages.Hooks[messageName].PreArgs = { ... }
end

function UserMessages.OnMessageReceived(messageName, message)
	if (UserMessages.Hooks[messageName]) then
		UserMessages.Hooks[messageName].Function(message, table.unpack(UserMessages.Hooks[messageName].PreArgs))
		return
	end

	print("Warning: Unhandled usermessage '" .. messageName .. "'")
end
