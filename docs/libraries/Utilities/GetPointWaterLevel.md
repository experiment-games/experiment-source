---
template: lua-library-function.html
title: GetPointWaterLevel
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: GetPointWaterLevel
  realm: shared
  description: "Get the water level at the specified position"
  
  arguments:
  - position: 1
    name: "point"
    type: Vector
  - position: 2
    name: "minz"
    type: number
  - position: 3
    name: "maxz"
    type: number
  returns:
    - type: number
      description: "Water level at the specified position"
---

<div class="lua__search__keywords">
Utilities.GetPointWaterLevel &#x2013; Get the water level at the specified position
</div>
