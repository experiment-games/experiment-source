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
    - name: "start"
      type: Vector
    - name: "end"
      type: Vector
    - name: "entityIndex"
      type: number
      default: "0"
    - name: "attachment"
      type: number
      default: "0"
    - name: "velocity"
      type: number
      default: "0"
    - name: "shouldWhiz"
      type: boolean
      default: "false"
    - name: "customTracerName"
      type: string
      default: "NULL"
    - name: "particleId"
      type: number
      default: "0"
  returns:
    
---

<div class="lua__search__keywords">
Effects.SpawnTracer &#x2013; Creates a tracer effect.
</div>
