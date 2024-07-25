TEAM_CONNECTING 	=	0
TEAM_UNASSIGNED 	=	1001
TEAM_SPECTATOR  = 1002

BOX_FRONT = 0
BOX_BACK = 1
BOX_RIGHT = 2
BOX_LEFT = 3
BOX_TOP = 4
BOX_BOTTOM = 5

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

if (CLIENT) then
	NODOCK = DOCK_TYPE.NONE
	FILL = DOCK_TYPE.FILL
	LEFT = DOCK_TYPE.LEFT
	RIGHT = DOCK_TYPE.RIGHT
	TOP = DOCK_TYPE.TOP
	BOTTOM = DOCK_TYPE.BOTTOM
end

FL_ONGROUND = 1
FL_DUCKING = 2
FL_ANIMDUCKING = 4
FL_WATERJUMP = 8
FL_ONTRAIN = 16
FL_INRAIN = 32
FL_FROZEN = 64
FL_ATCONTROLS = 128
FL_CLIENT = 256
FL_FAKECLIENT = 512
FL_INWATER = 1024
FL_FLY = 2048
FL_SWIM = 4096
FL_CONVEYOR = 8192
FL_NPC = 16384
FL_GODMODE = 32768
FL_NOTARGET = 65536
FL_AIMTARGET = 131072
FL_PARTIALGROUND = 262144
FL_STATICPROP = 524288
FL_GRAPHED = 1048576
FL_GRENADE = 2097152
FL_STEPMOVEMENT = 4194304
FL_DONTTOUCH = 8388608
FL_BASEVELOCITY = 16777216
FL_WORLDBRUSH = 33554432
FL_OBJECT = 67108864
FL_KILLME = 134217728
FL_ONFIRE = 268435456
FL_DISSOLVING = 536870912
FL_TRANSRAGDOLL = 1073741824
FL_UNBLOCKABLE_BY_PLAYER = -2147483648

OBS_MODE_NONE = 0
OBS_MODE_DEATHCAM = 1
OBS_MODE_FREEZECAM = 2
OBS_MODE_FIXED = 3
OBS_MODE_IN_EYE = 4
OBS_MODE_CHASE = 5
OBS_MODE_POI = 6
OBS_MODE_ROAMING = 7
