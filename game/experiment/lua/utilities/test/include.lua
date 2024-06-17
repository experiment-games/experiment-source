SOME_GLOBAL_VALUE = "test"

local testData, fileName = Include("_data.lua")

assert(testData["test"] == "abcdefghijklmnopqrstuvwxyz")
assert(testData["global"] == SOME_GLOBAL_VALUE)
