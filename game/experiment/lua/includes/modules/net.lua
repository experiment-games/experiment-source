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

local SIZE_STRING_BYTES = 2

-- Client only:
local localClient

-- Server only:
local localServer
local clients = {}

MODULE.registeredCallbacks = MODULE.registeredCallbacks or {}

local debug = function(...)
	local prefix = CLIENT and "[NetModuleTest Client]" or "[NetModuleTest Server]"

    print(prefix, ...)
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

--[[
	Writer Metatable
--]]
local WRITER = {}
WRITER.__index = WRITER

function WRITER.new()
    return setmetatable({
        data = {},
		bitBuffer = {}
    }, WRITER)
end

--- Writes the bit to be sent later. Because we send data in bytes, just
--- sending a single bit is not possible. For that reason we will collect
--- all bits seperately into the first byte of the data to try and save space.
--- After that bits will be written as normal.
---
--- @param bit any
function WRITER:WriteBit(bit)
    assert(bit == 0 or bit == 1, "Bit must be 0 or 1")

    -- Collect bits until we have a full byte
    table.insert(self.bitBuffer, bit)

    if (#self.bitBuffer == 8) then
        self:ForceWriteBitBuffer()
    end
end

function WRITER:ForceWriteBitBuffer()
    local bitCount = #self.bitBuffer

	if (bitCount == 0) then
		return
	end

	local byte = 0

	for i = 1, bitCount do
		byte = byte + (self.bitBuffer[i] << (8 - i))
	end

	self.bitBuffer = {}
	self:WriteByte(byte)
end

function WRITER:WriteBool(bool)
	self:WriteBit(bool and 1 or 0)
end

function WRITER:WriteBytes(bytes)
    if (type(bytes) ~= "string") then
        error("Bytes must be a string, got " .. type(bytes))
    end

	self:ForceWriteBitBuffer()
    table.insert(self.data, bytes)
end

function WRITER:WriteByte(byte)
	self:WriteBytes(string.char(byte))
end

--- The string will be prefixed with a short containing how many bytes the string is.
--- This means the maximum length of a string is 65535 bytes.
function WRITER:WriteString(stringValue)
    self:ForceWriteBitBuffer()

	local length = #stringValue
    self:WriteUInt(length, SIZE_STRING_BYTES * 8)

    local textBytes = string.pack("c" .. length, stringValue)

	self:WriteBytes(textBytes)
end

function WRITER:WriteFloat(floatValue)
    self:ForceWriteBitBuffer()

	self:WriteBytes(string.pack("f", floatValue))
end

function WRITER:WriteInt(value, bitCount)
    self:ForceWriteBitBuffer()

    for i = bitCount - 1, 0, -1 do
        local bit = (value >> i) & 1
        self:WriteBit(bit)
    end
end

function WRITER:WriteUInt(value, bitCount)
    self:ForceWriteBitBuffer()

	for i = bitCount - 1, 0, -1 do
		local bit = (value >> i) & 1
		self:WriteBit(bit)
	end
end

--- Packs the data into a string to be sent over the network with string.pack
--- The first byte will contain any bits to be written, the rest will be the data
--- @return string
function WRITER:GetPackedData()
    -- If there's any bits left, write them to the next byte`
	self:ForceWriteBitBuffer()

	return table.concat(self.data)
end

function WRITER:DebugOutputData()
    debugData(self.data)
	debugData(self.bitBuffer)
	debugData(self:GetPackedData())
end

--[[
	Reader Metatable
--]]
local READER = {}
READER.__index = READER

function READER.new(data)
    return setmetatable({
        data = data,
        position = 1,
        bitBuffer = {}
    }, READER)
end

function READER:ForcePastBitBuffer()
	self.bitBuffer = {}
end

function READER:MovePosition(amount)
	self.position = self.position + amount
end

function READER:ReadBit()
    if (#self.bitBuffer == 0) then
        local byte = self:ReadByte()

        for i = 8, 1, -1 do
            table.insert(self.bitBuffer, (byte >> (i - 1)) & 1)
        end
    end

    return table.remove(self.bitBuffer, 1)
end

function READER:ReadBool()
    return self:ReadBit() == 1
end

function READER:ReadBytes(length)
    self:ForcePastBitBuffer()

    local bytes = self.data:sub(self.position, self.position + length - 1)
    self.position = self.position + length

    return bytes
end

function READER:ReadByte()
    return self:ReadBytes(1):byte()
end

function READER:ReadString()
    self:ForcePastBitBuffer()

    local length = self:ReadUInt(SIZE_STRING_BYTES * 8)

    local textBytes = self:ReadBytes(length)

	debug("Reading string of length", length, "with bytes", textBytes)
	return string.unpack("c" .. length, textBytes)
end

function READER:ReadFloat()
    self:ForcePastBitBuffer()

    return string.unpack("f", self:ReadBytes(4))
end

function READER:ReadInt(bitCount)
    self:ForcePastBitBuffer()

    local value = 0

    for i = 1, bitCount do
        value = value + (self:ReadBit() << (bitCount - i))
    end

    return value
end

function READER:ReadUInt(bitCount)
    self:ForcePastBitBuffer()

    local value = 0

    for i = 1, bitCount do
        value = value + (self:ReadBit() << (bitCount - i))
    end

    return value
end

function READER:GetSize()
    return #self.data * 8 -- In bits
end

function READER:DebugOutputData()
    debugData(self.data)
    debugData(self.bitBuffer)
end

--[[
	Common Library functions
--]]

local currentOutgoingMessage = nil

function MODULE.Cancel()
	currentOutgoingMessage = nil
end

function MODULE.Start(messageName)
	if (currentOutgoingMessage) then
		error("net.Start was called twice without sending a message.")
	end

    currentOutgoingMessage = WRITER.new()

    -- Write the message name as the header
	-- TODO: Store this in networked stringtables and send the index instead (adding net.AddNetworkString, with util.AddNetworkString as an alias for gmod compatibility)
    currentOutgoingMessage:WriteString(messageName)
end

if (SERVER) then
	function MODULE.Send(client)
		if (not currentOutgoingMessage) then
			error("net.Send was called without calling net.Start.")
		end

		local data = currentOutgoingMessage:GetPackedData()
		client:send(data .. "\n")

		currentOutgoingMessage = nil
	end

	function MODULE.Broadcast()
		if (not currentOutgoingMessage) then
			error("net.Broadcast was called without calling net.Start.")
		end

		local data = currentOutgoingMessage:GetPackedData()

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

		local data = currentOutgoingMessage:GetPackedData()
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
