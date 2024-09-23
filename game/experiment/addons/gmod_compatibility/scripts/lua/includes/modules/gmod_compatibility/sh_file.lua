local Files = require("Files")

file = {}

-- Appends content to a file relative to the data folder
function file.Append(fileName, content)
    local file = Files.Open(fileName, "a", "DATA")

	Files.Write(content, file)
	Files.Close(file)
end

-- Reads the content of a file asynchronously
function file.AsyncRead(fileName, gamePath, callback, sync)
	callback(file.Read(fileName, gamePath)) -- TODO: Implement async file reading
end

-- Creates a directory relative to the data folder
function file.CreateDir(name)
    Files.CreateDirectoryHierarchy(name, "DATA")
end

-- Deletes a file or empty folder relative to the data folder
function file.Delete(name, gamePath)
    Files.RemoveFile(name, gamePath or "DATA")
end

-- Checks if a file or directory exists
function file.Exists(name, gamePath)
    return Files.FileExists(name, gamePath or "DATA")
end

-- Returns a list of files and directories inside a folder
function file.Find(name, path, sorting)
    local files, directories = Files.Find(name, path)

    if (sorting) then
        MsgN("file.Find Sorting is not supported in the GMod compatibility layer!")
    end

	return files, directories
end

-- Checks if the given path is a directory
function file.IsDir(fileName, gamePath)
    return Files.IsDirectory(fileName, gamePath or "DATA")
end

-- Opens a file with the given mode
function file.Open(fileName, fileMode, gamePath)
    return Files.Open(fileName, fileMode, gamePath or "DATA")
end

-- Reads the content of a file
function file.Read(fileName, gamePath)
    local file = Files.Open(fileName, "r", gamePath)

    if (file == FILESYSTEM_INVALID_HANDLE) then
        return nil
    end

	local size = Files.Size(file)
	local size, content = Files.Read(size, file)
	Files.Close(file)
	return content
end

-- Renames a file
function file.Rename(originalFileName, targetFileName)
    return Files.RenameFile(originalFileName, targetFileName, "DATA")
end

-- Returns the size of a file in bytes
function file.Size(fileName, gamePath)
    return Files.Size(fileName, gamePath or "DATA")
end

-- Returns the last modified time of a file or folder in Unix time
function file.Time(path, gamePath)
    return Files.Time(path, gamePath or "DATA")
end

-- Writes content to a file, erasing previous data
function file.Write(fileName, content)
    local file = Files.Open(fileName, "w", "DATA")

	Files.Write(content, file)
	Files.Close(file)
end
