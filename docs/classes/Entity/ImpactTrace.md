---
template: lua-class-function.html
title: ImpactTrace
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: ImpactTrace
  realm: shared
  description: "Impact trace."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "trace"
    type: Trace
  - position: 3
    name: "damage type"
    type: number
  - position: 4
    name: "custom impact name"
    type: string
    default: "nullptr"
  returns:
    
---

<div class="lua__search__keywords">
Entity:ImpactTrace &#x2013; Impact trace.
</div>
