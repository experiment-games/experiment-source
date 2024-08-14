---
template: lua-library-function.html
title: SpawnMuzzleFlash
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Effects
  function: SpawnMuzzleFlash
  realm: server
  description: "Creates a muzzle flash effect. Types can be MUZZLEFLASH_TYPE_DEFAULT(1), MUZZLEFLASH_TYPE_GUNSHIP(2), MUZZLEFLASH_TYPE_STRIDER(4) - enums don't exist yet in Lua"
  
  arguments:
    - name: "origin"
      type: Vector
    - name: "angles"
      type: Angle
    - name: "scale"
      type: number
    - name: "type"
      type: number
      default: "MUZZLEFLASH_TYPE_DEFAULT"
  returns:
    
---

<div class="lua__search__keywords">
Effects.SpawnMuzzleFlash &#x2013; Creates a muzzle flash effect. Types can be MUZZLEFLASH_TYPE_DEFAULT(1), MUZZLEFLASH_TYPE_GUNSHIP(2), MUZZLEFLASH_TYPE_STRIDER(4) - enums don't exist yet in Lua
</div>
