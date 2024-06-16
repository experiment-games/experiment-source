local filesystem = require("filesystem")

file = {}

-- Appends content to a file relative to the data folder
function file.Append(fileName, content)
    local file = filesystem.Open(fileName, "a", "DATA")

    if (file) then
        filesystem.Write(content, file)
        filesystem.Close(file)
    end
end

-- Reads the content of a file asynchronously
function file.AsyncRead(fileName, gamePath, callback, sync)
	callback(file.Read(fileName, gamePath)) -- TODO: Implement async file reading
end

-- Creates a directory relative to the data folder
function file.CreateDir(name)
    filesystem.CreateDirHierarchy(name, "DATA")
end

-- Deletes a file or empty folder relative to the data folder
function file.Delete(name, gamePath)
    filesystem.RemoveFile(name, gamePath or "DATA")
end

-- Checks if a file or directory exists
function file.Exists(name, gamePath)
    return filesystem.FileExists(name, gamePath or "DATA")
end

-- Returns a list of files and directories inside a folder
function file.Find(name, path, sorting)
    local files = filesystem.Find(name, path)

    if (sorting) then
        MsgN("file.Find Sorting is not supported in the GMod compatibility layer!")
    end

	return files
end

-- Checks if the given path is a directory
function file.IsDir(fileName, gamePath)
    return filesystem.IsDirectory(fileName, gamePath or "DATA")
end

-- Opens a file with the given mode
function file.Open(fileName, fileMode, gamePath)
    return filesystem.Open(fileName, fileMode, gamePath or "DATA")
end

-- Reads the content of a file
function file.Read(fileName, gamePath)
    local file = filesystem.Open(fileName, "r", gamePath)
	local size = filesystem.Size(fileName, gamePath)
	local size, content = filesystem.Read(size, file)
	filesystem.Close(file)
	return content
end

-- Renames a file
function file.Rename(originalFileName, targetFileName)
    return filesystem.RenameFile(originalFileName, targetFileName, "DATA")
end

-- Returns the size of a file in bytes
function file.Size(fileName, gamePath)
    return filesystem.Size(fileName, gamePath or "DATA")
end

-- Returns the last modified time of a file or folder in Unix time
function file.Time(path, gamePath)
    return filesystem.Time(path, gamePath or "DATA")
end

-- Writes content to a file, erasing previous data
function file.Write(fileName, content)
    local file = filesystem.Open(fileName, "w", "DATA")

    if (not file) then
		error("file.Write: Couldn't open file '" .. fileName .. "' for writing!")
        return
    end

	filesystem.Write(content, file)
	filesystem.Close(file)
end
