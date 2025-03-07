TEAM_CONNECTING           = 0
TEAM_UNASSIGNED           = 1001
TEAM_SPECTATOR            = 1002

BOX_FRONT                 = 0
BOX_BACK                  = 1
BOX_RIGHT                 = 2
BOX_LEFT                  = 3
BOX_TOP                   = 4
BOX_BOTTOM                = 5

-- Copy all ACT_* and BUTTON_*/KEY_* enums to the global table
local enumsToCopyToGlobal = {
    ACTIVITY = false,

    BUTTON = function(enum, enumValue)
        -- KEYPAD should be KEY_PAD
        if (enum:StartsWith("KEYPAD")) then
            enum = "KEY_PAD" .. enum:sub(7)
        elseif (enum == "KEY_BRACKET_LEFT") then
            enum = "KEY_LBRACKET"
		elseif (enum == "KEY_BRACKET_RIGHT") then
            enum = "KEY_RBRACKET"
		elseif (enum == "KEY_SHIFT_LEFT") then
            enum = "KEY_LSHIFT"
		elseif (enum == "KEY_SHIFT_RIGHT") then
			enum = "KEY_RSHIFT"
		elseif (enum == "KEY_CONTROL_LEFT") then
			enum = "KEY_LCONTROL"
		elseif (enum == "KEY_CONTROL_RIGHT") then
			enum = "KEY_RCONTROL"
		elseif (enum == "KEY_ALT_LEFT") then
			enum = "KEY_LALT"
		elseif (enum == "KEY_ALT_RIGHT") then
			enum = "KEY_RALT"
		elseif (enum == "KEY_WINDOWS_LEFT") then
			enum = "KEY_LWIN"
		elseif (enum == "KEY_WINDOWS_RIGHT") then
            enum = "KEY_RWIN"
        else
            local replacements = {
                CAPS_LOCK = "CAPSLOCK",
                NUM_LOCK = "NUMLOCK",
                SCROLL_LOCK = "SCROLLLOCK",
				_TOGGLE = "TOGGLE",
            }

			for key, replacement in pairs(replacements) do
				if (enum:EndsWith(key)) then
					enum = enum:sub(1, -key:len()) .. replacement
				end
			end
        end

		return enum, enumValue
	end,
}

for enumKey, modifier in pairs(enumsToCopyToGlobal) do
    if (not _E[enumKey]) then
        error("Missing enumeration table for key: " .. enumKey)
        continue
    end

    for enum, enumValue in pairs(_E[enumKey]) do
		local enumName = enum
        if (modifier) then
            enumName, enumValue = modifier(enum, enumValue)
        end

		_G[enumName] = enumValue
	end
end

local function stripUnderscores(enum)
  return enum:gsub("_", "")
end

local enumsToMergeWithKey = {
  -- OUR_ENUMERATION = "GMOD_ENUM",

	CONTENTS = "CONTENTS",
	COLLISION_GROUP = "COLLISION_GROUP",
	-- ENTITY_EFFECT = "EF", -- See below
	FCVAR = "FCVAR",
	EDICT_FLAG = "FL_EDICT",
	-- ENGINE_FLAG = "FL", -- See below
	SOLID_FLAG = "FSOLID",
	GESTURE_SLOT = "GESTURE_SLOT",
	INPUT = "IN",
	LIFE = "LIFE",
	MASK = "MASK",
	MOVE_COLLIDE = "MOVECOLLIDE",
	MOVE_TYPE = "MOVETYPE",
	OBSERVER_MODE = "OBS_MODE",
	SOLID = "SOLID",
	SOUND_CHANNEL = "CHAN",
	SURFACE = "SURF",

	DAMAGE_TYPE = {"DMG", stripUnderscores},
	HIT_GROUP = {"HITGROUP", stripUnderscores},
	MATERIAL_TYPE = {"MAT", stripUnderscores},
}

for enumKey, enumTargetKey in pairs(enumsToMergeWithKey) do
  if (not _E[enumKey]) then
    error("Missing enumeration table for key: " .. enumKey)
    continue
  end

  local modifier

  if (istable(enumTargetKey)) then
    modifier = enumTargetKey[2]
    enumTargetKey = enumTargetKey[1]
  end

  for enumKey, enumValue in pairs(_E[enumKey]) do
    if (modifier) then
      enumKey = modifier(enumKey)
    end

		_G[enumTargetKey .. "_" .. enumKey] = enumValue
	end
end

if (CLIENT) then
	MATERIAL_CULLMODE_CCW = _E.CULL_MODE.COUNTER_CLOCKWISE
	MATERIAL_CULLMODE_CW = _E.CULL_MODE.CLOCKWISE
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
	NODOCK = _E.DOCK_TYPE.NONE
	FILL = _E.DOCK_TYPE.FILL
	LEFT = _E.DOCK_TYPE.LEFT
	RIGHT = _E.DOCK_TYPE.RIGHT
	TOP = _E.DOCK_TYPE.TOP
	BOTTOM = _E.DOCK_TYPE.BOTTOM

    STENCILCOMPARISONFUNCTION_NEVER = _E.STENCIL_COMPARISON_FUNCTION.NEVER
    STENCILCOMPARISONFUNCTION_LESS = _E.STENCIL_COMPARISON_FUNCTION.LESS
    STENCILCOMPARISONFUNCTION_EQUAL = _E.STENCIL_COMPARISON_FUNCTION.EQUAL
    STENCILCOMPARISONFUNCTION_LESSEQUAL = _E.STENCIL_COMPARISON_FUNCTION.LESS_OR_EQUAL
    STENCILCOMPARISONFUNCTION_GREATER = _E.STENCIL_COMPARISON_FUNCTION.GREATER
    STENCILCOMPARISONFUNCTION_NOTEQUAL = _E.STENCIL_COMPARISON_FUNCTION.NOT_EQUAL
    STENCILCOMPARISONFUNCTION_GREATEREQUAL = _E.STENCIL_COMPARISON_FUNCTION.GREATER_OR_EQUAL
    STENCILCOMPARISONFUNCTION_ALWAYS = _E.STENCIL_COMPARISON_FUNCTION.ALWAYS

    STENCIL_NEVER = _E.STENCIL_COMPARISON_FUNCTION.NEVER
    STENCIL_LESS = _E.STENCIL_COMPARISON_FUNCTION.LESS
    STENCIL_EQUAL = _E.STENCIL_COMPARISON_FUNCTION.EQUAL
    STENCIL_LESSEQUAL = _E.STENCIL_COMPARISON_FUNCTION.LESS_OR_EQUAL
    STENCIL_GREATER = _E.STENCIL_COMPARISON_FUNCTION.GREATER
    STENCIL_NOTEQUAL = _E.STENCIL_COMPARISON_FUNCTION.NOT_EQUAL
    STENCIL_GREATEREQUAL = _E.STENCIL_COMPARISON_FUNCTION.GREATER_OR_EQUAL
    STENCIL_ALWAYS = _E.STENCIL_COMPARISON_FUNCTION.ALWAYS

    STENCILOPERATION_KEEP = _E.STENCIL_OPERATION.KEEP
    STENCILOPERATION_ZERO = _E.STENCIL_OPERATION.ZERO
    STENCILOPERATION_REPLACE = _E.STENCIL_OPERATION.REPLACE
    STENCILOPERATION_INCRSAT = _E.STENCIL_OPERATION.INCREMENT_CLAMP
	STENCILOPERATION_DECRSAT = _E.STENCIL_OPERATION.DECREMENT_CLAMP
	STENCILOPERATION_INVERT = _E.STENCIL_OPERATION.INVERT
	STENCILOPERATION_INCR = _E.STENCIL_OPERATION.INCREMENT_WRAP
	STENCILOPERATION_DECR = _E.STENCIL_OPERATION.DECREMENT_WRAP

    STENCIL_KEEP = _E.STENCIL_OPERATION.KEEP
    STENCIL_ZERO = _E.STENCIL_OPERATION.ZERO
    STENCIL_REPLACE = _E.STENCIL_OPERATION.REPLACE
    STENCIL_INCRSAT = _E.STENCIL_OPERATION.INCREMENT_CLAMP
	STENCIL_DECRSAT = _E.STENCIL_OPERATION.DECREMENT_CLAMP
	STENCIL_INVERT = _E.STENCIL_OPERATION.INVERT
	STENCIL_INCR = _E.STENCIL_OPERATION.INCREMENT_WRAP
    STENCIL_DECR = _E.STENCIL_OPERATION.DECREMENT_WRAP

    RENDERGROUP_STATIC_HUGE = _E.RENDER_GROUP.OPAQUE_STATIC_HUGE
    RENDERGROUP_OPAQUE_HUGE = _E.RENDER_GROUP.OPAQUE_ENTITY_HUGE
    RENDERGROUP_STATIC = _E.RENDER_GROUP.OPAQUE_STATIC
    RENDERGROUP_OPAQUE = _E.RENDER_GROUP.OPAQUE_ENTITY
    RENDERGROUP_TRANSLUCENT = _E.RENDER_GROUP.TRANSLUCENT_ENTITY
    RENDERGROUP_BOTH = _E.RENDER_GROUP.TWOPASS
    RENDERGROUP_VIEWMODEL = _E.RENDER_GROUP.VIEW_MODEL_OPAQUE
    RENDERGROUP_VIEWMODEL_TRANSLUCENT = _E.RENDER_GROUP.VIEW_MODEL_TRANSLUCENT
    RENDERGROUP_OPAQUE_BRUSH = _E.RENDER_GROUP.OPAQUE_BRUSH
	RENDERGROUP_OTHER = _E.RENDER_GROUP.OTHER
elseif (SERVER) then
	CONTINUOUS_USE = _E.USABILITY_TYPE.CONTINUOUS
	ONOFF_USE = _E.USABILITY_TYPE.ON_OFF
	DIRECTIONAL_USE = _E.USABILITY_TYPE.DIRECTIONAL
	SIMPLE_USE = _E.USABILITY_TYPE.IMPULSE
end

TRANSMIT_ALWAYS = _E.EDICT_FLAG.ALWAYS
TRANSMIT_NEVER = _E.EDICT_FLAG.DONTSEND
TRANSMIT_PVS = _E.EDICT_FLAG.PVSCHECK

RENDERMODE_NORMAL = _E.RENDER_MODE.NORMAL
RENDERMODE_TRANSCOLOR = _E.RENDER_MODE.TRANSPARENT_COLOR
RENDERMODE_TRANSTEXTURE = _E.RENDER_MODE.TRANSPARENT_TEXTURE
RENDERMODE_GLOW = _E.RENDER_MODE.GLOW
RENDERMODE_TRANSALPHA = _E.RENDER_MODE.TRANSPARENT_ALPHA
RENDERMODE_TRANSADD = _E.RENDER_MODE.TRANSPARENT_ADD
RENDERMODE_ENVIROMENTAL = _E.RENDER_MODE.ENVIRONMENTAL
RENDERMODE_TRANSADDFRAMEBLEND = _E.RENDER_MODE.TRANSPARENT_ADD_FRAME_BLEND
RENDERMODE_TRANSALPHADD = _E.RENDER_MODE.TRANSPARENT_ALPHA_ADD
RENDERMODE_WORLDGLOW = _E.RENDER_MODE.WORLD_GLOW
RENDERMODE_NONE = _E.RENDER_MODE.NONE

PLAYER_IDLE = _E.PLAYER_ANIMATION.IDLE
PLAYER_WALK = _E.PLAYER_ANIMATION.WALK
PLAYER_JUMP = _E.PLAYER_ANIMATION.JUMP
PLAYER_SUPERJUMP = _E.PLAYER_ANIMATION.SUPER_JUMP
PLAYER_DIE = _E.PLAYER_ANIMATION.DIE
PLAYER_ATTACK1 = _E.PLAYER_ANIMATION.ATTACK1
PLAYER_IN_VEHICLE = _E.PLAYER_ANIMATION.IN_VEHICLE
PLAYER_RELOAD = _E.PLAYER_ANIMATION.RELOAD
PLAYER_START_AIMING = _E.PLAYER_ANIMATION.START_AIMING
PLAYER_LEAVE_AIMING = _E.PLAYER_ANIMATION.LEAVE_AIMING

HUD_PRINTNOTIFY = 1
HUD_PRINTCONSOLE = 2
HUD_PRINTTALK = 3
HUD_PRINTCENTER = 4

TYPE_NONE = -1
TYPE_NIL = 0
TYPE_BOOL = 1
TYPE_LIGHTUSERDATA = 2
TYPE_NUMBER = 3
TYPE_STRING = 4
TYPE_TABLE = 5
TYPE_FUNCTION = 6
TYPE_USERDATA = 7
TYPE_THREAD = 8
TYPE_ENTITY = 9
TYPE_VECTOR = 10
TYPE_ANGLE = 11
TYPE_PHYSOBJ = 12
TYPE_SAVE = 13
TYPE_RESTORE = 14
TYPE_DAMAGEINFO = 15
TYPE_EFFECTDATA = 16
TYPE_MOVEDATA = 17
TYPE_RECIPIENTFILTER = 18
TYPE_USERCMD = 19
TYPE_SCRIPTEDVEHICLE = 20
TYPE_MATERIAL = 21
TYPE_PANEL = 22
TYPE_PARTICLE = 23
TYPE_PARTICLEEMITTER = 24
TYPE_TEXTURE = 25
TYPE_USERMSG = 26
TYPE_CONVAR = 27
TYPE_IMESH = 28
TYPE_MATRIX = 29
TYPE_SOUND = 30
TYPE_PIXELVISHANDLE = 31
TYPE_DLIGHT = 32
TYPE_VIDEO = 33
TYPE_FILE = 34
TYPE_LOCOMOTION = 35
TYPE_PATH = 36
TYPE_NAVAREA = 37
TYPE_SOUNDHANDLE = 38
TYPE_NAVLADDER = 39
TYPE_PARTICLESYSTEM = 40
TYPE_PROJECTEDTEXTURE = 41
TYPE_PHYSCOLLIDE = 42
TYPE_SURFACEINFO = 43
TYPE_COUNT = 44
TYPE_COLOR = 255

SF_CITIZEN_AMMORESUPPLIER = 524288
SF_CITIZEN_FOLLOW = 65536
SF_CITIZEN_IGNORE_SEMAPHORE = 2097152
SF_CITIZEN_MEDIC = 131072
SF_CITIZEN_NOT_COMMANDABLE = 1048576
SF_CITIZEN_RANDOM_HEAD = 262144
SF_CITIZEN_RANDOM_HEAD_FEMALE = 8388608
SF_CITIZEN_RANDOM_HEAD_MALE = 4194304
SF_CITIZEN_USE_RENDER_BOUNDS = 16777216
SF_FLOOR_TURRET_CITIZEN = 512
SF_NPC_ALTCOLLISION = 4096
SF_NPC_ALWAYSTHINK = 1024
SF_NPC_DROP_HEALTHKIT = 8
SF_NPC_FADE_CORPSE = 512
SF_NPC_FALL_TO_GROUND = 4
SF_NPC_GAG = 2
SF_NPC_LONG_RANGE = 256
SF_NPC_NO_PLAYER_PUSHAWAY = 16384
SF_NPC_NO_WEAPON_DROP = 8192
SF_NPC_START_EFFICIENT = 16
SF_NPC_TEMPLATE = 2048
SF_NPC_WAIT_FOR_SCRIPT = 128
SF_NPC_WAIT_TILL_SEEN = 1
SF_PHYSBOX_MOTIONDISABLED = 32768
SF_PHYSBOX_ALWAYS_PICK_UP = 1048576
SF_PHYSBOX_NEVER_PICK_UP = 2097152
SF_PHYSBOX_NEVER_PUNT = 4194304
SF_PHYSPROP_MOTIONDISABLED = 8
SF_PHYSPROP_PREVENT_PICKUP = 512
SF_PHYSPROP_IS_GIB = 4194304
SF_ROLLERMINE_FRIENDLY = 65536
SF_WEAPON_START_CONSTRAINED = 1
SF_WEAPON_NO_PLAYER_PICKUP = 2
SF_WEAPON_NO_PHYSCANNON_PUNT = 4

CT_DEFAULT = 0
CT_DOWNTRODDEN = 1
CT_REFUGEE = 2
CT_REBEL = 3
CT_UNIQUE = 4

TEXFILTER = {
	NONE = 0,
	POINT = 1,
	LINEAR = 2,
	ANISOTROPIC = 3,
}

--[[
	We renamed some of these enums to fit our conventions, so for compatibility let's just
	copy the values from the wiki:
	https://github.com/luttje/glua-api-snippets/blob/lua-language-server-addon/library/enums.lua
--]]

---@enum EF
--- Performs bone merge on client side
EF_BONEMERGE = 1
--- For use with EF_BONEMERGE. If this is set, then it places this ents origin at its parent and uses the parent's bbox + the max extents of the aiment. Otherwise, it sets up the parent's bones every frame to figure out where to place the aiment, which is inefficient because it'll setup the parent's bones even if the parent is not in the PVS.
EF_BONEMERGE_FASTCULL = 128
--- DLIGHT centered at entity origin
EF_BRIGHTLIGHT = 2
--- Player flashlight
EF_DIMLIGHT = 4
--- Don't interpolate the next frame
EF_NOINTERP = 8
--- Disables shadow
EF_NOSHADOW = 16
--- Prevents the entity from drawing and networking.
EF_NODRAW = 32
--- Don't receive shadows
EF_NORECEIVESHADOW = 64
--- Makes the entity blink
EF_ITEM_BLINK = 256
--- Always assume that the parent entity is animating.
EF_PARENT_ANIMATES = 512
--- Internal flag that is set by [Entity:FollowBone](https://wiki.facepunch.com/gmod/Entity:FollowBone).
EF_FOLLOWBONE = 1024
--- Makes the entity not accept being lit by projected textures, including the player's flashlight.
EF_NOFLASHLIGHT = 8192

---@enum EFL
--- This entity is marked for death -- This allows the game to actually delete ents at a safe time.
--- **WARNING**: You should never set this flag manually.
EFL_KILLME = 1
--- Entity is dormant, no updates to client
EFL_DORMANT = 2
--- Lets us know when the noclip command is active
EFL_NOCLIP_ACTIVE = 4
--- Set while a model is setting up its bones
EFL_SETTING_UP_BONES = 8
--- This is a special entity that should not be deleted when we respawn entities via [game.CleanUpMap](https://wiki.facepunch.com/gmod/game.CleanUpMap).
EFL_KEEP_ON_RECREATE_ENTITIES = 16
--- One of the child entities is a player
EFL_HAS_PLAYER_CHILD = 16
--- (Client only) need shadow manager to update the shadow
EFL_DIRTY_SHADOWUPDATE = 32
--- Another entity is watching events on this entity (used by teleport)
EFL_NOTIFY = 64
--- The default behavior in ShouldTransmit is to not send an entity if it doesn't have a model. Certain entities want to be sent anyway because all the drawing logic is in the client DLL. They can set this flag and the engine will transmit them even if they don't have model
EFL_FORCE_CHECK_TRANSMIT = 128
--- This is set on bots that are frozen
EFL_BOT_FROZEN = 256
--- Non-networked entity
EFL_SERVER_ONLY = 512
--- Don't attach the edict
EFL_NO_AUTO_EDICT_ATTACH = 1024
--- Some 'dirty' bits with respect to absolute computations. Used internally by the engine when an entity's absolute position needs to be recalculated.
EFL_DIRTY_ABSTRANSFORM = 2048
--- Some 'dirty' bits with respect to absolute computations. Used internally by the engine when an entity's absolute velocity needs to be recalculated.
EFL_DIRTY_ABSVELOCITY = 4096
--- Some 'dirty' bits with respect to absolute computations. Used internally by the engine when an entity's absolute angular velocity needs to be recalculated.
EFL_DIRTY_ABSANGVELOCITY = 8192
--- Marks the entity as having a 'dirty' surrounding box. Used internally by the engine to recompute the entity's collision bounds.
EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = 16384
--- Used internally by the engine when an entity's "spatial partition" needs to be recalculated.
EFL_DIRTY_SPATIAL_PARTITION = 32768
--- This is set if the entity detects that it's in the skybox. This forces it to pass the "in PVS" for transmission
EFL_IN_SKYBOX = 131072
--- Entities with this flag set show up in the partition even when not solid
EFL_USE_PARTITION_WHEN_NOT_SOLID = 262144
--- Used to determine if an entity is floating
EFL_TOUCHING_FLUID = 524288
--- The entity is currently being lifted by a Barnacle.
EFL_IS_BEING_LIFTED_BY_BARNACLE = 1048576
--- The entity is not affected by 'rotorwash push'--the wind-push effect caused by helicopters close to the ground in Half-Life 2.
EFL_NO_ROTORWASH_PUSH = 2097152
--- Avoid executing the entity's Think
EFL_NO_THINK_FUNCTION = 4194304
--- The entity is currently not simulating any physics.
EFL_NO_GAME_PHYSICS_SIMULATION = 8388608
--- The entity is about to have its untouch callback checked, e.g. when this entity stops touching another entity.
EFL_CHECK_UNTOUCH = 16777216
--- Entity shouldn't block NPC line-of-sight
EFL_DONTBLOCKLOS = 33554432
--- NPCs should not walk on this entity
EFL_DONTWALKON = 67108864
--- The entity shouldn't dissolve
EFL_NO_DISSOLVE = 134217728
--- Mega physcannon can't ragdoll these guys
EFL_NO_MEGAPHYSCANNON_RAGDOLL = 268435456
--- Don't adjust this entity's velocity when transitioning into water
EFL_NO_WATER_VELOCITY_CHANGE = 536870912
--- Physcannon can't pick these up or punt them
EFL_NO_PHYSCANNON_INTERACTION = 1073741824
--- Doesn't accept forces from physics damage
EFL_NO_DAMAGE_FORCES = -2147483648

---@enum FL
--- Is the entity on ground or not
FL_ONGROUND = 1
--- Is player ducking or not
FL_DUCKING = 2
--- Is the player in the process of ducking or standing up
FL_ANIMDUCKING = 4
--- The player is jumping out of water
FL_WATERJUMP = 8
--- This player is controlling a func_train
FL_ONTRAIN = 16
--- Indicates the entity is standing in rain
FL_INRAIN = 32
--- Completely freezes the player
--- Bots will still be able to look around.
FL_FROZEN = 64
--- This player is controlling something UI related in the world, this prevents his movement, but doesn't freeze mouse movement, jumping, etc.
FL_ATCONTROLS = 128
--- Is this entity a player or not
FL_CLIENT = 256
--- Bots have this flag
FL_FAKECLIENT = 512
--- Is the player in water or not
FL_INWATER = 1024
--- This entity can fly
FL_FLY = 2048
--- This entity can swim
FL_SWIM = 4096
--- This entity is a func_conveyor
FL_CONVEYOR = 8192
--- NPCs have this flag (NPC: Ignore player push)
FL_NPC = 16384
--- Whether the player has god mode enabled
FL_GODMODE = 32768
--- Makes the entity invisible to AI
FL_NOTARGET = 65536
--- This entity can be aimed at
FL_AIMTARGET = 131072
--- Not all corners are valid
FL_PARTIALGROUND = 262144
--- It's a static prop
FL_STATICPROP = 524288
--- worldgraph has this ent listed as something that blocks a connection
FL_GRAPHED = 1048576
--- This entity is a grenade, unused
FL_GRENADE = 2097152
--- Changes the SV_Movestep() behavior to not do any processing
FL_STEPMOVEMENT = 4194304
--- Doesn't generate touch functions, calls [ENTITY:EndTouch](https://wiki.facepunch.com/gmod/ENTITY:EndTouch) when this flag gets set during a touch callback
FL_DONTTOUCH = 8388608
--- Base velocity has been applied this frame (used to convert base velocity into momentum)
FL_BASEVELOCITY = 16777216
--- This entity is a brush and part of the world
FL_WORLDBRUSH = 33554432
--- This entity can be seen by NPCs
FL_OBJECT = 67108864
--- This entity is about to get removed
FL_KILLME = 134217728
--- This entity is on fire
FL_ONFIRE = 268435456
--- The entity is currently dissolving
FL_DISSOLVING = 536870912
--- This entity is about to become a ragdoll
FL_TRANSRAGDOLL = 1073741824
--- This moving door can't be blocked by the player
FL_UNBLOCKABLE_BY_PLAYER = -2147483648
