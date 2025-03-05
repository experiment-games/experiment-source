local baseLoader = Include("base.lua") -- Garry's Mod compatibility.

return {
	--- @param fileContent string
	--- @param filePath string
	--- @return string
    PreProcessFile = function(fileContent, filePath)
        fileContent = baseLoader.PreProcessFile(fileContent, filePath)

        if (not filePath:match("gamemode[/\\]cl_init%.lua$") and not filePath:match("gamemode[/\\]init%.lua$")) then
            return fileContent
        end

		-- "super" is our default, but Garry's Mod uses "base" by default. So gamemodes not inheriting explicitly from "super" will inherit from "base" instead.
        -- For that reason derive from "base" instead of "super" unless this is the "base" gamemode (which derives from "super" by default).
        local isBaseGamemode = filePath:match("[/\\]base[/\\]gamemode[/\\]cl_init%.lua$")
			or filePath:match("[/\\]base[/\\]gamemode[/\\]init%.lua$")

		if (isBaseGamemode) then
			return fileContent
		end

		-- Only sandbox will inherit from "base" explicitly
        local isSandboxGamemode = filePath:match("[/\\]sandbox[/\\]gamemode[/\\]cl_init%.lua$")
            or filePath:match("[/\\]sandbox[/\\]gamemode[/\\]init%.lua$")

        if (isSandboxGamemode) then
            return "InheritGamemode(\"base\")\n\n"
                .. fileContent
        end

        -- We'll let other gamemodes (that derive from sandbox) specify their own inheritance.
		-- This way any gamemode loader for a gamemode that inherits from sandbox can just 'return include("loaders/sandbox.lua")' and it will work.
		return fileContent
	end
}
