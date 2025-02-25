---
template: lua-class-function.html
title: PhysicsSplash
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: PhysicsSplash
  realm: server
  description: "Physics splash."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "center"
    type: Vector
  - position: 3
    name: "normal"
    type: Vector
  - position: 4
    name: "rawSpeed"
    type: number
  - position: 5
    name: "scaledSpeed"
    type: number
  returns:
    - type: boolean
      description: "true if splashed, false otherwise."
---

<div class="lua__search__keywords">
Entity:PhysicsSplash &#x2013; Physics splash.
</div>
