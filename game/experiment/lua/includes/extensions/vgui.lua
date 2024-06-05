--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

if (not CLIENT) then
	return
end

require("vgui")

local error = error
local table = table

-- Private list of helpers
local registeredHelpers = {}

--- Registers a panel and creates a factory for it in the vgui namespace
--- @param panelTable Panel table object
--- @param panelName Name of the panel
--- @param baseClassName Name of the base class
function vgui.Register(panelTable, panelName, baseClassName)
	if (vgui[panelName] ~= nil) then
		error("attempt to register existing panel class \"" .. panelName .. "\"", 2)
	end

	if (vgui[baseClassName] == nil) then
		error("attempt to register panel class with non-existing base class", 2)
	end

	panelTable.__classname = panelName
	panelTable.Base = baseClassName

	registeredHelpers[panelName] = panelTable

	vgui[panelName] = function(...)
		local helper = registeredHelpers[panelName]
		local panel = vgui[helper.Base](...)

		table.Merge(panel:GetRefTable(), helper)

		panel.BaseClass = registeredHelpers[helper.Base]

		panel:Init(...)

		return panel
	end
end
