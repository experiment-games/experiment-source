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
	if (Gui[panelName] ~= nil) then
		error("attempt to register existing panel class \"" .. panelName .. "\"", 2)
	end

	if (Gui[baseClassName] == nil) then
		error("attempt to register panel class with non-existing base class", 2)
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
