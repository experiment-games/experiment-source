---
template: lua-library-function.html
title: SpawnBloodSpray
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Effects
  function: SpawnBloodSpray
  realm: server
  description: "Creates a blood spray effect.\nColor can be DONT_BLEED(-1), BLOOD_COLOR_RED(0), BLOOD_COLOR_YELLOW(1), BLOOD_COLOR_GREEN(2), BLOOD_COLOR_MECH(3) - enums don't exist yet in Lua\nFlags can be FX_BLOODSPRAY_DROPS(1), FX_BLOODSPRAY_GORE(2), FX_BLOODSPRAY_CLOUD(4), FX_BLOODSPRAY_ALL(255) - enums don't exist yet in Lua"
  
  arguments:
    - name: "origin"
      type: Vector
    - name: "direction"
      type: Vector
    - name: "color"
      type: number
    - name: "amount"
      type: number
    - name: "flags"
      type: number
  returns:
    
---

<div class="lua__search__keywords">
Effects.SpawnBloodSpray &#x2013; Creates a blood spray effect.\nColor can be DONT_BLEED(-1), BLOOD_COLOR_RED(0), BLOOD_COLOR_YELLOW(1), BLOOD_COLOR_GREEN(2), BLOOD_COLOR_MECH(3) - enums don't exist yet in Lua\nFlags can be FX_BLOODSPRAY_DROPS(1), FX_BLOODSPRAY_GORE(2), FX_BLOODSPRAY_CLOUD(4), FX_BLOODSPRAY_ALL(255) - enums don't exist yet in Lua
</div>
