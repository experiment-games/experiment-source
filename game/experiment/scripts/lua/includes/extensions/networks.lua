--[[
	Lua logic for the Networks library

	Provides an interface for the engine to communicate regarding networked
	messages
--]]

if (GAMEUI) then
	return
end

local registeredCallbacks = registeredCallbacks or {}

local currentIncomingMessageReader = nil
local currentOutgoingMessageWriter = nil
local currentOutgoingMessageTypeId = nil

--[[
	Forward Networks.Read* and Networks.Write* functions to the reader and writer
	that are currently active
--]]

if (getmetatable(Networks) == nil) then
	setmetatable(Networks, {
		__index = function(_, key)
			local forwardTo = {
				Read = currentIncomingMessageReader,
				Write = currentOutgoingMessageWriter
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

--[[
	Helper functions
--]]

local prefix = CLIENT and "[NetModuleTest Client]" or "[NetModuleTest Server]"
local printDebug = function(...)
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

	printDebug("Data: " .. table.concat(bytes, " "))
end

local function guardIsWriting(originFunction)
	if (not currentOutgoingMessageWriter or not currentOutgoingMessageTypeId) then
		error("Networks." .. originFunction .. " was called without calling Networks.Start.", 2)
	end
end

NET_TYPE_NIL = 0
NET_TYPE_NUMBER = 1
NET_TYPE_STRING = 2
NET_TYPE_TABLE = 3
NET_TYPE_BOOL = 4
NET_TYPE_FUNCTION = 5
NET_TYPE_USERDATA = 7

NET_TYPE_COLOR = 101
NET_TYPE_VECTOR = 102
NET_TYPE_ANGLE = 103

-- Maps the result of type() to a unique identifier
local typeIdMap = {
	["nil"] = NET_TYPE_NIL,
	["number"] = NET_TYPE_NUMBER,
	["string"] = NET_TYPE_STRING,
	["table"] = NET_TYPE_TABLE,
	["boolean"] = NET_TYPE_BOOL,
	["function"] = NET_TYPE_FUNCTION,
	["userdata"] = NET_TYPE_USERDATA,

	["Color"] = NET_TYPE_COLOR,
	["Vector"] = NET_TYPE_VECTOR,
	["Angle"] = NET_TYPE_ANGLE,
}

local function toTypeId(data)
	local typeId = typeIdMap[type(data)]

	if (not typeId) then
		error("Unsupported data type: " .. type(data))
	end

	return typeId
end

--[[
	Functions to convert network string names to ids and vice versa
--]]

local networkStrings = NetworkStringTables.FindTable("LuaNetworkStrings")

function Networks.AddNetworkString(name)
	if (SERVER) then
		networkStrings:AddString(true, name)
	end
end

function Networks.NetworkIdToString(id)
	return networkStrings:GetString(id)
end

function Networks.NetworkStringToId(name)
	return networkStrings:FindStringIndex(name)
end

--[[
	Writer Metatable extensions
--]]

local MESSAGE_WRITER_META = _R.MessageWriter

local writeFunctions = {
	[NET_TYPE_NUMBER] = MESSAGE_WRITER_META.WriteFloat,
	[NET_TYPE_STRING] = MESSAGE_WRITER_META.WriteString,
    [NET_TYPE_BOOL] = MESSAGE_WRITER_META.WriteBool,
	[NET_TYPE_NIL] = function(writer, value)
	end,
    [NET_TYPE_BOOL] = MESSAGE_WRITER_META.WriteBool,
    [NET_TYPE_TABLE] = function(writer, value)
		writer:WriteString(Json.Encode(value))
    end,

    [NET_TYPE_COLOR] = MESSAGE_WRITER_META.WriteColor,
    [NET_TYPE_VECTOR] = MESSAGE_WRITER_META.WriteVector,
    [NET_TYPE_ANGLE] = MESSAGE_WRITER_META.WriteAngle,
}

--- Writes any type of data, with a type identifier
--- @param data any
function MESSAGE_WRITER_META:WriteType(data)
    local typeId = toTypeId(data)
    self:WriteBitLong(typeId, 8, false)

    local writeFunction = writeFunctions[typeId]

    if (not writeFunction) then
        error("Unsupported data type: " .. typeId)
    end

    writeFunction(self, data)
end

--[[
	Reader Metatable extensions
--]]

local MESSAGE_READER_META = _R.MessageReader

local readFunctions = {
	[NET_TYPE_NUMBER] = MESSAGE_READER_META.ReadFloat,
	[NET_TYPE_STRING] = MESSAGE_READER_META.ReadString,
	[NET_TYPE_BOOL] = MESSAGE_READER_META.ReadBool,
	[NET_TYPE_NIL] = function(reader)
		return nil
	end,
	[NET_TYPE_TABLE] = function(reader)
		return Json.Decode(reader:ReadString())
	end,

	[NET_TYPE_COLOR] = MESSAGE_READER_META.ReadColor,
	[NET_TYPE_VECTOR] = MESSAGE_READER_META.ReadVector,
	[NET_TYPE_ANGLE] = MESSAGE_READER_META.ReadAngle,
}

--- Reads any type of data, by reading the type identifier first
--- @param typeId? number
--- @return any
function MESSAGE_READER_META:ReadType(typeId)
	typeId = typeId or self:ReadBitLong(8, false)

	local readFunction = readFunctions[typeId]

	if (not readFunction) then
		error("Unsupported data type: " .. typeId)
	end

	return readFunction(self)
end

--[[
	Networks Library functions for sending and receiving messages
--]]

function Networks.Cancel()
	currentOutgoingMessageWriter = nil
	currentOutgoingMessageTypeId = nil
end

--- Starts a new message, returns a writer to write data to
--- You can also use Networks.Write* functions to write data
--- to the same writer.
--- @param messageName string
--- @param unreliable boolean # Ignored, always reliable
--- @return MessageWriter
function Networks.Start(messageName, unreliable)
	if (currentOutgoingMessageWriter) then
		error("Networks.Start was called multiple times without sending the earlier message(s).")
	end

	currentOutgoingMessageWriter = MessageWriters.Create()
	currentOutgoingMessageTypeId = Networks.NetworkStringToId(messageName)

	if (not currentOutgoingMessageTypeId) then
		Networks.Cancel()
		error("Networks.Start was called with an unregistered message name: " .. messageName)
	end

	return currentOutgoingMessageWriter
end

if (SERVER) then
	function Networks.Send(clientOrClients)
		guardIsWriting("Send")

		-- Call the engine function to send the message
		Networks.InternalSendToClients(currentOutgoingMessageTypeId, currentOutgoingMessageWriter, clientOrClients)

		Networks.Cancel()
	end

	function Networks.Broadcast()
		guardIsWriting("Broadcast")

		-- Call the engine function to send the message
		Networks.InternalSendToClients(currentOutgoingMessageTypeId, currentOutgoingMessageWriter)

		Networks.Cancel()
	end

	function Networks.SendOmit(clientOrClientsToExclude)
		guardIsWriting("SendOmit")

		local clientsToExclude = type(clientOrClientsToExclude) == "table"
			and clientOrClientsToExclude
			or { clientOrClientsToExclude }

		for _, client in ipairs(Players.GetAllHumans()) do
			if (table.HasValue(clientsToExclude, client)) then
				continue
			end

			Networks.Send(client)
		end

		Networks.Cancel()
	end

	--- Sends the message to all players that have the entity in their PVS
	--- @param entity any
	function Networks.BroadcastPVS(entity)
		guardIsWriting("BroadcastPVS")

		local origin = entity:GetPosition()
		local entities = Entities.GetInPvs(origin)

		for _, entity in ipairs(entities) do
			if (not entity:IsPlayer()) then
				continue
			end

			Networks.Send(entity)
		end

		Networks.Cancel()
	end
elseif (CLIENT) then
	function Networks.SendToServer()
		guardIsWriting("SendToServer")

		-- Call the engine function to send the message
		Networks.InternalSendToServer(currentOutgoingMessageTypeId, currentOutgoingMessageWriter)

		Networks.Cancel()
	end
end

--- Called by the engine when a message is received
--- @param messageTypeId number
--- @param bufferReader MessageReader
--- @param sender Player|nil # nil if the message was sent by the server
function Networks.HandleIncomingMessage(messageTypeId, bufferReader, sender)
	if (currentIncomingMessageReader) then
		printDebug("Networks.HandleIncomingMessage was called twice without handling the previous message. Resetting...")
	end

	local messageName = Networks.NetworkIdToString(messageTypeId)

	if (not messageName) then
		printDebug("Warning: Unhandled message with network id " .. tostring(messageTypeId))
		return
	end

	local callback = registeredCallbacks[messageName]

	if (not callback) then
		printDebug("Warning: Unhandled message '" .. tostring(messageName) .. "' with network id " .. tostring(messageTypeId))
		return
	end

	currentIncomingMessageReader = bufferReader

	callback(bufferReader:GetNumBytesLeft(), sender)

	currentIncomingMessageReader = nil
end

--- Registers a callback for a message
--- @param messageName string
--- @param callback fun(length: number, sender: Player|nil)
function Networks.Receive(messageName, callback)
	registeredCallbacks[messageName] = callback
end

return Networks
