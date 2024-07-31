include("testframework.lua")
include("setup.lua")


local files = file.Find("gmod_minimal/gamemode/mysqloo_tests/tests/*.lua", "LUA")

for _,f in pairs(files) do
	include("tests/" .. f)
end
