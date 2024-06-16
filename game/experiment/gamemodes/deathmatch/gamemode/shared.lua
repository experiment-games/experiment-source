--[[
	Original code by Team Sandbox:
		Copyleft © 2010 - 2013, Team Sandbox, Some rights reserved.

	Modified for Experiment.
--]]

GM.Name = "Deathmatch"
GM.Homepage = "http://www.steampowered.com/"
GM.Developer = "Valve"
GM.Manual = nil

function GM:Initialize()
	self.m_bTeamPlayEnabled = cvars.FindVar("mp_teamplay"):GetBool()

	self.BaseClass.Initialize(self)
end

function GM:GetGameDescription()
	if (self:IsTeamplay()) then
		return "Team " .. self.Name
	end

	return self.Name
end

function GM:GetMapRemainingTime()
	local timelimit = cvars.FindVar("mp_timelimit"):GetInt()

	-- If timelimit is disabled, return 0
	if (timelimit <= 0) then
		return 0
	end

	local time = gpGlobals.curtime() - self:GetRoundStartTime()

	return math.max(0, timelimit * 60 - time)
end

function GM:IsTeamplay()
	return self.m_bTeamPlayEnabled
end

function GM:Precache()
	_R.CBaseEntity.PrecacheScriptSound("AlyxEmp.Charge")

	self.BaseClass.Precache(self)
end
