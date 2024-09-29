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
  - position: 1
    name: "effect"
    type: string
  - position: 2
    name: "start"
    type: Vector
  - position: 3
    name: "end"
    type: Vector
  - position: 4
    name: "whiz"
    type: boolean
    default: "false"
  - position: 5
    name: "entityIndex"
    type: number
    default: "0"
  - position: 6
    name: "attachment"
    type: number
    default: "0"
  returns:
    
---

<div class="lua__search__keywords">
Effects.SpawnParticleTracer &#x2013; Spawns a tracer particle effect.
</div>
