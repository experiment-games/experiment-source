-- lua_dofile utilities/test/include.lua

SOME_GLOBAL_VALUE = "test"

local testData, fileName = Include("_data.lua")

print(testData, fileName)

assert(testData["test"] == "abcdefghijklmnopqrstuvwxyz")
assert(testData["global"] == SOME_GLOBAL_VALUE)
