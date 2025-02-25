---
template: lua-class-function.html
title: PhysicsInitBox
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: PhysicsInitBox
  realm: shared
  description: "Initialize physics object as box."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "mins"
    type: Vector
  - position: 3
    name: "maxs"
    type: Vector
  - position: 4
    name: "surfaceProperty"
    type: string
    default: "\"default\""
  returns:
    
---

<div class="lua__search__keywords">
Entity:PhysicsInitBox &#x2013; Initialize physics object as box.
</div>
