---
template: lua-class-function.html
title: PhysicsInit
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: PhysicsInit
  realm: shared
  description: "Initialize physics object."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "solidType"
    type: integer
    default: "SOLID_VPHYSICS"
  - position: 3
    name: "solidFlags"
    type: number
    default: "0"
  - position: 4
    name: "createAsleep"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Entity:PhysicsInit &#x2013; Initialize physics object.
</div>
