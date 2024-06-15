timer = timer or {}
local MODULE = timer

MODULE.registeredTimers = MODULE.registeredTimers or {}

function MODULE.Create(id, delay, repetitions, callback)
    MODULE.registeredTimers[id] = {
        delay = delay,
        repetitions = repetitions,
        callback = callback,
        nextTick = gpGlobals.curtime() + delay
    }
end

function MODULE.Remove(id)
    MODULE.registeredTimers[id] = nil
end

function MODULE.Simple(delay, callback)
    local id = "simple_" .. tostring({})
    MODULE.Create(id, delay, 1, callback)
    return id
end

function MODULE.Tick()
    local curTime = gpGlobals.curtime()

    for id, timer in pairs(MODULE.registeredTimers) do
        if (curTime >= timer.nextTick) then
			xpcall(timer.callback, function(err)
				print("Timer error: " .. err)
			end)

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

hooks.Add("Think", "TimerTick", MODULE.Tick)
