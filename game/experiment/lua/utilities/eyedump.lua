--======== Copyleft � 2010-2011, Team Sandbox, Some rights reserved. ========--
--
-- Purpose: Dump our eye origin and angles.
--
--===========================================================================--

hook.Add( "CalcPlayerView", "DumpPlayerView", function( pPlayer, eyeOrigin, eyeAngles, fov )
  MsgN( "eyeOrigin: " .. tostring( eyeOrigin ) )
  MsgN( "eyeAngles: " .. tostring( eyeAngles ) )
  MsgN( "fov: " .. tostring( fov ) )
  hook.Remove( "CalcPlayerView", "CalcPlayerView" )
end )
