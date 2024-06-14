-- lua_dofile utilities/test/net.lua;lua_dofile_cl utilities/test/net.lua;

local debug = function(...)
    print("[NetModuleTest]", ...)
end

if (SERVER) then
    net.Receive("TestMessage", function(length, client)
        local message = net.ReadString()
		local num = net.ReadFloat()
        debug("Received message from client: " .. message, "Number:", num)

        net.Start("CTestMessage")
        net.WriteString("Hello, client!")
        net.Send(client)
    end)

    net.Receive("TestMessage2", function(length, client)
        local message = net.ReadString()
        debug("Received message from client: " .. message)

        net.Start("CTestMessage2")
        net.WriteString("Hello, client!")
        net.Send(client)
    end)
end

if (CLIENT) then
	net.Receive("CTestMessage", function(length, client)
		local message = net.ReadString()
		debug("Received message from server: " .. message)
	end)

	net.Receive("CTestMessage2", function(length, client)
		local message = net.ReadString()
		debug("Received message from server: " .. message)
	end)

	net.Start("TestMessage")
	net.WriteString("Hello, server!")
	net.WriteFloat(3.14)
	net.SendToServer()

	net.Start("TestMessage2")
	net.WriteString("Hello, server!")
	net.SendToServer()
end
