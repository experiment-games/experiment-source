--[[
	TODO:
		- Add the other datatypes
		- Use string.pack and string.unpack for writing and reading data
		- Check if these are indeed all the datatypes:
			"b": signed byte (char)
			"B": unsigned byte (unsigned char)
			"h": signed short (16-bit)
			"H": unsigned short (16-bit)
			"l": signed long (32-bit)
			"L": unsigned long (32-bit)
			"j": lua_Integer (size varies by implementation, typically 64-bit)
			"J": lua_Unsigned (size varies by implementation, typically 64-bit)
			"T": size_t (variable size, typically 32 or 64-bit)
			"i" and "I": signed and unsigned integers with a specified size (e.g., "i4" for a 32-bit signed integer)
			"f": single-precision float (32-bit)
			"d": double-precision float (64-bit)
			"s": a string preceded by its length coded as an unsigned integer
			"c": a fixed-size string (e.g., "c10" for 10 bytes)
--]]

net = net or {}
local MODULE = net

local socket = require("luasocket")
local IP = "127.0.0.1" -- TODO: game server IP here
local PORT = 12345

-- Client only:
local localClient

-- Server only:
local localServer
local clients = {}

MODULE.registeredCallbacks = MODULE.registeredCallbacks or {}

local debug = function(...)
    print("[NetModuleTest]", ...)
end

-- Shows the data as byte numbers
local debugData = function(data)
    local bytes = {}

    for i = 1, #data do
        table.insert(bytes, data:byte(i))
    end

	debug("Data: " .. table.concat(bytes, ", "))
end

--[[
	Logic to setup the client and server sockets
	and handle incoming data.
--]]

if (CLIENT) then
    localClient = assert(socket.tcp())
    localClient:settimeout(0)
	local connected = false

	local function clientUpdate()
        if (not connected) then
            localClient:connect(IP, PORT)
            connected = true

            debug("Connected to server", localClient:getpeername())
        end

		-- Receive any incoming data from the server
		local bytes, err = localClient:receive()

		-- On failure, lets close and remove the think hook so we don't spam errors
		if (err and err ~= "timeout") then
			debug("Error receiving response: " .. err)

			-- localClient:close()
			-- hooks.Remove("Think", "__NetModuleTestClientUpdate")
			return
		end

		if (not bytes) then
            -- No data to read
			return
		end

		MODULE.HandleIncomingMessage(bytes, nil)
	end

	hooks.Add("Think", "__NetModuleClientUpdate", clientUpdate)
elseif (SERVER) then
	localServer = socket.tcp()
    local success, err = localServer:bind(IP, PORT)

	if (not success) then
		debug("Error binding server:", err)
		return
	end

	success, err = localServer:listen()

	if (not success) then
		debug("Error listening on server:", err)
		return
	end

	local ip, port = localServer:getsockname()
    debug("Server listening on " .. tostring(ip) .. ":" .. tostring(port))

	-- Disable blocking on the server socket
	localServer:settimeout(0)

	local function handleClient(client)
		-- Disable blocking on the client socket
		client:settimeout(0)

		local line, err = client:receive()

		if (err and err ~= "timeout") then
			-- Remove the client from the list if it's disconnected
			for i, c in ipairs(clients) do
				if (c == client) then
					table.remove(clients, i)
					break
				end
			end

			client:close()
			debug("Client disconnected", err)

            return
        elseif (err == "timeout") then
			-- No data to read
			return
		end

        if (not line) then
            return
        end

		MODULE.HandleIncomingMessage(line, client)
	end

	local function serverUpdate()
		-- Accept any new clients
		local newClient = localServer:accept()

		if (newClient) then
			table.insert(clients, newClient)
			debug("New client connected", newClient:getpeername())
		end

		-- Handle all clients
		for _, client in ipairs(clients) do
			handleClient(client)
		end

		-- Sleep for a short time to prevent high CPU usage
		-- socket.sleep(0.01)
	end

	hooks.Add("Think", "__NetModuleServerUpdate", serverUpdate)
end

--[[
	Helper functions
--]]
NET_TYPE_NIL = 0
NET_TYPE_NUMBER = 1
NET_TYPE_STRING = 2
NET_TYPE_TABLE = 3
NET_TYPE_BOOL = 4
NET_TYPE_FUNCTION = 5
NET_TYPE_USERDATA = 7

-- Maps the result of type() to a unique identifier
local typeIdMap = {
	["nil"] = NET_TYPE_NIL,
	["number"] = NET_TYPE_NUMBER,
	["string"] = NET_TYPE_STRING,
	["table"] = NET_TYPE_TABLE,
	["boolean"] = NET_TYPE_BOOL,
	["function"] = NET_TYPE_FUNCTION,
	["userdata"] = NET_TYPE_USERDATA
}

-- Reverse lookup table
local typeIdLookup = {}
for dataType, id in pairs(typeIdMap) do
	typeIdLookup[id] = dataType
end

local function toTypeId(data)
	return typeIdMap[type(data)]
end

local function fromTypeId(typeId)
    return typeIdLookup[typeId]
end

--- https://github.com/ToxicFrog/vstruct/blob/d691a55ab6010e89f2669a52fcf24ccbe111748c/frexp.lua
local abs,floor,log = math.abs,math.floor,math.log
local log2 = log(2)
local function frexp(x)
    if x == 0 then return 0.0, 0.0 end
    local e = floor(log(abs(x)) / log2)
    if e > 0 then
        -- Why not x / 2^e? Because for large-but-still-legal values of e this
        -- ends up rounding to inf and the wheels come off.
        x = x * 2 ^ -e
    else
        x = x / 2 ^ e
    end
    -- Normalize to the range [0.5,1)
    if abs(x) >= 1.0 then
        x, e = x / 2, e + 1
    end
    return x, e
end

--[[
	Reader Metatable
--]]
local READER = {}
READER.__index = READER

function READER.new(data)
    return setmetatable({
        data = data,
        position = 1
    }, READER)
end

function READER:ReadBit()
    local byte = self.data:byte(self.position)
    local bit = byte % 2 -- get the least significant bit
    self.position = self.position + 1
    return bit
end

function READER:ReadString()
    local length = self:ReadUInt(16) -- short
    local stringValue = self.data:sub(self.position, self.position + length - 1)
	self.position = self.position + length
    return stringValue
end

-- IEEE-754 Single-Precision
function READER:ReadFloat()
    local bytes = {}

    for i = 1, 4 do
        table.insert(bytes, self.data:byte(self.position))
        self.position = self.position + 1
    end

	local byteStringChars = string.char(table.unpack(bytes))

	return string.unpack("f", byteStringChars)
end

function READER:ReadUInt(bitCount)
    local value = 0

    for i = 1, bitCount do
        value = value + self:ReadBit() * 2 ^ (bitCount - i)
    end

	return value
end

local readFunctions = {
	[NET_TYPE_NUMBER] = READER.ReadFloat,
	[NET_TYPE_STRING] = READER.ReadString,
	-- TODO: the rest
}

function READER:ReadType()
	local typeId = self:ReadUInt(16) -- short
	local dataType = fromTypeId(typeId)

	if (not dataType) then
		error("Invalid data type: " .. typeId)
	end

	local data = readFunctions[typeId](self)

	return data
end

function READER:GetPosition()
    return self.position
end

function READER:GetData()
	return self.data
end

function READER:GetSize()
    return #self.data
end

function READER:DebugOutputData()
	debugData(self.data)
end

--[[
	Writer Metatable
--]]
local WRITER = {}
WRITER.__index = WRITER

function WRITER.new()
    return setmetatable({
        data = ""
    }, WRITER)
end

function WRITER:WriteBit(bit)
    local byte = bit % 2 -- get the least significant bit
    self.data = self.data .. string.char(byte)
end

function WRITER:WriteString(stringValue)
    local length = #stringValue
    self:WriteUInt(length, 16) -- short means the string can be at most 65,535 characters long
    self.data = self.data .. stringValue
end

-- Write IEEE-754 Single-Precision
function WRITER:WriteFloat(floatValue)
    local packedFloat = string.pack("f", floatValue)
	local byteString = {string.byte(packedFloat, 1, -1)}

    self.data = self.data .. string.char(table.unpack(byteString))
end

function WRITER:WriteUInt(value, bitCount)
    for i = bitCount, 1, -1 do
		local bit = math.floor(value / 2 ^ (i - 1))
		self:WriteBit(bit)
		value = value % 2 ^ (i - 1)
	end
end

local writeFunctions = {
	[NET_TYPE_NUMBER] = WRITER.WriteFloat,
	[NET_TYPE_STRING] = WRITER.WriteString,
	-- TODO: the rest
}

function WRITER:WriteType(data)
    local typeId = toTypeId(data)
    self:WriteUInt(typeId, 16) -- short

	writeFunctions[typeId](self, data)
end

function WRITER:GetData()
	return self.data
end

function WRITER:GetSize()
    return #self.data
end

function WRITER:DebugOutputData()
	debugData(self.data)
end

--[[
	Common Library functions
--]]

local currentOutgoingMessage = nil

function MODULE.Start(messageName)
	if (currentOutgoingMessage) then
		error("net.Start was called twice without sending a message.")
	end

    currentOutgoingMessage = WRITER.new()

	-- Write the message name as the header
    currentOutgoingMessage:WriteString(messageName)
end

if (SERVER) then
	function MODULE.Send(client)
		if (not currentOutgoingMessage) then
			error("net.Send was called without calling net.Start.")
		end

		local data = currentOutgoingMessage:GetData()
		client:send(data .. "\n")

		currentOutgoingMessage = nil
	end

	function MODULE.Broadcast()
		if (not currentOutgoingMessage) then
			error("net.Broadcast was called without calling net.Start.")
		end

		local data = currentOutgoingMessage:GetData()

		for _, client in ipairs(clients) do
			client:send(data .. "\n")
		end

		currentOutgoingMessage = nil
	end
elseif (CLIENT) then
	function MODULE.SendToServer()
		if (not currentOutgoingMessage) then
			error("net.SendToServer was called without calling net.Start.")
		end

		local data = currentOutgoingMessage:GetData()
        local success, err = localClient:send(data .. "\n")

		if (not success) then
			debug("Error sending data to server:", err)
			return
		end

		currentOutgoingMessage = nil
	end
end

local currentIncomingMessage = nil

function MODULE.HandleIncomingMessage(bytes, client)
	if (currentIncomingMessage) then
		error("net.HandleIncomingMessage was called twice without handling the previous message.")
	end

    currentIncomingMessage = READER.new(bytes)

	if (not client) then
		debug("Incoming message from server:")
	else
		debug("Incoming message from client:", client)
	end
	currentIncomingMessage:DebugOutputData()

	MODULE.Incoming(currentIncomingMessage:GetSize(), client)
end

--- Registers a callback for a message
--- @param messageName string
--- @param callback fun(length: number, client: table)
function MODULE.Receive(messageName, callback)
    MODULE.registeredCallbacks[messageName] = callback
end

--- Fires callbacks for incoming messages
--- @param length any
--- @param client any
function MODULE.Incoming(length, client)
    if (not currentIncomingMessage) then
        error("net.Incoming was called without calling net.HandleIncomingMessage.")
    end

    local messageName = currentIncomingMessage:ReadString()
    local callback = MODULE.registeredCallbacks[messageName]

    if (not callback) then
        debug("Warning: Unhandled message '" .. messageName .. "'")
        return
    end

    callback(length, client)

    currentIncomingMessage = nil
end

function MODULE.GetOutgoingMessageWriter()
    return currentOutgoingMessage
end

function MODULE.GetIncomingMessageReader()
	return currentIncomingMessage
end

--[[
	Forward net.Read* and net.Write* functions to the reader and writer
	that are currently active
--]]

function MODULE.ReadBit()
    return currentIncomingMessage:ReadBit()
end

function MODULE.WriteBit(bit)
	currentOutgoingMessage:WriteBit(bit)
end

function MODULE.ReadString()
    return currentIncomingMessage:ReadString()
end

function MODULE.WriteString(stringValue)
    currentOutgoingMessage:WriteString(stringValue)
end

function MODULE.ReadFloat()
    return currentIncomingMessage:ReadFloat()
end

function MODULE.WriteFloat(float)
    currentOutgoingMessage:WriteFloat(float)
end

function MODULE.ReadUInt(bitCount)
    return currentIncomingMessage:ReadUInt(bitCount)
end

function MODULE.WriteUInt(value, bitCount)
    currentOutgoingMessage:WriteUInt(value, bitCount)
end

return MODULE
