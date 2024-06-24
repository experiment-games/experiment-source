--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

if (not CLIENT) then
	return
end

require("Gui")

-- Private list of helpers
local registeredHelpers = {}

local function getAppropriateBaseParent()
	if (GAMEUI) then
		return VGui_GetGameUIPanel()
	end

	return VGui_GetClientLuaRootPanel()
end

local function createHelper(panelName)
    Gui[panelName] = function(parentPanel, name)
        parentPanel = parentPanel or getAppropriateBaseParent()

        local helper = registeredHelpers[panelName]
		local panel = Gui[helper.Base](parentPanel, name or panelName)

		table.Merge(panel:GetRefTable(), helper)

		panel.BaseClass = registeredHelpers[helper.Base]

        panel:Init()

		return panel
	end
end

--- Registers a panel and creates a factory for it in the Gui namespace
--- @param panelTable Panel table object
--- @param panelName Name of the panel
--- @param baseClassName Name of the base class
function Gui.Register(panelTable, panelName, baseClassName)
    if (Gui[baseClassName] == nil) then
        error(
        "attempt to register panel class \"" ..
        tostring(panelName) .. "\" with non-existing base class \"" .. tostring(baseClassName) .. "\"", 2)
    end

	panelTable.__classname = panelName
	panelTable.Base = baseClassName

	registeredHelpers[panelName] = panelTable

	createHelper(panelName)
end

--- Registers a panel and creates a factory for it in the Gui namespace,
--- but uses a metatable for inheritance, to allow out-of-order registration
--- @param panelTable Panel table object
--- @param panelName Name of the panel
--- @param baseClassName Name of the base class
function Gui.RegisterWithMetatable(panelTable, panelName, baseClassName)
	-- __index to allow inheritance
	panelTable.__classname = panelName
	panelTable.Base = baseClassName

	registeredHelpers[panelName] = panelTable

    setmetatable(panelTable, {
		__index = function(self, key)
            local rawValue = rawget(self, key)

            if (key == "Base") then
                local baseClassName = rawValue

                if (Gui[baseClassName] == nil) then
                    error(
                        "attempt to register panel class \"" ..
                        tostring(panelName) .. "\" with non-existing base class \"" .. tostring(baseClassName) .. "\"", 2)
                end

                return Gui[baseClassName]
            end

			return rawValue
		end
    })

	createHelper(panelName)
end

-- gBuildMenuInterface = Gui.CBuildMenu(VGui_GetClientLuaRootPanel(), "build")
--- Creates a new panel with the given name and base class
--- @param panelName string Class name of the panel
--- @param parentPanel? Panel The parent panel to attach the new panel to
--- @param name? string An optional name for the panel (default is panelName)
--- @return Panel The created panel
function Gui.Create(panelName, parentPanel, name)
	local helper = registeredHelpers[panelName]
	parentPanel = parentPanel or getAppropriateBaseParent()

    if (helper ~= nil) then
        local panel = Gui[helper.Base](parentPanel, name or panelName)

        table.Merge(panel:GetRefTable(), helper)

        panel.BaseClass = registeredHelpers[helper.Base]

        panel:Init()

        return panel
    end

	if (not Gui[panelName]) then
		error("attempt to create non-existing panel class \"" .. tostring(panelName) .. "\"", 2)
	end

	return Gui[panelName](parentPanel, name or panelName)
end

--[[
	Enumerations

	TODO: Move these to C-side so they don't misalign.
--]]

--- @enum CURSOR_CODE
CURSOR_CODE = {
	USER = 0,
	NONE = 1,
	ARROW = 2,
	I_BEAM = 3,
	HOURGLASS = 4,
	WAIT_ARROW = 5,
	CROSSHAIR = 6,
	UP = 7,
	SIZE_NWSE = 8,
	SIZE_NESW = 9,
	SIZE_WE = 10,
	SIZE_NS = 11,
	SIZE_ALL = 12,
	NO = 13,
	HAND = 14,
	BLANK = 15,
	LAST = 16,
	ALWAYS_VISIBLE_PUSH = 17,
	ALWAYS_VISIBLE_POP = 18
}
