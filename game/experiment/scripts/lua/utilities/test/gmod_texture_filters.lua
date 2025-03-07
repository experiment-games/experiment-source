--[[
	Source: https://wiki.facepunch.com/gmod/render_min_mag_filters#usageexample

	lua_openscript_cl utilities/test/gmod_texture_filters.lua
--]]

-- Calling Material() every frame is quite expensive
-- So we call it once, outside of any hooks, and cache the result in a local variable
local ourMat = Material("silkicons/anchor.png")
local sizeBig = 500
local sizeSmall = 100
local pos = 100

local filters = {}
filters[TEXFILTER.NONE] = "NONE"
filters[TEXFILTER.POINT] = "POINT"
filters[TEXFILTER.LINEAR] = "LINEAR"
filters[TEXFILTER.ANISOTROPIC] = "ANISOTROPIC"

hook.Add("HudDraw", "PutAUniqueHookNameHere", function()
  surface.SetDrawColor(255, 255, 255, 255)  -- Set the drawing color
  surface.SetMaterial(ourMat)               -- Use our cached material

  -- Show each filter for 1 second each
  -- (TEXFILTER enums correspond to integer numbers)
  local filter = math.floor(CurTime() % 4)

  render.PushFilterMag(filter)
  render.PushFilterMin(filter)

  -- Actually draw the rectangle with a magnification filter
  surface.DrawTexturedRectangle(pos, pos, sizeBig, sizeBig)

  -- Actually draw the rectangle with a minification filter
  surface.DrawTexturedRectangle(pos + sizeBig + 1, pos, sizeSmall, sizeSmall)

  render.PopFilterMin()
  render.PopFilterMag()

  draw.SimpleText(filters[filter], "ChatFont", pos + 1, pos + 1, color_white)
end)
