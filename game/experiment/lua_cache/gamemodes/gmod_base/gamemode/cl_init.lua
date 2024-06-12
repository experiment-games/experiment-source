include("shared.lua")

surface.SetDrawColor = surface.DrawSetColor
surface.DrawRect = surface.DrawFilledRect

function GM:HudViewportPaint()
    hook.Run("HUDPaint")
end
