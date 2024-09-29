---
template: lua-library-function.html
title: SpawnBloodStream
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Effects
  function: SpawnBloodStream
  realm: server
  description: "Creates a blood stream effect. Color can be DONT_BLEED(-1), BLOOD_COLOR_RED(0), BLOOD_COLOR_YELLOW(1), BLOOD_COLOR_GREEN(2), BLOOD_COLOR_MECH(3) - enums don't exist yet in Lua"
  
  arguments:
  - position: 1
    name: "origin"
    type: Vector
  - position: 2
    name: "direction"
    type: Vector
  - position: 3
    name: "color"
    type: number
  - position: 4
    name: "amount"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Effects.SpawnBloodStream &#x2013; Creates a blood stream effect. Color can be DONT_BLEED(-1), BLOOD_COLOR_RED(0), BLOOD_COLOR_YELLOW(1), BLOOD_COLOR_GREEN(2), BLOOD_COLOR_MECH(3) - enums don't exist yet in Lua
</div>
