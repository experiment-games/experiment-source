--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

require("dbg")
require("filesystem")

local printError = debug.PrintError

local hFile = filesystem.Open("gameinfo.txt", "r")
-- ...ourcemods\experiment\lua\utilities\test\FileHandle_t.lua:14: bad argument #1 to 'Size' (FileHandle_t expected, got FILESYSTEM_INVALID_HANDLE)
local size = filesystem.Size(hFile)
MsgN("hFile = " .. tostring(hFile))
local bytesRead, pOutput = filesystem.Read(size, hFile)
MsgN("bytes: " .. bytesRead)
MsgN("output:\n" .. pOutput)
MsgN("Closing hFile, handle should become invalid!")
filesystem.Close(hFile)
MsgN("hFile = " .. tostring(hFile))

MsgN("Testing error handling...")
local failedTest = xpcall(function(strError)
	MsgN("filesystem.IsOk( hFile ) = " .. tostring(filesystem.IsOk(hFile)))
end, printError)

if (failedTest) then
	error("WARNING: test did not fail as expected on line 32!", 2)
end

MsgN("Testing error handling with non-filehandle type...")
local failedTest = xpcall(function(strError)
	MsgN("filesystem.IsOk( hFile ) = " .. tostring(filesystem.IsOk(-1)))
end, printError)

if (failedTest) then
	error("WARNING: test did not fail as expected on line 47!", 2)
end
