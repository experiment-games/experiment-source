local MODULE = {}
local printError = debug.PrintError

MODULE.registeredTimers = MODULE.registeredTimers or {}

function MODULE.Create(id, delay, repetitions, callback)
    MODULE.registeredTimers[id] = {
        delay = delay,
        repetitions = repetitions,
        callback = callback,
        nextTick = Engines.GetCurrentTime() + delay
    }
end

function MODULE.Exists(id)
	return MODULE.registeredTimers[id] ~= nil
end

function MODULE.Remove(id)
    MODULE.registeredTimers[id] = nil
end

function MODULE.Simple(delay, callback)
    local id = "simple_" .. tostring({})
    MODULE.Create(id, delay, 1, callback)
    return id
end

function MODULE.Adjust(id, delay, repetitions, callback)
	local timer = MODULE.registeredTimers[id]

	if (timer == nil) then
		return false
	end

	timer.delay = delay
	timer.repetitions = repetitions
	timer.callback = callback
	timer.nextTick = Engines.GetCurrentTime() + delay

	return true
end

function MODULE.Check()
	return false
end

function MODULE.Destroy(id)
    MODULE.Remove(id)
end

function MODULE.Pause(id)
    local timer = MODULE.registeredTimers[id]

    if (timer == nil) then
        return false
    end

    timer.paused = true

    return true
end

function MODULE.RepetitionsLeft(id)
    local timer = MODULE.registeredTimers[id]

    if (timer == nil) then
        return 0
    end

    return timer.repetitions
end

function MODULE.Start(id)
    local timer = MODULE.registeredTimers[id]

    if (timer == nil) then
        return false
    end

    timer.nextTick = Engines.GetCurrentTime() + timer.delay

    return true
end

function MODULE.Stop(id)
	local timer = MODULE.registeredTimers[id]

	if (timer == nil) then
		return false
	end

	timer.nextTick = 0

	return true
end

function MODULE.TimeLeft(id)
    local timer = MODULE.registeredTimers[id]

    if (timer == nil) then
        return 0
    end

    return timer.nextTick - Engines.GetCurrentTime()
end

function MODULE.Toggle(id)
	local timer = MODULE.registeredTimers[id]

	if (timer == nil) then
		return false
	end

	if (timer.paused) then
		MODULE.UnPause(id)
	else
		MODULE.Pause(id)
	end

	return true
end

function MODULE.UnPause(id)
    local timer = MODULE.registeredTimers[id]

    if (timer == nil) then
        return false
    end

    timer.paused = false

    return true
end

function MODULE.Tick()
	local curTime = Engines.GetCurrentTime()

	for id, timer in pairs(MODULE.registeredTimers) do
		if (curTime >= timer.nextTick and not timer.paused) then
			xpcall(timer.callback, printError)

			-- If repetitions was already 0 (or -1) this will result in the
			-- timer continuing to run indefinitely. This is the intended behavior.
			timer.repetitions = timer.repetitions - 1

			if (timer.repetitions == 0) then
				MODULE.Remove(id)
			else
				timer.nextTick = curTime + timer.delay
			end
		end
	end
end

Hooks.Add("Think", "TimerTick", MODULE.Tick)

return MODULE
