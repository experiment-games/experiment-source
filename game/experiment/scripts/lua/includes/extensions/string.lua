require("string")

--- Removes the extension from a file path.
--- @param path string The file path
--- @return string # The file path without the extension
function string.StripExtension(path)
	return path:match("(.+)%..+") or path
end

--- Escapes characters that would match Lua's pattern matching.
--- @param target string The string to escape
--- @return string # The escaped string
function string.PatternSafe(target)
	return target:gsub("[%(%)%.%%%+%-%*%?%[%]%^%$]", "%%%1")
end

--- Trims the beginning of a string.
--- @param target string The string to trim
--- @param characters? string The characters to trim
--- @return string # The trimmed string
function string.TrimLeft(target, characters)
    if (characters) then
		characters = string.PatternSafe(characters)
        return target:match("^[" .. characters .. "]+(.+)$") or target
    end

    return target:match("^%s*(.+)$") or target
end

--- Trims the end of a string.
--- @param target string The string to trim
--- @param characters? string The characters to trim
--- @return string # The trimmed string
function string.TrimRight(target, characters)
	if (characters) then
		characters = string.PatternSafe(characters)
		return target:match("^(.-)[" .. characters .. "]+$") or target
	end

	return target:match("^(.+)%s*$") or target
end

--- Trims the beginning and end of a string.
--- @param target string The string to trim
--- @param characters? string The characters to trim
--- @return string # The trimmed string
function string.Trim(target, characters)
	return string.TrimRight(string.TrimLeft(target, characters), characters)
end

--- Checks if a string starts with a given string.
--- @param target string The string to check
--- @param start string The string to check for
--- @return boolean # Whether the string starts with the given string
function string.StartsWith(target, start)
    return target:sub(1, start:len()) == start
end

--- Checks if a string ends with a given string.
--- @param target string The string to check
--- @param ending string The string to check for
--- @return boolean # Whether the string ends with the given string
function string.EndsWith(target, ending)
    return target:sub(-ending:len()) == ending
end
