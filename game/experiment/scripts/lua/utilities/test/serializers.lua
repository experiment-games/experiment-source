--[[
	Tests for the Serializers library
--]]

--[[
	CRC32
--]]

-- TODO: test CRC32

--[[
	Base64
--]]

-- TODO: test Base64

--[[
	JSON
--]]

-- TODO: test JSON

--[[
	LZMA
--]]

local rawText =
	"Lorem ipsum dolor sit amet, consectetur adipiscing elit\nSpecial characters: !@#$%^&*()_+{}|:\"<>?`-=[]\\;',./ cool\n yay"
local lzmaCompressed = Serializers.LzmaCompress(rawText)
local lzmaDecompressed = Serializers.LzmaDecompress(lzmaCompressed)

print("LZMA Compressed: " .. lzmaCompressed)
print("LZMA Decompressed: " .. lzmaDecompressed)

assert(lzmaDecompressed == rawText, "LZMA decompressed text does not match raw text")
assert(lzmaCompressed ~= rawText, "LZMA compressed text should not match raw text")
