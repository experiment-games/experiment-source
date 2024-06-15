--[[
	TODO:
		- Add the other datatypes for reading and writing
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

local BYTE_SIZE_IN_BITS = 8
local SIZE_STRING_BYTES = 2
local SIZE_FLOAT_BYTES = 4
local SIZE_INT_BYTES = 4

-- Client only:
local localClient

-- Server only:
local localServer
local clients = {}

MODULE.registeredCallbacks = MODULE.registeredCallbacks or {}

local prefix = CLIENT and "[NetModuleTest Client]" or "[NetModuleTest Server]"
local debug = function(...)
    print(prefix, ...)
end

-- Shows the data in binary format
local debugData = function(data)
    local bytes = {}
	local isString = type(data) == "string"

    for i = 1, #data do
        if (isString) then
			local byte = data:byte(i)
			table.insert(bytes, byte)
			continue
        end

        local dataPart = data[i].data or data[i]
        local dataBits = data[i].bits or BYTE_SIZE_IN_BITS

        local byte = ""

        for j = dataBits - 1, 0, -1 do
            local bit = (dataPart >> j) & 1
            byte = byte .. bit
        end

        table.insert(bytes, byte)
    end

	debug("Data: " .. table.concat(bytes, " "))
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
        end

		-- Receive any incoming data from the server
		local bytes, err = localClient:receive(SIZE_INT_BYTES)

		-- On failure, lets close and remove the think hook so we don't spam errors
		if (err and err ~= "timeout") then
			debug("Error receiving response: " .. err)

			-- localClient:close()
			-- hooks.Remove("Think", "__NetModuleTestClientUpdate")
			return
		end

        -- If we don't receive length data, we don't have a message yet
        if (not bytes) then
            return
        end

        local length = string.unpack("I", bytes)

        bytes, err = localClient:receive(length)

		if (err and err ~= "timeout") then
			debug("Error receiving response: " .. err)

			-- localClient:close()
			-- hooks.Remove("Think", "__NetModuleTestClientUpdate")
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

	-- Disable blocking on the server socket
	localServer:settimeout(0)

	local function handleClient(client)
		-- Disable blocking on the client socket
		client:settimeout(0)

		local bytes, err = client:receive(SIZE_INT_BYTES)

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
		end

        if (not bytes) then
            return
        end

        local length = string.unpack("I", bytes)

        bytes, err = client:receive(length)

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
		end

		MODULE.HandleIncomingMessage(bytes, client)
	end

	local function serverUpdate()
		-- Accept any new clients
		local newClient = localServer:accept()

		if (newClient) then
			table.insert(clients, newClient)
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
    }, WRITER)
end

--- Writes data as a byte, or the specified number of bits
--- If a string is passed, each character will be written as a byte
--- @param data number|string
--- @param bits? number
function WRITER:WriteRaw(data, bits)
	if (type(data) == "string") then
		for i = 1, #data do
			local byte = data:byte(i)
			self:WriteRaw(byte, BYTE_SIZE_IN_BITS)
		end

        return
    elseif (type(data) ~= "number") then
        error("Data must be a number or string")
	elseif (bits and bits ~= BYTE_SIZE_IN_BITS) then
		assert(data >= 0 and data < 2 ^ bits, "Data is too large to fit in the specified number of bits")
	end

	table.insert(self.data, {
		data = data,
		bits = bits or BYTE_SIZE_IN_BITS,
	})
end

--- Writes the bit and marking it so when we pack the data,
--- we can determine if multiple bits are written in the same byte
--- @param bit any
function WRITER:WriteBit(bit)
    assert(bit == 0 or bit == 1, "Bit must be 0 or 1")

	self:WriteRaw(bit, 1)
end

function WRITER:WriteBool(bool)
	assert(type(bool) == "boolean", "Value must be a boolean")

	self:WriteBit(bool and 1 or 0)
end

--- The string will be prefixed with a short containing how many bytes the string is.
--- This means the maximum length of a string is 65535 bytes.
function WRITER:WriteString(stringValue)
    local length = #stringValue

    assert(length <= 65535, "String is too long to send over the network")

    self:WriteUInt(length, SIZE_STRING_BYTES * BYTE_SIZE_IN_BITS)

    local textBytesAsString = string.pack("c" .. length, stringValue)
	self:WriteRaw(textBytesAsString, length * BYTE_SIZE_IN_BITS)
end

function WRITER:WriteFloat(floatValue)
	self:WriteRaw(string.pack("f", floatValue), SIZE_FLOAT_BYTES * BYTE_SIZE_IN_BITS)
end

function WRITER:WriteInt(value, bitCount)
    for i = bitCount - 1, 0, -1 do
        local bit = (value >> i) & 1
        self:WriteBit(bit)
    end
end

function WRITER:WriteUInt(value, bitCount)
	for i = bitCount - 1, 0, -1 do
		local bit = (value >> i) & 1
		self:WriteBit(bit)
	end
end

--- Packs the data compactly into a string to be sent over the network
--- @param withoutPrefixingLength? boolean
--- @return string
function WRITER:GetPackedData(withoutPrefixingLength)
	withoutPrefixingLength = withoutPrefixingLength or false
	local packedData = ""
    local currentByte = 0
    local bitsFilled = 0

    for _, entry in ipairs(self.data) do
        local data = entry.data
        local bits = entry.bits

        for i = bits - 1, 0, -1 do
            local bit = (data >> i) & 1
            currentByte = (currentByte << 1) | bit
            bitsFilled = bitsFilled + 1

            if bitsFilled == 8 then
                packedData = packedData .. string.char(currentByte)
                currentByte = 0
                bitsFilled = 0
            end
        end
    end

    -- Handle any remaining bits that didn't make up a full byte
    if (bitsFilled > 0) then
        currentByte = currentByte << (BYTE_SIZE_IN_BITS - bitsFilled)
        packedData = packedData .. string.char(currentByte)
    end

    if (withoutPrefixingLength) then
        return packedData
    end

	return string.pack("I", #packedData) .. packedData
end

function WRITER:DebugOutputData()
    debugData(self.data)
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
    }, READER)
end

function READER:MovePosition(amount)
    amount = amount or 1
    self.position = self.position + amount
end

function READER:ReadRaw(bitCount)
    local data = 0

    for i = 1, bitCount do
        local byteIndex = math.floor((self.position - 1) / BYTE_SIZE_IN_BITS) + 1
        local bitIndex = (self.position - 1) % BYTE_SIZE_IN_BITS + 1

        local byte = self.data:byte(byteIndex)
        local bit = (byte >> (BYTE_SIZE_IN_BITS - bitIndex)) & 1

        data = (data << 1) | bit
        self:MovePosition()
    end

    return data
end

function READER:ReadBit()
    return self:ReadRaw(1)
end

function READER:ReadBool()
    return self:ReadBit() == 1
end

function READER:ReadString()
    local length = self:ReadUInt(SIZE_STRING_BYTES * BYTE_SIZE_IN_BITS)
    local textBytes = {}

    for i = 1, length do
        local byte = self:ReadRaw(BYTE_SIZE_IN_BITS)
        table.insert(textBytes, string.char(byte))
    end

    return table.concat(textBytes)
end

function READER:ReadFloat()
    local floatBytes = {}

    for i = 1, SIZE_FLOAT_BYTES do
        local byte = self:ReadRaw(BYTE_SIZE_IN_BITS)
        table.insert(floatBytes, string.char(byte))
    end

    return string.unpack("f", table.concat(floatBytes))
end

function READER:ReadInt(bitCount)
    local value = 0

    for i = 1, bitCount do
        local bit = self:ReadBit()
        value = (value << 1) | bit
    end

    return value
end

function READER:ReadUInt(bitCount)
	return self:ReadInt(bitCount)
end

function READER:ReadType()
    return self:ReadUInt(8)
end

function READER:GetSize()
    return #self.data * BYTE_SIZE_IN_BITS
end

function READER:DebugOutputData()
    debugData(self.data)
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
		client:send(data)

		currentOutgoingMessage = nil
	end

	function MODULE.Broadcast()
		if (not currentOutgoingMessage) then
			error("net.Broadcast was called without calling net.Start.")
		end

        for _, client in ipairs(clients) do
			MODULE.Send(client)
		end

		currentOutgoingMessage = nil
	end
elseif (CLIENT) then
	function MODULE.SendToServer()
		if (not currentOutgoingMessage) then
			error("net.SendToServer was called without calling net.Start.")
		end

        local data = currentOutgoingMessage:GetPackedData()
		local success, err = localClient:send(data)

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
