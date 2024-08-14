---
template: lua-library-function.html
title: SpawnParticleTracer
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Effects
  function: SpawnParticleTracer
  realm: shared
  description: "Spawns a tracer particle effect."
  
  arguments:
    - name: "effect"
      type: string
    - name: "start"
      type: Vector
    - name: "end"
      type: Vector
    - name: "whiz"
      type: boolean
      default: "false"
    - name: "entityIndex"
      type: number
      default: "0"
    - name: "attachment"
      type: number
      default: "0"
  returns:
    
---

<div class="lua__search__keywords">
Effects.SpawnParticleTracer &#x2013; Spawns a tracer particle effect.
</div>
