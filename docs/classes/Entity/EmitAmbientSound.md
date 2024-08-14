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
    - name: "soundIndex"
      type: number
    - name: "origin"
      type: Vector
    - name: "soundName"
      type: string
    - name: "flags"
      type: number
      default: "0"
    - name: "soundTime"
      type: number
      default: "0"
  returns:
    
---

<div class="lua__search__keywords">
_R.Entity.EmitAmbientSound &#x2013; Emit ambient sound.
</div>
