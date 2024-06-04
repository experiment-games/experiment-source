--[[
	Original code by Team Sandbox:
		Copyleft © 2010, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local type = type

function ToPanel( pPanel )
  if ( not pPanel or type( pPanel ) ~= "panel" ) then
    return INVALID_PANEL;
  end

  local success, hPanel = pcall( _R.Panel.GetVPanel, pPanel )
  if ( not success ) then
    hPanel = INVALID_PANEL
  end
if _DEBUG then
  assert( hPanel ~= INVALID_PANEL );
end

  return hPanel;
end
