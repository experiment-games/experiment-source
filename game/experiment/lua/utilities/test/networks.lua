-- lua_dofile utilities/test/net.lua;lua_dofile_cl utilities/test/net.lua;

local countReceived = 0

if (SERVER) then
    net.Receive("TestMessage", function(length, client)
        local message = net.ReadString()
		local num = net.ReadFloat()

        assert(math.AlmostEquals(num, 3.14), "Received incorrect number from client!")
		assert(message == "Hello, server!", "Received incorrect message from client!")

        net.Start("CTestMessage")
		net.WriteFloat(1234.5677490234)
        net.WriteString("Hello, client!")
		net.WriteVector(client:GetAbsOrigin())
        net.Send(client)

		countReceived = countReceived + 1
    end)

    net.Receive("TestMessage2", function(length, client)
        net.Start("CTestMessage2")
        net.WriteString("Hello again, client!")
        net.Send(client)

		countReceived = countReceived + 1
    end)

	timer.Simple(1, function()
		assert(countReceived == 2, "Did not receive all messages from client!")

		print("net module server tests passed!")
	end)
end

if (CLIENT) then
	net.Receive("CTestMessage", function(length, client)
        local num = net.ReadFloat()
        local message = net.ReadString()
		local position = net.ReadVector()

        assert(math.AlmostEquals(num, 1234.5677490234), "Received incorrect number from server!")
        assert(message == "Hello, client!", "Received incorrect message from server!")
		assert(type(position) == "vector", "Received incorrect position from server!")

		countReceived = countReceived + 1
	end)

	net.Receive("CTestMessage2", function(length, client)
        local message = net.ReadString()

        assert(message == "Hello again, client!", "Received incorrect message from server!")

		countReceived = countReceived + 1
	end)

	net.Start("TestMessage")
	net.WriteString("Hello, server!")
	net.WriteFloat(3.14)
	net.SendToServer()

	net.Start("TestMessage2")
    net.SendToServer()

	timer.Simple(1, function()
        assert(countReceived == 2, "Did not receive all messages from server!")

		print("net module client tests passed!")
	end)
end
