-- lua_openscript utilities/test/networks.lua;lua_openscript_cl utilities/test/networks.lua;

local countReceived = 0

-- After registering the network strings we must wait a bit
local DELAY_BEFORE_START = 2
local DELAY_BEFORE_DONE = 2

if (SERVER) then
	Networks.AddNetworkString("TestMessage")
	Networks.AddNetworkString("CTestMessage")
	Networks.AddNetworkString("TestMessage2")
	Networks.AddNetworkString("CTestMessage2")

    Networks.Receive("TestMessage", function(length, client)
        local message = Networks.ReadString()
		local num = Networks.ReadFloat()

        assert(math.AlmostEquals(num, 3.14), "Received incorrect number from client!")
		assert(message == "Hello, server!", "Received incorrect message from client!")

        Networks.Start("CTestMessage")
		Networks.WriteFloat(1234.5677490234)
        Networks.WriteString("Hello, client!")
		Networks.WriteVector(client:GetPosition())
        Networks.Send(client)

		countReceived = countReceived + 1
    end)

    Networks.Receive("TestMessage2", function(length, client)
        Networks.Start("CTestMessage2")
        Networks.WriteString("Hello again, client!")
        Networks.Send(client)

		countReceived = countReceived + 1
    end)

	Timers.Simple(DELAY_BEFORE_START + DELAY_BEFORE_DONE, function()
		assert(countReceived == 2, "Did not receive all messages from client!")

		print("net module server tests passed!")
	end)
end

if (CLIENT) then
	Networks.Receive("CTestMessage", function(length, client)
        local num = Networks.ReadFloat()
        local message = Networks.ReadString()
		local position = Networks.ReadVector()

        assert(math.AlmostEquals(num, 1234.5677490234), "Received incorrect number from server!")
        assert(message == "Hello, client!", "Received incorrect message from server!")
		assert(type(position) == "vector", "Received incorrect position from server!")

		countReceived = countReceived + 1
	end)

	Networks.Receive("CTestMessage2", function(length, client)
        local message = Networks.ReadString()

        assert(message == "Hello again, client!", "Received incorrect message from server!")

		countReceived = countReceived + 1
	end)

	timer.Simple(DELAY_BEFORE_START, function()
		Networks.Start("TestMessage")
		Networks.WriteString("Hello, server!")
		Networks.WriteFloat(3.14)
		Networks.SendToServer()

		Networks.Start("TestMessage2")
		Networks.SendToServer()
	end)

	Timers.Simple(DELAY_BEFORE_START + DELAY_BEFORE_DONE, function()
        assert(countReceived == 2, "Did not receive all messages from server!")

		print("net module client tests passed!")
	end)
end
