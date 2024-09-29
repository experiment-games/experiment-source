---
template: lua-library-function.html
title: TraceModel
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Traces
  function: TraceModel
  realm: shared
  description: "Sweeps against a particular model, using collision rules."
  
  arguments:
  - position: 1
    name: "start"
    type: Vector
  - position: 2
    name: "end"
    type: Vector
  - position: 3
    name: "hullMinimum"
    type: Vector
  - position: 4
    name: "hullMaximum"
    type: Vector
  - position: 5
    name: "entity"
    type: Entity
  returns:
    - type: TraceResult
      description: "The trace result."
---

<div class="lua__search__keywords">
Traces.TraceModel &#x2013; Sweeps against a particular model, using collision rules.
</div>
