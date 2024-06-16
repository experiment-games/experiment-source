TEAM_CONNECTING 	=	0
TEAM_UNASSIGNED 	=	1001
TEAM_SPECTATOR 		=	1002

-- Copy all ACT_* enums to the global table
for enum, enumValue in pairs(_E.Activity) do
	_G[enum] = enumValue
end

local enumsToMergeWithKey = {
	"CONTENTS",
	"FCVAR",
	"IN",
	"MASK",
	"SURF",
}

for _, enumKey in ipairs(enumsToMergeWithKey) do
    if (not _E[enumKey]) then
        error("Missing enumeration table for key: " .. enumKey)
        continue
    end

	for enum, enumValue in pairs(_E[enumKey]) do
		_G[enumKey .. "_" .. enum] = enumValue
	end
end
