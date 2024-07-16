-- lua_dofile utilities/test/networks.lua;lua_dofile_cl utilities/test/networks.lua;

local countReceived = 0

if (SERVER) then
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

	Timers.Simple(1, function()
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

	Networks.Start("TestMessage")
	Networks.WriteString("Hello, server!")
	Networks.WriteFloat(3.14)
	Networks.SendToServer()

	Networks.Start("TestMessage2")
    Networks.SendToServer()

	Timers.Simple(1, function()
        assert(countReceived == 2, "Did not receive all messages from server!")

		print("net module client tests passed!")
	end)
end
