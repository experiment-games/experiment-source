---
template: lua-library-function.html
title: FindWaterSurface
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Utilities
  function: FindWaterSurface
  realm: server
  description: "Finds the water surface at the given position."
  
  arguments:
  - position: 1
    name: "position"
    type: Vector
  - position: 2
    name: "minZ"
    type: number
  - position: 3
    name: "maxZ"
    type: number
  returns:
    - type: number
      description: "The water surface z position."
---

<div class="lua__search__keywords">
Utilities.FindWaterSurface &#x2013; Finds the water surface at the given position.
</div>
