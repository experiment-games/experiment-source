local MODULE = {}

local metaMethods = {
	["__tostring"] = true,
	["__eq"] = true,
	["__gc"] = true,
	["__mode"] = true,
	["__metatable"] = true,
	["__index"] = true,
	["__newindex"] = true,
	["__call"] = true,
	["__len"] = true,
	["__pairs"] = true,
	["__ipairs"] = true,
	["__unm"] = true,
	["__add"] = true,
	["__sub"] = true,
	["__mul"] = true,
	["__div"] = true,
	["__mod"] = true,
	["__pow"] = true,
	["__concat"] = true,
	["__lt"] = true,
	["__le"] = true,
}

MODULE.METAMETHODS = metaMethods

--- TODO: Clean up this mess we have surrounding metatables and inheritance.
function MODULE.SetupSpecialInheritance(meta, metaToInherit)
    -- The newindex will be the same as the base metatable.
    meta.__newindex = metaToInherit.__newindex
    meta.__index = function(self, key)
        -- Search the metatable first.
        local value = meta[key]
        if (value ~= nil) then
            return value
        end

        -- Search the metatable to inherit.
        local valueToInherit = metaToInherit[key]

        if (valueToInherit ~= nil) then
            return valueToInherit
        end

        -- If there's a GetRefTable function, use it and see if it has the key.
        local refTableFunction = meta.GetRefTable or metaToInherit.GetRefTable

        if (refTableFunction) then
            local refTable = refTableFunction(self)

            if (refTable) then
                return refTable[key]
            end
        end

        return nil
    end
end

local function collapse(base, handleMetamethods, ...)
	local target = base

    for _, metatableToMergeFrom in ipairs({ ... }) do
        if (not metatableToMergeFrom) then
            continue
        end

        for key, value in pairs(metatableToMergeFrom) do
            -- Only the 'Entity' baseclass should have metamethods.
            if (metaMethods[key]) then
                if (handleMetamethods == 'complain') then
                    debug.PrintError("Attempted to merge a metatable with a key ("
                        .. key .. ") that is a metamethod.")
					continue
                elseif (handleMetamethods == 'skip') then
					continue
                end
            end

            -- Skip __name and __type so we don't overwrite the name of the metatable.
            if (key == "__name" or key == "__type") then
                continue
            end

            if (type(target[key]) == "table" and type(value) == "table") then
                table.Merge(target[key], value)
            elseif (target[key] ~= nil) then
                -- Catch mistakes where we double defined methods
                debug.PrintError("Attempted to merge a metatable (" .. tostring(target)
					.. ") with a key (" .. key .. ") that already exists.")
                continue
            else
                target[key] = value
            end
        end
    end

    return target
end

--- Merges the provided metatables into base and returns it.
--- This will ensure that you can call subclass methods on the base class.
--- This can be useful for NULL players for example, since you will be able
--- to call Player methods on them.
--- @param base any
--- @param ... unknown
--- @return table # The metatable merged into.
function MODULE.Collapse(base, ...)
    return collapse(base, 'complain', ...)
end

function MODULE.CollapseSkipMetamethods(base, ...)
    return collapse(base, 'skip', ...)
end

return MODULE
