---
template: lua-library-function.html
title: FindNearestFacing
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindNearestFacing
  realm: server
  description: "Finds the nearest entity facing a direction"
  
  arguments:
  - position: 1
    name: "position"
    type: Vector
  - position: 2
    name: "direction"
    type: Vector
  - position: 3
    name: "maxDistance"
    type: number
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindNearestFacing &#x2013; Finds the nearest entity facing a direction
</div>
