--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.

	Purpose: Dump weapon data from WeaponInfo_t.
--]]

for k, v in pairs(Players.FindByIndex(1):GetActiveWeapon():GetWpnData()) do
	if (type(v) == "table") then
		MsgN(k .. ":")
		for l, w in pairs(v) do
			MsgN("\t" .. l, w)
		end
	else
		MsgN(k, v)
	end
end
