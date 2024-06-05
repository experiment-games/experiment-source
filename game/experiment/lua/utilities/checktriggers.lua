--======== Copyleft � 2010-2011, Team Sandbox, Some rights reserved. ========--
--
-- Purpose: Checks the existence of trigger_changelevel*.
--
--===========================================================================--

local pTrigger = gEntList.FindEntityByClassname( NULL, "trigger_changelevel" )
local tTriggers = {}
while ( pTrigger ~= NULL ) do
  MsgN( pTrigger )
  table.insert( tTriggers, pTrigger )
  pTrigger = gEntList.FindEntityByClassname( pTrigger, "trigger_changelevel" )
end

MsgN( "Found " .. #tTriggers .. " trigger_changelevels!" )

for i, pTrigger in ipairs( tTriggers ) do
	pTrigger:Remove()
end

pTrigger = gEntList.FindEntityByClassname( NULL, "trigger_changelevel_scripted" )
tTriggers = {}
while ( pTrigger ~= NULL ) do
  MsgN( pTrigger )
  table.insert( tTriggers, pTrigger )
  pTrigger = gEntList.FindEntityByClassname( pTrigger, "trigger_changelevel_scripted" )
end

MsgN( "Found " .. #tTriggers .. " trigger_changelevel_scripted!" )
