local MODULE = {}

local Hooks = require("hooks")
local socket = require("luasocket")
local IP = "127.0.0.1" -- TODO: game server IP here
local PORT = 12345

local BYTE_SIZE_IN_BITS = 8
local SIZE_STRING_BYTES = 2
local SIZE_FLOAT_BYTES = 4
local SIZE_INT_BYTES = 4
local SIZE_ENTITY_INDEX = 24 -- TODO: Find out if we should use entindex or GetSerialNumber, or something else

-- Client only:
local localClient

-- Server only:
local localServer
local socketClients = {}

MODULE.registeredCallbacks = MODULE.registeredCallbacks or {}

local currentIncomingMessage = nil
local currentOutgoingMessage = nil

--[[
	Forward Networks.Read* and Networks.Write* functions to the reader and writer
	that are currently active
--]]
if (getmetatable(MODULE) == nil) then
	setmetatable(MODULE, {
        __index = function(_, key)
            local forwardTo = {
				Read = currentIncomingMessage,
				Write = currentOutgoingMessage
            }

            for prefix, target in pairs(forwardTo) do
                if (key:sub(1, #prefix) ~= prefix) then
                    continue
                end

                if (not target) then
					error("Networks." .. key .. " was called without calling Networks.Start.", 2)
				end

				if (not target[key]) then
					return nil
				end

				return function(...)
					return target[key](target, ...)
				end
            end
		end
	})
end

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
	Logic to setup the socketClient and server sockets
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
			-- Hooks.Remove("Think", "__NetModuleTestClientUpdate")
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
			-- Hooks.Remove("Think", "__NetModuleTestClientUpdate")
			return
		end

		MODULE.HandleIncomingMessage(bytes, nil)
	end

	Hooks.Add("Think", "__NetModuleClientUpdate", clientUpdate)
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

	-- Disable blocking on the server socket
	localServer:settimeout(0)

    -- If the server is sending a message, it may be to a client that is still connecting
    -- This is due to our system only connecting once the client has loaded Lua.
    -- For this we create a queue of messages to send to the client once they connect.
    local clientPackedDataQueue = {}

	local function processQueuedMessages(client)
		if (not clientPackedDataQueue[client]) then
			return
		end

		for _, packedData in pairs(clientPackedDataQueue[client]) do
			local socketClient = MODULE.ClientToSocketClient(client)
			socketClient:send(packedData)

			debug("Sent queued message to client")
		end

		clientPackedDataQueue[client] = nil
	end

    local function handleClient(clientData)
        local socketClient = clientData.socketClient

		-- Disable blocking on the socketClient socket
		socketClient:settimeout(0)

		local bytes, err = socketClient:receive(SIZE_INT_BYTES)

		if (err and err ~= "timeout") then
			-- Remove the socketClient from the list if it's disconnected
			for i, c in ipairs(socketClients) do
				if (c == socketClient) then
					table.remove(socketClients, i)
					break
				end
			end

			socketClient:close()
			debug("Client disconnected", err)

            return
		end

        if (not bytes) then
            return
        end

        local length = string.unpack("I", bytes)

        bytes, err = socketClient:receive(length)

		if (err and err ~= "timeout") then
			-- Remove the socketClient from the list if it's disconnected
			for i, c in ipairs(socketClients) do
				if (c == socketClient) then
					table.remove(socketClients, i)
					break
				end
			end

			socketClient:close()
			debug("Client disconnected", err)

			return
		end

		MODULE.HandleIncomingMessage(bytes, clientData.client)
	end

    local function serverUpdate()
        -- Accept any new clients
        local newSocketClient = localServer:accept()

        if (newSocketClient) then
            -- TODO: We need some sort of: Util.PlayerByAddress(socketClient:getpeername()) here
			-- ! This will stop working once we're not the only client anymore
            local client = Util.PlayerByIndex(1)

            -- TODO: We need to clean this up when the client disconnects
            table.insert(socketClients, {
                socketClient = newSocketClient,

                client = client
            })

			processQueuedMessages(client)
        end

        -- Handle all clients
        for _, clientData in ipairs(socketClients) do
            handleClient(clientData)
        end

        -- Sleep for a short time to prevent high CPU usage
        -- socket.sleep(0.01)
    end

    function MODULE.QueueMessage(client, packedData)
        if (not clientPackedDataQueue[client]) then
            clientPackedDataQueue[client] = {}
        end

        table.insert(clientPackedDataQueue[client], packedData)
    end

    function MODULE.ClientToSocketClient(client)
		for _, clientData in ipairs(socketClients) do
			if (clientData.client == client) then
				return clientData.socketClient
			end
		end
	end

	Hooks.Add("Think", "__NetModuleServerUpdate", serverUpdate)
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

local function toTypeId(data)
	return typeIdMap[type(data)]
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

--- Writes the components of an angle
--- @param angle Angle
function WRITER:WriteAngle(angle)
	self:WriteFloat(angle.pitch)
	self:WriteFloat(angle.yaw)
	self:WriteFloat(angle.roll)
end

--- Writes the bit and marking it so when we pack the data,
--- we can determine if multiple bits are written in the same byte
--- @param bit any
function WRITER:WriteBit(bit)
    assert(bit == 0 or bit == 1, "Bit must be 0 or 1")

	self:WriteRaw(bit, 1)
end

--- Writes a boolean as a bit
--- @param bool boolean
function WRITER:WriteBool(bool)
    assert(type(bool) == "boolean", "Value must be a boolean")

    self:WriteBit(bool and 1 or 0)
end

--- Writes the components of a color
--- @param color Color
--- @param withAlpha? boolean
function WRITER:WriteColor(color, withAlpha)
    withAlpha = withAlpha or false

    self:WriteUInt(color.r, 8)
    self:WriteUInt(color.g, 8)
    self:WriteUInt(color.b, 8)

    if (withAlpha) then
        self:WriteUInt(color.a, 8)
    end
end

--- Writes the given entity as an index
--- @param entity CBaseEntity
function WRITER:WriteEntity(entity)
    -- TODO: See note near SIZE_ENTITY_INDEX
    self:WriteUInt(entity:entindex(), SIZE_ENTITY_INDEX)
end

--- Writes the given float
--- @param floatValue number
function WRITER:WriteFloat(floatValue)
	self:WriteRaw(string.pack("f", floatValue), SIZE_FLOAT_BYTES * BYTE_SIZE_IN_BITS)
end

--- Writes an integer with the specified number of bits
--- @param value number
--- @param bitCount number
function WRITER:WriteInt(value, bitCount)
    for i = bitCount - 1, 0, -1 do
        local bit = (value >> i) & 1
        self:WriteBit(bit)
    end
end

--- Writes an unsigned integer with the specified number of bits
--- @param value number
--- @param bitCount number
function WRITER:WriteUInt(value, bitCount)
	for i = bitCount - 1, 0, -1 do
		local bit = (value >> i) & 1
		self:WriteBit(bit)
	end
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

local writeFunctions = {
	[NET_TYPE_NUMBER] = WRITER.WriteFloat,
	[NET_TYPE_STRING] = WRITER.WriteString,
	[NET_TYPE_BOOL] = WRITER.WriteBool,
	-- TODO: The rest of the types
}

--- Writes any type of data, with a type identifier
--- @param data any
function WRITER:WriteType(data)
    local typeId = toTypeId(data)
    self:WriteUInt(typeId, 8)

    local writeFunction = writeFunctions[typeId]

    if (not writeFunction) then
        error("Unsupported data type: " .. typeId)
    end

    writeFunction(self, data)
end

--- Writes the components of a vector
--- @param vector Vector
function WRITER:WriteVector(vector)
	self:WriteFloat(vector.x)
	self:WriteFloat(vector.y)
	self:WriteFloat(vector.z)
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

--- Moves the position in the data by the specified amount
--- @param amount number
function READER:MovePosition(amount)
    amount = amount or 1
    self.position = self.position + amount
end

--- Reads data as a raw number, with the specified number of bits
--- @param bitCount number
--- @return number
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

--- Reads the components of an angle
--- @return Angle
function READER:ReadAngle()
    local pitch = self:ReadFloat()
    local yaw = self:ReadFloat()
    local roll = self:ReadFloat()

    return Angle(pitch, yaw, roll)
end

--- Reads a bit from the data
--- @return number
function READER:ReadBit()
    return self:ReadRaw(1)
end

--- Reads a boolean from the data
--- @return boolean
function READER:ReadBool()
    return self:ReadBit() == 1
end

--- Reads the components of a color
--- @param withAlpha? boolean
--- @return Color
function READER:ReadColor(withAlpha)
    withAlpha = withAlpha or false

    local r = self:ReadUInt(8)
    local g = self:ReadUInt(8)
    local b = self:ReadUInt(8)
    local a = withAlpha and self:ReadUInt(8) or 255

    return Color(r, g, b, a)
end

--- Reads an entity index from the data.
--- Seeing how only entities in the PVS will be available, it is best
--- to use the second return value, which is a function that will return
--- the entity when called (if it is then in the PVS).
--- @return CBaseEntity|nil, fun(): CBaseEntity
function READER:ReadEntity()
    local index = self:ReadUInt(SIZE_ENTITY_INDEX)
    local entityGetter = function()
		print(index, Entity, Entity(index))
        return Entity(index)
    end

	return entityGetter(), entityGetter
end

--- Reads a float from the data
--- @return number
function READER:ReadFloat()
    local floatBytes = {}

    for i = 1, SIZE_FLOAT_BYTES do
        local byte = self:ReadRaw(BYTE_SIZE_IN_BITS)
        table.insert(floatBytes, string.char(byte))
    end

    return string.unpack("f", table.concat(floatBytes))
end

--- Reads an integer from the data
--- @param bitCount number
function READER:ReadInt(bitCount)
    local value = 0

    for i = 1, bitCount do
        local bit = self:ReadBit()
        value = (value << 1) | bit
    end

    return value
end

--- Reads an unsigned integer from the data
--- @param bitCount number
function READER:ReadUInt(bitCount)
	return self:ReadInt(bitCount)
end

--- Reads a string from the data
--- @return string
function READER:ReadString()
    local length = self:ReadUInt(SIZE_STRING_BYTES * BYTE_SIZE_IN_BITS)
    local textBytes = {}

    for i = 1, length do
        local byte = self:ReadRaw(BYTE_SIZE_IN_BITS)
        table.insert(textBytes, string.char(byte))
    end

    return table.concat(textBytes)
end

local readFunctions = {
	[NET_TYPE_NUMBER] = READER.ReadFloat,
	[NET_TYPE_STRING] = READER.ReadString,
	[NET_TYPE_BOOL] = READER.ReadBool,
	-- TODO: The rest of the types
}

--- Reads any type of data, by reading the type identifier first
--- @param typeId? number
--- @return any
function READER:ReadType(typeId)
    typeId = typeId or self:ReadUInt(8)

    local readFunction = readFunctions[typeId]

    if (not readFunction) then
        error("Unsupported data type: " .. typeId)
    end

    return readFunction(self)
end

--- Reads the components of a vector
--- @return Vector
function READER:ReadVector()
	local x = self:ReadFloat()
	local y = self:ReadFloat()
	local z = self:ReadFloat()

	return Vector(x, y, z)
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

function MODULE.Cancel()
	currentOutgoingMessage = nil
end

function MODULE.Start(messageName)
	if (currentOutgoingMessage) then
		error("Networks.Start was called twice without sending a message.")
	end

    currentOutgoingMessage = WRITER.new()

    -- Write the message name as the header
	-- TODO: Store this in networked stringtables and send the index instead (adding Networks.AddNetworkString, with util.AddNetworkString as an alias for gmod compatibility)
    currentOutgoingMessage:WriteString(messageName)
end

if (SERVER) then
	function MODULE.Send(client)
        if (not currentOutgoingMessage) then
            error("Networks.Send was called without calling Networks.Start.")
        end

        local socketClient = MODULE.ClientToSocketClient(client)

        local data = currentOutgoingMessage:GetPackedData()
        currentOutgoingMessage = nil

        if (not socketClient) then
			debug("Client is not connected yet, queueing message...", client)
			MODULE.QueueMessage(client, data)
			return
		end

		socketClient:send(data)
	end
    function MODULE.Broadcast()
        if (not currentOutgoingMessage) then
            error("Networks.Broadcast was called without calling Networks.Start.")
        end

        for _, socketClient in ipairs(socketClients) do
            MODULE.Send(socketClient)
        end

        currentOutgoingMessage = nil
    end

	--- Sends the message to all players that have the entity in their PVS
	--- @param entity any
    function MODULE.BroadcastPVS(entity)
        if (not currentOutgoingMessage) then
            error("Networks.BroadcastPVS was called without calling Networks.Start.")
        end

		local origin = entity:GetPosition()
		local count, entitiesInPVS = Util.EntitiesInPVS(origin)

        for _, ent in ipairs(entitiesInPVS) do
			print(ent)
            if (not ent:IsPlayer()) then
                continue
            end

            MODULE.Send(ent)
		end

		currentOutgoingMessage = nil
	end
elseif (CLIENT) then
	function MODULE.SendToServer()
		if (not currentOutgoingMessage) then
			error("Networks.SendToServer was called without calling Networks.Start.")
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

function MODULE.HandleIncomingMessage(bytes, socketClient)
	if (currentIncomingMessage) then
        debug("Networks.HandleIncomingMessage was called twice without handling the previous message. Resetting...")
	end

    currentIncomingMessage = READER.new(bytes)

	MODULE.Incoming(currentIncomingMessage:GetSize(), socketClient)
end

--- Registers a callback for a message
--- @param messageName string
--- @param callback fun(length: number, socketClient: table)
function MODULE.Receive(messageName, callback)
    MODULE.registeredCallbacks[messageName] = callback
end

--- Fires callbacks for incoming messages
--- @param length any
--- @param socketClient any
function MODULE.Incoming(length, socketClient)
    if (not currentIncomingMessage) then
        error("Networks.Incoming was called without calling Networks.HandleIncomingMessage.")
    end

    local messageName = currentIncomingMessage:ReadString()
    local callback = MODULE.registeredCallbacks[messageName]

    if (not callback) then
        debug("Warning: Unhandled message '" .. messageName .. "'")
        return
    end

    callback(length, socketClient)

    currentIncomingMessage = nil
end

function MODULE.GetOutgoingMessageWriter()
    return currentOutgoingMessage
end

function MODULE.GetIncomingMessageReader()
	return currentIncomingMessage
end

return MODULE
