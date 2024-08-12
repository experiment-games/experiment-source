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
    - name: "position"
      type: Vector
    - name: "direction"
      type: Vector
    - name: "maxDistance"
      type: number
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindNearestFacing &#x2013; Finds the nearest entity facing a direction
</div>
