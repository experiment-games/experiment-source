---
template: lua-library-function.html
title: SpawnTracer
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Effects
  function: SpawnTracer
  realm: shared
  description: "Creates a tracer effect."
  
  arguments:
  - position: 1
    name: "start"
    type: Vector
  - position: 2
    name: "end"
    type: Vector
  - position: 3
    name: "entityIndex"
    type: number
    default: "0"
  - position: 4
    name: "attachment"
    type: number
    default: "0"
  - position: 5
    name: "velocity"
    type: number
    default: "0"
  - position: 6
    name: "shouldWhiz"
    type: boolean
    default: "false"
  - position: 7
    name: "customTracerName"
    type: string
    default: "NULL"
  - position: 8
    name: "particleId"
    type: number
    default: "0"
  returns:
    
---

<div class="lua__search__keywords">
Effects.SpawnTracer &#x2013; Creates a tracer effect.
</div>
