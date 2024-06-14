-- lua_dofile utilities/test/luasocket.lua
local socket = require("luasocket")
local IP = "127.0.0.1" -- TODO: game server IP here
local PORT = 12345

local debug = function(...)
	print("[LuaSocketTest]", ...)
end

if (SERVER) then
	local server = socket.tcp()
	local success, err = server:bind(IP, PORT)

	if (not success) then
		debug("Error binding server:", err)
		return
	end

	success, err = server:listen()

	if (not success) then
		debug("Error listening on server:", err)
		return
	end

	local ip, port = server:getsockname()
	debug("Server listening on " .. tostring(ip) .. ":" .. tostring(port))

	-- Disable blocking on the server socket
	server:settimeout(0)

	local clients = {}

	local function handleClient(client)
		-- Disable blocking on the client socket
		client:settimeout(0)

		local line, err = client:receive()

		if (line) then
			debug("Received from client: " .. line)
			local num = tonumber(line:match("#(%d+)")) + 1
			local message = "Hello, client! #" .. num .. "\n"
			client:send(message)
		elseif (err and err ~= "timeout") then
			-- Remove the client from the list if it's disconnected
			for i, c in ipairs(clients) do
				if (c == client) then
					table.remove(clients, i)
					break
				end
			end

			client:close()
			debug("Client disconnected")
		end
	end

	local function serverUpdate()
		-- Accept any new clients
		local newClient = server:accept()

		if (newClient) then
			table.insert(clients, newClient)
			debug("New client connected", newClient:getpeername())
		end

		-- Handle existing clients
		for _, client in ipairs(clients) do
			handleClient(client)
		end

		-- Sleep for a short time to prevent high CPU usage
		-- socket.sleep(0.01)
	end

	hooks.Add("Think", "LuaSocketTestUpdate", serverUpdate)
end

if (CLIENT) then
	local client = assert(socket.tcp())
	client:settimeout(0)
	local connected = false

	local function clientUpdate()
		if (not connected) then
			client:connect(IP, PORT)

			-- Send a message to the server
			local message = "Hello, server! #1"
			debug("Sent to server: " .. message)
			client:send(message .. "\n")

			connected = true
		end

		-- Receive the response from the server
		local response, err = client:receive()

		if (response) then
			debug("Received from server: " .. response)
			-- let's echo back, getting the number after #, incrementing it by 1
			local num = tonumber(response:match("#(%d+)")) + 1
			local message = "Hello, server! #" .. num .. "\n"
			debug("Sent to server: " .. message)
			client:send(message)
			-- client:close()
			-- hooks.Remove("Think", "LuaSocketTestClientUpdate")
		elseif (err and err ~= "timeout") then
			debug("Error receiving response: " .. err)
			client:close()
			hooks.Remove("Think", "LuaSocketTestClientUpdate")
		end
	end

	hooks.Add("Think", "LuaSocketTestClientUpdate", clientUpdate)
end
