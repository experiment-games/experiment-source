TEAM_CONNECTING 	=	0
TEAM_UNASSIGNED 	=	1001
TEAM_SPECTATOR 		=	1002

-- Copy all ACT_* and BUTTON_*/KEY_* enums to the global table
local enumsToCopyToGlobal = {
	"Activity",
	"BUTTON",
}

for _, enumKey in ipairs(enumsToCopyToGlobal) do
	if (not _E[enumKey]) then
		error("Missing enumeration table for key: " .. enumKey)
		continue
	end

	for enum, enumValue in pairs(_E[enumKey]) do
		_G[enum] = enumValue
	end
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

--[[
We don't have the Kinect SDK
--]]
SENSORBONE = {
	SHOULDER_RIGHT = 8,
	SHOULDER_LEFT = 4,
	HIP = 0,
	ELBOW_RIGHT = 9,
	KNEE_RIGHT = 17,
	WRIST_RIGHT = 10,
	ANKLE_LEFT = 14,
	FOOT_LEFT = 15,
	WRIST_LEFT = 6,
	FOOT_RIGHT = 19,
	HAND_RIGHT = 11,
	SHOULDER = 2,
	HIP_LEFT = 12,
	HIP_RIGHT = 16,
	HAND_LEFT = 7,
	ANKLE_RIGHT = 18,
	SPINE = 1,
	ELBOW_LEFT = 5,
	KNEE_LEFT = 13,
	HEAD = 3,
}
