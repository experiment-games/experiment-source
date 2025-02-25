---
template: lua-class-function.html
title: EmitAmbientSound
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: EmitAmbientSound
  realm: shared
  description: "Emit ambient sound."
  
  static: true
  arguments:
  - position: 1
    name: "soundIndex"
    type: number
  - position: 2
    name: "origin"
    type: Vector
  - position: 3
    name: "soundName"
    type: string
  - position: 4
    name: "flags"
    type: number
    default: "0"
  - position: 5
    name: "soundTime"
    type: number
    default: "0"
  returns:
    
---

<div class="lua__search__keywords">
_R.Entity.EmitAmbientSound &#x2013; Emit ambient sound.
</div>
