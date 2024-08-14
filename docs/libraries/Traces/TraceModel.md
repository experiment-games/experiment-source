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
    - name: "entity"
      type: Entity
    - name: "start"
      type: Vector
    - name: "end"
      type: Vector
    - name: "hullMinimum"
      type: Vector
    - name: "hullMaximum"
      type: Vector
  returns:
    - type: TraceResult
      description: "The trace result."
---

<div class="lua__search__keywords">
Traces.TraceModel &#x2013; Sweeps against a particular model, using collision rules.
</div>
