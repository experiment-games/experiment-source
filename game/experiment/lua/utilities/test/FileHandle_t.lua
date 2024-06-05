--======== Copyleft © 2010-2011, Team Sandbox, Some rights reserved. ========--
--
-- Purpose: Tests the FileHandle_t implementation.
--
--===========================================================================--

require("dbg")
require("filesystem")

local printError = debug.PrintError

local hFile = filesystem.Open("gamecontent.txt", "r")
local size = filesystem.Size(hFile)
MsgN("hFile = " .. tostring(hFile))
local bytesRead, pOutput = filesystem.Read(size, hFile)
MsgN("bytes: " .. bytesRead)
MsgN("output:\n" .. pOutput)
MsgN("Closing hFile, handle should become invalid!")
filesystem.Close(hFile)
MsgN("hFile = " .. tostring(hFile))

MsgN("Testing error handling...")
local status, strError = xpcall(function()
	MsgN("filesystem.IsOk( hFile ) = " .. tostring(filesystem.IsOk(hFile)))
end, printError)

if (status == false) then
	printError(strError .. "\n")
else
	error("WARNING: test did not fail as expected on line 32!", 2)
end

MsgN("Testing error handling with non-filehandle type...")
local status, strError = xpcall(function()
	MsgN("filesystem.IsOk( hFile ) = " .. tostring(filesystem.IsOk(-1)))
end, printError)

if (status == false) then
	printError(strError .. "\n")
else
	error("WARNING: test did not fail as expected on line 47!", 2)
end
