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

        -- Derive from "base" instead of "super" (unless this is the "base" gamemode)
        local isBaseGamemode = filePath:match("[/\\]base[/\\]gamemode[/\\]cl_init%.lua$")
			or filePath:match("[/\\]base[/\\]gamemode[/\\]init%.lua$")

		if (isBaseGamemode) then
			return fileContent
		end

        return "InheritGamemode(\"base\")\n\n"
			.. fileContent
	end
}
