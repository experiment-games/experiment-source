---
template: lua-library-function.html
title: FindClassNearestFacing
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindClassNearestFacing
  realm: server
  description: "Finds the nearest entity of a class facing a direction"
  
  arguments:
    - name: "position"
      type: Vector
    - name: "direction"
      type: Vector
    - name: "maxDistance"
      type: number
    - name: "className"
      type: string
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindClassNearestFacing &#x2013; Finds the nearest entity of a class facing a direction
</div>
