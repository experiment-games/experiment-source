---
template: lua-library-function.html
title: TraceEntity
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Traces
  function: TraceEntity
  realm: shared
  description: "Runs a trace using the entity's collisionmodel between two points. This does not take the entity's angles into account and will trace its unrotated collisionmodel."
  
  arguments:
    - name: "entity"
      type: Entity
    - name: "trace"
      type: Trace
  returns:
    - type: TraceResult
      description: "The trace result."
---

<div class="lua__search__keywords">
Traces.TraceEntity &#x2013; Runs a trace using the entity's collisionmodel between two points. This does not take the entity's angles into account and will trace its unrotated collisionmodel.
</div>
