--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local type = type
local printError = debug.PrintError

function ToPanel( pPanel )
  if ( not pPanel or type( pPanel ) ~= "Panel" ) then
    return INVALID_PANEL
  end

  local success, hPanel = xpcall( _R.Panel.GetVPanel, printError, pPanel )
  if ( not success ) then
    hPanel = INVALID_PANEL
  end
if _DEBUG then
  assert( hPanel ~= INVALID_PANEL )
end

  return hPanel
end
