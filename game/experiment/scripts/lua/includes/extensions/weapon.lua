--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

if (GAMEUI) then
	return
end

_R.Weapon.__WeaponSound = _R.Weapon.WeaponSound
local WeaponSound = _R.Weapon.WeaponSound
local Globals = Globals

-- HACKHACK: We override this here, because for some reason scripted weapons
-- don't properly play weapon sounds in singleplayer
function _R.Weapon.WeaponSound( self, sound_type, soundtime )
  if ( CLIENT ) then
    return
  end

  -- Only the player fires this way so we can cast
  local client = self:GetOwner()

  -- if ( Engines.GetMaxClients() > 1 ) then
  --   WeaponSound( self, sound_type, soundtime )
  -- else
    local shootsound = self:GetShootSound( sound_type )
    if ( client ~= NULL ) then
      client:EmitSound( shootsound )
    else
      self:EmitSound( shootsound )
    end
  -- end
end
