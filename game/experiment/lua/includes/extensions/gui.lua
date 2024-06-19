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

	Gui[panelName] = function(...)
        local helper = registeredHelpers[panelName]
		local panel = Gui[helper.Base](...)

		table.Merge(panel:GetRefTable(), helper)

		panel.BaseClass = registeredHelpers[helper.Base]

		panel:Init(...)

		return panel
	end
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
end
