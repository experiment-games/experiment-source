--[[
	Original code by Team Sandbox:
		Copyleft © 2010, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

local type = type
local printError = debug.PrintError

function ToBaseEntity( pEntity )
  if ( not pEntity or type( pEntity ) ~= "entity" ) then
    return NULL
  end

  local success, hEntity = xpcall( _R.CBaseEntity.GetBaseEntity, printError, pEntity )
  if ( not success ) then
    hEntity = NULL
  end
if _DEBUG then
  assert( hEntity ~= NULL )
end

  return hEntity
end
